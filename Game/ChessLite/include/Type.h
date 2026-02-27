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
struct NetworkMsgTag {};

using LayerEventSubscriptionID = CoreAppID<LayerEventSubscriptionTag>;
using AgentID = CoreAppID<AgentTag>;
using NetworkMsgID = CoreAppID<NetworkMsgTag>;

namespace SkinKeys {

	inline constexpr const char* BIG_LIGHT = "skin.big.light";
	inline constexpr const char* BIG_DARK = "skin.big.dark";

	inline constexpr const char* CLASSIC_LIGHT = "skin.classic.light";
	inline constexpr const char* CLASSIC_DARK = "skin.classic.dark";

	inline constexpr const char* DEVIL_LIGHT = "skin.devil.light";
	inline constexpr const char* DEVIL_DARK = "skin.devil.dark";

	inline constexpr const char* PIXEL_LIGHT = "skin.pixel.light";
	inline constexpr const char* PIXEL_DARK = "skin.pixel.dark";

}

namespace SoundKeys {
	
	inline constexpr const char* MOVE_SOUND = "sound.piece_move";
	inline constexpr const char* CAPTURE_SOUND = "sound.piece_capture";

}

namespace SDLCore::SoundTags {
	
	inline constexpr const char* SFX = "sfx";

}

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