#pragma once
#include <CoreLib/FormatUtils.h>
#include <CoreLib/CoreID.h>
#include <CoreLib/IDManager.h>
#include <SDLCoreLib/Types/Types.h>

#include "ChessOptions.h"

using CoreAppIDManager = IDManager<uint32_t, SDLCORE_INVALID_ID>;

template<typename Tag>
using CoreAppID = CoreID<uint32_t, SDLCORE_INVALID_ID, Tag>;

struct LayerEventSubscriptionTag {};
struct AgentTag {};

using LayerEventSubscriptionID = CoreAppID<LayerEventSubscriptionTag>;
using AgentID = CoreAppID<AgentTag>;

enum class ChessSkinType {
	UNKOWN = 0,
	BIG,
	CLASSIC,
	DEVIL,
	PIXEL,
	SPACE
};

enum class PlayerType {
	PLAYER = 0,
	AI
};

enum class ResourceType {
	UNKOWN = 0,
	TEXTURE,
	DATA_OTN,
	AUDIO
};

enum class AgentSelectMode {
	AGENT_1_ONLY = 0,
	AGENT_2_ONLY,
	BOTH_AGENTS
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
	case ChessSkinType::BIG:	return "Eckig";
	case ChessSkinType::CLASSIC:			return "Light";
	case ChessSkinType::DEVIL:			return "Neo";
	case ChessSkinType::PIXEL:			return "Simple";
	case ChessSkinType::SPACE:			return "Space";
	case ChessSkinType::UNKOWN:
	default:							return "UNKNOWN";
	}
}