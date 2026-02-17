#include "Styles/Comman/Color.h"

namespace Style {

	// Board colors
	CoreUI::UIColorID commanColorBoardLight;
	CoreUI::UIColorID commanColorBoardDark;

	// UI base colors
	CoreUI::UIColorID commanColorUIBackground;
	CoreUI::UIColorID commanColorUIBackgroundLight;
	CoreUI::UIColorID commanColorUIBackgroundDark;

	CoreUI::UIColorID commanColorUIPanel;
	CoreUI::UIColorID commanColorUIPanelLight;
	CoreUI::UIColorID commanColorUIPanelDark;

	CoreUI::UIColorID commanColorOverlay;

	CoreUI::UIColorID commanColorOutlineLight;
	CoreUI::UIColorID commanColorOutlineDark;

	// Button colors
	CoreUI::UIColorID commanColorBtnNormal;
	CoreUI::UIColorID commanColorBtnHover;
	CoreUI::UIColorID commanColorBtnPressed;
	CoreUI::UIColorID commanColorBtnDisabled;

	// Accent and highlights
	CoreUI::UIColorID commanColorAccent;
	CoreUI::UIColorID commanColorAccentDark;
	CoreUI::UIColorID commanColorAccentLight;

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

		commanColorUIBackgroundLight = UIReg::RegisterColor(Vector4(40, 40, 40, 255));
		commanColorUIBackgroundDark = UIReg::RegisterColor(Vector4(20, 20, 20, 255));

		commanColorUIPanelLight = UIReg::RegisterColor(Vector4(55, 55, 55, 255));
		commanColorUIPanelDark = UIReg::RegisterColor(Vector4(35, 35, 35, 255));

		commanColorOutlineLight = UIReg::RegisterColor(Vector4(60, 60, 60, 255));
		commanColorOutlineDark = UIReg::RegisterColor(Vector4(25, 25, 25, 255));

		commanColorBtnNormal = UIReg::RegisterColor(Vector4(70, 70, 70, 255));
		commanColorBtnHover = UIReg::RegisterColor(Vector4(90, 90, 90, 255));
		commanColorBtnPressed = UIReg::RegisterColor(Vector4(110, 110, 110, 255));
		commanColorBtnDisabled = UIReg::RegisterColor(Vector4(25, 25, 25, 255));

		commanColorAccent = UIReg::RegisterColor(Vector4(49, 80, 173, 255));
		commanColorAccentDark = UIReg::RegisterColor(Vector4(35, 60, 130, 255));
		commanColorAccentLight = UIReg::RegisterColor(Vector4(100, 130, 220, 255));

		commanColorHighlightMove = UIReg::RegisterColor(Vector4(148, 219, 227, 160));
		commanColorHighlightSelected = UIReg::RegisterColor(Vector4(255, 215, 0, 160));
		commanColorHighlightCheck = UIReg::RegisterColor(Vector4(200, 40, 40, 180));

		commanColorTextPrimary = UIReg::RegisterColor(Vector4(230, 230, 230, 255));
	}
}
