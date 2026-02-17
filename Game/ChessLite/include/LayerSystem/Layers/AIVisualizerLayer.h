#pragma once
#include <SDLCoreLib/SDLCoreUI.h>
#include "AI/AgentManager.h"
#include "LayerSystem/Layer.h"

namespace Layers {

	class AIVisualizerLayer : public Layer {
	public:
		AIVisualizerLayer(PlayerType player1, PlayerType player2);
		~AIVisualizerLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		SDLCore::WindowID m_winID;
		SDLCore::UI::UIContext* m_UICtx = nullptr;

		SDLCore::UI::UIStyle m_styleHeader;

		PlayerType m_player1 = PlayerType::PLAYER;
		PlayerType m_player2 = PlayerType::PLAYER;
		AgentID m_agentID1;
		AgentID m_agentID2;

		const float m_headerHeightPer = 10.0f;

		bool m_isAgent1Shown = true;

		// Layout
		float m_padding = 12.0f;
		float m_headerHeight = 80.0f;

		// Scroll History
		float m_historyScrollOffset = 0.0f;
		float m_historyScrollVelocity = 0.0f;
		float m_historyScrollMax = 0.0f;

		// Scroll Details
		float m_detailScrollOffset = 0.0f;
		float m_detailScrollVelocity = 0.0f;
		float m_detailScrollMax = 0.0f;

		// Scroll tuning
		float m_scrollSpeed = 450.0f;
		float m_scrollDrag = 12.0f;

		// Clipboard timer
		float m_clipBoardCopyTime = 1.5f;
		float m_currentClipBoardCopyTimer = 0.0f;

		void RenderUIBody(const Agent* agent);

		void UpdateScroll(float& offset, float& velocity, float maxScroll);
		bool DrawButton(const std::string& text, float x, float y, float w, float h);
		void DrawBoard(const std::string& state, float x, float y, float size);

		static std::string ToChessNotation(const Vector2& pos);
	};

}