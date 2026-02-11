#include "GameClient.h"
#include <CoreLib/Log.h>

GameClient::GameClient() {
}

GameClient::~GameClient() {
	Disconnect();
}

bool GameClient::Connect(const std::string& host, uint16_t port) {
	if (IsConnected())
		return true;

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
	if (!IsConnected())
		return false;

	return NET_WriteToStreamSocket(m_socket, msg.c_str(), static_cast<int>(msg.size()));
}

bool GameClient::Receive(std::string& out) {
	if (!IsConnected())
		return false;

	char buffer[512];
	int received = NET_ReadFromStreamSocket(m_socket, buffer, sizeof(buffer));

	if (received > 0) {
		out.assign(buffer, received);
		return true;
	}

	if (received == 0) {
		return false;
	}

	Disconnect();
	AddError(SDL_GetError());
	return false;
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

void GameClient::AddError(const std::string& msg) {
	m_error += msg;
}