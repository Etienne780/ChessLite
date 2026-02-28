#include <CoreLib/OTNFile.h>
#include "AI/AgentSyncService.h"
#include "App.h"
#include "FilePaths.h"

AgentSyncService::~AgentSyncService() {
	auto* app = App::GetInstance();
	if (!app)
		return;

	AppContext* ctx = app->GetContext();
	if (!ctx)
		return;

	ctx->gameClient.RemoveGlobalCallback(m_globalNetworkCB);
}


void AgentSyncService::Init(AppContext& ctx) {
	if (m_initCalled)
		return;

	m_initCalled = true;
	std::weak_ptr<AgentSyncService> weakSelf = shared_from_this();

	m_globalNetworkCB = ctx.gameClient.AddGlobalCallback(
		[weakSelf](const std::string& msg) {
			if (auto self = weakSelf.lock()) {
				self->GlobalCallback(msg);
			}
		});
}

/*
* What kinds of syncs happen
* 
* sync agent not on db
* - check if server id == 0, add agent to server
* 
* sync agnet missing on db (delete local agent)
* - retrive list of all server ids, if agent server id missing. delete local agent
* 
* sync agent local delete
* - sent list to server with deleted agents
* 
* sync agent changed
* - list of agents changed and check if version is higher than on server
*/

void AgentSyncService::FullSync(AppContext* ctx) {
	if (!ctx || m_isSyncInProgress)
		return;

	OTN::OTNObject headerObj = ctx->gameClient.CreateHeaderBlock("GetAgentIDList");
	OTN::OTNObject bodyObj{ "body" };

	std::string msg;
	OTN::OTNWriter writer;
	writer.AppendObject(headerObj);
	writer.AppendObject(bodyObj);
	if (!writer.SaveToString(msg)) {
		Log::Error("Failed to sync agents: {}", writer.GetError());
		return;
	}

	AddSyncAction();
	auto self = shared_from_this();

	ctx->gameClient.Send(msg,
		[self](bool result, const std::string& payload) {
			self->RemoveSyncAction();

			if (!result) {
				Log::Error("Failed to sync agents: {}", payload);
				return;
			}

			self->HandleServerIDList(payload);
		});
}

void AgentSyncService::Sync(AppContext* ctx) {
	if (!ctx || m_isSyncInProgress)
		return;

	const auto& missingIDs = ctx->agentManager.GetUnregisteredAgentIDs();
	if (!missingIDs.empty())
		SyncMissingData(ctx, missingIDs);
}

bool AgentSyncService::IsSyncInProgress() const {
	return m_isSyncInProgress;
}

void AgentSyncService::AddSyncAction() {
	m_syncActionCount++;
	m_isSyncInProgress = true;
}

void AgentSyncService::RemoveSyncAction() {
	if (m_syncActionCount > 0)
		m_syncActionCount--;

	if (m_syncActionCount == 0)
		m_isSyncInProgress = false;
}

void AgentSyncService::SyncMissingData(AppContext* ctx, const std::unordered_set<AgentID>& ids) {
	OTN::OTNObject headerObj = ctx->gameClient.CreateHeaderBlock("SyncMissingData");
	OTN::OTNObject bodyObj = ctx->agentManager.BuildOTNObjectFromIDs(ids, true);
	bodyObj.SetName("body");

	std::string msg;
	OTN::OTNWriter writer;
	writer.AppendObject(headerObj);
	writer.AppendObject(bodyObj);
	if (!writer.SaveToString(msg)) {
		Log::Error("Failed to sync agents: {}", writer.GetError());
		return;
	}

	AddSyncAction();
	auto self = shared_from_this();

	ctx->gameClient.Send(msg,
		[self](bool result, const std::string& payload) {
			self->RemoveSyncAction();

			if (!result) {
				Log::Error("Failed to sync agents: {}", payload);
				return;
			}

			self->RegisterAgents(payload);
		});
}

void AgentSyncService::SyncDelete(AppContext* ctx, const std::unordered_set<AgentID>& ids) {
	
}

void AgentSyncService::RegisterAgents(const std::string& serverIDs) {
	auto* app = App::GetInstance();
	if (!app)
		return;
	auto* ctx = app->GetContext();
	if (!ctx)
		return;

	OTN::OTNReader reader;
	if (!reader.ReadString(serverIDs)) {
		Log::Error("Failed to sync agents: {}", reader.GetError());
		return;
	}

	auto obj = reader.TryGetObject("ids");
	if (!obj || obj->GetRowCount() <= 0)
		return;

	AgentID localID;
	AgentID serverID;

	for (size_t i = 0; i < obj->GetRowCount(); ++i)	{
		auto otnLocalID = obj->TryGetValue<int64_t>(i, "localID");
		auto otnServerID = obj->TryGetValue<int64_t>(i, "serverID");
		if (!otnLocalID || !otnServerID)
			return;

		localID.value = static_cast<uint32_t>(*otnLocalID);
		serverID.value = static_cast<uint32_t>(*otnServerID);
		ctx->agentManager.MarkAgentAsRegistered(localID, serverID);
	}

	ctx->agentManager.Save(FilePaths::GetDataPath());
}

void AgentSyncService::HandleServerIDList(const std::string& agentIDList) {
	OTN::OTNReader reader;
	if (!reader.ReadString(agentIDList)) {
		Log::Error("Failed to parse server agent list: {}", reader.GetError());
		return;
	}

	auto listObj = reader.TryGetObject("List");
	if (!listObj) {
		Log::Error("Failed to retrieve 'List' object from server data: {}", reader.GetError());
		return;
	}

	// Build a set of server agent IDs for fast lookup
	std::unordered_set<int64_t> serverIDs;
	serverIDs.reserve(listObj->GetRowCount());
	for (size_t i = 0; i < listObj->GetRowCount(); i++) {
		auto idOpt = listObj->TryGetValue<int64_t>(i, "id");
		if (!idOpt) {
			Log::Error("Invalid server ID at row {}: {}", i, reader.GetError());
			return;
		}
		serverIDs.emplace(*idOpt);
	}

	auto* app = App::GetInstance();
	if (!app) 
		return;
	auto* ctx = app->GetContext();
	if (!ctx) 
		return;

	auto& agentManager = ctx->agentManager;
	auto localAgents = agentManager.GetAgentIDAndServerIDs();

	for (auto& [localID, serverID] : localAgents) {
		// if agent not on server, skip
		if (serverID == 0)
			continue;

		if (serverIDs.find(static_cast<int64_t>(serverID.value)) == serverIDs.end())
			agentManager.RemoveAgent(localID);
	}

	agentManager.Save(FilePaths::GetDataPath());
}

void AgentSyncService::GlobalCallback(const std::string& msg) {
	OTN::OTNReader reader;
	if (!reader.ReadString(msg)) {
		return;
	}

	auto headerObj = reader.TryGetObject("header");
	if (!headerObj)
		return;

	auto action = headerObj->TryGetValue<std::string>(0, "action");
	if (!action)
		return;

	if (*action == "") {
		auto bodyObj = reader.TryGetObject("body");
	}
}