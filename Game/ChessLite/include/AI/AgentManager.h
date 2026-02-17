#pragma once
#include <unordered_map>
#include "AI/Agent.h"
#include "Type.h"

class AgentManager {
public:
	AgentManager() = default;
	~AgentManager() = default;

	void AddAgent(AgentID id, Agent agent);

	Agent* GetAgent(AgentID id);

	const std::unordered_map<AgentID, Agent>& GetAgents() const;

private:
	std::unordered_map<AgentID, Agent> m_agents;
};