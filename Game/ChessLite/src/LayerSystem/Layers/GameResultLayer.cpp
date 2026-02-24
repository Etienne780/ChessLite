#include "LayerSystem/Layers/GameResultLayer.h"
#include "LayerSystem/Layers/GameLayer.h"
#include "LayerSystem/Layers/MainMenuLayer.h"

#include "App.h"
#include "FilePaths.h"
#include "Styles/Comman/Style.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;

namespace Layers {

	GameResult::GameResult(bool whiteWon, AgentID id)
		: m_whiteWon(whiteWon), m_agentWonID(id) {
	}

	void GameResult::OnStart(AppContext* ctx) {
		auto* app = ctx->app;
		auto id = app->GetWinID();
		auto* win = app->GetWindow(id);
		
		if (win) {
			m_windowWidth = win->GetWidth();

			m_windowResizeCBID = win->AddOnWindowResize([this](SDLCore::Window& win) {
				m_windowWidth = win.GetWidth();
			});
		}

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
			auto result = CalculateWonTitel(ctx);
			m_styleTitle.SetValue(UI::Properties::textSize, result.second);
			UI::Text(Key("title"), result.first, m_styleTitle);

			if (UIComp::DrawButton("btn_resume", "Retry", Style::commanBTNBase)) {
				Log::Debug("GameResult: Retry");
				ctx->app->PopLayer(LayerID::GAME_RESULT);
			}

			if (UIComp::DrawButton("btn_back_to_menu", "Back To Menu", Style::commanBTNBase)) {
				Log::Debug("GameResult: BackToMenu");
				ctx->agentManager.Save(FilePaths::GetDataPath());
				ctx->app->ClearLayers();
				ctx->app->PushLayer<MainMenuLayer>();
			}
		}
		UI::EndFrame();
	}

	void GameResult::OnQuit(AppContext* ctx) {
		auto* app = ctx->app;
		auto id = app->GetWinID();
		auto* win = app->GetWindow(id);

		if (win)
			win->RemoveOnWindowResize(m_windowResizeCBID);
	}

	LayerID GameResult::GetLayerID() const {
		return LayerID::GAME_RESULT;
	}

	std::pair<std::string, float> GameResult::CalculateWonTitel(AppContext* ctx) {
		namespace RE = SDLCore::Render;

		const std::string color = m_whiteWon ? "White " : "Black ";
		std::string playerType = "(Player)";

		if (!m_agentWonID.IsInvalid()) {
			if (Agent* agent = ctx->agentManager.GetAgent(m_agentWonID)) {
				playerType = "(Agent: " + agent->GetName() + ")";
			}
		}

		const std::string title = color + playerType + " Won";

		const float widthPercent = 75.0f;
		const float maxSize = 64.0f;
		const float desiredWidth = widthPercent * m_windowWidth / 100.0f;

		const float textSize =
			RE::CalculateTextSizeForBounds(title, desiredWidth, -1.0f);

		return { title, std::min(maxSize, textSize) };
	}

}