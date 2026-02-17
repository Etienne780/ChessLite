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

	bool DrawTabButton(
		std::string key,
		const char* text,
		bool isActive,
		const SDLCore::UI::UIStyle& styleNormal,
		const SDLCore::UI::UIStyle& styleActive
	);

}