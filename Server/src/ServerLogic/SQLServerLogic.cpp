#include <memory>
#include <iostream>
#include <stdexcept>

#include "NetServerManager.h"
#include "ServerLogic/SQLServerLogic.h"

SQLServerLogic::SQLServerLogic(NetServer* server, const DBConfig& config)
    : m_config(config), IServerLogic(server){
}

void SQLServerLogic::OnMessage(NET_StreamSocket* client, const std::string& msg) {
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
    else if(*action == "GetAgentIDs")
        HandleGetAgentIDs(serverName, static_cast<uint32_t>(*requestID), reader);
    else
        std::cerr << "Unkown action sql server '" << *action << "'\n";
}

void SQLServerLogic::HandleInsertAgents(
    const std::string& dstServer,
    uint32_t requestID,
    OTN::OTNReader& reader)
{
    const char* actionName = "InsertAgentsResult";

    if (!m_connection) {
        std::cerr << "Not connected to DB\n";
        SentError("Not connected to DB", dstServer, actionName, requestID);
        return;
    }

    OTN::OTNObject result{ "ids" };
    result.SetNames("localID", "serverID");
    result.SetTypes("int64", "int64");

    try {
        // sets auto commit to false
        TransactionGuard guard(m_connection.get());

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
        
        guard.commit();
    }
    catch (sql::SQLException& e) {
        m_connection->rollback();
        SentError(std::string("SQL Error: ") + e.what(), dstServer, actionName, requestID);
        return;
    }

    OTN::OTNObject header = CreateRequestHeader(actionName, requestID);
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

void SQLServerLogic::HandleGetAgentIDs(const std::string& dstServer, uint32_t requestID, OTN::OTNReader& reader) {
    const char* actionName = "GetAgentIDResult";

    if (!m_connection) {
        std::cerr << "Not connected to DB\n";
        SentError("Not connected to DB", dstServer, actionName, requestID);
        return;
    }

    try {
        auto body = FetchStatement("SELECT id FROM agents;");
        if (!body) {
            SentError("Query returned no result", dstServer, actionName, requestID);
            return;
        }

        OTN::OTNObject header = CreateRequestHeader(actionName, requestID);
        OTN::OTNWriter writer;
        writer.AppendObject(header);
        writer.AppendObject(*body);

        std::string output;
        writer.SaveToString(output);

        NetServerManager::SendMessage(
            m_server,
            dstServer,
            output
        );
    }
    catch (sql::SQLException& e) {
        SentError(std::string("SQL Error: ") + e.what(), dstServer, actionName, requestID);
        return;
    }
}

OTN::OTNObject SQLServerLogic::CreateRequestHeader(const std::string& action, uint32_t requestID, bool response) {
    OTN::OTNObject headerObj{ "header" };

    headerObj.SetNames("action", "request_id", "response");
    headerObj.SetTypes("String", "int64", "bool");
    headerObj.AddDataRow(action, requestID, response);

    return headerObj;
}

void SQLServerLogic::SentError(
    const std::string& errorMsg, 
    const std::string dstServer, 
    const std::string& action, 
    uint32_t requestID) 
{
    OTN::OTNObject header = CreateRequestHeader(action, requestID, false);
    
    OTN::OTNObject body{ "body" };
    body.SetNames("error");
    body.SetTypes("String");
    body.AddDataRow(errorMsg);

    OTN::OTNWriter writer;
    writer.AppendObject(header);
    writer.AppendObject(body);

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
            driver->connect(hostString, m_config.user, m_config.password)
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

std::optional<int64_t> SQLServerLogic::FetchLastInsertID() {
    auto result = FetchStatement("SELECT LAST_INSERT_ID();");
    if (!result) 
        return std::nullopt;

    auto& obj = *result;
    if (obj.GetRowCount() == 0 || obj.GetRow(0).empty())
        return std::nullopt;

    return result->TryGetValue<int64_t>(0, 0);
}

void SQLServerLogic::ExecuteStatement(const std::string& query) {
    try {
        std::unique_ptr<sql::Statement> stmt(m_connection->createStatement());
        stmt->execute(query);
    }
    catch (sql::SQLException& e) {
        std::cerr << "Query error: " << e.what() << '\n';
        throw;
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