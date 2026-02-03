#pragma once
#include <CoreLib/FormatUtils.h>
#include <CoreLib/CoreID.h>
#include <CoreLib/IDManager.h>

static constexpr uint16_t CORE_CHESS_INVALID_ID = INT16_MAX;

namespace CoreChess {

	enum class ChessGameState : uint16_t {
		IDLE = 0,
		PLAYING,
		BLACK_WON,
		WHITE_WON
	};

	using CoreChessIDManager = IDManager<uint32_t, CORE_CHESS_INVALID_ID>;

	template<typename Tag>
	using ChessCoreID = CoreID<uint32_t, CORE_CHESS_INVALID_ID, Tag>;

	struct ChessPieceTag {};

	using ChessPieceID = ChessCoreID<ChessPieceTag>;

}

template<>
static inline std::string FormatUtils::toString<CoreChess::ChessPieceID>(CoreChess::ChessPieceID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<CoreChess::ChessGameState>(CoreChess::ChessGameState state) {
	switch (state) {
	case CoreChess::ChessGameState::IDLE:		return "Idle";
	case CoreChess::ChessGameState::PLAYING:	return "Playing";
	case CoreChess::ChessGameState::BLACK_WON:	return "Black won";
	case CoreChess::ChessGameState::WHITE_WON:	return "White won";
	default:									return "UNKOWN";
	}
}