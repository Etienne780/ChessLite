#pragma once
#include <vector>

#include <SDLCoreLib/SDLCore.h>
#include <SDLCoreLib/SDLCoreUI.h>

#include "Layer.h"
#include "AppContext.h"

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
			LayerCmdType::Push,
			[argsTuple]() {
				return std::apply([](auto&& ...innerArgs) {
					return std::make_unique<T>(std::forward<decltype(innerArgs)>(innerArgs)...);
					}, *argsTuple);
			}
		));
	}

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

	bool m_clearLayers = false;

	void ProcessLayerCommands();

	template<typename Func>
	void ForeachLayer(Func&& func) {
		for (auto& layer : m_layerStack) {
			func(*layer);
		}
	}
};