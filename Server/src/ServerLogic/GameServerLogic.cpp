#include <iostream>
#include <SDL3_net/SDL_Net.h>

#include "OTNFile.h"
#include "BinaryDeserializer.h"
#include "BinarySerializer.h"
#include "NetServerManager.h"
#include "ServerLogic/GameServerLogic.h"

GameServerLogic::GameServerLogic(NetServer* server) 
    : IServerLogic(server) {
}

void GameServerLogic::OnMessage(NET_StreamSocket* client, const std::string& msg) {
    if (!client)
        return;

    auto reqs = GetRequests(msg);
    for (auto& r : reqs) {
        if (!r.response)
            continue;

        HandleRequest(client, r);
    }
}

void GameServerLogic::OnServerMessage(
    const std::string& serverName,
    const std::string& msg)
{
    if (serverName == "sql_server")
        HandleSQLServer(msg);
}

int64_t GameServerLogic::RegisterPendingSQL(NET_StreamSocket* client, uint32_t requestID) {
    int64_t internalID = m_nextInternalID++;
    m_pendingSQL[internalID] = { client, requestID };
    return internalID;
}

bool GameServerLogic::RemovePendingSQL(int64_t requstID, PendingSQLRequest& outRequest) {
    auto it = m_pendingSQL.find(requstID);
    if (it == m_pendingSQL.end())
        return false;

    outRequest = it->second;
    m_pendingSQL.erase(it);
    return true;
}

void GameServerLogic::HandleSQLServer(const std::string& msg) {
    OTN::OTNReader reader;
    if (!reader.ReadString(msg))
        return;

    auto header = reader.TryGetObject("header");
    if (!header)
        return;

    auto action = header->TryGetValue<std::string>(0, "action");
    auto requestID = header->TryGetValue<int64_t>(0, "request_id");
    auto response = header->TryGetValue<bool>(0, "response");
    if (!action || !requestID || !response)
        return;

    PendingSQLRequest pending;
    if (!RemovePendingSQL(static_cast<uint32_t>(*requestID), pending))
        return;
    
    if (*response == false) {
        auto body = reader.TryGetObject("body");
        if (!body)
            return;
        auto errorMsg = body->TryGetValue<std::string>(0, "error");
        if (!errorMsg)
            return;
        SentError(pending.client, pending.clientRequestID, *errorMsg);
    }

    if (action == "InsertAgentsResult")
        HandleReceiveSQLSyncMissingData(reader, pending);
    else if (action == "GetAgentIDResult")
        HandleReceiveSQLServerAgentIDList(reader, pending);
    else if (action == "HandleGetMissinAgents")
        HandleReceiveSQLServerAgentList(reader, pending);
    else if(action == "HandleDeleteAgentsResult")
        HandleReceiveSQLDeleteAgentsResult(reader, pending);
    else if(action == "HandleUpdateAgentsResult")
        HandleReceiveSQLUpdateAgentsResult(reader, pending);
    else
        std::cerr << "Unkown action game server '" << *action << "'\n";

}

void GameServerLogic::HandleReceiveSQLSyncMissingData(OTN::OTNReader& reader, const PendingSQLRequest& pending) {
    auto idsObj = reader.TryGetObject("ids");
    if (!idsObj)
        return;

    OTN::OTNWriter writer;
    writer.AppendObject(*idsObj);

    std::string payload;
    if (!writer.SaveToString(payload))
        return;

    Request response;
    response.id = pending.clientRequestID;
    response.response = true;
    response.otnPayload = payload;

    SentRequest(pending.client, response);
}

void GameServerLogic::HandleReceiveSQLServerAgentIDList(OTN::OTNReader& reader, const PendingSQLRequest& pending) {
    auto idsObj = reader.TryGetObject("Result");
    if (!idsObj)
        return;
    idsObj->SetObjectName("List");

    OTN::OTNWriter writer;
    writer.AppendObject(*idsObj);

    std::string payload;
    if (!writer.SaveToString(payload))
        return;

    Request response;
    response.id = pending.clientRequestID;
    response.response = true;
    response.otnPayload = payload;

    SentRequest(pending.client, response);
}

void GameServerLogic::HandleReceiveSQLServerAgentList(OTN::OTNReader& reader, const PendingSQLRequest& pending) {
    auto agentObj = reader.TryGetObject("agents");
    auto boardStateObj = reader.TryGetObject("board_states");
    auto gameMoveObj = reader.TryGetObject("game_moves");
    if (!agentObj)
        return;

    OTN::OTNWriter writer;
    writer.AppendObject(*agentObj);
    if (boardStateObj)
        writer.AppendObject(*boardStateObj);
    if (gameMoveObj)
        writer.AppendObject(*gameMoveObj);

    std::string payload;
    if (!writer.SaveToString(payload))
        return;

    Request response;
    response.id = pending.clientRequestID;
    response.response = true;
    response.otnPayload = payload;

    SentRequest(pending.client, response);
}

void GameServerLogic::HandleReceiveSQLDeleteAgentsResult(OTN::OTNReader& reader, const PendingSQLRequest& pending) {
    auto idsObj = reader.TryGetObject("Result");
    if (!idsObj)
        return;

    OTN::OTNWriter writer;
    writer.AppendObject(*idsObj);

    std::string payload;
    if (!writer.SaveToString(payload))
        return;

    Request response;
    response.id = pending.clientRequestID;
    response.response = true;
    response.otnPayload = payload;

    SentRequest(pending.client, response);
}

void GameServerLogic::HandleReceiveSQLUpdateAgentsResult(OTN::OTNReader& reader, const PendingSQLRequest& pending) {
    auto idsObj = reader.TryGetObject("Result");
    if (!idsObj)
        return;

    OTN::OTNWriter writer;
    idsObj->SetObjectName("ids");
    writer.AppendObject(*idsObj);

    std::string payload;
    if (!writer.SaveToString(payload))
        return;

    Request response;
    response.id = pending.clientRequestID;
    response.response = true;
    response.otnPayload = payload;

    SentRequest(pending.client, response);
}

void GameServerLogic::HandleRequest(NET_StreamSocket* client, const Request& req) {
    OTN::OTNReader reader;
    if (!reader.ReadString(req.otnPayload)) {
        SentError(client, req.id, "Failed to parse OTN: " + reader.GetError());
        return;
    }

    auto header = reader.TryGetObject("header");
    auto body = reader.TryGetObject("body");

    if (!header || header->GetRowCount() == 0) {
        SentError(client, req.id, "Failed to extract header of the request");
        return;
    }

    if (!body) {
        SentError(client, req.id, "Failed to extract body of the request");
        return;
    }

    auto action = header->TryGetValue<std::string>(0, "action");
    if (!action) {
        SentError(client, req.id, "Failed to extract action of the request");
        return;
    }

    if (*action == "SyncMissingData")
        HandleSyncMissingData(client, req.id, *body);
    else if (*action == "GetAgentIDList")
        HandleServerAgentIDList(client, req.id);
    else if (*action == "RequestMissingAgents")
        HandleGetMissinAgents(client, req.id, *body);
    else if (*action == "SyncDeleteData")
        HandleDeleteAgents(client, req.id, *body);
    else if (*action == "SyncDirtyData")
        HandleDirtyAgents(client, req.id, *body);
    else
        std::cerr << "Unkown action game server '" << *action << "'\n";
}

void GameServerLogic::HandleSyncMissingData(NET_StreamSocket* client, uint32_t requestID, const OTN::OTNObject& body) {
    OTN::OTNObject headerObj = CreateSQLRequestHeader("InsertAgents", client, requestID);

    OTN::OTNWriter writer;
    writer.AppendObject(headerObj);
    writer.AppendObject(body);

    std::string msg;
    writer.SaveToString(msg);

    NetServerManager::SendMessage(
        m_server,
        "sql_server",
        msg
    );
}

void GameServerLogic::HandleServerAgentIDList(NET_StreamSocket* client, uint32_t requestID) {
    int64_t internalID = RegisterPendingSQL(client, requestID);

    OTN::OTNObject headerObj = CreateSQLRequestHeader("GetAgentIDs", client, requestID);
    OTN::OTNObject body{ "body" };

    OTN::OTNWriter writer;
    writer.AppendObject(headerObj);
    writer.AppendObject(body);

    std::string msg;
    writer.SaveToString(msg);

    NetServerManager::SendMessage(
        m_server,
        "sql_server",
        msg
    );
}

void GameServerLogic::HandleGetMissinAgents(NET_StreamSocket* client, uint32_t requestID, const OTN::OTNObject& body) {
    int64_t internalID = RegisterPendingSQL(client, requestID);

    OTN::OTNObject headerObj = CreateSQLRequestHeader("HandleGetMissinAgents", client, requestID);
    
    OTN::OTNWriter writer;
    writer.AppendObject(headerObj);
    writer.AppendObject(body);

    std::string msg;
    writer.SaveToString(msg);

    NetServerManager::SendMessage(
        m_server,
        "sql_server",
        msg
    );
}

void GameServerLogic::HandleDeleteAgents(NET_StreamSocket* client, uint32_t requestID, const OTN::OTNObject& body) {
    int64_t internalID = RegisterPendingSQL(client, requestID);

    OTN::OTNObject headerObj = CreateSQLRequestHeader("HandleDeleteAgents", client, requestID);

    OTN::OTNWriter writer;
    writer.AppendObject(headerObj);
    writer.AppendObject(body);

    std::string msg;
    writer.SaveToString(msg);

    NetServerManager::SendMessage(
        m_server,
        "sql_server",
        msg
    );
}

void GameServerLogic::HandleDirtyAgents(NET_StreamSocket* client, uint32_t requestID, const OTN::OTNObject& body) {
    int64_t internalID = RegisterPendingSQL(client, requestID);

    OTN::OTNObject headerObj = CreateSQLRequestHeader("HandleDirtyAgents", client, requestID);

    OTN::OTNWriter writer;
    writer.AppendObject(headerObj);
    writer.AppendObject(body);

    std::string msg;
    writer.SaveToString(msg);

    NetServerManager::SendMessage(
        m_server,
        "sql_server",
        msg
    );
}

OTN::OTNObject GameServerLogic::CreateSQLRequestHeader(const std::string& action, NET_StreamSocket* client, uint32_t requestID) {
    int64_t internalID = RegisterPendingSQL(client, requestID);

    OTN::OTNObject headerObj{ "header" };
    headerObj.SetNames("action", "request_id");
    headerObj.SetTypes("String", "int64");
    headerObj.AddDataRow(action, internalID);

    return headerObj;
}

std::vector<GameServerLogic::Request>
GameServerLogic::GetRequests(const std::string& msg) {
    std::vector<Request> reqs;

    m_receiveBuffer.insert(
        m_receiveBuffer.end(),
        msg.begin(),
        msg.end()
    );

    while (m_receiveBuffer.size() >= sizeof(uint32_t)) {
        uint32_t length = 0;
        std::memcpy(&length, m_receiveBuffer.data(), sizeof(uint32_t));

        if (length > MAX_PACKET_SIZE) {
            m_receiveBuffer.clear();
            break;
        }

        if (m_receiveBuffer.size() < sizeof(uint32_t) + length)
            break;

        std::vector<uint8_t> packet(
            m_receiveBuffer.begin() + sizeof(uint32_t),
            m_receiveBuffer.begin() + sizeof(uint32_t) + length
        );

        BinaryDeserializer des(packet);
        Request req;
        req.response = true;
        req.id = des.Read<uint32_t>();
        std::vector<uint8_t> payloadData = des.ReadVector<uint8_t>();

        req.otnPayload.assign(
            payloadData.begin(),
            payloadData.end()
        );

        reqs.push_back(std::move(req));

        m_receiveBuffer.erase(
            m_receiveBuffer.begin(),
            m_receiveBuffer.begin() + sizeof(uint32_t) + length
        );
    }

    return reqs;
}

bool GameServerLogic::SentRequest(NET_StreamSocket* client, const Request& request) {
    if (!client)
        return false;

    auto& id = request.id;
    auto& response = request.response;
    auto& payLoad = request.otnPayload;

    // send: [size][id][response][payload length][payload bytes]
    BinarySerializer ser;
    ser.AddField(id);
    ser.AddField<bool>(response);
    ser.AddField(payLoad);

    std::vector<uint8_t> body = ser.ToBuffer();
    uint32_t len = static_cast<uint32_t>(body.size());

    std::vector<uint8_t> packet(sizeof(uint32_t) + body.size());
    
    std::memcpy(packet.data(), &len, sizeof(uint32_t));
    std::memcpy(packet.data() + sizeof(uint32_t), 
                body.data(), 
                body.size());

    return NET_WriteToStreamSocket(client, packet.data(), static_cast<int>(packet.size()));
}

void GameServerLogic::SentError(NET_StreamSocket* client, uint32_t requestID, const std::string& errorMsg) {
    if (!client)
        return;

    Request rq;
    rq.id = requestID;
    rq.response = false;
    rq.otnPayload = errorMsg;

    SentRequest(client, rq);
}