#include <iostream>

#include "ConfigLoader.h"

#include "NetworkSystem.h"
#include "NetServerManager.h"

#include "ServerLogic/SQLServerLogic.h"

int main(int argc, char* argv[]) {
	if (!NetworkSystem::Init())
		return 1;
	
	DBConfig config = LoadDBConfigRelative("config.otn");

	NetServer* server = NetServerManager::CreateServer("testSQLServer");
	server->SetLogic<SQLServerLogic>(config);
	server->Start(5000);

	NetServerManager::StartAll();

	std::cout << "-------------------------------------\n";
	std::cout << "           Server Startup            \n";
	std::cout << "-------------------------------------\n\n";

	while (true) {
	}

	NetworkSystem::Quit();
	return 0;
}