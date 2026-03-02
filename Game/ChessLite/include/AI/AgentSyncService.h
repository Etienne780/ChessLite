#pragma once
#include <atomic>
#include "AppContext.h"

class AgentSyncService : public std::enable_shared_from_this<AgentSyncService> {
public:
    AgentSyncService() = default;
    ~AgentSyncService();

    void Init(AppContext& ctx);

    void FullSync(AppContext* ctx);
    void Sync(AppContext* ctx);

    bool IsSyncInProgress() const;

private:
    bool m_initCalled = false;
    int m_syncActionCount = 0;
    bool m_isSyncInProgress = false;
    NetworkCallbackID m_globalNetworkCB;

    void AddSyncAction();
    void RemoveSyncAction();
    void SendErrorNotification(const std::string& msg);

    void RequestServerAgentIDList(AppContext* ctx);
    void RequestMissingAgentsFromServer(AppContext* ctx);

    void SyncMissingData(AppContext* ctx, const std::unordered_set<AgentID>& ids);
    void SyncDelete(AppContext* ctx, const std::unordered_set<AgentID>& deleteIDs);
    void SyncDirty(AppContext* ctx, const std::unordered_set<AgentID>& dirtyIDs);

    void RegisterAgents(const std::string& serverIDs);
    void HandleServerIDList(const std::string& agentIDList);
    void HandleAddAgents(const std::string& agentList);
    void HandleDeletedAgents();
    void HandleDirtyAgents(const std::string& dirtyList);

    void GlobalCallback(const std::string& msg);
};