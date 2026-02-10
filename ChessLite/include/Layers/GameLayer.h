#pragma once
#include <CoreChessLib/CoreChess.h>

#include "Layer.h"

namespace Layers {

	class GameLayer : public Layer {
	public:
		GameLayer() = default;
		~GameLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		CoreChess::ChessGame m_game;
		bool m_gameEnded = false;
		bool m_isEscapeMenuOpen = false;
		CoreChess::ChessWinResult m_gameResult = CoreChess::ChessWinResult::NONE;
		LayerEventSubscriptionID m_escapeMenuCloseEventID;

		Vector2 m_selectedPiecePos = Vector2::zero;

		void SetupGame();
		void RenderBoard();
	};

}