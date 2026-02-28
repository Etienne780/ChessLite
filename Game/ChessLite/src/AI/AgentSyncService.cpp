#include <CoreLib/OTNFile.h>
#include "AI/AgentSyncService.h"
#include "App.h"
#include "FilePaths.h"

void AgentSyncService::Sync(AppContext* ctx) {
	if (!ctx || m_syncInProgress)
		return;

	const auto& missingIDs = ctx->agentManager.GetUnregisteredAgentIDs();
	if (!missingIDs.empty())
		SyncMissingData(ctx, missingIDs);
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

	m_syncInProgress = true;
	auto self = shared_from_this();

	ctx->gameClient.Send(msg,
		[self](bool result, const std::string& payload) {
			self->m_syncInProgress = false;

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