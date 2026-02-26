#include "GameClient.h"
#include <CoreLib/Log.h>

GameClient::GameClient() {
}

GameClient::~GameClient() {
	Disconnect();
}

bool GameClient::Connect(const std::string& host, uint16_t port) {
	if (IsConnected()  && 
		m_host == host && 
		m_port == port)
		return true;

	m_host = host;
	m_port = port;
	NET_Address* addr = NET_ResolveHostname(host.c_str());
	if (!addr) {
		AddError("Failed to resolve host!\n");
		return false;
	}

	if (NET_WaitUntilResolved(addr, 5000) != NET_Status::NET_SUCCESS) {
		AddError("DNS resolution failed!\n");
		NET_UnrefAddress(addr);
		return false;
	}

	m_socket = NET_CreateClient(addr, port);
	NET_UnrefAddress(addr);

	if (!m_socket) {
		AddError("Failed to connect to server\n");
		return false;
	}

	return true;
}

void GameClient::Disconnect() {
	if (!IsConnected())
		return;

	NET_DestroyStreamSocket(m_socket);
	m_socket = nullptr;
}

bool GameClient::Send(const std::string& msg) {
	if (!IsConnected()) {
		AddError("Not connected");
		return false;
	}

	int sent = NET_WriteToStreamSocket(m_socket, msg.c_str(), static_cast<int>(msg.size()));
	if (sent < 0) {
		AddError(SDL_GetError());
		Disconnect();
		return false;
	}

	return true;
}

bool GameClient::Receive(std::string& out) {
	if (!IsConnected()) {
		AddError("Not connected");
		return false;
	}
	constexpr int bufferSize = 4096;
	char buffer[bufferSize];
	int received = NET_ReadFromStreamSocket(m_socket, buffer, sizeof(buffer) - 1);

	if (received > 0  && received < bufferSize) {
		buffer[received] = '\0';
		out.assign(buffer, received);
		return true;
	}

	if (received < 0) {
		Disconnect();
		AddError(SDL_GetError());
		return false;
	}

	Disconnect();
	AddError(SDL_GetError());
	return false;
}

void GameClient::ClearError() {
	m_error.clear();
}

bool GameClient::IsConnected() const {
	if (!m_socket)
		return false;

	NET_Status status = NET_GetConnectionStatus(m_socket);
	if (status == NET_Status::NET_SUCCESS)
		return true;

	return false;
}

const std::string& GameClient::GetError() const {
	return m_error;
}

const std::string& GameClient::GetHost() const {
	return m_host;
}

uint16_t GameClient::GetPort() const {
	return m_port;
}

void GameClient::AddError(const std::string& msg) {
	m_error += msg;
}