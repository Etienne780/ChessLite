#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

namespace Style {

	namespace CoreUI = SDLCore::UI;

	static inline CoreUI::UIStyle commanRoot{ "comman_root" };

	static inline CoreUI::UIStyle commanStretchX{ "comman_stretch_x" };
	static inline CoreUI::UIStyle commanStretchY{ "comman_stretch_y" };
	static inline CoreUI::UIStyle commanStretch{ "comman_stretch" };

	static inline CoreUI::UIStyle commanContainer{ "comman_container" };
	static inline CoreUI::UIStyle commanBox{ "comman_box" };

	static inline CoreUI::UIStyle commanBTNBase{ "comman_btn_base" };
	static inline CoreUI::UIStyle commanBTNTabNormal{ "comman_tab_normal" };
	static inline CoreUI::UIStyle commanBTNTabActive{ "comman_tab_active" };

	static inline CoreUI::UIStyle commanTextBase{ "comman_text_base" };

	void Comman_InitStyles();
	
}