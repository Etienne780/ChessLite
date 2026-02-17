#include "AI/AgentManager.h"

void AgentManager::AddAgent(AgentID id, Agent agent) {
	m_agents[id] = std::move(agent);
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