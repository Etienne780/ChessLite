#include "NetServerManager.h"

#include <iostream>
#include <string>
#include <algorithm>

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

void NetServerManager::SendMessage(const std::string& serverName, const std::string& msg) {
	for (auto* s : m_serverList) {
		if (s->GetName() == serverName) {
			if (s->IsInitialized()) {
				s->m_logic->OnMessage(nullptr, msg);
			}
			break;
		}
	}
}

void NetServerManager::StartAll() {
	for (auto* server : m_serverList) {
		m_threads.emplace_back([server]() {
			server->Run();
		});
	}
}

void NetServerManager::StopAll() {
	for (auto* server : m_serverList) {
		server->Stop();
	}

	for (auto& thread : m_threads) {
		if (thread.joinable())
			thread.join();
	}

	m_threads.clear();
}
