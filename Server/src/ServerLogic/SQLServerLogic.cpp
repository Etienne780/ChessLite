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
    if (!m_connected)
        Connect();
    
    OTN::OTNReader reader;
    if (!reader.ReadString(msg))
        return;

    auto header = reader.TryGetObject("header");
    if (!header)
        return;

    auto action = header->TryGetValue<std::string>(0, "action");
    auto requestID = header->TryGetValue<int64_t>(0, "request_id");

    if (!action || !requestID)
        return;

    if (*action == "InsertAgents")
        HandleInsertAgents(serverName, static_cast<uint32_t>(*requestID), reader);
}

void SQLServerLogic::HandleInsertAgents(
    const std::string& dstServer,
    uint32_t requestID,
    OTN::OTNReader& reader)
{
    if (!m_connection) {
        std::cerr << "Not connected to DB\n";
        //das schelcht muss was returnen
        return;
    }

    OTN::OTNObject result{ "ids" };
    result.SetNames("localID", "serverID");
    result.SetTypes("int64", "int64");

    try {
        m_connection->setAutoCommit(false);

        auto obj = reader.TryGetObject("body");
        if (!obj) 
            return;

        for (size_t i = 0; i < obj->GetRowCount(); ++i) {
            auto localID = obj->TryGetValue<int64_t>(i, "local_id");
            auto name = obj->TryGetValue<std::string>(i, "name");
            auto config = obj->TryGetValue<std::string>(i, "config");
            auto matches_played = obj->TryGetValue<int>(i, "matches_played");
            auto matches_won = obj->TryGetValue<int>(i, "matches_won");
            auto matches_played_white = obj->TryGetValue<int>(i, "matches_played_white");
            auto matches_won_white = obj->TryGetValue<int>(i, "matches_won_white");

            if (!localID || !name || !config)
                continue;

            ExecuteStatement(R"""(
                INSERT INTO agents 
                (name, config, version, matches_played, matches_won, matches_played_white, matches_won_white)
                VALUES (?, ?, ?, ?, ?, ?, ?);)""",
                *name, *config, 0, 
                (matches_played ? *matches_played : 0),
                (matches_won ? *matches_won : 0),
                (matches_played_white ? *matches_played_white : 0),
                (matches_won_white ? *matches_won_white : 0)
                );
            auto currentAgentID = FetchLastInsertID();

            if (!currentAgentID)
                continue;

            result.AddDataRow(*localID, *currentAgentID);

            auto boardStates = obj->TryGetValue<std::vector<OTN::OTNObject>>(i, "board_states");
            if (!boardStates || boardStates->empty())
                continue;

            for (auto& bs : *boardStates) {
                auto stateStr = bs.TryGetValue<std::string>(0, "board_state");
                auto moves = bs.TryGetValue<std::vector<OTN::OTNObject>>(0, "moves");

                if (!stateStr || !moves)
                    continue;

                ExecuteStatement(R"""(
                INSERT INTO board_states 
                (agent_id, board_state)
                VALUES (?, ?);)""",
                    *currentAgentID, *stateStr
                );
                auto currentBoardStateID = FetchLastInsertID();
                if (!currentBoardStateID)
                    continue;

                std::vector<std::tuple<int64_t, float, float, float, float, float>> gameMoves;

                for (auto& move : *moves) {
                    auto eval = move.TryGetValue<float>(0, "eval");
                    auto fromX = move.TryGetValue<float>(0, "from_x");
                    auto fromY = move.TryGetValue<float>(0, "from_y");
                    auto toX = move.TryGetValue<float>(0, "to_x");
                    auto toY = move.TryGetValue<float>(0, "to_y");

                    if (!eval || !fromX || !fromY || !toX || !toY)
                        continue;

                    gameMoves.emplace_back(*currentBoardStateID, *eval, *fromX, *fromY, *toX, *toY);
                }

                ExecuteBatchInsert("game_moves", gameMoves, 
                    "board_state_id, evaluation, from_x, from_y, to_x, to_y");
            }
        }
        
        m_connection->commit();
    }
    catch (...) {
        m_connection->rollback();
        throw;
    }

    m_connection->setAutoCommit(true);

    OTN::OTNObject header{ "header" };
    header.SetNames("action", "request_id");
    header.SetTypes("String", "int64");
    header.AddDataRow("InsertAgentsResult", requestID);

    OTN::OTNWriter writer;
    writer.AppendObject(header);
    writer.AppendObject(result);

    std::string output;
    writer.SaveToString(output);

    NetServerManager::SendMessage(
        m_server,
        dstServer,
        output
    );
}

void SQLServerLogic::Connect() {
    m_connected = false;

    try {
       std::string hostString = "tcp://" + m_config.host + ":" + std::to_string(m_config.port);
       std::cout << "Connecting to: " << hostString << std::endl;

        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> conn(
            driver->connect(hostString, "root", "root")
        );

        conn->setSchema(m_config.schema);

        m_connection = std::move(conn); 
        std::cout << "Connected to MySQL successfully to schema '" << m_config.schema << "'!" << '\n';
        m_connected = true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error connecting to MySQL: "
            << e.what() << " (MySQL error code: "
            << e.getErrorCode() << ")" << '\n';
    }
}

std::optional<OTN::OTNObject> SQLServerLogic::FetchStatement(const std::string& query) {
    if (!m_connection) {
        std::cerr << "Query error: not connected\n";
        return std::nullopt;
    }
    try {
        sql::Statement* stmt = m_connection->createStatement();
        sql::ResultSet* res = stmt->executeQuery(query);
        sql::ResultSetMetaData* metaData = res->getMetaData();

        if (!metaData) {
            std::cerr << "Query error: metadata is nullptr for query: " << query << "\n";
            delete res;
            delete stmt;
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
                case sql::DataType::BIT:
                    rowValues.emplace_back(res->getBoolean(i + 1));
                    break;
                case sql::DataType::VARCHAR: {
                    std::string strValue(res->getString(i + 1).c_str());
                    rowValues.emplace_back(strValue);
                    break;
                }
                default:
                    rowValues.emplace_back();
                    break;
                }
            }
            obj.AddDataRowList(rowValues);
        }

        delete res;
        delete stmt;

        return obj;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Query error: " << e.what() << "\n";
        return std::nullopt;
    }
}

std::optional<int64_t> SQLServerLogic::FetchLastInsertID() {
    auto result = FetchStatement("SELECT LAST_INSERT_ID();");
    if (!result) 
        return std::nullopt;

    auto& obj = *result;
    if (obj.GetRowCount() == 0 || obj.GetRow(0).empty())
        return std::nullopt;

    return result->TryGetValue<int64_t>(0, 0);
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
static inline void BindParamHelper(
    sql::PreparedStatement* stmt,
    int index,
    const std::optional<T>& value,
    int sqlType,
    Setter setter)
{
    if (!value) {
        stmt->setNull(index, sqlType);
        return;
    }

    (stmt->*setter)(index, *value);
}

void SQLServerLogic::BindParam(
    sql::PreparedStatement* stmt,
    int index,
    const std::optional<std::string>& value)
{
    BindParamHelper(stmt, index, value, sql::DataType::VARCHAR, &sql::PreparedStatement::setString);
}

void SQLServerLogic::BindParam(
    sql::PreparedStatement* stmt,
    int index,
    const std::optional<int>& value)
{
    BindParamHelper(stmt, index, value, sql::DataType::INTEGER, &sql::PreparedStatement::setInt);
}

void SQLServerLogic::BindParam(
    sql::PreparedStatement* stmt,
    int index,
    const std::optional<int64_t>& value)
{
    BindParamHelper(stmt, index, value, sql::DataType::BIGINT, &sql::PreparedStatement::setInt64);
}

void SQLServerLogic::BindParam(
    sql::PreparedStatement* stmt,
    int index,
    const  std::optional<double>& value)
{
    BindParamHelper(stmt, index, value, sql::DataType::DOUBLE, &sql::PreparedStatement::setDouble);
}

void SQLServerLogic::BindParam(
    sql::PreparedStatement* stmt,
    int index,
    const std::optional<bool>& value)
{
    BindParamHelper(stmt, index, value, sql::DataType::BIT, &sql::PreparedStatement::setBoolean);
}