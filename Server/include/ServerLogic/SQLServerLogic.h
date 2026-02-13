#pragma once
#include <optional>
#include <mysql/jdbc.h>

#include "OTNFile.h"
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
	SQLServerLogic(NetServer* server, const DBConfig& config);

	void OnMessage(NET_StreamSocket* client, const std::string& msg) override;
    void OnServerMessage(const std::string& serverName, const std::string& msg) override;

private:
	bool m_connected = false;
	DBConfig m_config;
	std::unique_ptr<sql::Connection> m_connection;

	void Connect();


    /**
    * @brief Executes a raw SQL query without parameters and fetches the column structure.
    *
    * This function is intended for simple queries that do not require parameter binding.
    * It executes the query and constructs an OTNObject containing the column names.
    *
    * @param query The SQL query string to execute.
    * @return std::optional<OTN::OTNObject> containing column names if successful, std::nullopt on error.
    */
    std::optional<OTN::OTNObject> FetchStatment(const std::string& query);

    /**
    * @brief Executes a parameterized SQL query and fetches results as an OTNObject.
    *
    * This function prepares a statement, binds the provided parameters, executes
    * the query, and constructs an OTNObject containing the resulting column names.
    * Note: Rows are not yet added; only the column structure is initialized.
    *
    * @tparam Args Variadic template parameter types for binding.
    * @param query The SQL query string containing '?' placeholders for parameters.
    * @param args  Values to bind to the prepared statement in order.
    * @return std::optional<OTN::OTNObject> containing column names if successful, std::nullopt on error.
    */
    template<typename... Args>
    std::optional<OTN::OTNObject> FetchStatement(const std::string& query, Args&&... args) {
        if (!m_connection) {
            std::cerr << "Query error: not connected\n";
            return std::nullopt;
        }

        try {
            std::unique_ptr<sql::PreparedStatement> stmt(m_connection->prepareStatement(query));

            int index = 1;
            (BindParam(stmt.get(), index++, args), ...);

            std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
            sql::ResultSetMetaData* metaData = res->getMetaData();

            if (!metaData) {
                std::cerr << "Query error: metadata is nullptr for query: " << query << "\n";
                return std::nullopt;
            }

            size_t columnCount = static_cast<size_t>(metaData->getColumnCount());
            std::vector<std::string> columnNames;
            columnNames.reserve(columnCount);
            for (size_t i = 1; i <= columnCount; i++) {
                columnNames.push_back(metaData->getColumnName(i));
            }

            OTN::OTNObject obj("Result");
            obj.SetNamesList(columnNames);

        

            return obj;
        }
        catch (sql::SQLException& e) {
            std::cerr << "Query error: " << e.what() << "\n";
            return std::nullopt;
        }
    }
    
    /**
    * @brief Executes a raw SQL query without parameters.
    *
    * This overload is intended for simple SQL statements that do not require
    * parameter binding (e.g. static queries without user input).
    *
    * @param query The SQL query string to execute.
    * @return true if the query executed successfully, false otherwise.
    */
	bool ExecuteStatement(const std::string& query);

    /**
    * @brief Executes a parameterized SQL query using a prepared statement.
    *
    * This overload creates a sql::PreparedStatement, binds the provided
    * parameters in order to the positional placeholders ('?') in the query,
    * and executes the statement.
    *
    * The number and order of arguments must match the number and order of
    * placeholders in the SQL string. Binding is performed sequentially
    * starting at index 1, as required by MySQL Connector/C++.
    *
    * note: This function requires an active database connection.
    * warning: The number of arguments must match the number of placeholders.
    * 
    * example:
    * RunQuery(
    *     "INSERT INTO players (name, rating) VALUES (?, ?)",
    *     name,
    *     rating
    * );
    * 
    * @tparam Args Variadic template parameter types for binding.
    * @param query The SQL query string containing '?' placeholders.
    * @param args  Values to bind to the prepared statement.
    *
    * @return true if the query executed successfully, false otherwise.
    */
    template<typename... Args>
    bool ExecuteStatement(const std::string& query, Args&&... args) {
        if (!m_connection) {
            std::cerr << "Query error: not connected\n";
            return false;
        }

        try {
            std::unique_ptr<sql::PreparedStatement> stmt(
                m_connection->prepareStatement(query)
            );

            int index = 1;
            (BindParam(stmt.get(), index++, args), ...);

            stmt->execute();
            return true;
        }
        catch (sql::SQLException& e) {
            std::cerr << "Query error: " << e.what() << '\n';
            return false;
        }
    }

	template<typename T>
	void BindParam(sql::PreparedStatement* stmt, int index, const T* value);
};