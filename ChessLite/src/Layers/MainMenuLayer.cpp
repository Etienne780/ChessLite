#include "Layers/MainMenuLayer.h"
#include "Layers/OptionsMenuLayer.h"
#include "Layers/GameLayer.h"

#include "App.h"
#include "Styles/Comman/Style.h"
#include "Styles/Comman/Space.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;

namespace Layers {

	void MainMenuLayer::OnStart(AppContext* ctx) {
		m_UICtx = UI::CreateContext();

		namespace Prop = UI::Properties;

		m_StyleRoot
			.Merge(Style::commanRoot)
			.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER);

		m_StyleMenu
			.Merge(Style::commanBox)
			.Merge(Style::commanStretch)
			.SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
			.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
			.SetValue(Prop::padding, Style::commanSpaceL)
			.SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
			.SetValue(Prop::size, 550.0f, 400.0f);

		m_StyleButton
			.Merge(Style::commanBTNBase)
			.SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
			.SetValue(Prop::size, 250.0f, 75.0f);

		m_StyleTitle
			.Merge(Style::commanTextTitle)
			.SetValue(Prop::margin, Vector4(0, 0, 20, 0));
	}

	void MainMenuLayer::OnUpdate(AppContext* ctx) {
		
	}

	void MainMenuLayer::OnRender(AppContext* ctx) {
		typedef UI::UIKey Key;

		UI::SetContextWindow(m_UICtx, ctx->app->GetWinID());
		UI::BindContext(m_UICtx);

		UI::BeginFrame(Key("root"), m_StyleRoot);
		{
			UI::BeginFrame(Key("menu"), m_StyleMenu);
			{
				UI::Text(Key("title"), "Chess Lite", m_StyleTitle);

				if (UIComp::DrawButton("btn_play", "Play", m_StyleButton)) {
					Log::Debug("MainMenu: Play");
					ctx->app->ClearLayers();
					ctx->app->PushLayer<GameLayer>();
				}

				if (UIComp::DrawButton("btn_settings", "Settings", m_StyleButton)) {
					Log::Debug("MainMenu: Settings");
					ctx->app->PushLayer<OptionsMenuLayer>();
				}

				if (UIComp::DrawButton("btn_quit", "Quit", m_StyleButton)) {
					Log::Debug("MainMenu: Quit");
					// ctx->app->Quit();
				}
			}
			UI::EndFrame();
		}
		UI::EndFrame();
	}

	void MainMenuLayer::OnQuit(AppContext* ctx) {
		UI::DestroyContext(m_UICtx);
		m_UICtx = nullptr;
	}

}