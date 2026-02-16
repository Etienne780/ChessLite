#include "ResourcesManagement/ResourceLoader.h"

ResourceLoader::ResourceLoader(std::vector<ResourceRequest>&& request)
    : m_requests(std::move(request)) {
}

ResourceLoader::~ResourceLoader() {
    Wait();
}

void ResourceLoader::AddRequest(const ResourceRequest& request) {
    m_requests.push_back(request);
}

void ResourceLoader::LoadAsync() {
    Wait();// Joins old thread

    m_textures.clear();
    m_sounds.clear();
    m_finishedLoad = false;
    m_loadedCount = 0;
    m_totalCount = m_requests.size();
    m_thread = std::thread(&ResourceLoader::LoadInternal, this);
}

void ResourceLoader::Wait() {
    if (m_thread.joinable())
        m_thread.join();
}

bool ResourceLoader::IsFinished() const {
    return m_finishedLoad.load(std::memory_order_acquire);
}

size_t ResourceLoader::GetLoadedCount() const {
    return m_loadedCount;
}

size_t ResourceLoader::GetTotalCount() const {
    return m_totalCount;
}

const std::vector<LoadedTexture>&
ResourceLoader::GetTextures() const {
    return m_textures;
}

const std::vector<LoadedSoundClip>&
ResourceLoader::GetSounds() const {
    return m_sounds;
}

void ResourceLoader::LoadInternal() {
    for (auto& req : m_requests) {
        switch (req.type) {
        case ResourceType::TEXTURE:
            m_textures.emplace_back(
                std::move(req.key),
                std::make_shared<SDLCore::Texture>(req.path)
            );
            break;

        case ResourceType::AUDIO:
            m_sounds.emplace_back(
                std::move(req.key),
                std::make_shared<SDLCore::SoundClip>(req.path)
            );
            break;

        default:
            break;
        }
        m_loadedCount++;
        
        // Delay to slow down loading for testing
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    m_finishedLoad.store(true, std::memory_order_release);
}