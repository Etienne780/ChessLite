#pragma once
#include <functional>
#include <string>

#include "Type.h"
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
 * Allows layers and systems to subscribe to specific LayerEventType values
 * and receive events emitted by other layers.
 *
 * Events are queued and dispatched explicitly via Dispatch() to ensure
 * deterministic ordering and avoid reentrancy issues.
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
    *
    * @return Subscription identifier used for unsubscription
    */
    LayerEventSubscriptionID Subscribe(LayerEventType type, Callback cb);

    /**
    * @brief Unsubscribe from all layer event types using a subscription ID.
    *
    * @param id Subscription identifier
    * @return true if the subscription was found and removed
    */
    bool Unsubscribe(LayerEventSubscriptionID id);

    /**
    * @brief Unsubscribe from a specific layer event type.
    *
    * @param type Event type
    * @param id   Subscription identifier
    * @return true if the subscription was found and removed
    */
    bool Unsubscribe(LayerEventType type, LayerEventSubscriptionID id);

    /**
    * @brief Emit a layer event.
    *
    * The event is queued and will be delivered on the next call
    * to Dispatch().
    *
    * @param type    Event type
    * @param id      Layer identifier
    * @param payload Optional payload string
    */
    void Emit(LayerEventType type, LayerID id, std::string payload = {});

    /**
    * @brief Dispatch all queued layer events.
    *
    * Invokes all callbacks subscribed to the corresponding event types,
    * then clears the event queue.
    */
    void Dispatch();

private:
    /**
    * @brief Internal subscription entry.
    */
    struct Entry {
        LayerEventSubscriptionID id;
        Callback cb;

        Entry(LayerEventSubscriptionID _id, Callback _cb) 
            : id(_id), cb(_cb) {
        }
    };

    CoreAppIDManager m_idManager;
    std::unordered_map<LayerEventType, std::vector<Entry>> m_subscribers;
    std::vector<LayerEvent> m_eventQueue;

    /**
    * @brief Remove a subscription entry from a vector.
    *
    * @param entrys Vector of subscription entries
    * @param id     Subscription identifier to remove
    * @return true if an entry was removed
    */
    bool DeleteInternal(std::vector<Entry>& entrys, LayerEventSubscriptionID id);
};