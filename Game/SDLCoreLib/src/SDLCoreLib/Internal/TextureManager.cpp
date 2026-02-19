#include <CoreLib/Log.h>
#include "Application.h"
#include "Internal/TextureManager.h"

namespace SDLCore {

	TextureManager::TextureAsset::TextureAsset(SDL_Surface* surfaceVal, uint32_t initRefCount)
		:surface(surfaceVal), refCount(initRefCount) {
	}

	TextureManager::TextureAsset::~TextureAsset() {
		if (!IsSDLQuit())
			SDL_DestroySurface(surface);
		surface = nullptr;
	}

	TextureManager::~TextureManager() {
		ClearAllTexturesAssets();
	}

	TextureManager& TextureManager::GetInstance() {
		static TextureManager instance;
		return instance;
	}

	TextureID TextureManager::RegisterTexture(SDL_Surface* surface) {
		std::lock_guard lock(m_mutex);

		TextureID newID = TextureID(m_idManager.GetNewUniqueIdentifier());

		auto [it, inserted] = m_textureAssets.emplace(
			newID, TextureAsset(surface, 1)
		);

		if (!inserted) {
			SDL_DestroySurface(surface);
			return TextureID(SDLCORE_INVALID_ID);
		}
		return newID;
	}
	
	void TextureManager::IncreaseRef(TextureID id) {
		std::lock_guard lock(m_mutex);

		auto it = m_textureAssets.find(id);
		if (it == m_textureAssets.end())
			return;

		it->second.refCount++;
	}

	void TextureManager::DecreaseRef(TextureID id) {
		std::lock_guard lock(m_mutex);

		auto it = m_textureAssets.find(id);
		if (it == m_textureAssets.end())
			return;

		auto& asset = it->second;
		if (asset.refCount > 0)
			--asset.refCount;

		// delete texture if not used
		if (it->second.refCount == 0) {
			m_idManager.FreeUniqueIdentifier(it->first.value);
			m_textureAssets.erase(it);
		}
	}

	SDL_Surface* TextureManager::GetSurface(TextureID id) const {
		std::lock_guard lock(m_mutex);

		auto it = m_textureAssets.find(id);
		if (it == m_textureAssets.end())
			return nullptr;

		return it->second.surface;
	}

	void TextureManager::ClearAllTexturesAssets() {
		std::lock_guard lock(m_mutex);

		while (!m_textureAssets.empty()) {
			auto it = m_textureAssets.begin();

			m_idManager.FreeUniqueIdentifier(it->first.value);
			m_textureAssets.erase(it);
		}
	}

}