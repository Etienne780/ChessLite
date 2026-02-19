#pragma once
#include <mutex>
#include <unordered_map>
#include <SDL3/SDL.h>

#include "Types/Types.h"

namespace SDLCore {

	class Application;
	class TextureSurface;

	/*
	* Manages how many refs to an SDL_Suface exist and deletes it if none exist anymore
	*/
	class TextureManager {
	friend class Application;
	friend class TextureSurface;
	private:
		struct TextureAsset {
			SDL_Surface* surface = nullptr;
			uint32_t refCount = 0;
			TextureAsset(SDL_Surface* surfaceVal, uint32_t initRefCount);
			~TextureAsset();
		};

		SDLCoreIDManager m_idManager;
		std::unordered_map<TextureID, TextureAsset> m_textureAssets;
		mutable std::mutex m_mutex;

		TextureManager() = default;
		~TextureManager();

		static TextureManager& GetInstance();

		TextureID RegisterTexture(SDL_Surface* surface);
		void IncreaseRef(TextureID id);
		void DecreaseRef(TextureID id);

		SDL_Surface* GetSurface(TextureID id) const;

		// should only be called at programm end
		void ClearAllTexturesAssets();
	};

}