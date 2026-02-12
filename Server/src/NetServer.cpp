#include "NetServer.h"

#include <thread>
#include <iostream>

#include "NetworkSystem.h"

NetServer::~NetServer() {
	FreeServerLogic();
}

bool NetServer::Start(uint16_t port) {
	NET_Address* addr = NetworkSystem::GetBindAddress();

	m_server = NET_CreateServer(addr, port);
	if (!m_server) {
		std::cerr << "Failed to create server '" << m_name << "' on port '" << port << "'!\n";
		return false;
	}

	m_port = port;
	m_isInitialized = true;
	return true;
}

void NetServer::Stop() {
	NET_DestroyServer(m_server);
	m_server = nullptr;
	m_isInitialized = false;
	m_port = 0;
}

void NetServer::Run() {
	if (!m_isInitialized) {
		std::cerr << "Faild to run uninitialized server '" << m_name << "' on port '" << m_port << "'!\n";
		return;
	}

	m_running = true;

	while (m_running) {
		if (m_logic)
			m_logic->OnRun();

		NET_StreamSocket* client = WaitForClient();
		if (!client) {
			SDL_Delay(10);
			continue;
		}

		std::thread(&NetServer::HandleClient, this, client).detach();
	}
}

bool NetServer::IsInitialized() const {
	return m_isInitialized;
}

bool NetServer::IsRunning() const {
	return m_running;
}

const std::string& NetServer::GetName() const {
	return m_name;
}

uint16_t NetServer::GetPort() const {
	return m_port;
}

NetServer::NetServer(const std::string& name) 
	: m_name(name) {
}

void NetServer::HandleClient(NET_StreamSocket* client) const {
	if (m_logic)
		m_logic->OnClientConnected(client);

	char buffer[512];
	while (true) {
		int received = NET_ReadFromStreamSocket(client, buffer, sizeof(buffer));

		if (received > 0) {
			std::string msg(buffer, received);
			std::cout << "[" << m_name << "] Received: " << msg << "\n";

			if (m_logic)
				m_logic->OnMessage(client, msg);
		}
		else if (received == 0) {
			SDL_Delay(10);
			continue;
		}
		else {
			break;
		}
	}

	if (m_logic)
		m_logic->OnClientDisconnected(client);

	NET_DestroyStreamSocket(client);
}

NET_StreamSocket* NetServer::WaitForClient() {
	NET_StreamSocket* client = nullptr;
	if (NET_AcceptClient(m_server, &client))
		return client;
	return nullptr;
}

void NetServer::FreeServerLogic() {
	delete m_logic;
}