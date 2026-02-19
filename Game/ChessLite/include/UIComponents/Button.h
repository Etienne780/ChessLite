#pragma once
#include <string>

namespace SDLCore::UI {
	class UIStyle;
}

namespace UIComponent {

	bool DrawButton(
		const std::string& key,
		const std::string& text,
		const SDLCore::UI::UIStyle& style,
		bool isDisabled = false
	);

	bool DrawTabButton(
		const std::string& key,
		const std::string& text,
		bool isActive,
		const SDLCore::UI::UIStyle& styleNormal,
		const SDLCore::UI::UIStyle& styleActive,
		bool isDisabled = false
	);

}