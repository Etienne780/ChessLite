#pragma once
#include "IServerLogic.h"

class GameServerLogic : public IServerLogic {
public:
	GameServerLogic(NetServer* server);

	void OnMessage(NET_StreamSocket* client, const std::string& msg) override;
	void OnServerMessage(const std::string& serverName, const std::string& msg) override;
};