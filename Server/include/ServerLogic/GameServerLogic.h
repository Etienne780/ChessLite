#pragma once
#include "IServerLogic.h"

class GameServerLogic : public IServerLogic {
public:
	void OnMessage(NET_StreamSocket* client, const std::string& msg) override;
};