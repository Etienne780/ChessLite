#pragma once
#include "IServerLogic.h"

constexpr size_t MAX_PACKET_SIZE = 5000;

class GameServerLogic : public IServerLogic {
public:
	GameServerLogic(NetServer* server);

	void OnMessage(NET_StreamSocket* client, const std::string& msg) override;
	void OnServerMessage(const std::string& serverName, const std::string& msg) override;

private:
	struct Request {
		uint32_t id = 0;
		bool response = false;
		std::string otnPayload;
	};

	struct PendingSQLRequest {
		NET_StreamSocket* client = nullptr;
		uint32_t clientRequestID = 0;
	};

	std::vector<uint8_t> m_receiveBuffer;
	std::unordered_map<uint32_t, PendingSQLRequest> m_pendingSQL;
	uint32_t m_nextInternalID = 1;

	void HandleRequest(NET_StreamSocket* client, const Request& req);
	void HandleSyncMissingData(NET_StreamSocket* client, uint32_t requestID, const OTN::OTNObject& body);

	Request GetRequest(const std::string& msg);
	bool SentRequest(NET_StreamSocket* client, const Request& request);
	void SentError(NET_StreamSocket* client, uint32_t requestID, const std::string& errorMsg);
};