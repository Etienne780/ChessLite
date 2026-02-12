#pragma once
#include <string>
#include <SDL3_net/SDL_net.h>

class IServerLogic {
public:
	virtual ~IServerLogic() = default;
	virtual void OnClientConnected(NET_StreamSocket* client) {};
	virtual void OnMessage(NET_StreamSocket* client, const std::string& msg) {};
	virtual void OnClientDisconnected(NET_StreamSocket* client) {};
};