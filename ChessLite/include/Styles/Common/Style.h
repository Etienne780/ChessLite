#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

namespace Style {

	namespace CoreUI = SDLCore::UI;

	extern CoreUI::UIStyle commonStretchX{ "common_Stretch_x" };
	extern CoreUI::UIStyle commonStretchY{ "common_Stretch_y" };
	extern CoreUI::UIStyle commonStretch{ "common_Stretch" };

	extern CoreUI::UIStyle commonContainer{ "common_container" };
	extern CoreUI::UIStyle commonBox{ "common_box" };

	extern CoreUI::UIStyle commonBTNBase{ "common_btn_base" };
	extern CoreUI::UIStyle commonBTNTabNormal{ "common_tab_normal" };
	extern CoreUI::UIStyle commonBTNTabActive{ "common_tab_active" };

	extern CoreUI::UIStyle commonTextBase{ "common_text_base" };

	void Common_InitStyles();
	
}