#pragma once
#include <CoreLib/Random.h>

#include "LayerSystem/Layers/MainMenuLayer.h"
#include "LayerSystem/Layers/StartLoadLayer.h"
#include "Styles/Comman/Color.h"
#include "App.h"
#include "FilePaths.h"
#include "Type.h"

namespace Layers {

	StartLoadLayer::StartLoadLayer(const std::shared_ptr<AgentSyncService>& agentSync) 
		: m_agentSync(agentSync) {
	}

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

		switch (m_currentState) {
		case LoadState::ASSETS:
			LoadAssets(ctx);
			break;
		case LoadState::SYNC:
			SyncDatabase(ctx);
			break;
		case LoadState::FINISHED:
		default: {
			// final delay before switching layer
			m_currentStartDelay += SDLCore::Time::GetDeltaTimeSecF();
			if (m_currentStartDelay >= m_startDelay) {
				ctx->app->ClearLayers();
				ctx->app->PushLayer<MainMenuLayer>();
			}
		}
		}
	}

	void StartLoadLayer::OnRender(AppContext* ctx) {
		namespace RE = SDLCore::Render;

		float scaleX = m_displaySize.x / m_RefDisplaySize.x;
		float scaleY = m_displaySize.y / m_RefDisplaySize.y;
		float displayScale = std::min(scaleX, scaleY);

		float winXHalf = m_windowSize.x * 0.5f;
		float winYHalf = m_windowSize.y * 0.5f;

		RE::SetColor(255);
		RE::SetTextAlign(SDLCore::Align::CENTER);
		RE::SetTextSize(64.0f * displayScale);
		RE::Text("ChessLite", winXHalf, winYHalf - 64.0f * displayScale);

		switch (m_currentState) {
		case LoadState::ASSETS:
			RenderLoadAssets();
			break;
		case LoadState::SYNC:
			RenderSyncDatabase();
			break;
		case LoadState::FINISHED:
		default:
			break;
		}
	}

	void StartLoadLayer::OnUIRender(AppContext* ctx) {
	}

	void StartLoadLayer::OnQuit(AppContext* ctx) {
	}
	
	LayerID StartLoadLayer::GetLayerID() const {
		return LayerID::START_LOAD;
	}

	void StartLoadLayer::MoveToNextState() {
		LoadState newState = LoadState::UNKNOWN;

		switch (m_currentState) {
		case LoadState::ASSETS:
			newState = LoadState::SYNC;
			break;
		case LoadState::SYNC:
			newState = LoadState::FINISHED;
			break;
		case LoadState::FINISHED:
		default:
			newState = LoadState::FINISHED;
		}

		m_currentState = newState;
	}

	void StartLoadLayer::LoadAssets(AppContext* ctx) {
		float dt = SDLCore::Time::GetDeltaTimeSecF();

		if (m_allSectionsFinished) {
			MoveToNextState();
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

	void StartLoadLayer::RenderLoadAssets() {
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
		RE::SetTextSize(48.0f * displayScale);
		RE::Text(m_currentSectionName.c_str(), winXHalf, winYHalf - 16.0f * displayScale + loadingBarYOffset);

		RE::SetTextSize(32.0f * displayScale);
		RE::TextF(winXHalf, winYHalf + loadingBarH * 0.5f + loadingBarYOffset, "{}/100%", m_progress);
		RE::Rect(winXHalf - loadingBarW * 0.5f, winYHalf + loadingBarYOffset, loadingBarW, loadingBarH);
	}

	void StartLoadLayer::SyncDatabase(AppContext* ctx) {
		if (!m_agentSync) {
			MoveToNextState();
			return;
		}

		if (m_startedSync && !m_agentSync->IsSyncInProgress()) {
			MoveToNextState();
		}
		else if(!m_startedSync) {
			m_startedSync = true;
			m_agentSync->FullSync(ctx);
		}
	}

	void StartLoadLayer::RenderSyncDatabase() {
		
	}

	void StartLoadLayer::AddLoadingSectionAssets() {
		SystemFilePath bPath = FilePaths::GetAssetsPath();

		std::vector<ResourceRequest> requests{
			{ ResourceType::TEXTURE, SkinKeys::BIG_DARK,  bPath / "big-black.png" },
			{ ResourceType::TEXTURE, SkinKeys::BIG_LIGHT, bPath / "big-white.png" },
			{ ResourceType::TEXTURE, SkinKeys::CLASSIC_DARK,  bPath / "classic-black.png" },
			{ ResourceType::TEXTURE, SkinKeys::CLASSIC_LIGHT, bPath / "classic-white.png" },
			{ ResourceType::TEXTURE, SkinKeys::DEVIL_DARK,  bPath / "devil-black.png" },
			{ ResourceType::TEXTURE, SkinKeys::DEVIL_LIGHT, bPath / "devil-white.png" },
			{ ResourceType::TEXTURE, SkinKeys::PIXEL_DARK,  bPath / "pixel-black.png" },
			{ ResourceType::TEXTURE, SkinKeys::PIXEL_LIGHT, bPath / "pixel-white.png" },
			{ ResourceType::AUDIO, SoundKeys::MOVE_SOUND, bPath / "move_sound.wav" },
			{ ResourceType::AUDIO, SoundKeys::CAPTURE_SOUND, bPath / "capture.mp3" }
		};

		AddLoadingSection(
			"Assets", 
			std::move(requests), 
			[this](AppContext* ctx, ResourceLoader& loader) {
				RegisterAssets(ctx, loader);
			}
		);
	}

	void StartLoadLayer::AddLoadingSectionData() {
		SystemFilePath bPath = FilePaths::GetDataPath();

		std::vector<ResourceRequest> requests{
			{ ResourceType::DATA_OTN, bPath / FilePaths::agentsFileName },
			{ ResourceType::DATA_OTN, bPath / FilePaths::optionsFileName },
			{ ResourceType::DATA_OTN, bPath / FilePaths::userFileName }
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

	void StartLoadLayer::RegisterAssets(AppContext* ctx, ResourceLoader& loader) {
		RegistSkins(ctx);
	}

	void StartLoadLayer::RegistSkins(AppContext* ctx) {
		ctx->skinManager.AddSkin(
			ChessSkinType::BIG,
			SkinKeys::BIG_LIGHT,
			SkinKeys::BIG_DARK
		);

		ctx->skinManager.AddSkin(
			ChessSkinType::CLASSIC,
			SkinKeys::CLASSIC_LIGHT,
			SkinKeys::CLASSIC_DARK
		);

		ctx->skinManager.AddSkin(
			ChessSkinType::DEVIL,
			SkinKeys::DEVIL_LIGHT,
			SkinKeys::DEVIL_DARK
		);

		ctx->skinManager.AddSkin(
			ChessSkinType::PIXEL,
			SkinKeys::PIXEL_LIGHT,
			SkinKeys::PIXEL_DARK
		);
	}

	void StartLoadLayer::RegisterData(AppContext* ctx, ResourceLoader& loader) {		
		const auto& assets = loader.GetOTNObjects();
		if (assets.empty())
			return;

		for (const auto& assetKey : assets) {
			for (const auto& [name, obj] : *(assetKey.asset.get())) {
				if (name == "Agent") {
					LoadAgent(ctx, obj);
					break;
				}
				else if (name == "Options") {
					LoadOptions(ctx, obj);
					break;
				}
				else if (name == "userData") {
					ctx->app->LoadUserData(obj);
					break;
				}
			}
		}
	}

	void StartLoadLayer::LoadAgent(AppContext* ctx, const OTN::OTNObject& agentOTN) {
		ctx->agentManager.Load(agentOTN);
	}

	void StartLoadLayer::LoadOptions(AppContext* ctx, const OTN::OTNObject& optionsOTN) {
		if (auto obj = optionsOTN.TryGetValue<bool>(0, "showPossibleMoves")) {
			ctx->options.showPossibleMoves = *obj;
		}

		if (auto obj = optionsOTN.TryGetValue<bool>(0, "autoRetryGame")) {
			ctx->options.autoRetryGame = *obj;
		}

		if (auto obj = optionsOTN.TryGetValue<float>(0, "sfxVolume")) {
			ctx->options.sfxVolume = *obj;
			SDLCore::SoundManager::SetTagVolume(SDLCore::SoundTags::SFX, *obj);
		}
	}

}