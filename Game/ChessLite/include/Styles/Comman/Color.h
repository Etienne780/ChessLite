#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

namespace Style {

	namespace CoreUI = SDLCore::UI;

	// Board colors
	extern CoreUI::UIColorID commanColorBoardLight;
	extern CoreUI::UIColorID commanColorBoardDark;

	// UI base colors
	extern CoreUI::UIColorID commanColorUIBackground;
	extern CoreUI::UIColorID commanColorUIBackgroundLight;
	extern CoreUI::UIColorID commanColorUIBackgroundDark;

	extern CoreUI::UIColorID commanColorUIPanel;
	extern CoreUI::UIColorID commanColorUIPanelLight;
	extern CoreUI::UIColorID commanColorUIPanelDark;

	extern CoreUI::UIColorID commanColorOverlay;

	extern CoreUI::UIColorID commanColorOutlineLight;
	extern CoreUI::UIColorID commanColorOutlineDark;

	// Button colors
	extern CoreUI::UIColorID commanColorBtnNormal;
	extern CoreUI::UIColorID commanColorBtnHover;
	extern CoreUI::UIColorID commanColorBtnPressed;
	extern CoreUI::UIColorID commanColorBtnDisabled;

	// Accent and highlights
	extern CoreUI::UIColorID commanColorAccent;
	extern CoreUI::UIColorID commanColorAccentDark;
	extern CoreUI::UIColorID commanColorAccentLight;

	extern CoreUI::UIColorID commanColorHighlightMove;
	extern CoreUI::UIColorID commanColorHighlightSelected;
	extern CoreUI::UIColorID commanColorHighlightCheck;

	// text
	extern CoreUI::UIColorID commanColorTextPrimary;

	void Comman_InitColors();
}
