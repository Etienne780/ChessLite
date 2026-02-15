#include "SkinManager.h"
#include "App.h"

void SkinManager::SetCurrentSkinType(ChessSkinType type) {
	m_currentSkinType = type;
}

ChessSkinType SkinManager::GetCurrentSkin() const {
	return m_currentSkinType;
}

std::shared_ptr<SDLCore::Texture> SkinManager::GetSkinLight() const {
	return GetSkinInternal(LIGHT_SKIN);
}

std::shared_ptr<SDLCore::Texture> SkinManager::GetSkinDark() const {
	return GetSkinInternal(!LIGHT_SKIN);
}

void SkinManager::AddSkin(ChessSkinType type, const std::string& light, const std::string& dark) {
	m_skins[type] = SkinPair{ light, dark };
}

std::shared_ptr<SDLCore::Texture> SkinManager::GetSkinInternal(bool light) const {
	auto it = m_skins.find(m_currentSkinType);

	if (it == m_skins.end()) {
		Log::Error("SkinManager: Skin not found for current type '{}'!", m_currentSkinType);
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