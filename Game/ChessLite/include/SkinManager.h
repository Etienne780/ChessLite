#pragma once
#include <tuple>
#include <unordered_map>
#include <SDLCoreLib/SDLCore.h>
#include <SDLCoreLib/SDLCoreUI.h>
#include "Type.h"

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

	ChessSkinType m_currentSkinType = ChessSkinType::DEVIL;
	std::unordered_map<ChessSkinType, SkinPair> m_skins;
    mutable std::vector<ChessSkinType> m_cachedSkinTypes;
    mutable bool m_skinTypesDirty = false;

	std::shared_ptr<SDLCore::Texture> GetSkinInternal(ChessSkinType type, bool light) const;
    SDLCore::UI::UITextureID GetSkinTextureIDInternal(ChessSkinType type, bool light) const;
};