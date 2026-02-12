#include <iostream>

#include "ConfigLoader.h"

#include "NetworkSystem.h"
#include "NetServerManager.h"

#include "ServerLogic/SQLServerLogic.h"
#include "ServerLogic/GameServerLogic.h"

int main(int argc, char* argv[]) {
	if (!NetworkSystem::Init())
		return 1;
	
	DBConfig config = LoadDBConfigRelative("config.otn");

	NetServer* server = NetServerManager::CreateServer("sql_server");
	server->SetLogic<SQLServerLogic>(config);
	server->Start(5000);

	NetServer* server = NetServerManager::CreateServer("game_server");
	server->SetLogic<GameServerLogic>();
	server->Start(5001);

	NetServerManager::StartAll();

	std::cout << "-------------------------------------\n";
	std::cout << "           Server Startup            \n";
	std::cout << "-------------------------------------\n\n";

	while (true) {
	}

	NetworkSystem::Quit();
	return 0;
}