#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

#include "LayerSystem/Layer.h"

namespace Layers {

	class GameSetupLayer : public Layer {
	public:
		GameSetupLayer() = default;
		~GameSetupLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		LayerEventSubscriptionID m_menuCloseEventID;

		SDLCore::UI::UIStyle m_styleRoot;
		SDLCore::UI::UIStyle m_styleMenu;
		SDLCore::UI::UIStyle m_stylePlayerContainer;
		SDLCore::UI::UIStyle m_styleTitle;

		PlayerType m_player1 = PlayerType::PLAYER;
		PlayerType m_player2 = PlayerType::PLAYER;

		bool m_agentSelectOpen = false;
	};

}