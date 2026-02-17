#pragma once
#include <SDLCoreLib/SDLCore.h>

#include "App.h"
#include "LayerSystem/LayerEventBus.h"
#include "ResourcesManagement/ResourcesManager.h"
#include "AI/AgentManager.h"
#include "SkinManager.h"
#include "Type.h"

class App;
class AppContext {
friend class App;
public:
	AppContext(App* _app)
		: app(_app) {
	}

	App* app;
	ResourcesManager resourcesManager;
	SkinManager skinManager;
	ChessOptions options;
	AgentManager agentManager;

	AgentID selectedAgentID1;
	AgentID selectedAgentID2;

	Vector2 refDisplaySize{ 1920.0f, 1080.0f };
	Vector2 displaySize{ 0.0f };
	Vector2 windowSize{ 0.0f };

private:
	LayerEventBus m_eventBus;
};