#include "LayerSystem/Layers/OptionsMenuLayer.h"
#include "LayerSystem/Layers/MainMenuLayer.h"

#include "App.h"
#include "Styles/Comman/Style.h"
#include "Styles/Comman/Color.h"
#include "Styles/Comman/Space.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;

namespace Layers {

	void OptionsMenuLayer::OnStart(AppContext* ctx) {
		namespace Prop = UI::Properties;

		m_styleRoot
			.Merge(Style::commanRoot)
			.SetValue(Prop::positionType, UI::UIPositionType::ABSOLUTE)
			.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER);
	}

	void OptionsMenuLayer::OnUpdate(AppContext* ctx) {

	}

	void OptionsMenuLayer::OnRender(AppContext* ctx) {
		
	}

	void OptionsMenuLayer::OnUIRender(AppContext* ctx) {
		typedef UI::UIKey Key;

		UI::BeginFrame(Key("options_menu"), m_styleRoot);
		{
			if (UIComp::DrawButton("btn_back", "Back", Style::commanBTNBase)) {
				Log::Debug("OptinsMenu: Back");
				ctx->app->PopLayer();
			}
		}
		UI::EndFrame();
	}

	void OptionsMenuLayer::OnQuit(AppContext* ctx) {
		
	}

	LayerID OptionsMenuLayer::GetLayerID() const {
		return LayerID::OPTIONS_MENU;
	}

}