#pragma once
#include "App.h"

class App;
class AppContext {
public:
	AppContext(App* _app)
		: app(_app) {
	}

	App* app;
};