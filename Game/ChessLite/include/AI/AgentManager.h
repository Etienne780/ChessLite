#pragma once
#include <unordered_map>
#include <unordered_set>
#include <CoreLib/OTNFile.h>
#include "AI/Agent.h"
#include "Type.h"

class AgentManager {
public:
	AgentManager() = default;
	~AgentManager() = default;

	bool Save(const OTN::OTNFilePath& path);
	void Load(const OTN::OTNObject& agents);

	void AddAgent(Agent agent);
	bool RemoveAgent(AgentID id);
	void MarkAgentAsRegistered(AgentID localId, AgentID serverId);

	OTN::OTNObject BuildOTNObjectFromIDs(const std::unordered_set<AgentID>& agents, bool includeLocalID = false);

	Agent* GetAgent(AgentID id);

	const std::unordered_map<AgentID, Agent>& GetAgents() const;
	const std::unordered_set<AgentID>& GetUnregisteredAgentIDs() const;

private:
	CoreAppIDManager m_idManager;
	std::unordered_map<AgentID, Agent> m_agents;
	std::unordered_set<AgentID> m_unregisteredAgentIds;
};