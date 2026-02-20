#pragma once
#include <vector>
#include <atomic>
#include <thread>
#include <CoreLib/File.h>
#include <CoreLib/OTNFile.h>
#include <SDLCoreLib/SDLCore.h>

#include "Type.h"

struct ResourceRequest {
    ResourceType type = ResourceType::UNKOWN;
    std::string key;
    SystemFilePath path;

    ResourceRequest(ResourceType type, SystemFilePath path);
    ResourceRequest(ResourceType type, const std::string& key, SystemFilePath path);
};

class ResourceLoader {
public:
    template<typename T>
    struct LoadedAsset {
        std::string key;
        std::shared_ptr<T> asset;

        LoadedAsset(std::string _key, std::shared_ptr<T> _asset)
            : key(std::move(_key)), asset(std::move(_asset)) {
        }

        LoadedAsset(std::shared_ptr<T> _asset)
            : asset(std::move(_asset)) {
        }
    };

    ResourceLoader() = default;
    ResourceLoader(std::vector<ResourceRequest>&& request);
    ~ResourceLoader();

    void AddRequest(const ResourceRequest& request);
    void LoadAsync();
    void Wait();

    bool IsFinished() const;

    size_t GetLoadedCount() const;
    size_t GetTotalCount() const;

    const std::vector<LoadedAsset<SDLCore::Texture>>& GetTextures() const;
    const std::vector<LoadedAsset<SDLCore::SoundClip>>& GetSounds() const;
    const std::vector<LoadedAsset<std::unordered_map<std::string, OTN::OTNObject>>>& GetOTNObjects() const;

private:
    std::thread m_thread;
    std::atomic<bool> m_finishedLoad = true;
    std::atomic<size_t> m_loadedCount{ 0 };
    std::atomic<size_t> m_totalCount{ 0 };

    std::vector<ResourceRequest> m_requests;

    std::vector<LoadedAsset<SDLCore::Texture>> m_textures;
    std::vector<LoadedAsset<SDLCore::SoundClip>> m_sounds;
    std::vector<LoadedAsset<std::unordered_map<std::string, OTN::OTNObject>>> m_otnObjects;

    void LoadInternal();
};

typedef ResourceLoader::LoadedAsset<SDLCore::Texture> LoadedTexture;
typedef ResourceLoader::LoadedAsset<SDLCore::SoundClip> LoadedSoundClip;
typedef ResourceLoader::LoadedAsset<std::unordered_map<std::string, OTN::OTNObject>> LoadedOTNObjects;