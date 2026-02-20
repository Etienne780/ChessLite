#pragma once
#include <unordered_map>
#include <CoreLib/OTNFile.h>
#include "AI/Agent.h"
#include "Type.h"

class AgentManager {
public:
	AgentManager() = default;
	~AgentManager() = default;

	bool Save(const OTN::OTNFilePath& path);

	void AddAgent(Agent agent);
	bool RemoveAgent(AgentID id);

	Agent* GetAgent(AgentID id);

	const std::unordered_map<AgentID, Agent>& GetAgents() const;

private:
	CoreAppIDManager m_idManager;
	std::unordered_map<AgentID, Agent> m_agents;
};