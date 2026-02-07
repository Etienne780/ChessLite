#include "Styles/Common/Space.h"

namespace Style {

	void Common_InitSpaces() {
		typedef CoreUI::UIRegistry UIReg;

		commanSpaceXS = UIReg::RegisterNumber(4.0f);
		commanSpaceS = UIReg::RegisterNumber(8.0f);
		commanSpaceM = UIReg::RegisterNumber(12.0f);
		commanSpaceL = UIReg::RegisterNumber(16.0f);
		commanSpaceXL = UIReg::RegisterNumber(24.0f);

		commanBtnPaddingX = UIReg::RegisterNumber(14.0f);
		commanBtnPaddingY = UIReg::RegisterNumber(8.0f);
	}
}
