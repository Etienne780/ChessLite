#include "Internal/TextureManager.h"
#include "Types/TextureSurface.h"

namespace SDLCore {

    TextureSurface::TextureSurface(SDL_Surface* surface) {
        m_id = TextureManager::GetInstance().RegisterTexture(surface);
    }

    TextureSurface::TextureSurface(const TextureSurface& other)
        : m_id(other.m_id) {
        TextureManager::GetInstance().IncreaseRef(m_id);
    }

    TextureSurface::TextureSurface(TextureSurface&& other) noexcept
        : m_id(other.m_id){
        other.m_id.value = SDLCORE_INVALID_ID;
    }

    TextureSurface::~TextureSurface() {
        if (m_id != SDLCORE_INVALID_ID)
            TextureManager::GetInstance().DecreaseRef(m_id);
    }

    TextureSurface& TextureSurface::operator=(const TextureSurface& other) {
        if (this == &other)
            return *this;

        if (m_id != SDLCORE_INVALID_ID)
            TextureManager::GetInstance().DecreaseRef(m_id);

        m_id = other.m_id;

        if (m_id != SDLCORE_INVALID_ID)
            TextureManager::GetInstance().IncreaseRef(m_id);

        return *this;
    }

    TextureSurface& TextureSurface::operator=(TextureSurface&& other) noexcept {
        if (this == &other)
            return *this;
        
        if (m_id != SDLCORE_INVALID_ID)
            TextureManager::GetInstance().DecreaseRef(m_id);

        m_id = other.m_id;
        other.m_id.SetInvalid();

        return *this;
    }

	bool TextureSurface::IsInvalid() {
		return (m_id == SDLCORE_INVALID_ID || GetSurface() == nullptr);
	}

	TextureID TextureSurface::GetID() const {
		return m_id;
	}
	
	SDL_Surface* TextureSurface::GetSurface() const {
        return TextureManager::GetInstance().GetSurface(m_id);
	}

}