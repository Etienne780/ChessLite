#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

#include "LayerSystem/Layer.h"

namespace Layers {

	class OptionsMenuLayer : public Layer {
	public:
		OptionsMenuLayer() = default;
		~OptionsMenuLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		SDLCore::UI::UIStyle m_styleRoot;
		SDLCore::UI::UIStyle m_styleTitle;
		SDLCore::UI::UIStyle m_styleRowBTNContainer;
		SDLCore::UI::UIStyle m_styleTrimmer;
		SDLCore::UI::UIStyle m_largerBTNTabNormal;
		SDLCore::UI::UIStyle m_largerBTNTabActive;

		static inline const float m_largeTabBTNWidth = 508.0f;
	};

}