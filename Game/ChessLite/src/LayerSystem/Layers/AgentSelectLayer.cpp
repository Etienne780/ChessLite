#include "LayerSystem/Layers/AgentSelectLayer.h"
#include "App.h"
#include "FilePaths.h"

namespace Layers {

    AgentSelect::AgentSelect(AgentSelectMode mode)
        : m_selectMode(mode) {
    }

    void AgentSelect::OnStart(AppContext* ctx) {
        m_activeSlot = (m_selectMode == AgentSelectMode::AGENT_2_ONLY) ? 1 : 0;
    }

    void AgentSelect::OnUpdate(AppContext* ctx) {}

    void AgentSelect::OnLateRender(AppContext* ctx) {
        namespace RE = SDLCore::Render;

        auto vp = RE::GetViewport();
        float W = static_cast<float>(vp.w);
        float H = static_cast<float>(vp.h);

        RE::SetColor(20);
        RE::FillRect(0, 0, W, H);

        float headerH = 60.0f;
        const float backBtnW = 100.0f;
        const float backBtnH = 38.0f;
        float backBtnX = 15.0f;
        float backBtnY = headerH * 0.5f - backBtnH * 0.5f;

        bool backHovered = IsPointInRect(SDLCore::Input::GetMousePosition(), backBtnX, backBtnY, backBtnW, backBtnH);
        RE::SetColor(backHovered ? 45 : 30);
        RE::FillRect(backBtnX, backBtnY, backBtnW, backBtnH);
        RE::SetStrokeWidth(1);
        RE::SetColor(backHovered ? 100 : 55);
        RE::Rect(backBtnX, backBtnY, backBtnW, backBtnH);

        RE::SetTextSize(22.0f);
        std::string backLabel = "< Back";
        float blw = RE::GetTextWidth(backLabel);
        RE::SetColor(backHovered ? 220 : 150);
        RE::Text(backLabel, backBtnX + (backBtnW - blw) * 0.5f, backBtnY + (backBtnH - RE::GetTextHeight()) * 0.5f);

        if (SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT)
            && IsPointInRect(SDLCore::Input::GetMousePosition(), backBtnX, backBtnY, backBtnW, backBtnH)) {
            ctx->app->PopLayer();
        }

        RE::SetTextSize(32.0f);
        RE::SetColor(220);
        float titleW = RE::GetTextWidth("Select Agent");
        RE::Text("Select Agent", (W - titleW) * 0.5f, headerH * 0.5f - RE::GetTextHeight() * 0.5f);

        const float saveBtnW = 130.0f;
        const float saveBtnH = 38.0f;
        float saveBtnX = W - saveBtnW - 15.0f;
        float saveBtnY = headerH * 0.5f - saveBtnH * 0.5f;

        bool saveHovered = IsPointInRect(SDLCore::Input::GetMousePosition(), saveBtnX, saveBtnY, saveBtnW, saveBtnH);
        RE::SetColor(m_saveTimer > 0.0f ? 30 : (saveHovered ? 40 : 28),
            m_saveTimer > 0.0f ? 65 : (saveHovered ? 55 : 40),
            m_saveTimer > 0.0f ? 30 : (saveHovered ? 40 : 28));
        RE::FillRect(saveBtnX, saveBtnY, saveBtnW, saveBtnH);
        RE::SetStrokeWidth(1);
        RE::SetColor(m_saveTimer > 0.0f ? Vector3(60, 180, 60) : Vector3(55));
        RE::Rect(saveBtnX, saveBtnY, saveBtnW, saveBtnH);

        RE::SetTextSize(22.0f);
        std::string saveLabel = (m_saveTimer > 0.0f) ? "Saved!" : "Save";
        float slw = RE::GetTextWidth(saveLabel);
        RE::SetColor(m_saveTimer > 0.0f ? Vector3(120, 220, 120) : Vector3(180));
        RE::Text(saveLabel, saveBtnX + (saveBtnW - slw) * 0.5f, saveBtnY + (saveBtnH - RE::GetTextHeight()) * 0.5f);

        if (SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT)
            && IsPointInRect(SDLCore::Input::GetMousePosition(), saveBtnX, saveBtnY, saveBtnW, saveBtnH)) {
            ctx->agentManager.Save(FilePaths::GetDataPath());
            m_saveTimer = m_saveTimerDuration;
        }

        if (m_saveTimer > 0.0f) {
            m_saveTimer -= SDLCore::Time::GetDeltaTimeSecF();
        }

        float y = headerH + 16.0f;
        float selBarH = 90.0f;
        RenderSelectionBar(ctx, 20, y, W - 40.0f, selBarH);
        y += selBarH + 12.0f;

        RE::SetColor(45);
        RE::FillRect(20, y, W - 40.0f, 1.0f);
        y += 10.0f;

        RE::SetTextSize(22.0f);
        RE::SetColor(90);
        std::string hint;
        if (m_selectMode == AgentSelectMode::AGENT_1_ONLY) hint = "Click an agent to assign to Agent 1";
        else if (m_selectMode == AgentSelectMode::AGENT_2_ONLY) hint = "Click an agent to assign to Agent 2";
        else hint = (m_activeSlot == 0)
            ? "Assigning to Agent 1 — click Agent 2 slot above to switch"
            : "Assigning to Agent 2 — click Agent 1 slot above to switch";
        RE::Text(hint, 20, y);
        y += RE::GetTextHeight() + 8.0f;

        float addBarH = 52.0f;
        RenderAddBar(ctx, 20, y, W - 40.0f, addBarH);
        y += addBarH + 6.0f;

        RenderAgentList(ctx, 20, y, W - 40.0f, H - y - 10.0f);
    }

    void AgentSelect::OnQuit(AppContext* ctx) {
        ctx->agentManager.Save(FilePaths::GetDataPath());
    }
    LayerID AgentSelect::GetLayerID() const {
        return LayerID::AGENT_SELECT;
    }

    void AgentSelect::RenderSelectionBar(AppContext* ctx, float x, float y, float w, float h) {
        namespace RE = SDLCore::Render;

        const auto& agents = ctx->agentManager.GetAgents();
        bool showBoth = (m_selectMode == AgentSelectMode::BOTH_AGENTS);
        int cardCount = showBoth ? 2 : 1;
        float gap = 12.0f;
        float cardW = (w - gap * (cardCount - 1)) / cardCount;

        auto drawCard = [&](int slotIdx, float cx, float cy, float cw, float ch) {
            AgentID assignedID = (slotIdx == 0) ? ctx->selectedAgentID1 : ctx->selectedAgentID2;
            bool isActive = (m_activeSlot == slotIdx);
            bool clickable = showBoth;

            RE::SetColor(isActive ? 36 : 26);
            RE::FillRect(cx, cy, cw, ch);

            RE::SetStrokeWidth(2);
            RE::SetColor(isActive && showBoth ? Vector3(90, 150, 255) : Vector3(50));
            RE::Rect(cx, cy, cw, ch);

            std::string slotLabel = (slotIdx == 0) ? "Agent 1" : "Agent 2";
            RE::SetTextSize(18.0f);
            RE::SetColor(isActive ? 140 : 80);
            RE::Text(slotLabel, cx + 10, cy + 6);

            RE::SetTextSize(26.0f);
            auto it = agents.find(assignedID);
            if (it != agents.end()) {
                RE::SetColor(230);
                RE::Text(it->second.GetName(), cx + 10, cy + ch * 0.5f - RE::GetTextHeight() * 0.5f + 4);
            }
            else {
                RE::SetColor(55);
                RE::Text("— none selected —", cx + 10, cy + ch * 0.5f - RE::GetTextHeight() * 0.5f + 4);
            }

            if (clickable) {
                Vector2 mPos = SDLCore::Input::GetMousePosition();
                if (SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT)
                    && IsPointInRect(mPos, cx, cy, cw, ch)) {
                    m_activeSlot = slotIdx;
                }
            }
            };

        if (showBoth) {
            drawCard(0, x, y, cardW, h);
            drawCard(1, x + cardW + gap, y, cardW, h);
        }
        else {
            int slot = (m_selectMode == AgentSelectMode::AGENT_2_ONLY) ? 1 : 0;
            drawCard(slot, x, y, w, h);
        }
    }

    void AgentSelect::RenderAddBar(AppContext* ctx, float x, float y, float w, float h) {
        namespace RE = SDLCore::Render;

        const float btnW = 120.0f;
        const float gap = 8.0f;
        const float fieldW = w - btnW - gap;
        float textY = y + h * 0.5f - RE::GetTextHeight() * 0.5f;

        // Input field
        RE::SetColor(m_isAddingAgent ? 32 : 24);
        RE::FillRect(x, y, fieldW, h);
        RE::SetStrokeWidth(1);
        RE::SetColor(m_isAddingAgent ? Vector3(90, 150, 255) : Vector3(50));
        RE::Rect(x, y, fieldW, h);

        RE::SetTextSize(22.0f);

        if (m_isAddingAgent) {
            m_cursorBlinkTimer += SDLCore::Time::GetDeltaTimeSecF();
            std::string display = m_newAgentName;
            if (std::fmod(m_cursorBlinkTimer, 1.0f) < 0.5f) display += "|";

            RE::SetColor(220);
            RE::Text(display.empty() ? "|" : display, x + 10, textY);

            if (!SDLCore::Input::IsTextInputActive())
                SDLCore::Input::StartTextInput();

            std::string typed = SDLCore::Input::GetTextInputBuffer();
            for (char c : typed) {
                if (std::isprint(static_cast<unsigned char>(c))) m_newAgentName += c;
            }

            if (SDLCore::Input::KeyRepeating(SDLCore::KeyCode::BACKSPACE) && !m_newAgentName.empty()) {
                m_newAgentName.pop_back();
            }

            if (SDLCore::Input::KeyJustPressed(SDLCore::KeyCode::ESCAPE)) {
                m_isAddingAgent = false;
                m_newAgentName.clear();
                m_cursorBlinkTimer = 0.0f;
                SDLCore::Input::StopTextInput();
            }

            if (SDLCore::Input::KeyJustPressed(SDLCore::KeyCode::RETURN) && !m_newAgentName.empty()) {
                ctx->agentManager.AddAgent(Agent(m_newAgentName, ctx->currentContext.GetConfigString()));
                m_isAddingAgent = false;
                m_newAgentName.clear();
                m_cursorBlinkTimer = 0.0f;
                SDLCore::Input::StopTextInput();
            }
        }
        else {
            RE::SetColor(60);
            RE::Text("Agent name...", x + 10, textY);

            Vector2 mPos = SDLCore::Input::GetMousePosition();
            if (SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT)
                && IsPointInRect(mPos, x, y, fieldW, h)) {
                m_isAddingAgent = true;
                m_newAgentName.clear();
                m_cursorBlinkTimer = 0.0f;
                SDLCore::Input::StartTextInput();
            }
        }

        // Button
        float btnX = x + fieldW + gap;
        bool canConfirm = m_isAddingAgent && !m_newAgentName.empty();
        std::string btnLabel = m_isAddingAgent ? "Add" : "+ New Agent";

        if (m_isAddingAgent) {
            RE::SetColor(canConfirm ? 35 : 25, canConfirm ? 60 : 35, 25);
        }
        else {
            RE::SetColor(30, 40, 30);
        }
        RE::FillRect(btnX, y, btnW, h);
        RE::SetStrokeWidth(1);
        RE::SetColor(canConfirm ? Vector3(60, 160, 60) : Vector3(45));
        RE::Rect(btnX, y, btnW, h);

        RE::SetTextSize(20.0f);
        float lw = RE::GetTextWidth(btnLabel);
        RE::SetColor(m_isAddingAgent
            ? (canConfirm ? Vector3(180, 230, 180) : Vector3(70, 90, 70))
            : Vector3(130));
        RE::Text(btnLabel, btnX + (btnW - lw) * 0.5f, textY);

        Vector2 mPos = SDLCore::Input::GetMousePosition();
        if (SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT)
            && IsPointInRect(mPos, btnX, y, btnW, h)) {
            if (m_isAddingAgent && canConfirm) {
                ctx->agentManager.AddAgent(Agent(m_newAgentName, ctx->currentContext.GetConfigString()));
                m_isAddingAgent = false;
                m_newAgentName.clear();
                m_cursorBlinkTimer = 0.0f;
                SDLCore::Input::StopTextInput();
            }
            else if (!m_isAddingAgent) {
                m_isAddingAgent = true;
                m_newAgentName.clear();
                m_cursorBlinkTimer = 0.0f;
                SDLCore::Input::StartTextInput();
            }
        }
    }

    void AgentSelect::RenderAgentList(AppContext* ctx, float x, float y, float w, float h) {
        namespace RE = SDLCore::Render;

        const auto& agents = ctx->agentManager.GetAgents();

        if (agents.empty()) {
            RE::SetTextSize(24.0f);
            RE::SetColor(60);
            RE::Text("No agents available...", x + 10, y + 10);
            return;
        }

        const float rowH = 62.0f;
        const float totalH = static_cast<float>(agents.size()) * rowH;
        const float scrollMax = std::max(0.0f, totalH - h);
        UpdateScroll(m_listScrollOffset, m_listScrollVelocity, scrollMax);

        const float sbW = 6.0f;
        float listW = w;
        if (scrollMax > 0.0f) {
            float trackX = x + w - sbW;
            float thumbH = h * (h / totalH);
            float thumbY = y + (m_listScrollOffset / scrollMax) * (h - thumbH);

            RE::SetColor(35);
            RE::FillRect(trackX, y, sbW, h);
            RE::SetColor(70);
            RE::FillRect(trackX, thumbY, sbW, thumbH);

            listW = w - sbW - 4.0f;
        }

        RE::SetClipRect(static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(w), static_cast<int>(h));

        float rowY = y - m_listScrollOffset;
        int index = 0;
        Vector2 mPos = SDLCore::Input::GetMousePosition();
        bool clicked = SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT);

        for (const auto& [id, agent] : agents) {
            if (rowY + rowH < y) { rowY += rowH; ++index; continue; }
            if (rowY > y + h) break;

            bool isHovered = IsPointInRect(mPos, x, rowY, listW, rowH - 2.0f);
            bool isAgent1 = (ctx->selectedAgentID1 == id);
            bool isAgent2 = (ctx->selectedAgentID2 == id);
            bool isAssigned = isAgent1 || isAgent2;

            if (isAssigned) RE::SetColor(28, 45, 28);
            else if (isHovered) RE::SetColor(35, 35, 42);
            else RE::SetColor(index % 2 == 0 ? 24 : 28);
            RE::FillRect(x, rowY, listW, rowH - 2.0f);

            if (isAgent1) { RE::SetColor(90, 150, 255); RE::FillRect(x, rowY, 4.0f, rowH - 2.0f); }
            if (isAgent2) { RE::SetColor(180, 90, 255); RE::FillRect(x + (isAgent1 ? 4.0f : 0.0f), rowY, 4.0f, rowH - 2.0f); }

            RE::SetTextSize(26.0f);
            RE::SetColor(isAssigned ? 230 : 190);
            RE::Text(agent.GetName(), x + 14, rowY + 8);

            RE::SetTextSize(19.0f);
            RE::SetColor(100);
            char statsBuf[64];
            std::snprintf(statsBuf, sizeof(statsBuf), "W: %d   L: %d",
                agent.GetWonMatches(), agent.GetLostMatches());
            RE::Text(statsBuf, x + 14, rowY + rowH - 24.0f);

            // Delete button
            RE::SetTextSize(18.0f);
            const float delBtnW = 28.0f;
            const float delBtnH = 28.0f;
            float delBtnX = x + listW - delBtnW - 6.0f;
            float delBtnY = rowY + (rowH - 2.0f - delBtnH) * 0.5f;
            bool delHovered = IsPointInRect(mPos, delBtnX, delBtnY, delBtnW, delBtnH);

            RE::SetColor(delHovered ? 180 : 80, 30, 30);
            RE::FillRect(delBtnX, delBtnY, delBtnW, delBtnH);
            RE::SetColor(delHovered ? 255 : 160, 80, 80);
            float xw = RE::GetTextWidth("X");
            RE::Text("X", delBtnX + (delBtnW - xw) * 0.5f,
                delBtnY + (delBtnH - RE::GetTextHeight()) * 0.5f);

            if (clicked && delHovered) {
                if (ctx->selectedAgentID1 == id) ctx->selectedAgentID1 = AgentID{};
                if (ctx->selectedAgentID2 == id) ctx->selectedAgentID2 = AgentID{};
                ctx->agentManager.RemoveAgent(id);
                RE::ResetClipRect();
                return;
            }

            // Badges
            float badgeY = rowY + (rowH - 2.0f - 22.0f) * 0.5f;
            float badgeX = delBtnX - 6.0f;

            auto drawBadge = [&](const char* label, uint8_t r, uint8_t g, uint8_t b) {
                float bw = RE::GetTextWidth(label) + 16.0f;
                badgeX -= bw;
                RE::SetColor(r / 4, g / 4, b / 4);
                RE::FillRect(badgeX, badgeY, bw, 22.0f);
                RE::SetColor(r, g, b);
                RE::Text(label, badgeX + 8, badgeY + 3);
                badgeX -= 6.0f;
                };

            if (isAgent2) drawBadge("Agent 2", 180, 90, 255);
            if (isAgent1) drawBadge("Agent 1", 90, 150, 255);

            // Click to assign
            if (clicked && isHovered && !delHovered) {
                if (m_selectMode == AgentSelectMode::AGENT_1_ONLY) {
                    ctx->selectedAgentID1 = id;
                }
                else if (m_selectMode == AgentSelectMode::AGENT_2_ONLY) {
                    ctx->selectedAgentID2 = id;
                }
                else {
                    if (m_activeSlot == 0) {
                        ctx->selectedAgentID1 = id;
                        if (ctx->selectedAgentID2 == AgentID{}) m_activeSlot = 1;
                    }
                    else {
                        ctx->selectedAgentID2 = id;
                    }
                }
            }

            rowY += rowH;
            ++index;
        }

        RE::ResetClipRect();
    }

    bool AgentSelect::DrawButton(const std::string& text, float x, float y, float w, float h) {
        namespace RE = SDLCore::Render;

        RE::SetColor(40, 40, 40);
        RE::FillRect(x, y, w, h);
        RE::SetColor(80, 80, 80);
        RE::SetStrokeWidth(2);
        RE::Rect(x, y, w, h);

        float textWidth = RE::GetTextWidth(text);
        float textHeight = RE::GetTextHeight();
        RE::SetColor(255);
        RE::Text(text, x + (w - textWidth) * 0.5f, y + (h - textHeight) * 0.5f);

        Vector2 mPos = SDLCore::Input::GetMousePosition();
        bool leftPressed = SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT);
        return leftPressed && IsPointInRect(mPos, x, y, w, h);
    }

    void AgentSelect::UpdateScroll(float& offset, float& velocity, float maxScroll) {
        int dir = 0;
        if (SDLCore::Input::GetScrollDir(dir)) velocity -= dir * m_scrollSpeed;

        velocity -= velocity * m_scrollDrag * SDLCore::Time::GetDeltaTimeSecF();
        offset += velocity * SDLCore::Time::GetDeltaTimeSecF();

        if (offset < 0.0f) { offset = 0.0f; velocity = 0.0f; }
        if (offset > maxScroll) { offset = maxScroll; velocity = 0.0f; }
    }

    bool AgentSelect::IsPointInRect(const Vector2& mPos, float x, float y, float w, float h) {
        return (mPos.x > x && mPos.x < x + w) && (mPos.y > y && mPos.y < y + h);
    }

}