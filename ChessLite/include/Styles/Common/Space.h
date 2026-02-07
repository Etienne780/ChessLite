#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

namespace Style {

	namespace CoreUI = SDLCore::UI;

	// Common spacing values
	extern CoreUI::UINumberID commanSpaceXS;
	extern CoreUI::UINumberID commanSpaceS;
	extern CoreUI::UINumberID commanSpaceM;
	extern CoreUI::UINumberID commanSpaceL;
	extern CoreUI::UINumberID commanSpaceXL;

	// Common button spacing
	extern CoreUI::UINumberID commanBtnPaddingX;
	extern CoreUI::UINumberID commanBtnPaddingY;

	void Common_InitSpaces();
}
