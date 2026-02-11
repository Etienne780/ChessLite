#include "Layers/EscapeMenuLayer.h"
#include "Layers/MainMenuLayer.h"
#include "Layers/OptionsMenuLayer.h"

#include "App.h"
#include "Styles/Comman/Style.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;

namespace Layers {

	void EscapeMenuLayer::OnStart(AppContext* ctx) {
		
	}

	void EscapeMenuLayer::OnUpdate(AppContext* ctx) {
		using namespace SDLCore;
		
		if (Input::KeyJustPressed(KeyCode::ESCAPE)) {
			ctx->app->PopLayer();
		}
	}

	void EscapeMenuLayer::OnRender(AppContext* ctx) {
		
	}

	void EscapeMenuLayer::OnUIRender(AppContext* ctx) {
		typedef UI::UIKey Key;

		UI::BeginFrame(Key("escape_menu_overlay"), Style::commanOverlay);
		{
			if (UIComp::DrawButton("btn_resume", "Resume", Style::commanBTNBase)) {
				Log::Debug("EscapeMenu: Resume");
				ctx->app->PopLayer();
			}

			if (UIComp::DrawButton("btn_settings", "Settings", Style::commanBTNBase)) {
				Log::Debug("EscapeMenu: Settings");
				ctx->app->PushLayer<OptionsMenuLayer>();
			}

			if (UIComp::DrawButton("btn_back_to_menu", "Back To Menu", Style::commanBTNBase)) {
				Log::Debug("EscapeMenu: BackToMenu");
				ctx->app->ClearLayers();
				ctx->app->PushLayer<MainMenuLayer>();
			}
		}
		UI::EndFrame();
	}

	void EscapeMenuLayer::OnQuit(AppContext* ctx) {
		
	}

	LayerID EscapeMenuLayer::GetLayerID() const {
		return LayerID::ESCAPE_MENU;
	}

}