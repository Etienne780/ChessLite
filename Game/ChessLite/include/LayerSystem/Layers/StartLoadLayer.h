#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

#include "ResourcesManagement/ResourceLoader.h"
#include "LayerSystem/Layer.h"

namespace Layers {

	class StartLoadLayer : public Layer {
	public:
		StartLoadLayer() = default;
		~StartLoadLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		std::unique_ptr<ResourceLoader> m_loader;

		Vector2 m_RefDisplaySize{ 0.0f };
		Vector2 m_displaySize{ 0.0f };
		Vector2 m_windowSize{ 0.0f };

		float m_loadTimeMin = 0.5f;
		float m_loadTimeMax = 1.0f;
		float m_startDelay = 0.5f;

		float m_loadTime = 0.0f;
		float m_currentLoadTime = 0.0f;
		float m_currentStartDelay = 0.0f;
		int m_progress = 0;

		void RegistSkins(AppContext* ctx);
	};

}