#pragma once
#include <CoreLib/Random.h>

#include "LayerSystem/Layers/MainMenuLayer.h"
#include "LayerSystem/Layers/StartLoadLayer.h"
#include "Styles/Comman/Color.h"
#include "App.h"
#include "FilePaths.h"

namespace Layers {

	void StartLoadLayer::OnStart(AppContext* ctx) {
		m_loadTime = Random::GetRangeNumber(m_loadTimeMin, m_loadTimeMax);

		AddLoadingSectionAssets();
		AddLoadingSectionData();

		if (!m_loadingSections.empty()) {
			m_currentSectionIndex = 0;
			m_loadingSections[0].loader->LoadAsync();
			m_currentSectionName = m_loadingSections[0].name;
		}
	}

	void StartLoadLayer::OnUpdate(AppContext* ctx) {
		m_RefDisplaySize = ctx->refDisplaySize;
		m_displaySize = ctx->displaySize;
		m_windowSize = ctx->windowSize;

		float dt = SDLCore::Time::GetDeltaTimeSecF();

		if (m_allSectionsFinished) {
			// final delay before switching layer
			m_currentStartDelay += dt;
			if (m_currentStartDelay >= m_startDelay) {
				RegistSkins(ctx);
				ctx->app->ClearLayers();
				ctx->app->PushLayer<MainMenuLayer>();
			}
			return;
		}

		if (m_currentSectionIndex >= m_loadingSections.size()) {
			m_allSectionsFinished = true;
			m_progress = 100;
			m_currentStartDelay = 0.0f;
			return;
		}

		auto& section = m_loadingSections[m_currentSectionIndex];
		auto& loader = section.loader;

		if (!loader->IsFinished()) {

			float progress = float(loader->GetLoadedCount()) /
				float(loader->GetTotalCount());

			m_progress = static_cast<int>(progress * 100.0f);

			if (m_progress >= 100)
				m_progress = 99;

			return;
		}

		if (!m_isHoldingSection) {
			m_isHoldingSection = true;
			m_loadTime = Random::GetRangeNumber(m_loadTimeMin, m_loadTimeMax);
			m_currentLoadTime = 0.0f;
			m_progress = 99;
		}

		m_currentLoadTime += dt;
		if (m_currentLoadTime < m_loadTime)
			return;

		loader->Wait();
		ctx->resourcesManager.RegisterLoadedResources(*loader);

		if (section.onFinished)
			section.onFinished(ctx, *loader);

		m_currentSectionIndex++;
		m_isHoldingSection = false;

		if (m_currentSectionIndex < m_loadingSections.size()) {
			auto& next = m_loadingSections[m_currentSectionIndex];
			m_currentSectionName = next.name;

			m_progress = 0;
			next.loader->LoadAsync();
		}
		else {
			m_allSectionsFinished = true;
			m_progress = 100;
			m_currentStartDelay = 0.0f;
		}
	}

	void StartLoadLayer::OnRender(AppContext* ctx) {
		namespace RE = SDLCore::Render;

		Vector4 colorAccent;
		SDLCore::UI::UIRegistry::TryGetRegisteredColor(Style::commanColorAccent, colorAccent);

		float scaleX = m_displaySize.x / m_RefDisplaySize.x;
		float scaleY = m_displaySize.y / m_RefDisplaySize.y;
		float displayScale = std::min(scaleX, scaleY);

		float loadingBarYOffset = 25.0f * displayScale;
		float winXHalf = m_windowSize.x * 0.5f;
		float winYHalf = m_windowSize.y * 0.5f;

		float loadingBarH = 56.0f * displayScale;
		float loadingBarW = (m_windowSize.x - 128.0f) * displayScale;
		float loadingFill = loadingBarW * (static_cast<float>(m_progress) / 100.0f);

		RE::SetColor(colorAccent);
		RE::SetStrokeWidth(4.0f * displayScale);
		RE::FillRect(winXHalf - loadingBarW * 0.5f, winYHalf + loadingBarYOffset, loadingFill, loadingBarH);

		RE::SetColor(255);
		RE::SetTextAlign(SDLCore::Align::CENTER);
		RE::SetTextSize(64.0f * displayScale);
		RE::Text("ChessLite", winXHalf, winYHalf - 64.0f * displayScale);

		RE::SetTextSize(48.0f * displayScale);
		RE::Text(m_currentSectionName.c_str(), winXHalf, winYHalf - 16.0f * displayScale + loadingBarYOffset);

		RE::SetTextSize(32.0f * displayScale);
		RE::TextF(winXHalf, winYHalf + loadingBarH * 0.5f + loadingBarYOffset, "{}/100%", m_progress);
		RE::Rect(winXHalf - loadingBarW * 0.5f, winYHalf + loadingBarYOffset, loadingBarW, loadingBarH);
	}

	void StartLoadLayer::OnUIRender(AppContext* ctx) {
	}

	void StartLoadLayer::OnQuit(AppContext* ctx) {
	}
	
	LayerID StartLoadLayer::GetLayerID() const {
		return LayerID::START_LOAD;
	}

	void StartLoadLayer::AddLoadingSectionAssets() {
		SystemFilePath bPath = FilePaths::GetAssetsPath();

		std::vector<ResourceRequest> requests{
			{ ResourceType::TEXTURE, SkinKeys::CHESS_ECKIG_DARK,  bPath / "ChessEckig_Dark.png" },
			{ ResourceType::TEXTURE, SkinKeys::CHESS_ECKIG_LIGHT, bPath / "ChessEckig_Light.png" },
			{ ResourceType::TEXTURE, SkinKeys::LIGHT_DARK,  bPath / "Light_Dark.png" },
			{ ResourceType::TEXTURE, SkinKeys::LIGHT_LIGHT, bPath / "Light_Light.png" },
			{ ResourceType::TEXTURE, SkinKeys::NEO_DARK,  bPath / "Neo_Dark.png" },
			{ ResourceType::TEXTURE, SkinKeys::NEO_LIGHT, bPath / "Neo_Light.png" },
			{ ResourceType::TEXTURE, SkinKeys::SIMPLE_DARK,  bPath / "Simple_Dark.png" },
			{ ResourceType::TEXTURE, SkinKeys::SIMPLE_LIGHT, bPath / "Simple_Light.png" },
			{ ResourceType::TEXTURE, SkinKeys::SPACE_DARK,  bPath / "Space_Dark.png" },
			{ ResourceType::TEXTURE, SkinKeys::SPACE_LIGHT, bPath / "Space_Light.png" },
		};

		AddLoadingSection("Assets", std::move(requests), nullptr);
	}

	void StartLoadLayer::AddLoadingSectionData() {
		SystemFilePath bPath = FilePaths::GetDataPath();

		std::vector<ResourceRequest> requests{
			{ ResourceType::DATA_OTN, bPath / "Agents.otn" }
		};

		AddLoadingSection(
			"Data",
			std::move(requests),
			[this](AppContext* ctx, ResourceLoader& loader) {
				RegisterData(ctx, loader);
			}
		);
	}

	void StartLoadLayer::AddLoadingSection(
		std::string&& name,
		std::vector<ResourceRequest>&& requests,
		std::function<void(AppContext*, ResourceLoader&)> onFinished) {
		m_loadingSections.emplace_back(
			std::move(name),
			std::move(requests),
			std::move(onFinished)
		);
	}

	void StartLoadLayer::RegisterData(AppContext* ctx, ResourceLoader& loader) {		
		const auto& assets = loader.GetOTNObjects();
		if (assets.empty())
			return;

		const auto& map = *(assets[0].asset);
		for (const auto& [name, obj] : map) {
			if (name == "Agent") {
				RegisterAgent(ctx, obj);
			}
		}
	}

	void StartLoadLayer::RegisterAgent(AppContext* ctx, const OTN::OTNObject& agentOTN) {
		auto& am = ctx->agentManager;
		const auto& obj = agentOTN;

		for (size_t i = 0; i < obj.GetColumnCount(); i++) {
			auto id = obj.TryGetValue<int64_t>(i, "server_id");
			auto name = obj.TryGetValue<std::string>(i, "name");
			auto boardStates = obj.TryGetValue<std::vector<OTN::OTNObject>>(i, "board_states");
			auto config = obj.TryGetValue<std::string>(i, "config");

			auto matchesPlayed = obj.TryGetValue<int>(i, "matches_played");
			auto matchesWon = obj.TryGetValue<int>(i, "matches_won");

			auto matchesPlayedWhite = obj.TryGetValue<int>(i, "matches_played_white");
			auto matchesWonWhite = obj.TryGetValue<int>(i, "matches_won_white");

			if (!id || !name || !config)
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

			if (boardStates) {
				std::unordered_map<std::string, BoardState> states;
				for (const auto& bState : *boardStates) {
					auto stateStr = bState.TryGetValue<std::string>(0, "board_state");
					auto moves = bState.TryGetValue<std::vector<GameMove>>(0, "moves");
					
					if (!stateStr || !moves)
						continue;

					BoardState b;
					b.LoadGameMoves(*moves);
					states[*stateStr] = b;
				}

				agent.LoadBoardState(states);
			}

			agent.LoadPersistentData(data);
			ctx->agentManager.AddAgent(agent);
		}
	}

	void StartLoadLayer::RegistSkins(AppContext* ctx) {
		ctx->skinManager.AddSkin(
			ChessSkinType::CHESS_ECKIG,
			SkinKeys::CHESS_ECKIG_LIGHT,
			SkinKeys::CHESS_ECKIG_DARK
		);

		ctx->skinManager.AddSkin(
			ChessSkinType::LIGHT,
			SkinKeys::LIGHT_LIGHT,
			SkinKeys::LIGHT_DARK
		);

		ctx->skinManager.AddSkin(
			ChessSkinType::NEO,
			SkinKeys::NEO_LIGHT,
			SkinKeys::NEO_DARK
		);

		ctx->skinManager.AddSkin(
			ChessSkinType::SIMPLE,
			SkinKeys::SIMPLE_LIGHT,
			SkinKeys::SIMPLE_DARK
		);

		ctx->skinManager.AddSkin(
			ChessSkinType::SPACE,
			SkinKeys::SPACE_LIGHT,
			SkinKeys::SPACE_DARK
		);
	}

}