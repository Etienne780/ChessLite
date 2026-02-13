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

	NetServer* gameServer = NetServerManager::CreateServer("game_server");
	gameServer->SetLogic<GameServerLogic>();
	gameServer->Start(5000);

	NetServer* sqlServer = NetServerManager::CreateServer("sql_server");
	sqlServer->SetLogic<SQLServerLogic>(config);
	sqlServer->Start(5001);

	NetServerManager::StartAll();

	std::cout << "-------------------------------------\n";
	std::cout << "           Server Startup            \n";
	std::cout << "-------------------------------------\n\n";

	while (true) {
	}

	NetServerManager::StopAll();
	NetworkSystem::Quit();
	return 0;
}