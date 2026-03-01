#include "AI/AgentManager.h"
#include "App.h"

bool AgentManager::Save(const OTN::OTNFilePath& path) {
    using namespace OTN;

    std::unordered_set<AgentID> allAgents;
    for (const auto& [id, _] : m_agents)
        allAgents.emplace(id);

    OTNObject agentObj = BuildOTNObjectFromIDs(allAgents);

    if (!agentObj.IsValid()) {
        Log::Error("Failed to save agent data: {}", agentObj.GetError());
        return false;
    }

    OTNWriter writer;
    writer.AppendObject(agentObj);
    writer.UseDeduplicateRows(false);

    if (!writer.Save(path / "Agents")) {
        Log::Error("Failed to save agent data: {}", writer.GetError());
        return false;
    }
    return true;
}

void AgentManager::Load(const OTN::OTNObject& agents) {
    const auto& obj = agents;

    if (obj.GetObjectName() != "Agent")
        return;

    for (size_t i = 0; i < obj.GetColumnCount(); i++) {
        auto serverID = obj.TryGetValue<int64_t>(i, "server_id");
        auto version = obj.TryGetValue<int64_t>(i, "version");
        auto name = obj.TryGetValue<std::string>(i, "name");
        auto boardStates = obj.TryGetValue<std::vector<OTN::OTNObject>>(i, "board_states");
        auto config = obj.TryGetValue<std::string>(i, "config");

        auto matchesPlayed = obj.TryGetValue<int>(i, "matches_played");
        auto matchesWon = obj.TryGetValue<int>(i, "matches_won");

        auto matchesPlayedWhite = obj.TryGetValue<int>(i, "matches_played_white");
        auto matchesWonWhite = obj.TryGetValue<int>(i, "matches_won_white");

        if (!serverID || !version || !name || !config)
            continue;

        Agent agent{ *name, *config };
        AgentPersistentData data;

        if (matchesPlayed)
            data.matchesPlayed = *matchesPlayed;

        if (matchesWon)
            data.matchesWon = *matchesWon;

        if (matchesPlayedWhite)
            data.matchesPlayedAsWhite = *matchesPlayedWhite;

        if (matchesWonWhite)
            data.matchesWonAsWhite = *matchesWonWhite;

        if (boardStates) {
            std::unordered_map<std::string, BoardState> states;
            for (const auto& bState : *boardStates) {
                auto stateStr = bState.TryGetValue<std::string>(0, "board_state");
                auto moves = bState.TryGetValue<std::vector<GameMove>>(0, "moves");

                if (!stateStr || !moves)
                    continue;

                BoardState b;
                b.LoadGameMoves(*moves);
                states[*stateStr] = b;
            }

            agent.LoadBoardState(states);
        }

        agent.LoadPersistentData(data);
        agent.SetServerID(AgentID(static_cast<uint32_t>(*serverID)));
        agent.SetVersion(static_cast<size_t>(*version));
        AddAgent(agent);
    }
}

void AgentManager::AddAgent(Agent agent) {
    AgentID id{ m_idManager.GetNewUniqueIdentifier() };
    agent.SetID(id);
    // if id is invalid
    if(agent.GetServerID() == 0) {
        m_unregisteredAgentIds.emplace(id);
    }

    m_agents[id] = std::move(agent);
}

bool AgentManager::RemoveAgent(AgentID id) {
    auto it = m_agents.find(id);
    if (it == m_agents.end())
        return false;

    AgentID serverID = it->second.GetServerID();
    if(serverID != 0)// if agent is on server
        m_deletedServerAgents.push_back(serverID);

    auto itUnre = m_unregisteredAgentIds.find(id);
    if (itUnre != m_unregisteredAgentIds.end()) {
        m_unregisteredAgentIds.erase(itUnre);
    }

    m_agents.erase(it);
    return true;
}

void AgentManager::MarkAgentAsRegistered(AgentID localId, AgentID serverId) {
    auto* agent = GetAgent(localId);
    if (!agent)
        return;

    agent->SetServerID(serverId);
    m_unregisteredAgentIds.erase(localId);
}

OTN::OTNObject AgentManager::BuildOTNObjectFromIDs(
    const std::unordered_set<AgentID>& agents, bool includeLocalID)
{
    using namespace OTN;
    
    OTNObject agentObj{ "Agent" };
    if (includeLocalID) {
        agentObj.SetNames("server_id", "version", "local_id", "name", "board_states", "config",
            "matches_played", "matches_won", "matches_played_white", "matches_won_white");
        agentObj.SetTypes("int64", "int64", "int64", "String", "-", "String", "int", "int", "int", "int");
    }
    else {
        agentObj.SetNames("server_id", "version", "name", "board_states", "config",
            "matches_played", "matches_won", "matches_played_white", "matches_won_white");
        agentObj.SetTypes("int64", "int64", "String", "-", "String", "int", "int", "int", "int");
    }


    agentObj.ReserveDataRows(agents.size());

    for (const auto& [id, agent] : m_agents) {
        auto it = agents.find(id);
        if (it == agents.end())
            continue;

        OTNObject boardStateObj{ "BoardState" };
        boardStateObj.SetNames("board_state", "moves");
        boardStateObj.SetTypes("String", "GameMove[]");
        const auto& states = agent.GetNormilzedBoardStates();
        boardStateObj.ReserveDataRows(states.size());

        for (const auto& [stateStr, boardState] : states) {
            const auto& moves = boardState.GetPossibleMoves();
            boardStateObj.AddDataRow(stateStr, moves);
        }

        if (includeLocalID) {
            agentObj.AddDataRow(
                static_cast<int64_t>(agent.GetServerID().value),
                static_cast<int64_t>(agent.GetVersion()),
                static_cast<int64_t>(id.value),
                agent.GetName(),
                boardStateObj,
                agent.GetChessConfig(),
                agent.GetMatchesPlayed(),
                agent.GetWonMatches(),
                agent.GetMatchesPlayedAsWhite(),
                agent.GetMatchesWonAsWhite()
            );
        }
        else {
            agentObj.AddDataRow(
                static_cast<int64_t>(agent.GetServerID().value),
                static_cast<int64_t>(agent.GetVersion()),
                agent.GetName(),
                boardStateObj,
                agent.GetChessConfig(),
                agent.GetMatchesPlayed(),
                agent.GetWonMatches(),
                agent.GetMatchesPlayedAsWhite(),
                agent.GetMatchesWonAsWhite()
            );
        }
    }
    return agentObj;
}

Agent* AgentManager::GetAgent(AgentID id) {
    if (id.IsInvalid())
        return nullptr;
    auto it = m_agents.find(id);
    return (it != m_agents.end()) ? &it->second : nullptr;
}

std::vector<std::pair<AgentID, AgentID>> AgentManager::GetAgentIDAndServerIDs() const {
    std::vector<std::pair<AgentID, AgentID>> result;
    result.reserve(m_agents.size());

    for (const auto& [id, agent] : m_agents) {
        result.push_back(std::pair(id, agent.GetServerID()));
    }

    return result;
}

std::vector<AgentID> AgentManager::GetAgentID() const {
    std::vector<AgentID> result;
    result.reserve(m_agents.size());

    for (const auto& [id, _] : m_agents) {
        result.push_back(id);
    }

    return result;
}

std::vector<AgentID> AgentManager::GetAgentServerIDs() const {
    std::vector<AgentID> result;
    result.reserve(m_agents.size());

    for (const auto& [_, agent] : m_agents) {
        if(agent.GetServerID() != 0)
            result.push_back(agent.GetServerID());
    }

    return result;
}

const std::unordered_map<AgentID, Agent>& AgentManager::GetAgents() const {
    return m_agents;
}

const std::unordered_set<AgentID>& AgentManager::GetUnregisteredAgentIDs() const {
    return m_unregisteredAgentIds;
}

const std::vector<AgentID>& AgentManager::GetDeletedServerAgents() const {
    return m_deletedServerAgents;
}

void AgentManager::SetDeletedServerAgents(const std::vector<AgentID>& ids) {
    m_deletedServerAgents = ids;
}

void AgentManager::ClearDeletedServerAgents() {
    m_deletedServerAgents.clear();
}