#pragma once
#include "App.h"
#include "LayerEventBus.h"

class App;
class AppContext {
public:
	AppContext(App* _app)
		: app(_app) {
	}

	App* app;
	LayerEventBus eventBus;
};