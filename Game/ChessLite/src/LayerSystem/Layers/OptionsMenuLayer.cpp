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
		m_localOptions = ctx->options;
		
		namespace Prop = UI::Properties;

		m_styleRoot
			.Merge(Style::commanRoot)
			.SetValue(Prop::positionType, UI::UIPositionType::ABSOLUTE)
			.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER);

		m_styleTitle
			.Merge(Style::commanTextTitle)
			.SetValue(Prop::margin, Vector4(0, 0, 20, 0));

		m_styleRowBTNContainer
			.Merge(Style::commanStretchX)
			.SetValue(Prop::height, 75.0f)
			.SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER)
			.SetValue(Prop::margin, Vector4(0.0f, 0.0f, 8.0f, 0.0f));

		m_styleTrimmer
			.SetValue(Prop::size, m_largeTabBTNWidth, 3.0f)
			.SetValue(Prop::backgroundColor, Style::commanColorUIBackgroundLight)
			.SetValue(Prop::margin, Style::commanSpaceM);

		m_largerBTNTabNormal
			.Merge(Style::commanBTNTabNormal)
			.SetValue(Prop::width, m_largeTabBTNWidth);

		m_largerBTNTabActive
			.Merge(Style::commanBTNTabActive)
			.SetValue(Prop::width, m_largeTabBTNWidth);
	}

	void OptionsMenuLayer::OnUpdate(AppContext* ctx) {
	}

	void OptionsMenuLayer::OnRender(AppContext* ctx) {
	}

	void OptionsMenuLayer::OnUIRender(AppContext* ctx) {
		typedef UI::UIKey Key;

		UI::BeginFrame(Key("options_menu"), m_styleRoot);
		{
			UI::Text(Key("title"), "Options", m_styleTitle);

			DrawToggleOption("Show Possible Moves", m_localOptions.showPossibleMoves);
			DrawToggleOption("Auto retry game", m_localOptions.autoRetryGame);

			UI::BeginFrame(Key("trimline"), m_styleTrimmer);
			UI::EndFrame();
			
			UI::BeginFrame(Key("save_btn_container"), m_styleRowBTNContainer); 
			{
				bool optionsDiff = m_localOptions != ctx->options;

				if (UIComp::DrawButton("btn_save", "Save", Style::commanBTNBase, !optionsDiff)) {
					Log::Debug("OptinsMenu: Save");
					ctx->options = m_localOptions;
					ctx->options.SaveOptions();
				}

				if (UIComp::DrawButton("btn_save_back", "Save and back", Style::commanBTNBase, !optionsDiff)) {
					Log::Debug("OptinsMenu: Save and back");
					ctx->options = m_localOptions;
					ctx->options.SaveOptions();
					ctx->app->PopLayer();
				}
			}
			UI::EndFrame();

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

	void OptionsMenuLayer::DrawToggleOption(const std::string& name, bool& outOption) {
		std::string text = name + ": ";
		text += (outOption) ? "true" : "false";
		if (UIComp::DrawTabButton("btn_" + name, text, outOption, m_largerBTNTabNormal, m_largerBTNTabActive)) {
			outOption = !outOption;
		}
	}

}