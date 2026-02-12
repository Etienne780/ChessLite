#pragma once
#include "IServerLogic.h"

class LoopBackServerLogic : public IServerLogic {
public:
	void OnMessage(NET_StreamSocket* client, const std::string& msg) override;
};