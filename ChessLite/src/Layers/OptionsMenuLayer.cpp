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
		m_UICtx = UI::CreateContext();

		namespace Prop = UI::Properties;

		m_StyleRoot.SetActiveState(UI::UIState::NORMAL)
			.SetValue(Prop::sizeUnit, UI::UISizeUnit::PERCENTAGE, UI::UISizeUnit::PERCENTAGE)
			.SetValue(Prop::size, 100.0f, 100.0f);

		m_StyleButton
			.Merge(Style::commanBTNBase)
			.SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
			.SetValue(Prop::size, 250.0f, 75.0f);
	}

	void OptionsMenuLayer::OnUpdate(AppContext* ctx) {

	}

	void OptionsMenuLayer::OnRender(AppContext* ctx) {
		typedef UI::UIKey Key;
		
		UI::SetContextWindow(m_UICtx, ctx->app->GetWinID());
		UI::BindContext(m_UICtx);
		
		UI::BeginFrame(Key("root"), m_StyleRoot);
		{
			UI::BeginFrame(Key("overlay"), Style::commanOverlay);
			{
				if (UIComp::DrawButton("btn_back", "Back", m_StyleButton)) {
					Log::Debug("OptinsMenu: Back");
					ctx->app->PopLayer();
				}
			}
			UI::EndFrame();
		}
		UI::EndFrame();
	}

	void OptionsMenuLayer::OnQuit(AppContext* ctx) {
		UI::DestroyContext(m_UICtx);
		m_UICtx = nullptr;
	}

}