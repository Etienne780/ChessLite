#pragma once
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

/*
* @brief Manges SDL3_Net life time
*/
class NetworkSystem {
public:
	NetworkSystem() = delete;
	NetworkSystem(const NetworkSystem&) = delete;
	void operator=(const NetworkSystem&) = delete;

	static bool Init();
	static void Quit();

	static bool IsInitialized();

	static NET_Address* GetBindAddress();

private:
	static inline bool s_isInitialized = false;
	static inline NET_Address* s_bindAddress = nullptr;
};