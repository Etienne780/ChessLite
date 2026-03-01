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
	std::unordered_map<int64_t, PendingSQLRequest> m_pendingSQL;
	int64_t m_nextInternalID = 1;

	int64_t RegisterPendingSQL(NET_StreamSocket* client, uint32_t requestID);
	bool RemovePendingSQL(int64_t requstID, PendingSQLRequest& outRequest);

	void HandleSQLServer(const std::string& msg);
	void HandleReceiveSQLSyncMissingData(OTN::OTNReader& reader, const PendingSQLRequest& pending);
	void HandleReceiveSQLServerAgentIDList(OTN::OTNReader& reader, const PendingSQLRequest& pending);
	void HandleReceiveSQLServerAgentList(OTN::OTNReader& reader, const PendingSQLRequest& pending);
	void HandleReceiveSQLDeleteAgentsResult(OTN::OTNReader& reader, const PendingSQLRequest& pending);
	void HandleReceiveSQLUpdateAgentsResult(OTN::OTNReader& reader, const PendingSQLRequest& pending);

	void HandleRequest(NET_StreamSocket* client, const Request& req);
	void HandleSyncMissingData(NET_StreamSocket* client, uint32_t requestID, const OTN::OTNObject& body);
	void HandleServerAgentIDList(NET_StreamSocket* client, uint32_t requestID);
	void HandleGetMissinAgents(NET_StreamSocket* client, uint32_t requestID, const OTN::OTNObject& body);
	void HandleDeleteAgents(NET_StreamSocket* client, uint32_t requestID, const OTN::OTNObject& body);
	void HandleDirtyAgents(NET_StreamSocket* client, uint32_t requestID, const OTN::OTNObject& body);

	OTN::OTNObject CreateSQLRequestHeader(const std::string& action, NET_StreamSocket* client, uint32_t requestID);

	std::vector<GameServerLogic::Request> GetRequests(const std::string& msg);
	bool SentRequest(NET_StreamSocket* client, const Request& request);
	void SentError(NET_StreamSocket* client, uint32_t requestID, const std::string& errorMsg);
};