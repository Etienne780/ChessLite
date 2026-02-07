#pragma once
#include <SDLCoreLib/SDLCoreUI.h>

namespace Style {

	namespace CoreUI = SDLCore::UI;

	// Common spacing values
	static inline CoreUI::UINumberID commanSpaceXS;
	static inline CoreUI::UINumberID commanSpaceS;
	static inline CoreUI::UINumberID commanSpaceM;
	static inline CoreUI::UINumberID commanSpaceL;
	static inline CoreUI::UINumberID commanSpaceXL;

	// Common button spacing
	static inline CoreUI::UINumberID commanBtnPaddingX;
	static inline CoreUI::UINumberID commanBtnPaddingY;

	void Comman_InitSpaces();
}
