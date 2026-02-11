#include <iostream>

#include "NetworkSystem.h"
#include "NetServerManager.h"

int main(int argc, char* argv[]) {
	if (!NetworkSystem::Init())
		return 1;
	
	NetServer* server = NetServerManager::CreateServer("testSerer");
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