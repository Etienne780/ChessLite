#include <iostream>
#include <SDL3_net/SDL_Net.h>

#include "BinaryDeserializer.h"
#include "BinarySerializer.h"
#include "NetServerManager.h"
#include "ServerLogic/GameServerLogic.h"

GameServerLogic::GameServerLogic(NetServer* server) 
    : IServerLogic(server) {
}

void GameServerLogic::OnMessage(NET_StreamSocket* client, const std::string& msg) {
    if (!client || msg.size() < sizeof(uint32_t))
        return;

    // packet: [size][id][payload length][payload bytes]
    uint32_t length;
    std::memcpy(&length, msg.data(), sizeof(uint32_t));

    std::vector<uint8_t> data{ msg.begin() + sizeof(uint32_t), msg.end()};

    BinaryDeserializer des(data);
    uint32_t id = des.Read<uint32_t>();
    std::vector<uint8_t> payloadData = des.ReadVector<uint8_t>();
    std::string payload(payloadData.begin(), payloadData.end());

    std::cout << "Received [id=" << id << "]: " << payload << "\n";

    std::string response = "Server: " + payload;
    std::vector<uint8_t> responsePayload(response.begin(), response.end());

    // send: [size][id][response][payload length][payload bytes]
    BinarySerializer ser;
    ser.AddField(id);
    ser.AddField<bool>(true);
    ser.AddField(responsePayload);

    std::vector<uint8_t> body = ser.ToBuffer();
    uint32_t len = static_cast<uint32_t>(body.size());

    std::vector<uint8_t> packet(sizeof(uint32_t) + body.size());
    std::memcpy(packet.data(), &len, sizeof(uint32_t));
    std::memcpy(packet.data() + sizeof(uint32_t), body.data(), body.size());

    bool ok = NET_WriteToStreamSocket(client, packet.data(), static_cast<int>(packet.size()));
    if (!ok)
        std::cerr << "Failed to send response: " << SDL_GetError() << "\n";
    else
        std::cout << "Sent [id=" << id << "]: " << response << "\n";
}

void GameServerLogic::OnServerMessage(const std::string& serverName, const std::string& msg) {
    if (serverName == "sql_server") {
        std::cout << "received data from sql server:\n";
        std::cout << msg;
    }

    if (msg == "test") {
        NetServerManager::SendMessage(m_server, "sql_server", "sql_test");
    }
}