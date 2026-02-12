#pragma once
#include <string>
#include <atomic>

#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

#include "IServerLogic.h"

class NetServerManager;

class NetServer {
friend class NetServerManager;
public:
	~NetServer();

	bool Start(uint16_t port);
	void Stop();

	void Run();

	bool IsInitialized() const;
	bool IsRunning() const;
	const std::string& GetName() const;
	uint16_t GetPort() const;

	template<typename T, typename ...Args>
	void SetLogic(Args&& ...args) {
		static_assert(std::is_base_of_v<IServerLogic, T>, "T musst have IServerLogic as base class!");
		FreeServerLogic();
		m_logic = new T(std::forward<Args>(args)...);
	}

private:
	std::string m_name;
	uint16_t m_port = 0;
	NET_Server* m_server = nullptr;
	bool m_isInitialized = false;
	std::atomic<bool> m_running = false;

	IServerLogic* m_logic = nullptr;

	NetServer(const std::string& name);

	void HandleClient(NET_StreamSocket* client) const;
	NET_StreamSocket* WaitForClient();
	void FreeServerLogic();
};