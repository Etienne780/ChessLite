#include "App.h"
#include "Layers.h"
#include "Styles/Comman/Style.h"
#include "UIComponents/Button.h"
#include "Styles/Comman/Style.h"
#include "Styles/Comman/Space.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;
namespace Prop = UI::Properties;
typedef UI::UIKey Key;

static App* g_appInstance;

static SDLCore::UI::UIStyle m_StyleRoot;
static SDLCore::UI::UIStyle m_StyleMenu;
static SDLCore::UI::UIStyle m_StyleButton;
static SDLCore::UI::UIStyle m_StyleTitle;

App::App()
    : Application("ChessLite", SDLCore::Version(1, 0)) {
    g_appInstance = this;
}

App* App::GetInstance(){
    return g_appInstance;
}

void App::OnStart() {
    auto* win = CreateWindow(&m_winID, "ChessLite", 800, 700);
    win->SetWindowMinSize(800, 700);
    m_UICtx = SDLCore::UI::CreateContext();
    
    Style::Comman_InitStyles();

    // push start layer on to stack
    // PushLayer<Layers::MainMenuLayer>();

    m_StyleRoot
        .Merge(Style::commanRoot)
        .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER);

    m_StyleMenu
        .Merge(Style::commanBox)
        .Merge(Style::commanStretch)
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
        .SetValue(Prop::padding, Style::commanSpaceL)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
        .SetValue(Prop::size, 550.0f, 400.0f);

    m_StyleButton
        .Merge(Style::commanBTNBase)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
        .SetValue(Prop::size, 250.0f, 75.0f);

    m_StyleTitle
        .Merge(Style::commanTextTitle)
        .SetValue(Prop::margin, Vector4(0, 0, 20, 0));
}

void App::OnUpdate() {
    
    // Quit if no windows remain 
    if (GetWindowCount() <= 0)
        Quit();

    if (!m_winID.IsInvalid()) {
        using namespace SDLCore;
        Input::SetWindow(m_winID);
        ForeachLayer([&](Layer& layer) { layer.OnUpdate(&m_context); });

        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
        RE::SetColor(0);
        RE::Clear();
        ForeachLayer([&](Layer& layer) { layer.OnRender(&m_context); });
        
        UI::SetContextWindow(m_UICtx, m_winID);
        UI::BindContext(m_UICtx);


        UI::BeginFrame(SDLCore::UI::UIKey("root"), Style::commanRoot);
        {
            UI::BeginFrame(Key("main_menu_root"), m_StyleRoot);
            {
                UI::BeginFrame(Key("menu"), m_StyleMenu);
                {
                    UI::Text(Key("title"), "Chess Lite", m_StyleTitle);

                    if (UIComp::DrawButton("btn_play", "Play", m_StyleButton)) {
                        Log::Debug("Play");   
                    }

                    if (UIComp::DrawButton("btn_settings", "Settings", m_StyleButton)) {
                        Log::Debug("Settings");
                    }

                    if (UIComp::DrawButton("btn_quit", "Quit", m_StyleButton)) {
                        Log::Debug("Quit");
                    }
                }
                UI::EndFrame();
            }
            UI::EndFrame();

            UI::BeginFrame(Key("options_menu_overlay"), Style::commanOverlay);
            {
                if (UIComp::DrawButton("btn_back", "Back", m_StyleButton)) {
                    Log::Debug("back");
                }
            }
            UI::EndFrame();
            // ForeachLayer([&](Layer& layer) {
            //     layer.OnUIRender(&m_context);
            // });
        }
        UI::EndFrame();
      
        RE::Present();

        ProcessLayerCommands();

        // should be removed for final version
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);
    }
}

void App::OnQuit() {
    ClearLayers();
    SDLCore::UI::DestroyContext(m_UICtx);
    m_UICtx = nullptr;
}

void App::PopLayer() {
    m_layerCommands.emplace_back<LayerCommand>(LayerCmdType::Pop);
}

void App::ClearLayers() {
    ForeachLayer([&](Layer& layer) { layer.OnQuit(&m_context); });
    m_layerStack.clear();
    Log::Debug("App: ClearLayers");
}

SDLCore::WindowID App::GetWinID() const {
    return m_winID;
}

size_t App::GetLayerCount() const {
    return m_layerStack.size();
}

void App::ProcessLayerCommands() {
    for (auto& cmd : m_layerCommands) {
        switch (cmd.type) {
        case LayerCmdType::Push:
            m_layerStack.push_back(std::move(cmd.factory()));
            m_layerStack.back()->OnStart(&m_context);
            Log::Debug("App::Layer::Push:  + new count {}", m_layerStack.size());
            break;

        case LayerCmdType::Pop:
            if (!m_layerStack.empty()) {
                m_layerStack.back()->OnQuit(&m_context);
                m_layerStack.pop_back();
                Log::Debug("App::Layer::Pop: - new count {}", m_layerStack.size());
            }
            break;
        }
    }

    m_layerCommands.clear();
}