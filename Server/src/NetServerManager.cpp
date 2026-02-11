#include "NetServerManager.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <thread>

NetServer* NetServerManager::CreateServer(const std::string& name) {
	NetServer* server = new NetServer(name);
	m_serverList.push_back(server);
	return server;
}

bool NetServerManager::DestroyServer(NetServer* server) {
	auto it = std::find(m_serverList.begin(), m_serverList.end(), server);
	if (it == m_serverList.end())
		return false;

	if (server->IsRunning()) {
		std::cerr << "Faild to destroy server, server is currently active!";
		return false;
	}

	delete server;
	m_serverList.erase(it);
	return true;
}

void NetServerManager::StartAll() {
	for (auto* server : m_serverList) {
		std::thread([server]() {
			server->Run();
		}).detach();
	}
}

void NetServerManager::StopAll() {
	
}
