#include <CoreLib/OTNFile.h>

#include "App.h"
#include "FilePaths.h"
#include "LayerSystem/Layers.h"
#include "Styles/Comman/Style.h"
#include "UIComponents/Button.h"
#include "Styles/Comman/Space.h"
#include "Styles/Comman/Color.h"

static App* g_appInstance;

App::App()
    : Application("ChessLite", SDLCore::Version(1, 0)) {
    g_appInstance = this;
    m_agentSync->Init(m_context);
}

App::~App() {
    g_appInstance = nullptr;
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
    if(!FilePaths::InitPaths())
        throw std::runtime_error("Failed to initialises paths");

    InitChessContext();
    InstantiateWindow();

    Style::Comman_InitStyles();

    // push start layer on to stack
    PushLayer<Layers::StartLoadLayer>(m_agentSync);
}

void App::OnUpdate() {
    ConnectClient();
    UpdateNotifications();

    if (!m_winID.IsInvalid()) {
        using namespace SDLCore;

        Window* win = GetWindow(m_winID);
        if (!win || !win->HasWindow() || !win->HasRenderer()) {
            std::cout << "OnUpdate: Window is invalid, skipping render\n";
            return;
        }

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

        if (RE::GetActiveWindowID() != m_winID) {
            Input::SetWindow(m_winID);
            UI::BindContext(m_UICtx);
            RE::SetWindowRenderer(m_winID);
        }
        RenderNotifications();
        
        RE::Present();

        ProcessLayerCommands();
        ProcessGameClient();

        if (m_currentSyncTime > 0.0f)
            m_currentSyncTime -= Time::GetDeltaTimeSecF();

        if (m_currentSyncTime <= 0.0f) {
            if (m_agentSync)
                m_agentSync->Sync(&m_context);
            m_currentSyncTime = m_syncTime;
        }
    }
    else {
        // main window is closed
        Quit();
    }
}

void App::OnQuit() {
    Log::Info("Quit started");

    ClearLayers();
    SaveUserData();
    m_context.agentManager.Save(FilePaths::GetDataPath());

    WindowCleanup();

    SDLCore::UI::DestroyContext(m_UICtx);
    m_UICtx = nullptr;
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

bool App::SaveUserData() {
    OTN::OTNObject obj{ "userData" };
    obj.SetNames("deleted_server_agents");
    obj.SetTypes("int64[]");

    auto deletedServerAgents = m_context.agentManager.GetDeletedServerAgents();
    std::vector<int64_t> deletedServerAgents64;
    deletedServerAgents64.reserve(deletedServerAgents.size());
    for (auto id : deletedServerAgents)
        deletedServerAgents64.push_back(static_cast<int64_t>(id.value));

    obj.AddDataRow(deletedServerAgents64);

    OTN::OTNWriter writer;
    writer.AppendObject(obj);
    if(!writer.Save(FilePaths::GetDataPath() / FilePaths::userFileName))
        NotifyError("Failed to save user data");
    
    return true;
}

void App::LoadUserData(const OTN::OTNObject& object) {
    if (auto deletedServerAgents = object.TryGetValue<std::vector<int64_t>>(0, "deleted_server_agents")) {
        std::unordered_set<AgentID> ids;
        ids.reserve(deletedServerAgents->size());

        for (auto id : *deletedServerAgents)
            ids.emplace(static_cast<uint32_t>(id));

        m_context.agentManager.SetDeletedServerAgents(ids);
    }
}

void App::NotifyDefault(const std::string& message, uint64_t durationMs) {
    CreateNotification(message, AppNotificationType::DEFAULT, durationMs);
}

void App::NotifyWarning(const std::string& message, uint64_t durationMs) {
    CreateNotification(message, AppNotificationType::WARNING, durationMs);
}

void App::NotifyError(const std::string& message, uint64_t durationMs) {
    CreateNotification(message, AppNotificationType::ERROR, durationMs);
}

SDLCore::WindowID App::GetWinID() const {
    return m_winID;
}

size_t App::GetLayerCount() const {
    return m_layerStack.size();
}

void App::InitChessContext() {
    using namespace CoreChess;

    auto& reg = ChessPieceRegistry::GetInstance();

    // --- Pawn Setup ---
    auto* pawn = reg.AddChessPiece(m_context.pieceID, "pawn", 1);
    pawn->SetMoveProperties(1, false, false, TargetType::FREE);
    pawn->AddMoveRule(0, 1); // forward
    pawn->SetTargetType(TargetType::OPPONENT);
    pawn->AddMoveRule(1, 1); // capture
    pawn->AddMoveRule(-1, 1);

    // --- Chess Board Setup ---
    ChessContext& chessCTX = m_context.currentContext;
    chessCTX.SetBoardSize(3, 3);
    chessCTX.BoardCmdFillRow(0, m_context.pieceID);

    //--- Win Condition Setup
    chessCTX.SetWinCondition([this](const ChessGame& game) -> ChessWinResult {
        if (game.IsWhiteTurn()) {
            if (!game.HasAnyLegalMove(FieldType::BLACK)) {
                return ChessWinResult::WHITE_WON;
            }
        }
        else {
            if (!game.HasAnyLegalMove(FieldType::WHITE)) {
                return ChessWinResult::BLACK_WON;
            }
        }

        const auto& board = game.GetBoard();
        int w = board.GetWidth();
        int h = board.GetHeight();

        // White reaches top row
        for (size_t x = 0; x < w; ++x) {
            ChessField f = board.GetFieldAt(static_cast<int>(x), 0);
            if (f.GetFieldType() == FieldType::WHITE)
                return ChessWinResult::WHITE_WON;
        }

        // Black reaches bottom row
        for (size_t x = 0; x < w; ++x) {
            ChessField f = board.GetFieldAt(static_cast<int>(x), h - 1);
            if (f.GetFieldType() == FieldType::BLACK)
                return ChessWinResult::BLACK_WON;
        }

        return ChessWinResult::NONE;
    });
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

void App::ConnectClient() {
    auto& client = m_context.gameClient;
    if (client.IsConnected())
        return;

    if (m_currentClientTimeOut > 0) {
        m_currentClientTimeOut -= SDLCore::Time::GetDeltaTimeSecF();
        return;
    }

    if (!client.Connect(m_host, m_port)) {
        if (!m_connectionLostMsgSent) {
            NotifyError(client.GetError());
            m_connectionLostMsgSent = true;
        }

        Log::Error(client.GetError());
        client.ClearError();
    }

    if (client.IsConnected()) {
        m_connectionLostMsgSent = false;
        NotifyDefault("Connected to server");
    }

    m_currentClientTimeOut = m_clientTimeOut;
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

    std::vector<LayerCommand> copy = m_layerCommands;
    for (auto& cmd : copy) {
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

void App::ProcessGameClient() {
    auto& client = m_context.gameClient;

    if (!client.IsConnected())
        return;

    if (!client.ProcessSendQueue()) {
        Log::Error(client.GetError());
        client.ClearError();
    }
    
    if (!client.ProcessReceiveQueue()) {
        Log::Error(client.GetError());
        client.ClearError();
    }
}

void App::CreateNotification(const std::string& message, AppNotificationType type, uint64_t displayDurationMs) {
    m_notifications.emplace_back(
        message, 
        type, 
        (displayDurationMs == 0) ? m_notificationDisplayDurationMS : displayDurationMs,
        SDLCore::Time::GetTimeMS()
    );
}

void App::UpdateNotifications() {
    if (m_notifications.empty())
        return;

    uint64_t currentTimeMs = SDLCore::Time::GetTimeMS();

    size_t index = 0;
    while (index < m_notifications.size()) {
        auto& noti = m_notifications[index];

        if (currentTimeMs - noti.createdAtMs >= noti.displayDurationMs) {
            m_notifications.erase(m_notifications.begin() + index);
        }
        else {
            index++;
        }
    }
}

void App::RenderNotifications() {
    if (m_notifications.empty())
        return;

    namespace RE = SDLCore::Render;
    typedef SDLCore::UI::UIRegistry UIReg;

    Vector4 baseColor;
    Vector4 outlineColor;

    Vector4 defaultColor;
    Vector4 warnColor;
    Vector4 errorColor;
    UIReg::TryGetRegisteredColor(Style::commanColorUIBackgroundLight, baseColor);
    UIReg::TryGetRegisteredColor(Style::commanColorUIBackground, outlineColor);

    UIReg::TryGetRegisteredColor(Style::commanColorAccentDefault, defaultColor);
    UIReg::TryGetRegisteredColor(Style::commanColorAccentWarning, warnColor);
    UIReg::TryGetRegisteredColor(Style::commanColorAccentError, errorColor);

    auto GetColorForType = [&](AppNotificationType type) -> Vector4 {
        switch (type) {
        case AppNotificationType::DEFAULT:  return defaultColor;
        case AppNotificationType::WARNING:  return warnColor;
        case AppNotificationType::ERROR:    return errorColor;
        default:                            return defaultColor;
        }
    };

    auto& refDisplaySize = m_context.refDisplaySize;
    auto& displaySize = m_context.displaySize;
    auto& windowSize = m_context.windowSize;

    float winHalfX = windowSize.x * 0.5f;
    float winHalfY = windowSize.y * 0.5f;

    float scaleX = displaySize.x / refDisplaySize.x;
    float scaleY = displaySize.y / refDisplaySize.y;
    float displayScale = std::min(scaleX, scaleY);

    const float width = 300 * displayScale;
    const float minHeight = 30 * displayScale;
    const float maxHeight = 150 * displayScale;
    const float padding = 5 * displayScale;
    const float elementPadding = 15 * displayScale;
    const float textSize = 28 * displayScale;
    const float accentLineWidth = 3 * displayScale;
    const float outlineWidth = 4 * displayScale;

    RE::SetTextSize(textSize);
    RE::SetTextClipWidth(width);
    RE::SetTextAlign(SDLCore::Align::START);

    float currentPositionY = windowSize.y;
    for (auto& noti : m_notifications) {
        float textHeight = RE::GetTextBlockHeight(noti.message);
        float calcualtedHeight = std::min(maxHeight, std::max(textHeight + padding * 2, minHeight));
        currentPositionY -= calcualtedHeight + elementPadding;

        Vector4 baseRect{
            winHalfX - width * 0.5f,
            currentPositionY,
            width,
            calcualtedHeight
        };

        // base
        RE::SetColor(baseColor);
        RE::FillRect(baseRect);

        RE::SetColor(outlineColor);
        RE::SetStrokeWidth(outlineWidth);
        RE::SetInnerStroke(false);
        RE::Rect(baseRect);

        RE::SetClipRect(baseRect);

        // accent line
        RE::SetColor(GetColorForType(noti.type));
        RE::FillRect(baseRect.x, baseRect.y, accentLineWidth, baseRect.w);

        // text
        RE::SetColor(255);
        RE::CacheText(true);
        RE::Text(noti.message, baseRect.x + accentLineWidth + padding, baseRect.y + padding);

        RE::ResetClipRect();
    }

    RE::ResetTextClipWidth();
    RE::ResetClipRect();
}