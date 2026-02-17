#pragma once
#include <functional>
#include <vector>
#include <memory>

#include "Type.h"

class AppContext;

enum class LayerID {
	MAIN_MENU = 0,
	OPTIONS_MENU,
	ESCAPE_MENU,
	GAME,
	GAME_RESULT,
	START_LOAD,
	AI_VISUALIZER
};

class Layer {
public:
	Layer() = default;
	virtual ~Layer() = default;

	/*
	* @brief Gets called on layer creation
	*/
	virtual void OnStart(AppContext* ctx) = 0;

	/*
	* @brief Gets called every frame of the application
	*/
	virtual void OnUpdate(AppContext* ctx) = 0;

	/*
	* @brief Gets called every frame of the application, after update
	*/
	virtual void OnRender(AppContext* ctx) = 0;

	/*
	* @brief Gets called every frame of the application, after Render, Setups base UIctx
	*/
	virtual void OnUIRender(AppContext* ctx) {}

	/*
	* @brief Gets called when the layer closes quit (layers are being closed on application quit)
	*/
	virtual void OnQuit(AppContext* ctx) = 0;

	/*
	* @brief Needs to be override and set to the correct layer id
	*/
	virtual LayerID GetLayerID() const = 0;

};

enum class LayerCmdType {
	PUSH,
	POP,
	CLEAR
};

struct LayerCommand {
	using FactoryFunc = std::function<std::unique_ptr<Layer>()>;

	LayerCmdType type;
	FactoryFunc factory = nullptr;


	LayerCommand(LayerCmdType _type)
		: type(_type) {
	}
	LayerCommand(LayerCmdType _type, FactoryFunc _factory)
		: type(_type), factory(_factory) {
	}
};