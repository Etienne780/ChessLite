#include <iostream>
#include "IServerLogic.h"
#include "NetServer.h"

void IServerLogic::OnRunExternal() {
	std::lock_guard guard(m_logicMutex);
	OnRun();
}

void IServerLogic::OnClientConnectedExternal(NET_StreamSocket* client) {
	std::lock_guard guard(m_logicMutex);
	OnClientConnected(client);
}

void IServerLogic::OnMessageExternal(NET_StreamSocket* client, const std::string& msg) {
	std::lock_guard guard(m_logicMutex);
	OnMessage(client, msg);
}

void IServerLogic::OnServerMessageExternal(const std::string& serverName, const std::string& msg) {
	std::lock_guard guard(m_logicMutex);
	OnServerMessage(serverName, msg);
}

void IServerLogic::OnClientDisconnectedExternal(NET_StreamSocket* client) {
	std::lock_guard guard(m_logicMutex);
	OnClientDisconnected(client);
}