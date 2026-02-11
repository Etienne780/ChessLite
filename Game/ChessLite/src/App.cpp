#include "App.h"
#include "LayerSystem/Layers.h"
#include "Styles/Comman/Style.h"
#include "UIComponents/Button.h"
#include "Styles/Comman/Space.h"

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
    m_UICtx = SDLCore::UI::CreateContext();
    
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
        RE::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
        RE::SetColor(0);
        RE::Clear();
        ForeachLayer([&](Layer& layer) { layer.OnRender(&m_context); });
        
        UI::SetContextWindow(m_UICtx, m_winID);
        UI::BindContext(m_UICtx);


        UI::BeginFrame(SDLCore::UI::UIKey("root"), Style::commanRoot);
        {
            ForeachLayer([&](Layer& layer) {
                layer.OnUIRender(&m_context);
            });
        }
        UI::EndFrame();
      
        RE::Present();

        ProcessLayerCommands();
    }
}

void App::OnQuit() {
    ClearLayers();
    SDLCore::UI::DestroyContext(m_UICtx);
    m_UICtx = nullptr;
}

bool App::UnsubscribeToLayerEvent(LayerEventSubscriptionID id) {
    return m_context.eventBus.Unsubscribe(id);
}

bool App::UnsubscribeToLayerEvent(LayerEventType type, LayerEventSubscriptionID id) {
    return m_context.eventBus.Unsubscribe(type, id);
}

void App::PopLayer() {
    m_layerCommands.emplace_back<LayerCommand>(LayerCmdType::POP);
}

void App::ClearLayers() {
    m_layerCommands.emplace_back<LayerCommand>(LayerCmdType::CLEAR);
}

SDLCore::WindowID App::GetWinID() const {
    return m_winID;
}

size_t App::GetLayerCount() const {
    return m_layerStack.size();
}

void App::ProcessLayerCommands() {
    auto& eventBus = m_context.eventBus;

    for (auto& cmd : m_layerCommands) {
        switch (cmd.type) {
        case LayerCmdType::PUSH: {
            m_layerStack.push_back(std::move(cmd.factory()));
            auto& layer = m_layerStack.back();
            layer->OnStart(&m_context);

            eventBus.Emit(LayerEventType::OPENED, layer->GetLayerID());
            Log::Debug("App::Layer::Push:  + new count {}", m_layerStack.size());
            break;
        }
        case LayerCmdType::POP: {
            if (!m_layerStack.empty()) {
                auto& layer = m_layerStack.back();
                eventBus.Emit(LayerEventType::CLOSED, layer->GetLayerID());
                layer->OnQuit(&m_context);

                m_layerStack.pop_back();
                Log::Debug("App::Layer::Pop: - new count {}", m_layerStack.size());
            }
            break;
        }
        case LayerCmdType::CLEAR: {
            ForeachLayer([&](Layer& layer) {
                eventBus.Emit(LayerEventType::CLOSED, layer.GetLayerID());
                layer.OnQuit(&m_context);
                });
            m_layerStack.clear();
            Log::Debug("App::Layer::Clear: - Clear");
            break;
        }
        }
    }

    eventBus.Dispatch();
    m_layerCommands.clear();
}