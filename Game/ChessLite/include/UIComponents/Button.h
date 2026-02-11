#pragma once
#include <string>

namespace SDLCore::UI {
	class UIStyle;
}

namespace UIComponent {

	bool DrawButton(
		std::string key,
		const char* text,
		const SDLCore::UI::UIStyle& style
	);

}