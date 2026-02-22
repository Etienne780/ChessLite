#include "SkinManager.h"
#include "App.h"

void SkinManager::SetCurrentSkinType(ChessSkinType type) {
	m_currentSkinType = type;
}

ChessSkinType SkinManager::GetCurrentSkin() const {
	return m_currentSkinType;
}

std::shared_ptr<SDLCore::Texture> SkinManager::GetSkinLight() const {
	return GetSkinInternal(m_currentSkinType, LIGHT_SKIN);
}

std::shared_ptr<SDLCore::Texture> SkinManager::GetSkinDark() const {
	return GetSkinInternal(m_currentSkinType, !LIGHT_SKIN);
}

std::shared_ptr<SDLCore::Texture> SkinManager::GetSkinLightForType(ChessSkinType type) const {
	return GetSkinInternal(type, LIGHT_SKIN);
}

std::shared_ptr<SDLCore::Texture> SkinManager::GetSkinDarkForType(ChessSkinType type) const {
	return GetSkinInternal(type, !LIGHT_SKIN);
}

SDLCore::UI::UITextureID SkinManager::GetSkinLightTextureID() const {
	return GetSkinTextureIDInternal(m_currentSkinType, LIGHT_SKIN);
}

SDLCore::UI::UITextureID SkinManager::GetSkinDarkTextureID() const {
	return GetSkinTextureIDInternal(m_currentSkinType, !LIGHT_SKIN);
}

SDLCore::UI::UITextureID SkinManager::GetSkinLightTextureIDForType(ChessSkinType type) const {
	return GetSkinTextureIDInternal(type, LIGHT_SKIN);
}

SDLCore::UI::UITextureID SkinManager::GetSkinDarkTextureIDForType(ChessSkinType type) const {
	return GetSkinTextureIDInternal(type, !LIGHT_SKIN);
}

const std::vector<ChessSkinType>& SkinManager::GetSkinTypes() const {
	if (m_skinTypesDirty) {
		m_skinTypesDirty = false;

		m_cachedSkinTypes.clear();
		m_cachedSkinTypes.reserve(m_skins.size());
		for (const auto& [type, _] : m_skins) {
			m_cachedSkinTypes.emplace_back(type);
		}
	}
	return m_cachedSkinTypes;
}

void SkinManager::AddSkin(ChessSkinType type, const std::string& light, const std::string& dark) {
	m_skinTypesDirty = true;

	AppContext* ctx = App::GetContext();
	if (!ctx) {
		Log::Error("SkinManager: AppContext was nullptr!");
		return;
	}

	auto whiteTextureID = SDLCore::UI::UIRegistry::RegisterTexture(
		*ctx->resourcesManager.GetTexture(light).get()
	);
	auto blackTextureID = SDLCore::UI::UIRegistry::RegisterTexture(
		*ctx->resourcesManager.GetTexture(dark).get()
	);

	m_skins[type] = SkinPair{ light, dark, whiteTextureID, blackTextureID };
}

std::shared_ptr<SDLCore::Texture> SkinManager::GetSkinInternal(ChessSkinType type, bool light) const {
	auto it = m_skins.find(type);

	if (it == m_skins.end()) {
		Log::Error("SkinManager: Skin not found for current type '{}'!", type);
		return nullptr;
	}

	AppContext* ctx = App::GetContext();
	if (!ctx) {
		Log::Error("SkinManager: AppContext was nullptr!");
		return nullptr;
	}

	const std::string& key = light ? it->second.white : it->second.black;
	return ctx->resourcesManager.GetTexture(key);
}

SDLCore::UI::UITextureID SkinManager::GetSkinTextureIDInternal(ChessSkinType type, bool light) const {
	auto it = m_skins.find(type);

	if (it == m_skins.end()) {
		Log::Error("SkinManager: Skin not found for current type '{}'!", type);
		return {};
	}

	auto textureID = (light) ? 
		it->second.whiteTextureID :
		it->second.blackTextureID;
	return textureID;
}