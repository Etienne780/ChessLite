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

    Request req = GetRequest(msg);
    if (!req.response)
        return;

    HandleRequest(client, req);
}

void GameServerLogic::OnServerMessage(
    const std::string& serverName,
    const std::string& msg)
{
    if (serverName != "sql_server")
        return;

    OTN::OTNReader reader;
    if (!reader.ReadString(msg))
        return;

    auto header = reader.TryGetObject("header");
    if (!header)
        return;

    auto requestID = header->TryGetValue<int64_t>(0, "request_id");
    if (!requestID)
        return;

    auto it = m_pendingSQL.find(static_cast<uint32_t>(*requestID));
    if (it == m_pendingSQL.end())
        return;

    auto pending = it->second;
    m_pendingSQL.erase(it);

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
}

void GameServerLogic::HandleSyncMissingData(
    NET_StreamSocket* client,
    uint32_t requestID,
    const OTN::OTNObject& body)
{
    uint32_t internalID = m_nextInternalID++;

    m_pendingSQL[internalID] = { client, requestID };

    OTN::OTNObject header{ "header" };
    header.SetNames("action", "request_id");
    header.SetTypes("String", "int64");
    header.AddDataRow("InsertAgents", static_cast<int64_t>(internalID));

    OTN::OTNWriter writer;
    writer.AppendObject(header);
    writer.AppendObject(body);

    std::string msg;
    writer.SaveToString(msg);

    NetServerManager::SendMessage(
        m_server,
        "sql_server",
        msg
    );
}

GameServerLogic::Request GameServerLogic::GetRequest(const std::string& msg) {
    Request req;
    req.response = false;

    // packet: [size][[id][payload length][payload bytes]]
    m_receiveBuffer.insert(m_receiveBuffer.end(), msg.begin(), msg.end());

    while (m_receiveBuffer.size() >= sizeof(uint32_t)) {
        uint32_t length = 0;
        std::memcpy(&length, m_receiveBuffer.data(), sizeof(uint32_t));

        if (length > MAX_PACKET_SIZE) {
            m_receiveBuffer.clear();
            return req;
        }

        // return early because full data is not there
        if (m_receiveBuffer.size() < sizeof(uint32_t) + length)
            return req;

        std::vector<uint8_t> packet(m_receiveBuffer.begin() + sizeof(uint32_t),
            m_receiveBuffer.begin() + sizeof(uint32_t) + length);
        BinaryDeserializer des(packet);

        uint32_t id = des.Read<uint32_t>();
        std::vector<uint8_t> payloadData = des.ReadVector<uint8_t>();
        std::string payload(payloadData.begin(), payloadData.end());

        req.id = id;
        req.response = true;
        req.otnPayload = payload;

        m_receiveBuffer.erase(
            m_receiveBuffer.begin(), 
            m_receiveBuffer.begin() + sizeof(uint32_t) + length);

        return req;
    }

    return req;
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