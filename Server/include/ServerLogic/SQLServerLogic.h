#pragma once
#include "IServerLogic.h"

namespace sql {
	class Connection;
}

struct DBConfig {
	std::string host;
	uint16_t port = 0;
	std::string user;
	std::string password;
	std::string schema;
};

class SQLServerLogic : public IServerLogic {
public:
	SQLServerLogic(const DBConfig& config);

	void OnRun() override;
	void OnMessage(NET_StreamSocket* client, const std::string& msg) override;

private:
	bool m_connected = false;
	DBConfig m_config;
	std::unique_ptr<sql::Connection> m_connection;

	void Connect();
	bool RunQuery(const std::string& query);
};