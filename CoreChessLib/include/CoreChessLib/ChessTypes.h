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

	enum class FieldType : uint32_t {
		NONE = 0,
		WHITE,
		BLACK
	};

	enum class TargetType : uint8_t {
		ANY = 0,	// Field dosent matter
		FREE,		// Field musst be free
		OPPONENT	// Field musst have an enemy piece on it
	};

	enum class PathMode : uint8_t {
		LINEAR = 0,   // Moves along the direction vector simultaneously on all axes (e.g. rook, bishop, queen)
		AXIS_ORDER   // Moves one axis at a time; axis order is defined by PriorityAxis
	};

	enum class PriorityAxis : uint8_t {
		X = 0, // Moves along the x-axis first, then along the y-axis
		Y      // Moves along the y-axis first, then along the x-axis
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

template<>
static inline std::string FormatUtils::toString<CoreChess::FieldType>(CoreChess::FieldType field) {
	switch (field) {
	case CoreChess::FieldType::NONE:	return "None";
	case CoreChess::FieldType::WHITE:	return "White";
	case CoreChess::FieldType::BLACK:	return "Black";
	default:							return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<CoreChess::TargetType>(CoreChess::TargetType target) {
	switch (target)
	{
	case CoreChess::TargetType::ANY:		return "Any";
	case CoreChess::TargetType::FREE:		return "Free";
	case CoreChess::TargetType::OPPONENT:	return "Opponent";
	default:								return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<CoreChess::PathMode>(CoreChess::PathMode path) {
	switch (path) {
	case CoreChess::PathMode::LINEAR:		return "Linear";
	case CoreChess::PathMode::AXIS_ORDER:	return "Axis order";
	default:								return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<CoreChess::PriorityAxis>(CoreChess::PriorityAxis axis) {
	switch (axis) {
	case CoreChess::PriorityAxis::X:	return "X";
	case CoreChess::PriorityAxis::Y:	return "Y";
	default:							return "UNKOWN";
	}
}