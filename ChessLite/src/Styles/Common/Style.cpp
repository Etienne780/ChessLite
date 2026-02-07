#include "Styles/Common/Style.h"
#include "Styles/Common/Space.h"
#include "Styles/Common/Color.h"

namespace Style {

	void Common_InitStyles() {
		// --- Inits Base Values for styls ---
		Common_InitColors();
		Common_InitSpaces();

		namespace Prop = CoreUI::Properties;
	}

}