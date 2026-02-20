#include <CoreLib/FormatUtils.h>

#include "LayerSystem/Layers/GameSetupLayer.h"
#include "LayerSystem/Layers/MainMenuLayer.h"
#include "LayerSystem/Layers/GameLayer.h"
#include "LayerSystem/Layers/AgentSelectLayer.h"

#include "App.h"
#include "Styles/Comman/Style.h"
#include "Styles/Comman/Space.h"
#include "Styles/Comman/Color.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;

namespace Layers {
	
	void GameSetupLayer::OnStart(AppContext* ctx) {
		m_menuCloseEventID = ctx->app->SubscribeToLayerEvent<LayerEventType::CLOSED>(
		[&](const LayerEvent& e) -> void {
			if (e.layerID == LayerID::AGENT_SELECT) {
				m_agentSelectOpen = false;
			}
		});
		
		namespace Prop = UI::Properties;

		m_styleRoot
			.Merge(Style::commanRoot)
			.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER);

		m_styleMenu
			.Merge(Style::commanBox)
			.Merge(Style::commanStretch)
			.SetValue(Prop::borderWidth, 2.0f)
			.SetValue(Prop::borderColor, Style::commanColorOutlineLight)
			.SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
			.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
			.SetValue(Prop::padding, Style::commanSpaceL)
			.SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
			.SetValue(Prop::size, 600.0f, 500.0f);

		m_styleTitle
			.Merge(Style::commanTextTitle)
			.SetValue(Prop::margin, Vector4(0, 0, 20, 0));

		m_stylePlayerContainer
			.Merge(Style::commanStretchX)
			.SetValue(Prop::heightUnit, UI::UISizeUnit::PX)
			.SetValue(Prop::height, 125.0f)
			.SetValue(Prop::margin, Vector4(24.0f, 0.0f, 24.0f, 0.0f))
			.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER);
	}

	void GameSetupLayer::OnUpdate(AppContext* ctx) {

	}

	void GameSetupLayer::OnRender(AppContext* ctx) {

	}

	void GameSetupLayer::OnUIRender(AppContext* ctx) {
		typedef UI::UIKey Key;

		UI::BeginFrame(Key("game_setup_root"), m_styleRoot);
		{
			UI::BeginFrame(Key("menu"), m_styleMenu);
			{
				UI::Text(Key("title"), "Chess Lite", m_styleTitle);

				UI::BeginFrame(Key("player_container"), m_stylePlayerContainer);
				{
					if (UIComp::DrawButton("player1", FormatUtils::toString(m_player1), Style::commanBTNBase) && !m_agentSelectOpen) {
						m_player1 = (m_player1 == PlayerType::PLAYER) ?
							PlayerType::AI :
							PlayerType::PLAYER;
					}

					UI::Text(Key("vs_text"), "vs", Style::commanTextBase);

					if (UIComp::DrawButton("player2", FormatUtils::toString(m_player2), Style::commanBTNBase) && !m_agentSelectOpen) {
						m_player2 = (m_player2 == PlayerType::PLAYER) ?
							PlayerType::AI :
							PlayerType::PLAYER;
					}
				}
				UI::EndFrame();

				if (UIComp::DrawButton("btn_start_game", "Start Game", Style::commanBTNBase) && !m_agentSelectOpen) {
					Log::Debug("GameSetupLayer: game");
					ctx->app->ClearLayers();
					ctx->app->PushLayer<GameLayer>(m_player1, m_player2);
				}

				bool hasAgentSelected =
					m_player1 == PlayerType::AI ||
					m_player2 == PlayerType::AI;

				if (UIComp::DrawButton("btn_select_agent", "Select Agent", Style::commanBTNBase) && hasAgentSelected && !m_agentSelectOpen) {
					Log::Debug("GameSetupLayer: Select Agent");
					AgentSelectMode selectMode = AgentSelectMode::AGENT_1_ONLY;

					if (m_player1 == PlayerType::AI &&
						m_player2 == PlayerType::AI)
						selectMode = AgentSelectMode::BOTH_AGENTS;
					else if (m_player2 == PlayerType::AI)
						selectMode = AgentSelectMode::AGENT_2_ONLY;

					m_agentSelectOpen = true;
					ctx->app->PushLayer<AgentSelect>(selectMode);
				}

				if (UIComp::DrawButton("btn_back", "Back", Style::commanBTNBase) && !m_agentSelectOpen) {
					Log::Debug("GameSetupLayer: Back");
					ctx->app->ClearLayers();
					ctx->app->PushLayer<MainMenuLayer>();
				}
			}
			UI::EndFrame();
		}
		UI::EndFrame();
	}

	void GameSetupLayer::OnQuit(AppContext* ctx) {
		ctx->app->UnsubscribeToLayerEvent(LayerEventType::CLOSED, m_menuCloseEventID);
	}

	LayerID GameSetupLayer::GetLayerID() const {
		return LayerID::GAME_SETUP_LAYER;
	}

}