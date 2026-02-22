#pragma once
#include <tuple>
#include <unordered_map>
#include <SDLCoreLib/SDLCore.h>
#include <SDLCoreLib/SDLCoreUI.h>
#include "Type.h"

namespace SkinKeys {
    // Chess Eckig
    constexpr const char* CHESS_ECKIG_LIGHT = "skin.chess_eckig.light";
    constexpr const char* CHESS_ECKIG_DARK = "skin.chess_eckig.dark";

    // Light
    constexpr const char* LIGHT_LIGHT = "skin.light.light";
    constexpr const char* LIGHT_DARK = "skin.light.dark";

    // Neo
    constexpr const char* NEO_LIGHT = "skin.neo.light";
    constexpr const char* NEO_DARK = "skin.neo.dark";

    // Simple
    constexpr const char* SIMPLE_LIGHT = "skin.simple.light";
    constexpr const char* SIMPLE_DARK = "skin.simple.dark";

    // Space
    constexpr const char* SPACE_LIGHT = "skin.space.light";
    constexpr const char* SPACE_DARK = "skin.space.dark";
}

class SkinManager {
public:
	SkinManager() = default;
	~SkinManager() = default;

	void SetCurrentSkinType(ChessSkinType type);

    ChessSkinType GetCurrentSkin() const;

	std::shared_ptr<SDLCore::Texture> GetSkinLight() const;
	std::shared_ptr<SDLCore::Texture> GetSkinDark() const;

    std::shared_ptr<SDLCore::Texture> GetSkinLightForType(ChessSkinType type) const;
    std::shared_ptr<SDLCore::Texture> GetSkinDarkForType(ChessSkinType type) const;

    SDLCore::UI::UITextureID GetSkinLightTextureID() const;
    SDLCore::UI::UITextureID GetSkinDarkTextureID() const;

    SDLCore::UI::UITextureID GetSkinLightTextureIDForType(ChessSkinType type) const;
    SDLCore::UI::UITextureID GetSkinDarkTextureIDForType(ChessSkinType type) const;

    const std::vector<ChessSkinType>& GetSkinTypes() const;

	void AddSkin(ChessSkinType type, const std::string& light, const std::string& dark);

private:
	struct SkinPair {
		std::string white;
		std::string black;
        SDLCore::UI::UITextureID whiteTextureID;
        SDLCore::UI::UITextureID blackTextureID;
	};

	static inline constexpr bool LIGHT_SKIN = true;

	ChessSkinType m_currentSkinType = ChessSkinType::NEO;
	std::unordered_map<ChessSkinType, SkinPair> m_skins;
    mutable std::vector<ChessSkinType> m_cachedSkinTypes;
    mutable bool m_skinTypesDirty = false;

	std::shared_ptr<SDLCore::Texture> GetSkinInternal(ChessSkinType type, bool light) const;
    SDLCore::UI::UITextureID GetSkinTextureIDInternal(ChessSkinType type, bool light) const;
};