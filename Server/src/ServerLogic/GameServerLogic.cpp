#include <iostream>
#include <SDL3_net/SDL_Net.h>

#include "NetServerManager.h"
#include "ServerLogic/GameServerLogic.h"

GameServerLogic::GameServerLogic(NetServer* server) 
    : IServerLogic(server) {
}

void GameServerLogic::OnMessage(NET_StreamSocket* client, const std::string& msg) {

}

void GameServerLogic::OnServerMessage(const std::string& serverName, const std::string& msg) {
    if (serverName == "sql_server") {
        std::cout << "received data from sql server:";
        std::cout << msg;
    }

    if (msg == "test") {
        NetServerManager::SendMessage(m_server, "sql_server", "sql_test");
    }
}