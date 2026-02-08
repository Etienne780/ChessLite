#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

namespace Style {

	namespace CoreUI = SDLCore::UI;

	extern CoreUI::UIStyle commanRoot;

	extern CoreUI::UIStyle commanStretchX;
	extern CoreUI::UIStyle commanStretchY;
	extern CoreUI::UIStyle commanStretch;

	extern CoreUI::UIStyle commanBox;
	extern CoreUI::UIStyle commanOverlay;

	extern CoreUI::UIStyle commanBoardTileWhite;
	extern CoreUI::UIStyle commanBoardTileBlack;

	extern CoreUI::UIStyle commanBTNBase;
	extern CoreUI::UIStyle commanBTNTabNormal;
	extern CoreUI::UIStyle commanBTNTabActive;

	extern CoreUI::UIStyle commanTextBase;
	extern CoreUI::UIStyle commanTextSmall;
	extern CoreUI::UIStyle commanTextTitle;


	void Comman_InitStyles();
	
}