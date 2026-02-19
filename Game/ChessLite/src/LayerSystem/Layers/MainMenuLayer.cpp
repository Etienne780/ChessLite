#include "LayerSystem/Layers/MainMenuLayer.h"
#include "LayerSystem/Layers/OptionsMenuLayer.h"
#include "LayerSystem/Layers/GameLayer.h"

#include "App.h"
#include "Styles/Comman/Style.h"
#include "Styles/Comman/Space.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;

namespace Layers {

	void MainMenuLayer::OnStart(AppContext* ctx) {
		namespace Prop = UI::Properties;

		m_styleRoot
			.Merge(Style::commanRoot)
			.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER);

		m_styleMenu
			.Merge(Style::commanBox)
			.Merge(Style::commanStretch)
			.SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
			.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
			.SetValue(Prop::padding, Style::commanSpaceL)
			.SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
			.SetValue(Prop::size, 600.0f, 500.0f);

		m_styleTitle
			.Merge(Style::commanTextTitle)
			.SetValue(Prop::margin, Vector4(0, 0, 20, 0));
	}

	void MainMenuLayer::OnUpdate(AppContext* ctx) {
		
	}

	void MainMenuLayer::OnRender(AppContext* ctx) {
		
	}

	void MainMenuLayer::OnUIRender(AppContext* ctx) {
		typedef UI::UIKey Key;

		UI::BeginFrame(Key("main_menu_root"), m_styleRoot);
		{
			UI::BeginFrame(Key("menu"), m_styleMenu);
			{
				UI::Text(Key("title"), "Chess Lite", m_styleTitle);

				if (UIComp::DrawButton("btn_play", "Play", Style::commanBTNBase)) {
					Log::Debug("MainMenu: Play");
					ctx->app->ClearLayers();
					ctx->app->PushLayer<GameLayer>(PlayerType::PLAYER, PlayerType::AI);
				}

				if (UIComp::DrawButton("btn_skins", "Skins", Style::commanBTNBase)) {
					Log::Debug("MainMenu: Skins");
				}

				if (UIComp::DrawButton("btn_settings", "Settings", Style::commanBTNBase)) {
					Log::Debug("MainMenu: Settings");
					ctx->app->PushLayer<OptionsMenuLayer>();
				}

				if (UIComp::DrawButton("btn_quit", "Quit", Style::commanBTNBase)) {
					Log::Debug("MainMenu: Quit");
#ifdef NDEBUG
						ctx->app->Quit();
#endif
				}
			}
			UI::EndFrame();
		}
		UI::EndFrame();
	}

	void MainMenuLayer::OnQuit(AppContext* ctx) {
		
	}

	LayerID MainMenuLayer::GetLayerID() const {
		return LayerID::MAIN_MENU;
	}

}