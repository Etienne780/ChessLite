#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

#include "Layer.h"

namespace Layers {

	class EscapeMenuLayer : public Layer {
	public:
		EscapeMenuLayer() = default;
		~EscapeMenuLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;

	};

}