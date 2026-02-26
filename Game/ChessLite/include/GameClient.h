#pragma once
#include <string>
#include <SDL3_net/SDL_net.h>

class  GameClient {
public:
	GameClient();
	~GameClient();

	bool Connect(const std::string& host, uint16_t port);
	void Disconnect();

	bool Send(const std::string& msg);
	bool Receive(std::string& out);
	void ClearError();

	bool IsConnected() const;

	const std::string& GetError() const;
	const std::string& GetHost() const;
	uint16_t GetPort() const;

private:
	std::string m_host;
	uint16_t m_port = 0;
	
	std::string m_error;
	NET_StreamSocket* m_socket = nullptr;

	void AddError(const std::string& msg);
};