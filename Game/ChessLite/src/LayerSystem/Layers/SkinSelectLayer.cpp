#include "LayerSystem/Layers/SkinSelectLayer.h"
#include "LayerSystem/Layers/MainMenuLayer.h"
#include "App.h"
#include "FilePaths.h"
#include "Styles/Comman/Style.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;

namespace Layers {

    constexpr float GRID_ITEM_WIDTH = 80.0f;
    constexpr float GRID_ITEM_HEIGHT = 80.0f;
    constexpr float GRID_ITEM_GAP = 10.0f;
    constexpr int   GRID_COLUMNS = 4;

    void SkinSelectLayer::OnStart(AppContext* ctx) {
        namespace Prop = UI::Properties;

        m_styleTitle
            .Merge(Style::commanTextTitle)
            .SetValue(Prop::margin, Vector4(0, 0, 20, 0));

        m_styleGridContainer
            .SetValue(Prop::size, 400, 400)
            .SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
            .SetValue(Prop::align, UI::UIAlignment::START, UI::UIAlignment::START)
            .SetValue(Prop::padding, Vector4(10, 10, 10, 10));

        m_styleGridRow
            .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
            .SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
            .SetValue(Prop::size,
                (float)GRID_COLUMNS * (GRID_ITEM_WIDTH + GRID_ITEM_GAP),
                GRID_ITEM_HEIGHT + GRID_ITEM_GAP)
            .SetValue(Prop::align, UI::UIAlignment::START, UI::UIAlignment::START);

        m_styleItemNormal
            .SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
            .SetValue(Prop::size, GRID_ITEM_WIDTH, GRID_ITEM_HEIGHT)
            .SetValue(Prop::margin, Vector4(GRID_ITEM_GAP / 2, GRID_ITEM_GAP / 2, GRID_ITEM_GAP / 2, GRID_ITEM_GAP / 2))
            .SetValue(Prop::borderWidth, 2.0f)
            .SetValue(Prop::borderColor, Vector4(80, 80, 80, 255))
            .SetValue(Prop::borderInset, true)
            .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
            .SetValue(Prop::duration, 0.1f)
            .SetValue(Prop::durationEasing, UI::UIEasing::EaseInOutSine);

        m_styleItemNormal.SetActiveState(UI::UIState::HOVER)
            .SetValue(Prop::borderColor, Vector4(180, 180, 180, 255));

        m_styleItemSelected
            .Merge(m_styleItemNormal)
            .SetValue(Prop::borderWidth, 3.0f)
            .SetValue(Prop::borderColor, Vector4(255, 200, 50, 255));

        m_styleItemSelected.SetActiveState(UI::UIState::HOVER)
            .SetValue(Prop::borderColor, Vector4(255, 220, 100, 255));

        m_styleItemInner
            .Merge(Style::commanStretch)
            .SetValue(Prop::hitTestTransparent, true)
            .SetValue(Prop::backgroundColor, Vector4(255))
            .SetValue(Prop::duration, 0.15f)
            .SetValue(Prop::durationEasing, UI::UIEasing::EaseInOutSine);
    }

    void SkinSelectLayer::OnUpdate(AppContext* ctx) {
    }

    void SkinSelectLayer::OnRender(AppContext* ctx) {
    }

    void SkinSelectLayer::OnUIRender(AppContext* ctx) {
        namespace Prop = UI::Properties;
        typedef UI::UIKey Key;

        const auto& skinTypes = ctx->skinManager.GetSkinTypes();
        const ChessSkinType currentSkin = ctx->skinManager.GetCurrentSkin();

        const int rowCount = (static_cast<int>(skinTypes.size()) + GRID_COLUMNS - 1) / GRID_COLUMNS;
        const float containerW = GRID_COLUMNS * (GRID_ITEM_WIDTH + GRID_ITEM_GAP);
        const float containerH = rowCount * (GRID_ITEM_HEIGHT + GRID_ITEM_GAP) + 20.0f;

        m_styleGridContainer
            .SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
            .SetValue(Prop::size, containerW, containerH);

        UI::BeginFrame(Key("skin_select_overlay"), Style::commanOverlay);
        {
            UI::Text(Key("title"), "Select a skin", m_styleTitle);

            UI::BeginFrame(Key("skin_grid"), m_styleGridContainer);
            {
                int col = 0;
                int rowIndex = 0;
                UI::BeginFrame(Key("skin_row_" + std::to_string(rowIndex)), m_styleGridRow);

                for (int i = 0; i < static_cast<int>(skinTypes.size()); i++) {
                    const ChessSkinType type = skinTypes[i];
                    const bool isSelected = (type == currentSkin);
                    const std::string itemKey = "skin_item_" + std::to_string(static_cast<int>(type));

                    const auto& outerStyle = isSelected ? m_styleItemSelected : m_styleItemNormal;
                    UI::BeginFrame(Key(itemKey), outerStyle);
                    {
                        const std::string innerKey = itemKey + "_inner";

                        auto texID = ctx->skinManager.GetSkinLightTextureIDForType(type);

                        UI::UIStyle tmp{ "tmp" };
                        tmp.SetValue(Prop::backgroundTexture, texID);

                        UI::BeginFrame(Key(innerKey), { m_styleItemInner, tmp });
                        UI::EndFrame();
                    }
                    UI::UIEvent outerEvent = UI::EndFrame();

                    if (outerEvent.IsClick()) {
                        ctx->skinManager.SetCurrentSkinType(type);
                    }

                    col++;
                    if (col >= GRID_COLUMNS && i + 1 < (int)skinTypes.size()) {
                        col = 0;
                        rowIndex++;
                        UI::EndFrame();
                        UI::BeginFrame(Key("skin_row_" + std::to_string(rowIndex)), m_styleGridRow);
                    }
                }

                UI::EndFrame();
            }
            UI::EndFrame();

            if (UIComp::DrawButton("btn_back", "Back", Style::commanBTNBase)) {
                ctx->app->ClearLayers();
                ctx->app->PushLayer<MainMenuLayer>();
            }
        }
        UI::EndFrame();
    }

    void SkinSelectLayer::OnQuit(AppContext* ctx) {
    }

    LayerID SkinSelectLayer::GetLayerID() const {
        return LayerID::SKIN_SELECT;
    }
}