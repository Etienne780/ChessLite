#include "LayerSystem/Layers/AgentSelect.h"
#include "App.h"

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

        // Background
        RE::SetColor(20);
        RE::FillRect(0, 0, W, H);

        // Header
        float headerH = 60.0f;
        RE::SetColor(30);
        RE::FillRect(0, 0, W, headerH);
        RE::SetTextSize(32.0f);
        RE::SetColor(220);
        RE::Text("Select Agent", 20, headerH * 0.5f - RE::GetTextHeight() * 0.5f);

        float y = headerH + 16.0f;

        // Selection bar
        float selBarH = 90.0f;
        RenderSelectionBar(ctx, 20, y, W - 40.0f, selBarH);
        y += selBarH + 12.0f;

        // Divider
        RE::SetColor(45);
        RE::FillRect(20, y, W - 40.0f, 1.0f);
        y += 10.0f;

        // Hint
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

        // Agent list
        RenderAgentList(ctx, 20, y, W - 40.0f, H - y - 10.0f);
    }

    void AgentSelect::OnQuit(AppContext* ctx) {}

    LayerID AgentSelect::GetLayerID() const {
        return LayerID::AGENT_SELECT;
    }

    void AgentSelect::RenderSelectionBar(AppContext* ctx,
        float x, float y,
        float w, float h) {
        namespace RE = SDLCore::Render;

        const auto& agents = ctx->agentManager.GetAgents();
        bool        showBoth = (m_selectMode == AgentSelectMode::BOTH_AGENTS);
        int         cardCount = showBoth ? 2 : 1;
        float       gap = 12.0f;
        float       cardW = (w - gap * (cardCount - 1)) / cardCount;

        auto drawCard = [&](int slotIdx, float cx, float cy, float cw, float ch) {
            AgentID assignedID = (slotIdx == 0)
                ? ctx->selectedAgentID1
                : ctx->selectedAgentID2;

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
                RE::Text(it->second.GetName(),
                    cx + 10, cy + ch * 0.5f - RE::GetTextHeight() * 0.5f + 4);
            }
            else {
                RE::SetColor(55);
                RE::Text("— none selected —",
                    cx + 10, cy + ch * 0.5f - RE::GetTextHeight() * 0.5f + 4);
            }

            if (clickable) {
                Vector2 mPos = SDLCore::Input::GetMousePosition();
                if (SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT)
                    && IsPointInRect(mPos, cx, cy, cw, ch))
                {
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

    void AgentSelect::RenderAgentList(AppContext* ctx,
        float x, float y,
        float w, float h) {
        namespace RE = SDLCore::Render;

        const auto& agents = ctx->agentManager.GetAgents();

        // Early out – nothing to render
        if (agents.empty()) {
            RE::SetTextSize(24.0f);
            RE::SetColor(60);
            RE::Text("No agents available.", x + 10, y + 10);
            return;
        }

        const float rowH = 62.0f;
        const float totalH = static_cast<float>(agents.size()) * rowH;
        const float scrollMax = std::max(0.0f, totalH - h);

        UpdateScroll(m_listScrollOffset, m_listScrollVelocity, scrollMax);

        // Scrollbar
        const float sbW = 6.0f;
        float       listW = w;
        if (scrollMax > 0.0f) {
            float trackX = x + w - sbW;
            float thumbH = h * (h / totalH);                              // safe: totalH > 0
            float thumbY = y + (m_listScrollOffset / scrollMax) * (h - thumbH);

            RE::SetColor(35);
            RE::FillRect(trackX, y, sbW, h);
            RE::SetColor(70);
            RE::FillRect(trackX, thumbY, sbW, thumbH);

            listW = w - sbW - 4.0f;
        }

        RE::SetClipRect(static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(w), static_cast<int>(h));

        float   rowY = y - m_listScrollOffset;
        int     index = 0;
        Vector2 mPos = SDLCore::Input::GetMousePosition();
        bool    clicked = SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT);

        for (const auto& [id, agent] : agents) {
            if (rowY + rowH < y) { rowY += rowH; ++index; continue; }
            if (rowY > y + h)    break;

            bool isHovered = IsPointInRect(mPos, x, rowY, listW, rowH - 2.0f);
            bool isAgent1 = (ctx->selectedAgentID1 == id);
            bool isAgent2 = (ctx->selectedAgentID2 == id);
            bool isAssigned = isAgent1 || isAgent2;

            // Row background
            if (isAssigned) RE::SetColor(28, 45, 28);
            else if (isHovered)  RE::SetColor(35, 35, 42);
            else                 RE::SetColor(index % 2 == 0 ? 24 : 28);
            RE::FillRect(x, rowY, listW, rowH - 2.0f);

            // Accent bars
            if (isAgent1) { RE::SetColor(90, 150, 255); RE::FillRect(x, rowY, 4.0f, rowH - 2.0f); }
            if (isAgent2) { RE::SetColor(180, 90, 255);  RE::FillRect(x + (isAgent1 ? 4.0f : 0.0f), rowY, 4.0f, rowH - 2.0f); }

            // Name
            RE::SetTextSize(26.0f);
            RE::SetColor(isAssigned ? 230 : 190);
            RE::Text(agent.GetName(), x + 14, rowY + 8);

            // Stats
            RE::SetTextSize(19.0f);
            RE::SetColor(100);
            char statsBuf[64];
            std::snprintf(statsBuf, sizeof(statsBuf),
                "W: %d   L: %d", agent.GetWonMatches(), agent.GetLostMatches());
            RE::Text(statsBuf, x + 14, rowY + rowH - 24.0f);

            // Badges
            RE::SetTextSize(18.0f);
            float badgeY = rowY + (rowH - 2.0f - 22.0f) * 0.5f;
            float badgeX = x + listW - 10.0f;

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
            if (clicked && isHovered) {
                if (m_selectMode == AgentSelectMode::AGENT_1_ONLY) ctx->selectedAgentID1 = id;
                else if (m_selectMode == AgentSelectMode::AGENT_2_ONLY) ctx->selectedAgentID2 = id;
                else {
                    if (m_activeSlot == 0) {
                        ctx->selectedAgentID1 = id;
                        // Auto-advance to slot 2 if still empty
                        if (ctx->selectedAgentID2 == AgentID{})
                            m_activeSlot = 1;
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

    void AgentSelect::UpdateScroll(float& offset, float& velocity, float maxScroll) {
        int dir = 0;
        if (SDLCore::Input::GetScrollDir(dir))
            velocity -= dir * m_scrollSpeed;

        velocity -= velocity * m_scrollDrag * SDLCore::Time::GetDeltaTimeSecF();
        offset += velocity * SDLCore::Time::GetDeltaTimeSecF();

        if (offset < 0.0f) { offset = 0.0f;      velocity = 0.0f; }
        if (offset > maxScroll) { offset = maxScroll;  velocity = 0.0f; }
    }

    bool AgentSelect::IsPointInRect(const Vector2& mPos,
        float x, float y,
        float w, float h) {
        return (mPos.x > x && mPos.x < x + w) &&
            (mPos.y > y && mPos.y < y + h);
    }

}