#include "LayerSystem/Layers/GameResult.h"
#include "LayerSystem/Layers/GameLayer.h"
#include "LayerSystem/Layers/MainMenuLayer.h"

#include "App.h"
#include "Styles/Comman/Style.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;

namespace Layers {

	GameResult::GameResult(bool whiteWon) 
		: m_whiteWon(whiteWon) {
	}

	void GameResult::OnStart(AppContext* ctx) {
		namespace Prop = UI::Properties;

		m_styleTitle
			.Merge(Style::commanTextTitle)
			.SetValue(Prop::margin, Vector4(0, 0, 20, 0));
	}

	void GameResult::OnUpdate(AppContext* ctx) {
		
	}

	void GameResult::OnRender(AppContext* ctx) {

	}

	void GameResult::OnUIRender(AppContext* ctx) {
		typedef UI::UIKey Key;

		UI::BeginFrame(Key("game_result_overlay"), Style::commanOverlay);
		{
			std::string titel = (m_whiteWon) ? "White Won" : "Black Won";
			UI::Text(Key("title"), titel, m_styleTitle);

			if (UIComp::DrawButton("btn_resume", "Retry", Style::commanBTNBase)) {
				Log::Debug("GameResult: Retry");
				ctx->app->PopLayer();
			}

			if (UIComp::DrawButton("btn_back_to_menu", "Back To Menu", Style::commanBTNBase)) {
				Log::Debug("GameResult: BackToMenu");
				ctx->app->ClearLayers();
				ctx->app->PushLayer<MainMenuLayer>();
			}
		}
		UI::EndFrame();
	}

	void GameResult::OnQuit(AppContext* ctx) {

	}

	LayerID GameResult::GetLayerID() const {
		return LayerID::GAME_RESULT;
	}

}