#include <mysql/jdbc.h>
#include <memory>
#include <iostream>
#include <stdexcept>

#include "ServerLogic/SQLServerLogic.h"

SQLServerLogic::SQLServerLogic(const DBConfig& config) 
    : m_config(config) {
}

void SQLServerLogic::OnRun() {
    if (!m_connected) {
        Connect();
    }
}

void SQLServerLogic::OnMessage(NET_StreamSocket* client, const std::string& msg) {
	
}

void SQLServerLogic::Connect() {
    m_connected = false;

    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();

        std::unique_ptr<sql::Connection> conn(
            driver->connect(
                "tcp://" + m_config.host + ":" + std::to_string(m_config.port), 
                m_config.user, 
                m_config.password
            )
        );

        conn->setSchema(m_config.schema);

        m_connection = std::move(conn); 
        std::cout << "Connected to MySQL successfully!" << '\n';
        m_connected = true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error connecting to MySQL: "
            << e.what() << " (MySQL error code: "
            << e.getErrorCode() << ")" << '\n';
    }
}

bool SQLServerLogic::RunQuery(const std::string& query) {
    if (!m_connection) {
        std::cerr << "Query error: not connected\n";
        return false;
    }

    try {
        std::unique_ptr<sql::Statement> stmt(m_connection->createStatement());
        stmt->execute(query);
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Query error: " << e.what() << '\n';
        return false;
    }
}