#include <memory>
#include <iostream>
#include <stdexcept>

#include "ServerLogic/SQLServerLogic.h"

SQLServerLogic::SQLServerLogic(const DBConfig& config) 
    : m_config(config) {
}

void SQLServerLogic::OnMessage(NET_StreamSocket* client, const std::string& msg) {
    if (!m_connected) {
        Connect();
    }

    if (!m_connected) {
        std::cerr << "Not connected to DB, cannot process: " << msg << '\n';
        return;
    }

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

template<>
void SQLServerLogic::BindParam<std::string>(
    sql::PreparedStatement* stmt,
    int index,
    const std::string* value
) {
    if (!value) {
        stmt->setNull(index, sql::DataType::VARCHAR);
        return;
    }

    stmt->setString(index, *value);
}

template<>
void SQLServerLogic::BindParam<int>(
    sql::PreparedStatement* stmt,
    int index,
    const int* value
) {
    if (!value) {
        stmt->setNull(index, sql::DataType::INTEGER);
        return;
    }

    stmt->setInt(index, *value);
}

template<>
void SQLServerLogic::BindParam<int64_t>(
    sql::PreparedStatement* stmt,
    int index,
    const int64_t* value
) {
    if (!value) {
        stmt->setNull(index, sql::DataType::BIGINT);
        return;
    }

    stmt->setInt64(index, *value);
}

template<>
void SQLServerLogic::BindParam<double>(
    sql::PreparedStatement* stmt,
    int index,
    const double* value
) {
    if (!value) {
        stmt->setNull(index, sql::DataType::DOUBLE);
        return;
    }

    stmt->setDouble(index, *value);
}

template<>
void SQLServerLogic::BindParam<bool>(
    sql::PreparedStatement* stmt,
    int index,
    const bool* value
) {
    if (!value) {
        stmt->setNull(index, sql::DataType::BIT);
        return;
    }

    stmt->setBoolean(index, *value);
}