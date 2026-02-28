#pragma once
#include "AppContext.h"

class AgentSyncService : public std::enable_shared_from_this<AgentSyncService> {
public:
    AgentSyncService() = default;
    ~AgentSyncService() = default;

    void Sync(AppContext* ctx);

private:
    bool m_syncInProgress = false;
    
    void SyncMissingData(AppContext* ctx, const std::unordered_set<AgentID>& ids);
    void SyncDelete(AppContext* ctx, const std::unordered_set<AgentID>& ids);

    void RegisterAgents(const std::string& serverIDs);
};