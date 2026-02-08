#include "Styles/Comman/Color.h"

namespace Style {

	// Board colors
	CoreUI::UIColorID commanColorBoardLight;
	CoreUI::UIColorID commanColorBoardDark;

	// UI base colors
	CoreUI::UIColorID commanColorUIBackground;
	CoreUI::UIColorID commanColorUIPanel;
	CoreUI::UIColorID commanColorOverlay;

	// Button colors
	CoreUI::UIColorID commanColorBtnNormal;
	CoreUI::UIColorID commanColorBtnHover;
	CoreUI::UIColorID commanColorBtnPressed;
	CoreUI::UIColorID commanColorBtnDisabled;

	// Accent and highlights
	CoreUI::UIColorID commanColorAccent;
	CoreUI::UIColorID commanColorHighlightMove;
	CoreUI::UIColorID commanColorHighlightSelected;
	CoreUI::UIColorID commanColorHighlightCheck;

	// text
	CoreUI::UIColorID commanColorTextPrimary;

	void Comman_InitColors() {
		typedef CoreUI::UIRegistry UIReg;

		commanColorBoardLight = UIReg::RegisterColor(Vector4(240, 217, 181, 255));
		commanColorBoardDark = UIReg::RegisterColor(Vector4(181, 136, 99, 255));

		commanColorUIBackground = UIReg::RegisterColor(Vector4(30, 30, 30, 255));
		commanColorUIPanel = UIReg::RegisterColor(Vector4(45, 45, 45, 255));
		commanColorOverlay = UIReg::RegisterColor(Vector4(0, 0, 0, 160));

		commanColorBtnNormal = UIReg::RegisterColor(Vector4(70, 70, 70, 255));
		commanColorBtnHover = UIReg::RegisterColor(Vector4(90, 90, 90, 255));
		commanColorBtnPressed = UIReg::RegisterColor(Vector4(110, 110, 110, 255));
		commanColorBtnDisabled = UIReg::RegisterColor(Vector4(60, 60, 60, 160));

		commanColorAccent = UIReg::RegisterColor(Vector4(200, 170, 90, 255));

		commanColorHighlightMove = UIReg::RegisterColor(Vector4(0, 180, 0, 120));
		commanColorHighlightSelected = UIReg::RegisterColor(Vector4(255, 215, 0, 160));
		commanColorHighlightCheck = UIReg::RegisterColor(Vector4(200, 40, 40, 180));

		commanColorTextPrimary = UIReg::RegisterColor(Vector4(230, 230, 230, 255));
	}
}
