#pragma once
#include <mutex>
#include <string>
#include <SDL3_net/SDL_net.h>

class NetServer;

class IServerLogic {
public:
	IServerLogic(NetServer* server) 
		: m_server(server) {
	}
	virtual ~IServerLogic() = default;

	/*< called every tick */
	void OnRunExternal();

	void OnClientConnectedExternal(NET_StreamSocket* client);
	void OnMessageExternal(NET_StreamSocket* client, const std::string& msg);
	void OnServerMessageExternal(const std::string& serverName, const std::string& msg);
	void OnClientDisconnectedExternal(NET_StreamSocket* client);

protected: 
	std::mutex m_logicMutex;
	NetServer* m_server = nullptr;

	virtual void OnRun() {};

	virtual void OnClientConnected(NET_StreamSocket* client) {};
	virtual void OnMessage(NET_StreamSocket* client, const std::string& msg) {};
	virtual void OnServerMessage(const std::string& serverName, const std::string& msg) {};
	virtual void OnClientDisconnected(NET_StreamSocket* client) {};
};