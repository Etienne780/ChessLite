#pragma once
#include <CoreLib/FormatUtils.h>
#include <CoreLib/CoreID.h>
#include <CoreLib/IDManager.h>
#include <SDLCoreLib/Types/Types.h>

using CoreAppIDManager = IDManager<uint32_t, SDLCORE_INVALID_ID>;

template<typename Tag>
using CoreAppID = CoreID<uint32_t, SDLCORE_INVALID_ID, Tag>;

struct LayerEventSubscriptionTag {};
struct AgentTag {};

using LayerEventSubscriptionID = CoreAppID<LayerEventSubscriptionTag>;
using AgentID = CoreAppID<AgentTag>;

struct ChessOptions {
	bool showPossibleMoves = true;

	ChessOptions() = default;
	ChessOptions(const ChessOptions& options) = default;
};

enum class ChessSkinType {
	UNKOWN = 0,
	CHESS_ECKIG,
	LIGHT,
	NEO,
	SIMPLE,
	SPACE
};

enum class PlayerType {
	PLAYER = 0,
	AI
};

enum class ResourceType {
	UNKOWN = 0,
	TEXTURE,
	AUDIO
};

template<>
static inline std::string FormatUtils::toString<PlayerType>(PlayerType type) {
	switch (type)
	{
	case PlayerType::PLAYER:	return "Player";
	case PlayerType::AI:		return "AI";
	default:					return "UNKNOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<ChessSkinType>(ChessSkinType type) {
	switch (type) {
	case ChessSkinType::CHESS_ECKIG:	return "Eckig";
	case ChessSkinType::LIGHT:			return "Light";
	case ChessSkinType::NEO:			return "Neo";
	case ChessSkinType::SIMPLE:			return "Simple";
	case ChessSkinType::SPACE:			return "Space";
	case ChessSkinType::UNKOWN:
	default:							return "UNKNOWN";
	}
}