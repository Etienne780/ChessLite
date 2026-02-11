#include "NetworkSystem.h"

#include <iostream>

bool NetworkSystem::Init() {
	if (s_isInitialized)
		return true;

	if (!SDL_Init(SDL_INIT_EVENTS)) {
		std::cerr << "SDL init failed: " << SDL_GetError() << "\n";
		return false;
	}

	if (!NET_Init()) {
		std::cerr << "SDL_Net init failed: " << SDL_GetError() << "\n";
		return false;
	}

	s_bindAddress = nullptr;
	s_isInitialized = true;
	return true;
}

void NetworkSystem::Quit() {	
	if (!s_isInitialized)
		return;

	NET_Quit();
	SDL_Quit();
}

bool NetworkSystem::IsInitialized() {
	return s_isInitialized;
}

NET_Address* NetworkSystem::GetBindAddress() {
	return s_bindAddress;
}