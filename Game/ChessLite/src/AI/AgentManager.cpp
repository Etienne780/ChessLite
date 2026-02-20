#include "AI/AgentManager.h"
#include "App.h"

template<>
void OTN::ToOTNDataType<GameMove>(OTN::OTNObjectBuilder& obj, GameMove& move) {
	obj.SetObjectName("GameMove");
	obj.AddNames("eval", "from_x", "from_y", "to_x", "to_y");

	if (obj.IsBuildingOTNObject()) {
		Vector2 from = move.GetFrom();
		Vector2 to = move.GetTo();
		obj.AddData(move.GetEvaluation(), from.x, from.y, to.x, to.y);
	}
	else {
		Vector2 from, to;
		float eval;
		
		obj.AddData(eval, from.x, from.y, to.x, to.y);
		
		move = GameMove(from, to);
		move.SetEvaluation(eval);
	}
}

bool AgentManager::Save(const OTN::OTNFilePath& path) {
	using namespace OTN;

	auto* app = App::GetInstance();
	if (!app)
		return false;

	OTNObject agentObj{ "Agent" };
	agentObj.SetNames("id", "name", "board_states", "config", "matches_played", "matches_won", "matches_played_white", "matches_won_white");
	agentObj.SetTypes("int", "string", "-", "string", "int", "int", "int", "int");

	agentObj.ReserveDataRows(m_agents.size());
	for (const auto& [id, a] : m_agents) {
		OTNObject boardStateObj{ "BoardState" };
		boardStateObj.SetNames("board_state", "moves");
		
		auto& states = a.GetNormilzedBoardStates();
		boardStateObj.ReserveDataRows(states.size());
		for (const auto& [stateStr, boardState] : states) {
			const auto& moves = boardState.GetPossibleMoves();
			boardStateObj.AddDataRow(stateStr, moves);
		}

		agentObj.AddDataRow(
			id.value,
			a.GetName(),
			boardStateObj,
			a.GetChessConfig(),
			a.GetMatchesPlayed(),
			a.GetWonMatches(),
			a.GetMatchesPlayedAsWhite(),
			a.GetMatchesWonAsWhite()
		);
	}

	if (!agentObj.IsValid())
		return false;

	OTN::OTNWriter writer;
	writer.AppendObject(agentObj);

	auto exePath = app->GetBasePath();
	writer.Save(exePath / path / "SaveData");

	return true;
}

void AgentManager::AddAgent(Agent agent) {
	AgentID id = AgentID(m_idManager.GetNewUniqueIdentifier());
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
	if (it == m_agents.end())
		return nullptr;
	return &it->second;
}

const std::unordered_map<AgentID, Agent>& AgentManager::GetAgents() const {
	return m_agents;
}