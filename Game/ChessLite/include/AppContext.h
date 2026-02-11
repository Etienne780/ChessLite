#pragma once
#include "App.h"
#include "LayerSystem/LayerEventBus.h"

class App;
class AppContext {
friend class App;
public:
	AppContext(App* _app)
		: app(_app) {
	}

	App* app;

private:
	LayerEventBus eventBus;
};