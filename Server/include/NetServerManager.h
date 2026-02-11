#pragma once
#include <vector>
#include "NetServer.h"

class NetServerManager {
public:
	NetServerManager() = delete;
	NetServerManager(const NetServerManager&) = delete;
	void operator=(const NetServerManager&) = delete;

	static NetServer* CreateServer(const std::string& name);
	static bool DestroyServer(NetServer* server);

	static void StartAll();
	static void StopAll();

private:
	static inline std::vector<NetServer*> m_serverList;
};