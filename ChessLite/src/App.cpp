#include "App.h"
#include "Layers.h"
#include "Styles/Comman/Style.h"

static App* g_appInstance;

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

    Style::Comman_InitStyles();

    // push start layer on to stack
    PushLayer<Layers::MainMenuLayer>();
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
        RE::SetColor(50, 60, 140);
        RE::Clear();
        ForeachLayer([&](Layer& layer) { layer.OnRender(&m_context); });
        RE::Present();

        ProcessLayerCommands();

        // should be removed for final version
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);
    }
}

void App::OnQuit() {
    ClearLayers();
}

void App::PopLayer() {
    m_layerCommands.emplace_back<LayerCommand>(LayerCmdType::Pop);
}

void App::ClearLayers() {
    ForeachLayer([&](Layer& layer) { layer.OnQuit(&m_context); });
    m_layerStack.clear();
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
            break;

        case LayerCmdType::Pop:
            if (!m_layerStack.empty()) {
                m_layerStack.back()->OnQuit(&m_context);
                m_layerStack.pop_back();
            }
            break;
        }
    }

    m_layerCommands.clear();
}