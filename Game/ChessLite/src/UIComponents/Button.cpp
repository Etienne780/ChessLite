#include <SDLCoreLib/SDLCoreUI.h>

#include "UIComponents/Button.h"
#include "Styles/Comman/Style.h"

namespace UI = SDLCore::UI;

namespace UIComponent {

	bool DrawButton(
		std::string key,
		const char* text,
		const UI::UIStyle& style
	) {
		UI::BeginFrame(UI::UIKey(key), style);
		{
			UI::Text(UI::UIKey("label"), text, Style::commanTextBase);
		}
		UI::UIEvent event = UI::EndFrame();
		return event.IsClick();
	}

}