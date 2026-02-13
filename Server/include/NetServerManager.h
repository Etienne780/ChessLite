#pragma once
#include <vector>
#include <thread>
#include "NetServer.h"

class NetServerManager {
public:
	NetServerManager() = delete;
	NetServerManager(const NetServerManager&) = delete;
	void operator=(const NetServerManager&) = delete;

	static NetServer* CreateServer(const std::string& name);
	static bool DestroyServer(NetServer* server);

	static void SendMessage(const std::string& serverName, const std::string& msg);

	static void StartAll();
	static void StopAll();

private:
	static inline std::vector<std::thread> m_threads;
	static inline std::vector<NetServer*> m_serverList;
};