#pragma once
#include <string>
#include <deque>
#include <functional>
#include <SDL3_net/SDL_net.h>
#include "Type.h"

class App;
class  GameClient {
friend App;
public:
	using Callback = std::function<void(bool, const std::string&)>;

	GameClient();
	~GameClient();

	bool Connect(const std::string& host, uint16_t port);
	void Disconnect();

	void Send(const std::string& msg, Callback&& callback);
	void ClearError();

	bool IsConnected() const;

	const std::string& GetError() const;
	const std::string& GetHost() const;
	uint16_t GetPort() const;

private:
	struct PendingSend {
		std::string data;
		size_t offset;
		NetworkMsgID id;
	};

	std::string m_host;
	uint16_t m_port = 0;
	CoreAppIDManager m_idManager;
	
	std::string m_error;
	NET_StreamSocket* m_socket = nullptr;

	std::deque<PendingSend> m_msgQueue;
	std::unordered_map<NetworkMsgID, Callback> m_pending;
	std::vector<uint8_t> m_receiveBuffer;

	bool ProcessSendQueue();
	bool ProcessReceiveQueue();

	void AddError(const std::string& msg);
};