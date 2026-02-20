#include "AI/AgentManager.h"
#include "App.h"

bool AgentManager::Save(const OTN::OTNFilePath& path) {
    using namespace OTN;

    OTNObject agentObj{ "Agent" };
    agentObj.SetNames("server_id", "name", "board_states", "config",
        "matches_played", "matches_won", "matches_played_white", "matches_won_white");
    agentObj.SetTypes("int64", "String", "-", "String", "int", "int", "int", "int");

    agentObj.ReserveDataRows(m_agents.size());

    for (const auto& [id, agent] : m_agents) {
        OTNObject boardStateObj{ "BoardState" };
        boardStateObj.SetNames("board_state", "moves");
        boardStateObj.SetTypes("String", "GameMove[]");
        const auto& states = agent.GetNormilzedBoardStates();
        boardStateObj.ReserveDataRows(states.size());

        for (const auto& [stateStr, boardState] : states) {
            const auto& moves = boardState.GetPossibleMoves();
            boardStateObj.AddDataRow(stateStr, moves);
        }

        agentObj.AddDataRow(
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

    if (!agentObj.IsValid()) {
        Log::Error("Failed to save agent data: {}", agentObj.GetError());
        return false;
    }

    OTNWriter writer;
    writer.AppendObject(agentObj);

    if (!writer.Save(path / "Agents")) {
        Log::Error("Failed to save agent data: {}", writer.GetError());
        return false;
    }
    return true;
}

void AgentManager::AddAgent(Agent agent) {
    AgentID id{ m_idManager.GetNewUniqueIdentifier() };
    agent.SetID(id);
    m_agents[id] = std::move(agent);
}

bool AgentManager::RemoveAgent(AgentID id) {
    auto it = m_agents.find(id);
    if (it == m_agents.end())
        return false;
    m_agents.erase(it);
    return true;
}

Agent* AgentManager::GetAgent(AgentID id) {
    auto it = m_agents.find(id);
    return (it != m_agents.end()) ? &it->second : nullptr;
}

const std::unordered_map<AgentID, Agent>& AgentManager::GetAgents() const {
    return m_agents;
}