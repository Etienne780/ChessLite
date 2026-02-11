#include "LayerEventBus.h"

LayerEventSubscriptionID LayerEventBus::Subscribe(LayerEventType type, Callback cb) {
    LayerEventSubscriptionID id(m_idManager.GetNewUniqueIdentifier());
    m_subscribers[type].emplace_back(id, std::move(cb));
    return id;
}

bool LayerEventBus::Unsubscribe(LayerEventSubscriptionID id) {
    for (auto& [_, entrys] : m_subscribers) {
        if (DeleteInternal(entrys, id)) {
            m_idManager.FreeUniqueIdentifier(id.value);
            return true;
        }
    }
    return false;
}

bool LayerEventBus::Unsubscribe(LayerEventType type, LayerEventSubscriptionID id) {
    auto it = m_subscribers.find(type);
    if (it == m_subscribers.end()) 
        return false;

    auto& entrys = it->second;
    bool result = DeleteInternal(entrys, id);
    if (result)
        m_idManager.FreeUniqueIdentifier(id.value);
    return result;
}

void LayerEventBus::Emit(LayerEventType type, LayerID id, std::string payload) {
    m_eventQueue.emplace_back(type, id, std::move(payload));
}

void LayerEventBus::Dispatch() {
    if (m_eventQueue.empty())
        return;

    for (const auto& evt : m_eventQueue) {
        auto it = m_subscribers.find(evt.type);
        if (it == m_subscribers.end())
            continue;
        
        auto copy = it->second;
        for (auto& entry : copy) {
            if(entry.cb)
                entry.cb(evt);
                
        }
    }
    m_eventQueue.clear();
}

bool LayerEventBus::DeleteInternal(std::vector<Entry>& entrys, LayerEventSubscriptionID id) {
    size_t s = entrys.size();

    entrys.erase(std::remove_if(entrys.begin(), entrys.end(),
        [id](auto& entry) { return entry.id == id; }),
        entrys.end());

    return s != entrys.size();
}