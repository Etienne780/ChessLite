#pragma once
#include <unordered_map>
#include <unordered_set>
#include <CoreLib/OTNFile.h>
#include "AI/Agent.h"
#include "Type.h"

class AgentSyncService;
class app;

class AgentManager {
friend class AgentSyncService;
friend class App;
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

	/**
	* @brief Retrieves a list of all agents' local and server IDs.
	*
	* This function iterates through all agents managed by the AgentManager
	* 
	* and creates a vector of pairs, where each pair contains:
	* 
	*  - first: the agent's local ID
	* 
	*  - second: the agent's server ID
	*
	* @return std::vector<std::pair<AgentID, AgentID>> A list of all agents with
	*         their corresponding local and server IDs.
	*/
	std::vector<std::pair<AgentID, AgentID>> GetAgentIDAndServerIDs() const;
	std::vector<AgentID> GetAgentID() const;
	std::vector<AgentID> GetAgentServerIDs() const;
	const std::unordered_map<AgentID, Agent>& GetAgents() const;
	const std::unordered_set<AgentID>& GetUnregisteredAgentIDs() const;
	const std::unordered_set<AgentID>& GetDeletedServerAgents() const;
	std::unordered_set<AgentID> GetDirtyAgents() const;
	void MarkAgentsClean(const std::unordered_map<AgentID, size_t>& synced);

private:
	CoreAppIDManager m_idManager;
	std::unordered_map<AgentID, Agent> m_agents;
	std::unordered_set<AgentID> m_unregisteredAgentIds;

	std::unordered_set<AgentID> m_deletedServerAgents;

	void SetDeletedServerAgents(const std::unordered_set<AgentID>& ids);
	void ClearDeletedServerAgents();
};