#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

#include "Layer.h"

namespace Layers {

	class MainMenuLayer : public Layer {
	public:
		MainMenuLayer() = default;
		~MainMenuLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;

	private:
		SDLCore::UI::UIStyle m_StyleRoot;
		SDLCore::UI::UIStyle m_StyleMenu;
		SDLCore::UI::UIStyle m_StyleButton;
		SDLCore::UI::UIStyle m_StyleTitle;
	};

}