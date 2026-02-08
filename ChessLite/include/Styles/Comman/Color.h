#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

namespace Style {

	namespace CoreUI = SDLCore::UI;

	// Board colors
	static inline CoreUI::UIColorID commanColorBoardLight;
	static inline CoreUI::UIColorID commanColorBoardDark;

	// UI base colors
	static inline CoreUI::UIColorID commanColorUIBackground;
	static inline CoreUI::UIColorID commanColorUIPanel;

	// Button colors
	static inline CoreUI::UIColorID commanBtnNormal;
	static inline CoreUI::UIColorID commanBtnHover;
	static inline CoreUI::UIColorID commanBtnPressed;
	static inline CoreUI::UIColorID commanBtnDisabled;

	// Accent and highlights
	static inline CoreUI::UIColorID commanAccent;
	static inline CoreUI::UIColorID commanHighlightMove;
	static inline CoreUI::UIColorID commanHighlightSelected;
	static inline CoreUI::UIColorID commanHighlightCheck;

	void Comman_InitColors();
}
