#pragma once
#include <string>
#include <deque>
#include <functional>
#include <SDL3_net/SDL_net.h>
#include <CoreLib/OTNFile.h>
#include "Type.h"

class App;
class  GameClient {
friend App;
public:
	using Callback = std::function<void(bool, const std::string&)>;
	using GlobalCallback = std::function<void(const std::string&)>;

	GameClient();
	~GameClient();

	bool Connect(const std::string& host, uint16_t port);
	void Disconnect();

	void Send(const std::string& msg, Callback&& callback);

	NetworkCallbackID AddGlobalCallback(GlobalCallback&& cb);
	bool RemoveGlobalCallback(NetworkCallbackID id);

	void ClearError();

	bool IsConnected() const;

	OTN::OTNObject CreateHeaderBlock(const std::string& action);

	const std::string& GetError() const;
	const std::string& GetHost() const;
	uint16_t GetPort() const;

private:
	struct PendingSend {
		NetworkMsgID id;
		std::vector<uint8_t> data;
	};

	std::string m_host;
	uint16_t m_port = 0;
	CoreAppIDManager m_idManager{ 1 };
	CoreAppIDManager m_callbackIDManager{ 1 };
	
	std::string m_error;
	NET_StreamSocket* m_socket = nullptr;

	std::deque<PendingSend> m_msgQueue;
	std::unordered_map<NetworkMsgID, Callback> m_pending;
	std::vector<uint8_t> m_receiveBuffer;

	std::unordered_map<NetworkCallbackID, GlobalCallback> m_globalCallbacks;

	bool ProcessSendQueue();
	bool ProcessReceiveQueue();

	void CallRequestCallback(NetworkMsgID id, bool result, const std::string& msg);
	void AddError(const std::string& msg);

	void CallGlobalCallBacks(const std::string& msg);
};