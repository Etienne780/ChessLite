#include <iostream>
#include <SDL3_net/SDL_Net.h>

#include "NetServerManager.h"
#include "ServerLogic/GameServerLogic.h"

GameServerLogic::GameServerLogic(NetServer* server) 
    : IServerLogic(server) {
}

void GameServerLogic::OnMessage(NET_StreamSocket* client, const std::string& msg) {
    if (!client || msg.size() < sizeof(uint32_t))
        return;

    std::string message = "Server: " + msg;

    int sent = NET_WriteToStreamSocket(client, message.c_str(), static_cast<int>(message.size()));
    if (sent < 0)
        std::cerr << "Failed to send echo: " << SDL_GetError() << "\n";
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