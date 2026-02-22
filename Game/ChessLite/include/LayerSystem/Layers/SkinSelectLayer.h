#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

#include "LayerSystem/Layer.h"

namespace Layers {

	class SkinSelectLayer : public Layer {
	public:
		SkinSelectLayer() = default;
		~SkinSelectLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		SDLCore::UI::UIStyle m_styleTitle;
		SDLCore::UI::UIStyle m_styleGridContainer;
		SDLCore::UI::UIStyle m_styleGridRow;
		SDLCore::UI::UIStyle m_styleItemNormal;
		SDLCore::UI::UIStyle m_styleItemSelected;
		SDLCore::UI::UIStyle m_styleItemInner;
	};

}