#pragma once
#include <SDLCoreLib/SDLCore.h>
#include <SDLCoreLib/SDLCoreUI.h>
#include <CoreChessLib/CoreChess.h>

#include "LayerSystem/Layer.h"
#include "AI/Agent.h"

namespace Layers {

	class GameLayer : public Layer {
	public:
		GameLayer(PlayerType player1, PlayerType player2);
		~GameLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		using ChessCoreResult = CoreChess::ChessWinResult;

		bool m_opendGameResult = false;
		bool m_isEscapeMenuOpen = false;
		bool m_isAIVisualizerOpen = false;
		LayerEventSubscriptionID m_menuCloseEventID;
		SDLCore::UI::UIStyle m_root;

		ChessSkinType m_skinType = ChessSkinType::UNKOWN;
		std::shared_ptr<SDLCore::Texture> m_pawnLightTexture = nullptr;
		std::shared_ptr<SDLCore::Texture> m_pawnDarkTexture = nullptr;
		ChessOptions m_options;

		float m_currentAgnetMoveDelayMin = 0.3;
		float m_currentAgnetMoveDelayMax = 1.0f;
		float m_currentAgentMoveDelay = 0.0f;// gets set by m_currentAgnetMoveDelayMin/m_currentAgnetMoveDelayMax
		float m_currentAgnetMovetime = 0.0f;

		float m_notationTextSize = 36.0f;
		float m_boardNotationMargin = 2.0f;
		float m_boardOutlineWidth = 8.0f;
		float m_boardTileSize = 128.0f;
		float m_boardTileSizeFinal = 0.0f;
		float m_tileScaler = 0.0f;
		bool m_calculateBoardTileSize = true;
		Vector4 m_boardMargin{ 128.0f };
		Vector2 m_RefDisplaySize{ 1920.0f, 1080.0f };
		Vector2 m_displaySize{ 0.0f };
		Vector2 m_windowSize{ 0.0f };
		Vector2 m_topLeftBoard{ 0.0f };

		bool m_isPlayer1Turn = false;
		PlayerType m_player1 = PlayerType::PLAYER;
		PlayerType m_player2 = PlayerType::PLAYER;
		bool m_player1White = false;
		AgentID m_agentID1;
		AgentID m_agentID2;

		bool m_gameEnded = false;
		CoreChess::ChessGame m_game;
		ChessCoreResult m_gameResult = ChessCoreResult::NONE;
		CoreChess::ChessPieceID m_pawnID;
		bool m_pieceSelected = false;
		CoreChess::ChessPieceID m_selectedPieceID;
		Vector2 m_selectedPieceLocalPos = Vector2::zero;
		Vector2 m_selectedPiecePos = Vector2::zero;

		void SetupGame(AppContext* ctx);
		void StartGame();
		void GameLogic();
		void EvaluateAIs();
		void ProcessTurn(PlayerType type);
		// returns true when a move was made
		bool PlayerLogic();
		// returns true when a move was made
		bool AILogic();
		void RenderBoard();

		void UpdateBoardTileSize();
		void ResetChessSelectedParams();
		// true on successful move
		bool TryMovePiece(const Vector2& from, const Vector2& to);
		bool TryMovePiece(const Vector2& from, float toX, float toY);

		static std::string ToChessNotationCol(int column);
		static std::string ToChessNotationRow(int row);
		static bool IsPointInRect(const Vector2& mPos, float x, float y, float size);
	};

}