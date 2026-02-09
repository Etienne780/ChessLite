#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

#include "Layer.h"

namespace Layers {

	class OptionsMenuLayer : public Layer {
	public:
		OptionsMenuLayer() = default;
		~OptionsMenuLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;

	private:
		SDLCore::UI::UIContext* m_UICtx = nullptr;

		SDLCore::UI::UIStyle m_StyleRoot;
		SDLCore::UI::UIStyle m_StyleOverlay;
		SDLCore::UI::UIStyle m_StyleButton;
	};

}