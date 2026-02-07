#include "Styles/Comman/Style.h"
#include "Styles/Comman/Space.h"
#include "Styles/Comman/Color.h"

namespace Style {

	void Comman_InitStyles() {
		// --- Inits Base Values for styls ---
		Comman_InitColors();
		Comman_InitSpaces();

		namespace Prop = CoreUI::Properties;

		commanStretchX.SetValue(Prop::widthUnit, CoreUI::UISizeUnit::PERCENTAGE_W)
			.SetValue(Prop::width, 100.0f);

		commanStretchY.SetValue(Prop::heightUnit, CoreUI::UISizeUnit::PERCENTAGE_H)
			.SetValue(Prop::height, 100.0f);

		commanStretch.SetValue(Prop::sizeUnit, CoreUI::UISizeUnit::PERCENTAGE, CoreUI::UISizeUnit::PERCENTAGE)
			.SetValue(Prop::size, 100.0f, 100.0f);


		commanRoot.Merge(commanStretch);
	}

}