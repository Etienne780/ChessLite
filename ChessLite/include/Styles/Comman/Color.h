#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

namespace Style {

	namespace CoreUI = SDLCore::UI;

	// Board colors
	static inline CoreUI::UIColorID commanBoardLight;
	static inline CoreUI::UIColorID commanBoardDark;

	// UI base colors
	static inline CoreUI::UIColorID commanUIBackground;
	static inline CoreUI::UIColorID commanUIPanel;

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
