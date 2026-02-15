#pragma once
#include <SDLCoreLib/SDLCore.h>
#include <CoreChessLib/CoreChess.h>

#include "LayerSystem/Layer.h"

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
		bool m_gameEnded = false;
		bool m_isEscapeMenuOpen = false;
		LayerEventSubscriptionID m_escapeMenuCloseEventID;

		ChessSkinType m_skinType = ChessSkinType::UNKOWN;
		std::shared_ptr<SDLCore::Texture> m_pawnLightTexture = nullptr;
		std::shared_ptr<SDLCore::Texture> m_pawnDarkTexture = nullptr;

		float m_boardTileSize = 128.0f;
		bool m_calculateBoardTileSize = true;
		Vector4 m_boardMargin{ 128.0f };
		Vector2 m_RefDisplaySize{ 1920.0f, 1080.0f };
		Vector2 m_displaySize{ 0.0f };
		Vector2 m_windowSize{ 0.0f };

		CoreChess::ChessGame m_game;
		CoreChess::ChessWinResult m_gameResult = CoreChess::ChessWinResult::NONE;
		CoreChess::ChessPieceID m_pawnID;
		bool m_pieceSelected = false
		Vector2 m_selectedPiecePos = Vector2::zero;

		void SetupGame();
		void GameLogic();
		void RenderBoard();
	};

}