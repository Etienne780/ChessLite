#include "Styles/Common/Color.h"

namespace Style {

	void Common_InitColors() {
		typedef CoreUI::UIRegistry UIReg;

		commanBoardLight = UIReg::RegisterColor(Vector4(240, 217, 181, 255));
		commanBoardDark = UIReg::RegisterColor(Vector4(181, 136, 99, 255));

		commanUIBackground = UIReg::RegisterColor(Vector4(30, 30, 30, 255));
		commanUIPanel = UIReg::RegisterColor(Vector4(45, 45, 45, 255));

		commanBtnNormal = UIReg::RegisterColor(Vector4(70, 70, 70, 255));
		commanBtnHover = UIReg::RegisterColor(Vector4(90, 90, 90, 255));
		commanBtnPressed = UIReg::RegisterColor(Vector4(110, 110, 110, 255));
		commanBtnDisabled = UIReg::RegisterColor(Vector4(60, 60, 60, 160));

		commanAccent = UIReg::RegisterColor(Vector4(200, 170, 90, 255));

		commanHighlightMove = UIReg::RegisterColor(Vector4(0, 180, 0, 120));
		commanHighlightSelected = UIReg::RegisterColor(Vector4(255, 215, 0, 160));
		commanHighlightCheck = UIReg::RegisterColor(Vector4(200, 40, 40, 180));
	}
}
