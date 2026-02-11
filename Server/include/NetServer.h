#pragma once
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

class NetServerManager;

class NetServer {
friend class NetServerManager;
public:
	~NetServer() = default;

	bool Start(uint16_t port);
	void Stop();

	void Run();

	bool IsInitialized() const;
	bool IsRunning() const;
	const std::string& GetName() const;

private:
	std::string m_name;
	uint16_t m_port = 0;
	NET_Server* m_server = nullptr;
	bool m_isInitialized = false;
	bool m_running = false;

	NetServer(const std::string& name);

	void HandleClient(NET_StreamSocket* client) const;
	NET_StreamSocket* WaitForClient();
};