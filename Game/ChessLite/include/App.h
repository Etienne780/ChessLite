#pragma once
#include <vector>

#include <SDLCoreLib/SDLCore.h>
#include <SDLCoreLib/SDLCoreUI.h>

#include "LayerSystem/Layer.h"
#include "AppContext.h"
#include "LayerSystem/LayerEventBus.h"

class App : public SDLCore::Application {
public:
	App();
	
	static App* GetInstance();

	void OnStart() override;
	void OnUpdate() override;
	void OnQuit() override;

	/*
	* @brief Pushes a layer on to the layer cmd stack. 
	* 
	* note: Layer will not be directly created
	*/
	template<typename T, typename ...Args>
	void PushLayer(Args&& ...args) {
		// Capture arguments in a tuple to safely forward them later
		auto argsTuple = std::make_shared<std::tuple<std::decay_t<Args>...>>(std::forward<Args>(args)...);

		m_layerCommands.emplace_back(LayerCommand(
			LayerCmdType::PUSH,
			[argsTuple]() {
				return std::apply([](auto&& ...innerArgs) {
					return std::make_unique<T>(std::forward<decltype(innerArgs)>(innerArgs)...);
					}, *argsTuple);
			}
		));
	}

	template<LayerEventType type, typename Func>
	LayerEventSubscriptionID SubscribeToLayerEvent(Func&& func) {
		static_assert(std::is_invocable_v<Func, const LayerEvent&>,
			"Callback must be callable with const LayerEvent&");

		return m_context.eventBus.Subscribe(type, std::forward<Func>(func));
	}

	bool UnsubscribeToLayerEvent(LayerEventSubscriptionID id);

	bool UnsubscribeToLayerEvent(LayerEventType type, LayerEventSubscriptionID id);

	/*
	* @brief Pops the top most layer from the stack
	*
	* note: Layer will not be directly poped
	*/
	void PopLayer();

	void ClearLayers();

	SDLCore::WindowID GetWinID() const;
	size_t GetLayerCount() const;

private:
	SDLCore::WindowID m_winID;
	
	AppContext m_context{ this };
	std::vector<std::unique_ptr<Layer>> m_layerStack;
	std::vector<LayerCommand> m_layerCommands;
	SDLCore::UI::UIContext* m_UICtx = nullptr;

	void ProcessLayerCommands();

	template<typename Func>
	void ForeachLayer(Func&& func) {
		for (auto& layer : m_layerStack) {
			func(*layer);
		}
	}
};