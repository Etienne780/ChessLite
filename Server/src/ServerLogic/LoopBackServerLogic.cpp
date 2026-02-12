#include <SDL3_net/SDL_Net.h>

#include "ServerLogic/LoopBackServerLogic.h"

void LoopBackServerLogic::OnMessage(NET_StreamSocket* client, const std::string& msg) {
	if (!client)
		return;
	
	NET_WriteToStreamSocket(client, msg.c_str(), static_cast<int>(msg.size()));
}