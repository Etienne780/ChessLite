#include <SDLCoreLib/SDLCoreUI.h>

#include "UIComponents/Button.h"
#include "Styles/Comman/Style.h"

namespace UI = SDLCore::UI;

namespace UIComponent {

	bool DrawButton(
		const std::string& key,
		const std::string& text,
		const UI::UIStyle& style,
		bool isDisabled
	) {
		UI::BeginFrame(UI::UIKey(key), style);
		{
			UI::Text(UI::UIKey("label"), text, Style::commanTextBase);

			if (isDisabled) {
				UI::BeginFrame(UI::UIKey("overlay"), Style::commanOverlay);
				UI::EndFrame();
			}
		}
		UI::UIEvent event = UI::EndFrame();
		return event.IsClick();
	}

	bool DrawTabButton(
		const std::string& key,
		const std::string& text,
		bool isActive,
		const SDLCore::UI::UIStyle& styleNormal,
		const SDLCore::UI::UIStyle& styleActive,
		bool isDisabled
	) {
		const SDLCore::UI::UIStyle& style = (isActive) ? styleActive : styleNormal;
		UI::BeginFrame(UI::UIKey(key), style);
		{
			UI::Text(UI::UIKey("label"), text, Style::commanTextBase);

			if (isDisabled) {
				UI::BeginFrame(UI::UIKey("overlay"), Style::commanOverlay);
				UI::EndFrame();
			}
		}
		UI::UIEvent event = UI::EndFrame();
		return event.IsClick();
	}

}