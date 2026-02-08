#include "Styles/Comman/Style.h"
#include "Styles/Comman/Space.h"
#include "Styles/Comman/Color.h"

namespace Style {

	CoreUI::UIStyle commanRoot{ "comman_root" };

	CoreUI::UIStyle commanStretchX{ "comman_stretch_x" };
	CoreUI::UIStyle commanStretchY{ "comman_stretch_y" };
	CoreUI::UIStyle commanStretch{ "comman_stretch" };

	CoreUI::UIStyle commanBox{ "comman_box" };
	CoreUI::UIStyle commanOverlay{ "comman_overlay" };

	CoreUI::UIStyle commanBoardTileWhite{ "comman_board_tile_white" };
	CoreUI::UIStyle commanBoardTileBlack{ "comman_board_tile_black" };

	CoreUI::UIStyle commanBTNBase{ "comman_btn_base" };
	CoreUI::UIStyle commanBTNTabNormal{ "comman_tab_normal" };
	CoreUI::UIStyle commanBTNTabActive{ "comman_tab_active" };

	CoreUI::UIStyle commanTextBase{ "comman_text_base" };
	CoreUI::UIStyle commanTextSmall{ "comman_text_small" };
	CoreUI::UIStyle commanTextTitle{ "comman_text_title" };

	void Comman_InitStyles() {
		// Initialize base values
		Comman_InitColors();
		Comman_InitSpaces();

		namespace Prop = CoreUI::Properties;

		// --- Stretch helpers ---
		commanStretchX
			.SetValue(Prop::widthUnit, CoreUI::UISizeUnit::PERCENTAGE_W)
			.SetValue(Prop::width, 100.0f);

		commanStretchY
			.SetValue(Prop::heightUnit, CoreUI::UISizeUnit::PERCENTAGE_H)
			.SetValue(Prop::height, 100.0f);

		commanStretch
			.SetValue(Prop::sizeUnit,
				CoreUI::UISizeUnit::PERCENTAGE,
				CoreUI::UISizeUnit::PERCENTAGE)
			.SetValue(Prop::size, 100.0f, 100.0f);

		// --- Root ---
		commanRoot
			.Merge(commanStretch)
			.SetValue(Prop::layoutDirection, CoreUI::UILayoutDir::COLUMN)
			.SetValue(Prop::positionType, CoreUI::UIPositionType::RELATIVE)
			.SetValue(Prop::backgroundColor, commanColorUIBackground);

		// --- Box / Panel ---
		commanBox
			.Merge(commanStretch)
			.SetValue(Prop::backgroundColor, commanColorUIPanel);
		commanBox.SetValue(Prop::padding, commanSpaceM);

		commanOverlay
			.Merge(commanStretch)
			.SetValue(Prop::positionType, CoreUI::UIPositionType::ABSOLUTE)
			.SetValue(Prop::backgroundColor, commanColorOverlay)
			.SetValue(Prop::align, CoreUI::UIAlignment::CENTER, CoreUI::UIAlignment::CENTER);

		// --- Chess board tiles ---
		commanBoardTileWhite
			.SetValue(Prop::backgroundColor, commanColorBoardLight);
		commanBoardTileWhite.SetActiveState(CoreUI::UIState::HOVER)
			.SetValue(Prop::visualScale, 1.02f, 1.02f);

		commanBoardTileBlack
			.SetValue(Prop::backgroundColor, commanColorBoardDark);
		commanBoardTileBlack.SetActiveState(CoreUI::UIState::HOVER)
			.SetValue(Prop::visualScale, 1.02f, 1.02f);

		// --- Button base ---
		commanBTNBase
			.Merge(commanStretch)
			.SetValue(Prop::align, CoreUI::UIAlignment::CENTER, CoreUI::UIAlignment::CENTER)
			.SetValue(Prop::backgroundColor, commanColorBtnNormal)
			.SetValue(Prop::padding, commanSpaceM)
			.SetValue(Prop::hideOverflow, true, true)
			.SetValue(Prop::duration, 0.15f)
			.SetValue(Prop::durationEasing, CoreUI::UIEasing::EaseInOutSine);

		commanBTNBase.SetActiveState(CoreUI::UIState::HOVER)
			.SetValue(Prop::backgroundColor, commanColorBtnHover);

		commanBTNBase.SetActiveState(CoreUI::UIState::PRESSED)
			.SetValue(Prop::backgroundColor, commanColorBtnPressed);

		commanBTNBase.SetActiveState(CoreUI::UIState::DISABLED)
			.SetValue(Prop::backgroundColor, commanColorBtnDisabled)
			.SetValue(Prop::pointerEvents, false);

		// --- Tab buttons ---
		commanBTNTabNormal
			.Merge(commanBTNBase)
			.SetValue(Prop::backgroundColor, commanColorBtnNormal);

		commanBTNTabActive
			.Merge(commanBTNBase)
			.SetValue(Prop::backgroundColor, commanColorAccent);

		// --- Text ---
		commanTextBase
			.SetValue(Prop::textColor, commanColorTextPrimary)
			.SetValue(Prop::textSize, 36.0f)
			.SetValue(Prop::hitTestTransparent, true);

		commanTextSmall
			.Merge(commanTextBase)
			.SetValue(Prop::textSize, 18.0f);

		commanTextTitle
			.Merge(commanTextBase)
			.SetValue(Prop::textSize, 48.0f);
	}
}