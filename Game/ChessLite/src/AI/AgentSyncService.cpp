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
* sync agent missing on db (delete local agent)
* - retrive list of all server ids, if agent server id missing. delete local agent
* 
* sync agent on db but not local
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

	// sync agnet on db but not on local
	RequestMissingAgentsFromServer(ctx);

	const auto& missingIDs = ctx->agentManager.GetUnregisteredAgentIDs();
	if (!missingIDs.empty())
		SyncMissingData(ctx, missingIDs);

	// sync agent not on db but local with db ID
	RequestServerAgentIDList(ctx);

	const auto& deletedIDs = ctx->agentManager.GetDeletedServerAgents();
	if (!deletedIDs.empty())
		SyncDelete(ctx, deletedIDs);
}

void AgentSyncService::Sync(AppContext* ctx) {
	if (!ctx || m_isSyncInProgress)
		return;

	const auto& missingIDs = ctx->agentManager.GetUnregisteredAgentIDs();
	if (!missingIDs.empty())
		SyncMissingData(ctx, missingIDs);

	const auto& deletedIDs = ctx->agentManager.GetDeletedServerAgents();
	if (!deletedIDs.empty())
		SyncDelete(ctx, deletedIDs);
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

	if (m_syncActionCount == 0) {
		m_isSyncInProgress = false;

		auto* app = App::GetInstance();
		if (!app)
			return;
		auto* ctx = app->GetContext();
		if (!ctx)
			return;

		ctx->agentManager.Save(FilePaths::GetDataPath());
		ctx->app->SaveUserData();
	}
}

void AgentSyncService::RequestServerAgentIDList(AppContext* ctx) {
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
			if (!result) {
				self->RemoveSyncAction();
				Log::Error("Failed to sync agents: {}", payload);
				return;
			}

			self->HandleServerIDList(payload);
			self->RemoveSyncAction();
		});
}

void AgentSyncService::RequestMissingAgentsFromServer(AppContext* ctx) {
	auto serverAgents = ctx->agentManager.GetAgentServerIDs();

	OTN::OTNObject headerObj = ctx->gameClient.CreateHeaderBlock("RequestMissingAgents");
	OTN::OTNObject bodyObj{ "body" };
	bodyObj.SetNames("id");
	bodyObj.SetTypes("int64");
	bodyObj.ReserveDataRows(serverAgents.size());
	for (auto& id : serverAgents) {
		bodyObj.AddDataRow(static_cast<int64_t>(id.value));
	}

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
			if (!result) {
				self->RemoveSyncAction();
				Log::Error("Failed to sync agents: {}", payload);
				return;
			}

			self->HandleAddAgents(payload);
			self->RemoveSyncAction();
		});
}

void AgentSyncService::SyncMissingData(AppContext* ctx, const std::unordered_set<AgentID>& ids) {
	OTN::OTNObject headerObj = ctx->gameClient.CreateHeaderBlock("SyncMissingData");
	OTN::OTNObject bodyObj = ctx->agentManager.BuildOTNObjectFromIDs(ids, true);
	bodyObj.SetObjectName("body");

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
			if (!result) {
				self->RemoveSyncAction();
				Log::Error("Failed to sync agents: {}", payload);
				return;
			}

			self->RegisterAgents(payload);
			self->RemoveSyncAction();
		});
}

void AgentSyncService::SyncDelete(AppContext* ctx, const std::vector<AgentID>& ids) {
	OTN::OTNObject headerObj = ctx->gameClient.CreateHeaderBlock("SyncDeleteData");
	OTN::OTNObject bodyObj{ "body" };
	bodyObj.SetNames("ids");
	bodyObj.SetTypes("int64");
	for (auto id : ids)
		bodyObj.AddDataRow(static_cast<int64_t>(id.value));

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
			if (!result) {
				self->RemoveSyncAction();
				Log::Error("Failed to sync agents: {}", payload);
				return;
			}
			self->HandleDeletedAgents();
			self->RemoveSyncAction();
		});
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
}

void AgentSyncService::HandleServerIDList(const std::string& agentIDList) {
	OTN::OTNReader reader;
	if (!reader.ReadString(agentIDList)) {
		Log::Error("Failed to parse server agent ID list: {}", reader.GetError());
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
}

void AgentSyncService::HandleAddAgents(const std::string& agentList) {
	OTN::OTNReader reader;
	if (!reader.ReadString(agentList)) {
		Log::Error("Failed to parse server agent list: {}", reader.GetError());
		return;
	}

	auto agentObj = reader.TryGetObject("agents");
	if (!agentObj)
		return;

	auto boardStatesObj = reader.TryGetObject("board_states");
	auto gameMovesObj = reader.TryGetObject("game_moves");

	auto* app = App::GetInstance();
	if (!app)
		return;
	auto* ctx = app->GetContext();
	if (!ctx)
		return;

	const auto& obj = *agentObj;

	for (size_t i = 0; i < obj.GetRowCount(); i++) {
		auto serverID = obj.TryGetValue<int64_t>(i, "id");
		auto name = obj.TryGetValue<std::string>(i, "name");
		auto config = obj.TryGetValue<std::string>(i, "config");

		auto matchesPlayed = obj.TryGetValue<int>(i, "matches_played");
		auto matchesWon = obj.TryGetValue<int>(i, "matches_won");

		auto matchesPlayedWhite = obj.TryGetValue<int>(i, "matches_played_white");
		auto matchesWonWhite = obj.TryGetValue<int>(i, "matches_won_white");

		if (!serverID || !name || !config)
			continue;

		Agent agent{ *name, *config };
		AgentPersistentData data;

		if (matchesPlayed) 
			data.matchesPlayed = *matchesPlayed;
		if (matchesWon) 
			data.matchesWon = *matchesWon;
		if (matchesPlayedWhite) 
			data.matchesPlayedAsWhite = *matchesPlayedWhite;
		if (matchesWonWhite) 
			data.matchesWonAsWhite = *matchesWonWhite;

		if (boardStatesObj) {
			std::unordered_map<std::string, BoardState> states;

			for (size_t j = 0; j < boardStatesObj->GetRowCount(); j++) {
				auto boardStateAgentID = boardStatesObj->TryGetValue<int64_t>(j, "agent_id");
				if (!boardStateAgentID || *boardStateAgentID != *serverID)
					continue;

				auto boardStateID = boardStatesObj->TryGetValue<int64_t>(j, "id");
				auto stateStr = boardStatesObj->TryGetValue<std::string>(j, "board_state");
				if (!boardStateID || !stateStr)
					continue;

				std::vector<GameMove> moves;
				if (gameMovesObj) {
					for (size_t k = 0; k < gameMovesObj->GetRowCount(); k++) {
						auto gameMoveBoardStateID = gameMovesObj->TryGetValue<int64_t>(k, "board_state_id");
						if (!gameMoveBoardStateID || *gameMoveBoardStateID != *boardStateID)
							continue;

						auto evaluation = gameMovesObj->TryGetValue<float>(k, "evaluation");
						auto fromX = gameMovesObj->TryGetValue<int>(k, "from_x");
						auto fromY = gameMovesObj->TryGetValue<int>(k, "from_y");
						auto toX = gameMovesObj->TryGetValue<int>(k, "to_x");
						auto toY = gameMovesObj->TryGetValue<int>(k, "to_y");

						if (!evaluation || !fromX || !fromY || !toX || !toY)
							continue;

						GameMove m{ *fromX, *fromY, *toX, *toY };
						m.SetEvaluation(*evaluation);
						moves.push_back(m);
					}
				}

				BoardState b;
				b.LoadGameMoves(moves);
				states[*stateStr] = b;
			}

			agent.LoadBoardState(states);
		}

		agent.LoadPersistentData(data);
		agent.SetServerID(AgentID(static_cast<uint32_t>(*serverID)));
		ctx->agentManager.AddAgent(agent);
	}
}

void AgentSyncService::HandleDeletedAgents() {
	auto* app = App::GetInstance();
	if (!app)
		return;
	auto* ctx = app->GetContext();
	if (!ctx)
		return;

	ctx->agentManager.ClearDeletedServerAgents();
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