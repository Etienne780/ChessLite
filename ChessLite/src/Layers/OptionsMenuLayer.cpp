#include "Layers/OptionsMenuLayer.h"
#include "Layers/MainMenuLayer.h"

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

		m_StyleButton
			.Merge(Style::commanBTNBase)
			.SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
			.SetValue(Prop::size, 250.0f, 75.0f);
	}

	void OptionsMenuLayer::OnUpdate(AppContext* ctx) {

	}

	void OptionsMenuLayer::OnRender(AppContext* ctx) {
		
	}

	void OptionsMenuLayer::OnUIRender(AppContext* ctx) {
		typedef UI::UIKey Key;

		UI::BeginFrame(Key("options_menu_overlay"), Style::commanOverlay);
		{
			if (UIComp::DrawButton("btn_back", "Back", m_StyleButton)) {
				Log::Debug("OptinsMenu: Back");
				ctx->app->PopLayer();
			}
		}
		UI::EndFrame();
	}

	void OptionsMenuLayer::OnQuit(AppContext* ctx) {
		
	}

}