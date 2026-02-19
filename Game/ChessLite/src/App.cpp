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

AppContext* App::GetContext() {
    if (!g_appInstance)
        return nullptr;
    return &(g_appInstance->m_context);
}

void App::OnStart() {
    InstantiateWindow();
    Style::Comman_InitStyles();

    // push start layer on to stack
    PushLayer<Layers::StartLoadLayer>();
}

void App::OnUpdate() {
    
    if (!m_winID.IsInvalid()) {
        using namespace SDLCore;
        Input::SetWindow(m_winID);
        ForeachLayer([&](Layer& layer) { 
            layer.OnUpdate(&m_context); 
            if (Input::GetWindowID() != m_winID) {
                Input::SetWindow(m_winID);
            }
        });

        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
        RE::SetColor(25);
        RE::Clear();
        ForeachLayer([&](Layer& layer) { 
            layer.OnRender(&m_context); 
            
            if (RE::GetActiveWindowID() != m_winID) {
                Input::SetWindow(m_winID);
                RE::SetWindowRenderer(m_winID);
            }
        });

        UI::SetContextWindow(m_UICtx, m_winID);
        UI::BindContext(m_UICtx);

        UI::BeginFrame(SDLCore::UI::UIKey("root"), Style::commanRootTransparent);
        {
            ForeachLayer([&](Layer& layer) {
                layer.OnUIRender(&m_context);

                if (RE::GetActiveWindowID() != m_winID) {
                    Input::SetWindow(m_winID);
                    UI::BindContext(m_UICtx);
                    RE::SetWindowRenderer(m_winID);
                }
            });
        }
        UI::EndFrame();

        ForeachLayer([&](Layer& layer) {
            layer.OnLateRender(&m_context);

            if (RE::GetActiveWindowID() != m_winID) {
                Input::SetWindow(m_winID);
                RE::SetWindowRenderer(m_winID);
            }
        });
        
        RE::Present();

        ProcessLayerCommands();
    }
    else {
        // main window is closed
        Quit();
    }
}

void App::OnQuit() {
    ClearLayers();
    SDLCore::UI::DestroyContext(m_UICtx);
    m_UICtx = nullptr;
    WindowCleanup();
}

bool App::UnsubscribeToLayerEvent(LayerEventSubscriptionID id) {
    return m_context.m_eventBus.Unsubscribe(id);
}

bool App::UnsubscribeToLayerEvent(LayerEventType type, LayerEventSubscriptionID id) {
    return m_context.m_eventBus.Unsubscribe(type, id);
}

void App::PopLayer() {
    m_layerCommands.emplace_back(LayerCmdType::POP);
}

void App::PopLayer(LayerID layerID) {
    m_layerCommands.emplace_back(LayerCmdType::POP, layerID);
}

void App::ClearLayers() {
    m_layerCommands.emplace_back(LayerCmdType::CLEAR);
}

SDLCore::WindowID App::GetWinID() const {
    return m_winID;
}

size_t App::GetLayerCount() const {
    return m_layerStack.size();
}

void App::InstantiateWindow() {
    auto* win = CreateWindow(&m_winID, "ChessLite", 800, 700);
    win->SetWindowMinSize(800, 700);

    namespace RE = SDLCore::Render;
    RE::SetWindowRenderer(m_winID);
    RE::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
    RE::SetColor(25);
    RE::Clear();
    RE::Present();

    m_UICtx = SDLCore::UI::CreateContext();

    m_context.windowSize = win->GetSize();
    m_windowResizeCBID = win->AddOnWindowResize([this](const SDLCore::Window& win) {
        m_context.windowSize = win.GetSize();
    });

    Vector4 bounds = win->GetDisplayBounds();
    m_context.displaySize.Set(bounds.z, bounds.w);
    m_windowDisplayChangedCBID = win->AddOnWindowDisplayChanged([this](const SDLCore::Window& win) {
        Vector4 bounds = win.GetDisplayBounds();
        m_context.displaySize.Set(bounds.z, bounds.w);
    });
}

void App::WindowCleanup() {
    auto winID = GetWinID();
    auto* win = GetWindow(winID);
    if (win) {
        win->RemoveOnWindowResize(m_windowResizeCBID);
        win->RemoveOnWindowDisplayChanged(m_windowDisplayChangedCBID);
    }
}

void App::ProcessLayerCommands() {
    auto& eventBus = m_context.m_eventBus;

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
                if (cmd.layerID != LayerID::NONE) {
                    auto it = std::find_if(m_layerStack.begin(), m_layerStack.end(),
                        [&](const std::unique_ptr<Layer>& layer) {
                            return layer->GetLayerID() == cmd.layerID;
                        });
                    
                    if (it != m_layerStack.end()) {
                        eventBus.Emit(LayerEventType::CLOSED, (*it)->GetLayerID());
                        (*it)->OnQuit(&m_context);
                        m_layerStack.erase(it);
                    }
                }
                else {
                    auto& layer = m_layerStack.back();
                    eventBus.Emit(LayerEventType::CLOSED, layer->GetLayerID());
                    layer->OnQuit(&m_context);

                    m_layerStack.pop_back();
                }
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