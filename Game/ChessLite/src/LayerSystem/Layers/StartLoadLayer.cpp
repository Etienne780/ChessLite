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

		m_loader = std::make_unique<ResourceLoader>(std::move(requests));
		m_loader->LoadAsync();
	}

	void StartLoadLayer::OnUpdate(AppContext* ctx) {
		m_RefDisplaySize = ctx->refDisplaySize;
		m_displaySize = ctx->displaySize;
		m_windowSize = ctx->windowSize;

		m_currentLoadTime += SDLCore::Time::GetDeltaTimeSecF();
		if (!m_loader->IsFinished()) {
			float progress = float(m_loader->GetLoadedCount()) / float(m_loader->GetTotalCount());
			m_progress = static_cast<int>(progress * 100.0f);
			return;
		}

		m_progress = 99;
		if (m_currentLoadTime < m_loadTime)
			return;

		m_progress = 100;
		m_currentStartDelay += SDLCore::Time::GetDeltaTimeSecF();
		if (m_currentStartDelay < m_startDelay)
			return;

		m_loader->Wait();

		ctx->resourcesManager.RegisterLoadedResources(*m_loader.get());
		
		RegistSkins(ctx);
		ctx->app->ClearLayers();
		ctx->app->PushLayer<MainMenuLayer>();
	}

	void StartLoadLayer::OnRender(AppContext* ctx) {
		namespace RE = SDLCore::Render;

		Vector4 colorAccent;
		SDLCore::UI::UIRegistry::TryGetRegisteredColor(Style::commanColorAccent, colorAccent);

		float scaleX = m_displaySize.x / m_RefDisplaySize.x;
		float scaleY = m_displaySize.y / m_RefDisplaySize.y;
		float displayScale = std::min(scaleX, scaleY);

		float winXHalf = m_windowSize.x * 0.5f;
		float winYHalf = m_windowSize.y * 0.5f;

		float loadingBarH = 56.0f * displayScale;
		float loadingBarW = (m_windowSize.x - 128.0f) * displayScale;
		float loadingFill = loadingBarW * (static_cast<float>(m_progress) / 100.0f);

		RE::SetColor(colorAccent);
		RE::SetStrokeWidth(4.0f * displayScale);
		RE::FillRect(winXHalf - loadingBarW * 0.5f, winYHalf, loadingFill, loadingBarH);

		RE::SetColor(255);
		RE::SetTextAlign(SDLCore::Align::CENTER);
		RE::SetTextSize(64.0f * displayScale);
		
		RE::Text("ChessLite", winXHalf, winYHalf - 64.0f * displayScale);

		RE::SetTextSize(32.0f * displayScale);
		RE::TextF(winXHalf, winYHalf + loadingBarH * 0.5f, "{}/100%", m_progress);
		RE::Rect(winXHalf - loadingBarW * 0.5f, winYHalf, loadingBarW, loadingBarH);
	}

	void StartLoadLayer::OnUIRender(AppContext* ctx) {
	}

	void StartLoadLayer::OnQuit(AppContext* ctx) {
	}
	
	LayerID StartLoadLayer::GetLayerID() const {
		return LayerID::START_LOAD;
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