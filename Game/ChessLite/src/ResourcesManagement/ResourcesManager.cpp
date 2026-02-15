#include "ResourcesManagement/ResourcesManager.h"

void ResourcesManager::RegisterLoadedResources(ResourceLoader& loader) {
    loader.Wait();

    for (const auto& texture : loader.GetTextures())
        RegisterTexture(texture.key, texture.asset);

    for (const auto& sound : loader.GetSounds())
        RegisterSound(sound.key, sound.asset);
}

void ResourcesManager::RegisterTexture(const std::string& key, std::shared_ptr<SDLCore::Texture> texture) {
    AddAssetInternal(key, m_textureAssets, std::move(texture));
}

void ResourcesManager::RegisterSound(const std::string& key, std::shared_ptr<SDLCore::SoundClip> sound) {
    AddAssetInternal(key, m_soundAssets, std::move(sound));
}

std::shared_ptr<SDLCore::Texture> ResourcesManager::GetTexture(const std::string& key) {
    return GetAssetInternal(m_textureAssets, key);
}

std::shared_ptr<SDLCore::SoundClip> ResourcesManager::GetSound(const std::string& key) {
    return GetAssetInternal(m_soundAssets, key);
}
