#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

namespace Style {

	namespace CoreUI = SDLCore::UI;

	// Board colors
	extern CoreUI::UIColorID commanBoardLight;
	extern CoreUI::UIColorID commanBoardDark;

	// UI base colors
	extern CoreUI::UIColorID commanUIBackground;
	extern CoreUI::UIColorID commanUIPanel;

	// Button colors
	extern CoreUI::UIColorID commanBtnNormal;
	extern CoreUI::UIColorID commanBtnHover;
	extern CoreUI::UIColorID commanBtnPressed;
	extern CoreUI::UIColorID commanBtnDisabled;

	// Accent and highlights
	extern CoreUI::UIColorID commanAccent;
	extern CoreUI::UIColorID commanHighlightMove;
	extern CoreUI::UIColorID commanHighlightSelected;
	extern CoreUI::UIColorID commanHighlightCheck;

	void Common_InitColors();
}
