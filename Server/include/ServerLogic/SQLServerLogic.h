#pragma once
#include <optional>
#include <tuple>
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
    struct TransactionGuard {
        sql::Connection* conn;
        bool committed = false;

        TransactionGuard(sql::Connection* c)
            : conn(c) {
            conn->setAutoCommit(false);
        }

        void commit() {
            conn->commit();
            committed = true;
        }

        ~TransactionGuard() {
            if (!committed)
                conn->rollback();
            conn->setAutoCommit(true);
        }
    };

	bool m_connected = false;
	DBConfig m_config;
	std::unique_ptr<sql::Connection> m_connection;

    void HandleInsertAgents(const std::string& dstServer, uint32_t requestID, OTN::OTNReader& reader);
    void HandleGetAgentIDs(const std::string& dstServer, uint32_t requestID, OTN::OTNReader& reader);
    void HandleGetMissinAgents(const std::string& dstServer, uint32_t requestID, OTN::OTNReader& reader);
    void HandleDeleteAgents(const std::string& dstServer, uint32_t requestID, OTN::OTNReader& reader);
    void HandleUpdateAgents(const std::string& dstServer, uint32_t requestID, OTN::OTNReader& reader);

    OTN::OTNObject CreateRequestHeader(const std::string& action, uint32_t requestID, bool response = true);
    void SentError(const std::string& errorMsg, const std::string dstServer, const std::string& action, uint32_t requestID);

	void Connect();

    std::optional<int64_t> SQLServerLogic::FetchLastInsertID();

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
    * @return std::optional<OTN::OTNObject> (OTNObject name Result) containing column names if successful, std::nullopt on error.
    */
    template<typename... Args>
    std::optional<OTN::OTNObject> FetchStatement(const std::string& query, Args&&... args) {
        try {
            std::unique_ptr<sql::PreparedStatement> stmt(m_connection->prepareStatement(query));

            int index = 1;
            (void)std::initializer_list<int>{(index = BindParam(stmt.get(), index, args), 0)...};

            std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
            sql::ResultSetMetaData* metaData = res->getMetaData();

            if (!metaData) {
                std::cerr << "Query error: metadata is nullptr for query: " << query << "\n";
                return std::nullopt;
            }

            uint32_t columnCount = metaData->getColumnCount();

            std::vector<std::string> columnNames;
            std::vector<int> columnTypes;
            columnNames.reserve(columnCount);
            columnTypes.reserve(columnCount);

            for (uint32_t i = 1; i <= columnCount; ++i) {
                std::string name = metaData->getColumnName(i);
                if (name.empty())
                    name = "col" + std::to_string(i);
                columnNames.push_back(name);
                columnTypes.push_back(metaData->getColumnType(i));
            }

            OTN::OTNObject obj("Result");
            obj.SetNamesList(columnNames);
            obj.SetTypesList(GetTypesFromColumns(columnTypes));

            while (res->next()) {
                OTN::OTNRow rowValues;
                rowValues.reserve(columnCount);

                for (uint32_t i = 0; i < columnCount; ++i) {
                    switch (columnTypes[i]) {
                    case sql::DataType::INTEGER:
                        rowValues.emplace_back(static_cast<int>(res->getInt(i + 1)));
                        break;
                    case sql::DataType::BIGINT:
                        rowValues.emplace_back(static_cast<int64_t>(res->getInt64(i + 1)));
                        break;
                    case sql::DataType::DOUBLE:
                        rowValues.emplace_back(static_cast<double>(res->getDouble(i + 1)));
                        break;
                    case sql::DataType::DECIMAL:
                    case sql::DataType::REAL:
                        rowValues.emplace_back(static_cast<float>(res->getDouble(i + 1)));
                        break;
                    case sql::DataType::BIT:
                        rowValues.emplace_back(res->getBoolean(i + 1));
                        break;
                    case sql::DataType::CHAR:
                    case sql::DataType::LONGVARCHAR:
                    case sql::DataType::VARCHAR:
                        rowValues.emplace_back(static_cast<std::string>(res->getString(i + 1)));
                        break;
                    default:
                        std::cerr << "SQL Invalid data type '" << columnTypes[i] << "'\n";
                        break;
                    }
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
    
    /**
    * @brief Executes a raw SQL query without parameters.
    *
    * This overload is intended for simple SQL statements that do not require
    * parameter binding (e.g. static queries without user input).
    *
    * @param query The SQL query string to execute.
    * @return true if the query executed successfully, false otherwise.
    */
	void ExecuteStatement(const std::string& query);

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
    void ExecuteStatement(const std::string& query, Args&&... args) {
        try {
            std::unique_ptr<sql::PreparedStatement> stmt(
                m_connection->prepareStatement(query)
            );

            int index = 1;
            (void)std::initializer_list<int>{(index = BindParam(stmt.get(), index, args), 0)...};

            stmt->execute();
        }
        catch (sql::SQLException& e) {
            std::cerr << "Query error: " << e.what() << '\n';
            throw;
        }
    }

    template<typename... Args>
    void ExecuteBatchInsert(
        const std::string& tableName, 
        const std::vector<std::tuple<Args...>>& rows,
        const std::string& columns)
    {
        if (rows.empty())
            throw sql::SQLException("Failed to ExecuteBatchInsert, the given rows where empty!");

        if (rows.size() > 1000) {
            size_t half = rows.size() / 2;
            std::vector<std::tuple<Args...>> row1(rows.begin(), rows.begin() + half);
            std::vector<std::tuple<Args...>> row2(rows.begin() + half, rows.end());

            ExecuteBatchInsert(tableName, row1, columns);
            ExecuteBatchInsert(tableName, row2, columns);
            return;
        }

        std::string query = "INSERT INTO " + tableName + " (" + columns + ") VALUES ";
        size_t numCols = std::tuple_size<std::tuple<Args...>>::value;

        for (size_t i = 0; i < rows.size(); ++i) {
            query += "(";
            for (size_t j = 0; j < numCols; ++j) {
                query += "?";
                if (j + 1 < numCols) 
                    query += ", ";
            }
            query += ")";
            if (i + 1 < rows.size()) 
                query += ", ";
        }

        try {
            std::unique_ptr<sql::PreparedStatement> stmt(
                m_connection->prepareStatement(query)
            );

            int index = 1;
            for (const auto& row : rows) {
                std::apply([&](auto&&... args) {
                    (void)std::initializer_list<int>{(index = BindParam(stmt.get(), index, args), 0)...};
                }, row);
            }

            stmt->execute();
        }
        catch (sql::SQLException& e) {
            std::cerr << "BatchInsert error: " << e.what() << "\n";
            throw;
        }
    }

    std::vector<std::string> GetTypesFromColumns(const std::vector<int>& types) {
        std::vector<std::string> result;
        result.reserve(types.size());

        namespace OTNTypes = OTN::Syntax::Types;

        for (int t : types) {
            switch (t) {
            case sql::DataType::INTEGER:
                result.emplace_back(OTNTypes::INT);
                break;

            case sql::DataType::BIGINT:
                result.emplace_back(OTNTypes::INT64);
                break;

            case sql::DataType::DOUBLE:
                result.emplace_back(OTNTypes::DOUBLE);
                break;

            case sql::DataType::DECIMAL:
            case sql::DataType::REAL:
                result.emplace_back(OTNTypes::FLOAT);
                break;

            case sql::DataType::BIT:
                result.emplace_back(OTNTypes::BOOL);
                break;

            case sql::DataType::CHAR:
            case sql::DataType::LONGVARCHAR:
            case sql::DataType::VARCHAR:
                result.emplace_back(OTNTypes::STRING);
                break;

            default:
                result.emplace_back(OTNTypes::STRING);
                break;
            }
        }

        return result;
    }

    template<typename T>
    int BindParam(sql::PreparedStatement* stmt,
        int index,
        const std::vector<T>& values)
    {
        for (const auto& v : values) {
            index = BindParam(stmt, index, v);
        }

        return index;
    }

    int BindParam(sql::PreparedStatement* stmt, int index, const std::optional<std::string>& value);
    int BindParam(sql::PreparedStatement* stmt, int index, const std::optional<int>& value);
    int BindParam(sql::PreparedStatement* stmt, int index, const std::optional<int64_t>& value);
    int BindParam(sql::PreparedStatement* stmt, int index, const std::optional<double>& value);
    int BindParam(sql::PreparedStatement* stmt, int index, const std::optional<bool>& value);

    inline int BindParam(sql::PreparedStatement* stmt, int index, int value) {
        return BindParam(stmt, index, std::optional<int>{value});
    }
    inline int BindParam(sql::PreparedStatement* stmt, int index, int64_t value) {
        return BindParam(stmt, index, std::optional<int64_t>{value});
    }
    inline int BindParam(sql::PreparedStatement* stmt, int index, double value) {
        return BindParam(stmt, index, std::optional<double>{value});
    }
    inline int BindParam(sql::PreparedStatement* stmt, int index, bool value) {
        return BindParam(stmt, index, std::optional<bool>{value});
    }
    inline int BindParam(sql::PreparedStatement* stmt, int index, const std::string& value) {
        return BindParam(stmt, index, std::optional<std::string>{value});
    }
};