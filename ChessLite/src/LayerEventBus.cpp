#include "LayerEventBus.h"

void LayerEventBus::Subscribe(LayerEventType type, Callback cb) {
    m_subscribers[type].push_back(std::move(cb));
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

        for (auto& cb : it->second) {
            if(cb)
                cb(evt);
        }
    }
    m_eventQueue.clear();
}