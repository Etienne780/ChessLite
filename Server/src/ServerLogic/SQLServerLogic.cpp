#include <memory>
#include <iostream>
#include <stdexcept>

#include "NetServerManager.h"
#include "ServerLogic/SQLServerLogic.h"

SQLServerLogic::SQLServerLogic(NetServer* server, const DBConfig& config)
    : m_config(config), IServerLogic(server){
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

void SQLServerLogic::OnServerMessage(const std::string& serverName, const std::string& msg) {
    if (!m_connected) {
        Connect();
    }

    if (!m_connected) {
        std::cerr << "Not connected to DB, cannot process: " << msg << '\n';
        return;
    }
    
    if (msg == "sql_test") {
        FetchStatement("SELECT * FROM testGame");

        std::string output;
        OTN::OTNWriter writer;
        if (!writer.SaveToString(output)) {
            output = "failed";
        }

        NetServerManager::SendMessage(m_server, serverName, output);
    }

}

void SQLServerLogic::Connect() {
    m_connected = false;

    try {
        std::string hostString = "tcp://" + std::string(m_config.host) + ":" + std::to_string(m_config.port);
        std::cout << "Connecting to: " << hostString << std::endl;

        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> conn(
            driver->connect(hostString, m_config.user, m_config.password)
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

std::optional<OTN::OTNObject> SQLServerLogic::FetchStatment(const std::string& query) {
    if (!m_connection) {
        std::cerr << "Query error: not connected\n";
        return std::nullopt;
    }

    try {
        std::unique_ptr<sql::Statement> stmt(m_connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        sql::ResultSetMetaData* metaData = res->getMetaData();

        if (!metaData) {
            std::cerr << "Query error: metadata is nullptr for query: " << query << "\n";
            return std::nullopt;
        }

        uint32_t columnCount = metaData->getColumnCount();
        std::vector<std::string> columnNames;
        columnNames.reserve(columnCount);
        for (uint32_t i = 1; i <= columnCount; i++) {
            columnNames.push_back(metaData->getColumnName(i));
        }

        OTN::OTNObject obj("Result");
        obj.SetNamesList(columnNames);

        while (res->next()) {
            OTN::OTNRow  rowValues;
            rowValues.reserve(columnCount);

            while (res->next()) {
                OTN::OTNRow rowValues;
                rowValues.reserve(columnCount);

                for (uint32_t i = 1; i <= columnCount; ++i) {
                    switch (metaData->getColumnType(i)) {
                    case sql::DataType::INTEGER:
                        rowValues.emplace_back(static_cast<int>(res->getInt(i)));
                        break;

                    case sql::DataType::BIGINT:
                        rowValues.emplace_back(static_cast<int64_t>(res->getInt64(i)));
                        break;

                    case sql::DataType::DOUBLE:
                        rowValues.emplace_back(static_cast<double>(res->getDouble(i)));
                        break;
                    
                    case sql::DataType::BIT:
                        rowValues.emplace_back(res->getBoolean(i));
                        break;
                    
                    case sql::DataType::VARCHAR:
                        rowValues.emplace_back(res->getString(i));
                        break;
                    
                    default:
                        rowValues.emplace_back();
                        break;
                    }
                }

                obj.AddDataRowList(rowValues);
            }


            obj.AddDataRowList(rowValues);
        }

        return obj;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Query error: " << e.what() << "\n";
        return std::nullopt;
    }
}

bool SQLServerLogic::ExecuteStatement(const std::string& query) {
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

template<typename T, typename Setter>
void BindParamHelper(
    sql::PreparedStatement* stmt,
    int index,
    const T* value,
    int sqlType,
    Setter setter)
{
    if (!value) {
        stmt->setNull(index, sqlType);
        return;
    }

    (stmt->*setter)(index, *value);
}

template<>
void SQLServerLogic::BindParam<std::string>(
    sql::PreparedStatement* stmt,
    int index,
    const std::string* value)
{
    BindParamHelper(stmt, index, value, sql::DataType::VARCHAR, &sql::PreparedStatement::setString);
}

template<>
void SQLServerLogic::BindParam<int>(
    sql::PreparedStatement* stmt,
    int index,
    const int* value)
{
    BindParamHelper(stmt, index, value, sql::DataType::INTEGER, &sql::PreparedStatement::setInt);
}

template<>
void SQLServerLogic::BindParam<int64_t>(
    sql::PreparedStatement* stmt,
    int index,
    const int64_t* value)
{
    BindParamHelper(stmt, index, value, sql::DataType::BIGINT, &sql::PreparedStatement::setInt64);
}

template<>
void SQLServerLogic::BindParam<double>(
    sql::PreparedStatement* stmt,
    int index,
    const double* value)
{
    BindParamHelper(stmt, index, value, sql::DataType::DOUBLE, &sql::PreparedStatement::setDouble);
}

template<>
void SQLServerLogic::BindParam<bool>(
    sql::PreparedStatement* stmt,
    int index,
    const bool* value)
{
    BindParamHelper(stmt, index, value, sql::DataType::BIT, &sql::PreparedStatement::setBoolean);
}