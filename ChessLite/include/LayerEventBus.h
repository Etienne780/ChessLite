#pragma once
#include <functional>
#include <string>

#include "Layer.h"

enum class LayerEventType {
    OPENED,
    CLOSED,
    CUSTOM
};

struct LayerEvent {
    LayerEventType type;
    LayerID layerID;
    std::string payload;  /*< optional: custom event */

    LayerEvent() = default;
    LayerEvent(LayerEventType t, LayerID id)
        : type(t), layerID(id) {
    }
    LayerEvent(LayerEventType t, LayerID id, const std::string& pl) 
        : type(t), layerID(id), payload(pl) {
    }
};

/**
* @brief Event bus for layer lifecycle and custom events.
*
* Allows layers and systems to subscribe to specific LayerEventType
* values and receive events emitted by other layers.
*
* Events are queued and dispatched explicitly via Dispatch().
*/
class LayerEventBus {
public:
    /**
    * @brief Callback type used for event subscriptions.
    */
    using Callback = std::function<void(const LayerEvent&)>;

    /**
    * @brief Subscribe to a specific layer event type.
    *
    * @param type Event type to subscribe to
    * @param cb   Callback invoked when the event is dispatched
    */
    void Subscribe(LayerEventType type, Callback cb);

    /**
    * @brief Emit a layer event.
    *
    * The event is queued and will be delivered on the next
    * call to Dispatch().
    *
    * @param event Event to emit
    */
    void Emit(LayerEventType type, LayerID id, std::string payload = {});

    /**
    * @brief Dispatch all queued layer events.
    *
    * Invokes all callbacks subscribed to the corresponding
    * event types, then clears the event queue.
    */
    void Dispatch();

private:
    std::unordered_map<LayerEventType, std::vector<Callback>> m_subscribers;
    std::vector<LayerEvent> m_eventQueue;
};