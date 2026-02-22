#include <sstream>
#include <algorithm>
#include <cctype>

#include "LayerSystem/Layers/AgentSelectLayer.h"
#include "App.h"
#include "FilePaths.h"

namespace Layers {

    AgentSelect::AgentSelect(AgentSelectMode mode)
        : m_selectMode(mode) {
    }

    void AgentSelect::OnStart(AppContext* ctx) {
        m_activeSlot = (m_selectMode == AgentSelectMode::AGENT_2_ONLY) ? 1 : 0;

        ctx->selectedAgentID1.SetInvalid();
        ctx->selectedAgentID2.SetInvalid();
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
        RenderInputBar(ctx, 20, y, W - 40.0f, addBarH);
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

    void AgentSelect::RenderInputBar(AppContext* ctx, float x, float y, float w, float h) {
        namespace RE = SDLCore::Render;

        const float btnW = 150.0f;
        const float gap = 8.0f;
        const float fieldW = w - btnW - gap;
        float textY = y + h * 0.5f - RE::GetTextHeight() * 0.5f;

        // Input field
        RE::SetColor(m_isInputBarActive ? 32 : 24);
        RE::FillRect(x, y, fieldW, h);
        RE::SetStrokeWidth(1);
        RE::SetColor(m_isInputBarActive ? Vector3(90, 150, 255) : Vector3(50));
        RE::Rect(x, y, fieldW, h);

        Vector2 mPos = SDLCore::Input::GetMousePosition();
        bool isInputBarHover = IsPointInRect(mPos, x, y, fieldW, h);
        bool canConfirm = m_isAddingAgent && m_inputBarText.size() > 3;

        RE::SetTextSize(22.0f);

        if (m_isInputBarActive) {
            m_cursorBlinkTimer += SDLCore::Time::GetDeltaTimeSecF();
            std::string display = m_inputBarText;
            if (std::fmod(m_cursorBlinkTimer, 1.0f) < 0.5f) display += "|";

            RE::SetColor(220);
            RE::Text(display.empty() ? "|" : display, x + 10, textY);

            std::string typed = SDLCore::Input::GetTextInputBuffer();
            for (char c : typed) {
                if (std::isprint(static_cast<unsigned char>(c))) m_inputBarText += c;
            }

            if (SDLCore::Input::KeyRepeating(SDLCore::KeyCode::BACKSPACE) && !m_inputBarText.empty()) {
                m_inputBarText.pop_back();
            }

            if (SDLCore::Input::KeyJustPressed(SDLCore::KeyCode::ESCAPE)) {
                m_isAddingAgent = false;
                m_isInputBarActive = false;
                m_inputBarText.clear();
                m_cursorBlinkTimer = 0.0f;
                SDLCore::Input::StopTextInput();
            }

            if (SDLCore::Input::KeyJustPressed(SDLCore::KeyCode::RETURN)) {
                if(canConfirm)
                    ctx->agentManager.AddAgent(Agent(m_inputBarText, ctx->currentContext.GetConfigString()));
                m_isAddingAgent = false;
                m_isInputBarActive = false;
                m_inputBarText.clear();
                m_cursorBlinkTimer = 0.0f;
                SDLCore::Input::StopTextInput();
            }
        }
        else {
            RE::SetColor(60);
            RE::Text("Search/Create Agent...", x + 10, textY);
        }

        // Button
        float btnX = x + fieldW + gap;
        std::string btnLabel = m_isAddingAgent ? "Create Agent" : "+ New Agent";
        bool isHoverCreateBTN = IsPointInRect(mPos, btnX, y, btnW, h);

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

        if (SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT) 
            && isHoverCreateBTN) {
            if (m_isAddingAgent && canConfirm) {
                ctx->agentManager.AddAgent(Agent(m_inputBarText, ctx->currentContext.GetConfigString()));
                m_isAddingAgent = false;
                m_inputBarText.clear();
                m_cursorBlinkTimer = 0.0f;
                SDLCore::Input::StopTextInput();
            }
            else if (!m_isAddingAgent) {
                if (!m_isInputBarActive) {
                    m_inputBarText.clear();
                    m_cursorBlinkTimer = 0.0f;
                    SDLCore::Input::StartTextInput();
                }
                m_isAddingAgent = true;
                m_isInputBarActive = true;
            }
        }

        if (SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT) && !isHoverCreateBTN) {
            m_inputBarText.clear();
            m_cursorBlinkTimer = 0.0f;

            if (isInputBarHover) {
                SDLCore::Input::StartTextInput();
                m_isInputBarActive = true;
            }
            else {
                SDLCore::Input::StopTextInput();
                m_isInputBarActive = false;
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

        const float rowH = 72.0f;
        const auto& sortedAgents = SortAgents(m_inputBarText, agents);

        float totalH = static_cast<float>(sortedAgents.size()) * rowH;
        float scrollMax = std::max(0.0f, totalH - h);
        UpdateScroll(m_listScrollOffset, m_listScrollVelocity, scrollMax);

        const float scrollBarW = 6.0f;
        float listW = w;
        if (scrollMax > 0.0f) {
            float trackX = x + w - scrollBarW;
            float thumbH = h * (h / totalH);
            float thumbY = y + (m_listScrollOffset / scrollMax) * (h - thumbH);

            RE::SetColor(35);
            RE::FillRect(trackX, y, scrollBarW, h);
            RE::SetColor(70);
            RE::FillRect(trackX, thumbY, scrollBarW, thumbH);

            listW = w - scrollBarW - 4.0f;
        }

        RE::SetClipRect(static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(w), static_cast<int>(h));

        float rowY = y - m_listScrollOffset;
        int index = 0;
        Vector2 mPos = SDLCore::Input::GetMousePosition();
        bool clicked = SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT);

        for (const auto& agent : sortedAgents) {
            float elementBottom = rowY + rowH;
            if (elementBottom < y) {
                rowY += rowH;
                ++index;
                continue;
            }
            if (rowY > y + h) 
                break;

            AgentID agentID = agent.GetID();

            bool isHovered = IsPointInRect(mPos, x, rowY, listW, rowH - 2.0f);
            bool isAgent1 = (ctx->selectedAgentID1 == agentID);
            bool isAgent2 = (ctx->selectedAgentID2 == agentID);
            bool isAssigned = isAgent1 || isAgent2;
            float selectLineWidth = 4.0f;

            if (isAssigned) 
                RE::SetColor(28, 45, 28);
            else if (isHovered) 
                RE::SetColor(35, 35, 42);
            else 
                RE::SetColor(index % 2 == 0 ? 24 : 28);
            RE::FillRect(x, rowY, listW, rowH - 2.0f);

            if (isAgent1) { 
                RE::SetColor(90, 150, 255); 
                RE::FillRect(x, rowY, selectLineWidth, rowH - 2.0f);
            }

            if (isAgent2) { 
                RE::SetColor(180, 90, 255); 
                RE::FillRect(x + (isAgent1 ? selectLineWidth : 0.0f), rowY, selectLineWidth, rowH - 2.0f);
            }

            // Name
            RE::SetTextSize(24.0f);
            RE::SetColor(isAssigned ? 230 : 190);
            RE::Text(agent.GetName(), x + 14, rowY + 6);

            // Winrate + Matches
            int played = agent.GetMatchesPlayed();
            int won = agent.GetWonMatches();
            int lost = agent.GetLostMatches();
            float winrate = (played > 0) ? (won * 100.0f / played) : 0.0f;

            char line1[80];
            std::snprintf(line1, sizeof(line1),
                "Win: %.1f%%   Won: %d   Lost: %d   Played: %d",
                winrate, won, lost, played);
            RE::SetTextSize(17.0f);
            RE::SetColor(isAssigned ? 140 : 110);
            RE::Text(line1, x + 14, rowY + rowH * 0.45f);

            // Zeile 2: Weiß / Schwarz
            char line2[80];
            std::snprintf(line2, sizeof(line2),
                "White — Won: %d  Lost: %d     Black — Won: %d  Lost: %d",
                agent.GetMatchesWonAsWhite(), agent.GetMatchesLostAsWhite(),
                agent.GetMatchesWonAsBlack(), agent.GetMatchesLostAsBlack());
            RE::SetColor(isAssigned ? 110 : 80);
            RE::Text(line2, x + 14, rowY + rowH * 0.72f);

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
                if (ctx->selectedAgentID1 == agentID) ctx->selectedAgentID1 = AgentID{};
                if (ctx->selectedAgentID2 == agentID) ctx->selectedAgentID2 = AgentID{};
                ctx->agentManager.RemoveAgent(agentID);
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
                    ctx->selectedAgentID1 = agentID;
                }
                else if (m_selectMode == AgentSelectMode::AGENT_2_ONLY) {
                    ctx->selectedAgentID2 = agentID;
                }
                else {
                    if (m_activeSlot == 0) {
                        ctx->selectedAgentID1 = agentID;
                        if (ctx->selectedAgentID2.IsInvalid()) m_activeSlot = 1;
                    }
                    else {
                        ctx->selectedAgentID2 = agentID;
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

    const std::vector<Agent>& AgentSelect::SortAgents(
        const std::string& searchInput,
        const std::unordered_map<AgentID, Agent>& agents)
    {
        std::string normalizedInput = ToLower(searchInput);

        if (m_sortedAgentsSearchInput == normalizedInput &&
            m_cachedSortedAgents.size() == agents.size()) {
            return m_cachedSortedAgents;
        }

        // empty search -> show all agents
        if (normalizedInput.empty() || m_isAddingAgent) {
            m_sortedAgentsSearchInput = normalizedInput;
            m_cachedSortedAgents.clear();
            m_cachedSortedAgents.reserve(agents.size());

            for (const auto& [_, agent] : agents)
                m_cachedSortedAgents.push_back(agent);

            return m_cachedSortedAgents;
        }

        std::vector<std::pair<int, Agent>> scoredAgents;
        std::vector<std::string> tokens = SplitTokens(normalizedInput);

        for (const auto& [_, agent] : agents) {
            std::string name = ToLower(agent.GetName());
            int matchesPlayed = agent.GetMatchesPlayed();
            int wonMatches = agent.GetWonMatches();
            int lostMatches = agent.GetLostMatches();

            int matchesPlayedAsWhite = agent.GetMatchesPlayedAsWhite();
            int matchesPlayedAsBlack = agent.GetMatchesPlayedAsBlack();

            int matchesWonAsWhite = agent.GetMatchesWonAsWhite();
            int matchesWonAsBlack = agent.GetMatchesWonAsBlack();

            int matchesLostAsWhite = agent.GetMatchesLostAsWhite();
            int matchesLostAsBlack = agent.GetMatchesLostAsBlack();

            float explorationChance = agent.GetExplorationChance();

            bool valid = true;
            int finalScore = 0;

            for (const std::string& token : tokens) {
                int score = 0;

                if (token.find("matchesplayed") == 0) {
                    if (!EvaluateIntFilter(token, matchesPlayed, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else if (token.find("matcheswon") == 0) {
                    if (!EvaluateIntFilter(token, wonMatches, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else if (token.find("matcheslost") == 0) {
                    if (!EvaluateIntFilter(token, lostMatches, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else if (token.find("matchesplayedaswhite") == 0) {
                    if (!EvaluateIntFilter(token, matchesPlayedAsWhite, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else if (token.find("matchesplayedasblack") == 0) {
                    if (!EvaluateIntFilter(token, matchesPlayedAsBlack, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else if (token.find("matcheswonaswhite") == 0) {
                    if (!EvaluateIntFilter(token, matchesWonAsWhite, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else if (token.find("matcheswonasblack") == 0) {
                    if (!EvaluateIntFilter(token, matchesWonAsBlack, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else if (token.find("matcheslostaswhite") == 0) {
                    if (!EvaluateIntFilter(token, matchesLostAsBlack, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else if (token.find("matcheslostasblack") == 0) {
                    if (!EvaluateIntFilter(token, matchesLostAsBlack, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else if (token.find("explorationchance") == 0) {
                    if (!EvaluateFloatFilter(token, explorationChance, score)) {
                        valid = false;
                        break;
                    }
                    finalScore += score;
                }
                else {
                    size_t pos = name.find(token);

                    if (pos == std::string::npos) {
                        valid = false;
                        break;
                    }

                    if (pos == 0) {
                        finalScore += 100;
                    }
                    else {
                        finalScore += 50;
                    }
                }
            }

            if (valid)
                scoredAgents.emplace_back(finalScore, agent);
        }

        // sort descending by score
        std::sort(scoredAgents.begin(), scoredAgents.end(),
            [](const auto& a, const auto& b) {
                return a.first > b.first;
            });

        m_cachedSortedAgents.clear();
        m_cachedSortedAgents.reserve(scoredAgents.size());

        for (const auto& [score, agent] : scoredAgents) {
            m_cachedSortedAgents.push_back(agent);
        }

        m_sortedAgentsSearchInput = normalizedInput;

        return m_cachedSortedAgents;
    }

    std::vector<std::string> AgentSelect::SplitTokens(const std::string& input) {
        std::vector<std::string> tokens;
        std::istringstream stream(input);
        std::string token;

        while (stream >> token) {
            tokens.push_back(token);
        }

        return tokens;
    }

    template<typename T, typename ConvertFunc>
    bool AgentSelect::EvaluateNumberFilter(
        const std::string& token,
        T value,
        ConvertFunc convertFunc,
        int& outScore)
    {
        const int weight = 10;
        outScore = 0;

        try {
            size_t pos;

            if ((pos = token.find("<")) != std::string::npos) {
                T compare = convertFunc(token.substr(pos + 1));

                if (value < compare) {
                    outScore = weight;
                    return true;
                }

                return false;
            }
            else if ((pos = token.find(">")) != std::string::npos) {
                T compare = convertFunc(token.substr(pos + 1));

                if (value > compare) {
                    outScore = weight;
                    return true;
                }

                return false;
            }
            else if ((pos = token.find("=")) != std::string::npos) {
                T compare = convertFunc(token.substr(pos + 1));

                if (value == compare) {
                    outScore = weight;
                    return true;
                }

                return false;
            }
        }
        catch (...) {
            return false;
        }

        return false;
    }

    bool AgentSelect::EvaluateIntFilter(const std::string& token, int value, int& outScore) {
        return EvaluateNumberFilter<int>(
            token,
            value,
            [](const std::string& s) {
                return std::stoi(s);
            },
            outScore);
    }

    bool AgentSelect::EvaluateFloatFilter(const std::string& token, float value, int& outScore) {
        return EvaluateNumberFilter<float>(
            token,
            value,
            [](const std::string& s) {
                return std::stof(s);
            },
            outScore);
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
        return (mPos.x > x && mPos.x < x + w) && 
            (mPos.y > y && mPos.y < y + h);
    }

    std::string AgentSelect::ToLower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return str;
    }

}