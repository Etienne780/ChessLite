#pragma once
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
	virtual void OnRun() {};

	virtual void OnClientConnected(NET_StreamSocket* client) {};
	virtual void OnMessage(NET_StreamSocket* client, const std::string& msg) {};
	virtual void OnServerMessage(const std::string& serverName, const std::string& msg) {};
	virtual void OnClientDisconnected(NET_StreamSocket* client) {};

protected: 
	NetServer* m_server = nullptr;
};