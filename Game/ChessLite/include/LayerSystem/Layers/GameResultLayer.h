#pragma once
#include <tuple>
#include <SDLCoreLib/SDLCoreUI.h>

#include "LayerSystem/Layer.h"

namespace Layers {

	class GameResult : public Layer {
	public:
		GameResult(bool whiteWon, AgentID id);
		~GameResult() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		bool m_whiteWon = false;
		SDLCore::WindowCallbackID m_windowResizeCBID;
		AgentID m_agentWonID;
		float m_windowWidth = 0.0f;

		std::string m_wonTitel;
		SDLCore::UI::UIStyle m_styleTitle;

		std::pair<std::string, float> CalculateWonTitel(AppContext* ctx);
	};

}