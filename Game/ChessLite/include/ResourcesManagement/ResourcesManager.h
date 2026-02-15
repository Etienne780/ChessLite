#pragma once
#include <SDLCoreLib/SDLCore.h>
#include <unordered_map>
#include "Type.h"

#include "ResourceLoader.h"

class ResourcesManager {
public:
    void RegisterLoadedResources(ResourceLoader& loader);

    void RegisterTexture(const std::string& key, std::shared_ptr<SDLCore::Texture> texture);
    void RegisterSound(const std::string& key, std::shared_ptr<SDLCore::SoundClip> sound);

    std::shared_ptr<SDLCore::Texture> GetTexture(const std::string& key);
    std::shared_ptr<SDLCore::SoundClip> GetSound(const std::string& key);

private:
    std::unordered_map<std::string, std::shared_ptr<SDLCore::Texture>> m_textureAssets;
    std::unordered_map<std::string, std::shared_ptr<SDLCore::SoundClip>> m_soundAssets;

    template<typename T>
    void AddAssetInternal(
        const std::string& key,
        std::unordered_map<std::string, std::shared_ptr<T>>& map,
        std::shared_ptr<T> asset)
    {
#ifndef NDEBUG
        if (map.find(key) != map.end())
            Log::Warn("ResourcesManager: Asset with key '{}' already exists!", key);
#endif

        map[key] = std::move(asset);
    }

    template<typename T>
    std::shared_ptr<T> GetAssetInternal(
        std::unordered_map<std::string, std::shared_ptr<T>>& map,
        const std::string& key)
    {
        auto it = map.find(key);
        if (it == map.end())
            return nullptr;

        return it->second;
    }
};