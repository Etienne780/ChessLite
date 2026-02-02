#pragma once
#include "Layer.h"

namespace Layers {

	class MainMenuLayer : public Layer {
	public:
		MainMenuLayer() = default;
		~MainMenuLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
	};

}