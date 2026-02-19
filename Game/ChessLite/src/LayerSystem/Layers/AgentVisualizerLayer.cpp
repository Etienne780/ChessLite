#include "LayerSystem/Layers/AgentVisualizerLayer.h"
#include "App.h"
#include "Styles/Comman/Space.h"
#include "Styles/Comman/Color.h"
#include "Styles/Comman/Style.h"
#include "UIComponents/Button.h"

namespace UI = SDLCore::UI;
namespace UIComp = UIComponent;

namespace Layers {

	AgentVisualizerLayer::AgentVisualizerLayer(PlayerType player1, PlayerType player2) 
		: m_player1(player1), m_player2(player2) {

		if (player1 == PlayerType::PLAYER && 
			player2 == PlayerType::AI) {
			m_isAgent1Shown = false;
		}
	}

	void AgentVisualizerLayer::OnStart(AppContext* ctx) {
		using namespace SDLCore;
		namespace Prop = UI::Properties;

		m_agentID1 = ctx->selectedAgentID1;
		m_agentID2 = ctx->selectedAgentID2;

		m_UICtx = UI::CreateContext();

		m_styleHeader
			.Merge(Style::commanStretch)
			.SetValue(Prop::heightUnit, UI::UISizeUnit::PX)
			.SetValue(Prop::height, m_headerHeightPixel)
			.SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
			.SetValue(Prop::align, UI::UIAlignment::START, UI::UIAlignment::CENTER)
			.SetValue(Prop::backgroundColor, Style::commanColorUIPanelDark);

		SetupWindow(ctx);
	}

	void AgentVisualizerLayer::OnUpdate(AppContext* ctx) {
		using namespace SDLCore;
		Input::SetWindow(m_winID);
	}

	void AgentVisualizerLayer::OnRender(AppContext* ctx) {
		namespace RE = SDLCore::Render;

		if (m_winID.IsInvalid())
			return;

		SDLCore::Input::SetWindow(m_winID);
		RE::SetWindowRenderer(m_winID);
		RE::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
		RE::SetColor(25);
		RE::Clear();

		AgentID id = (m_isAgent1Shown) ? m_agentID1 : m_agentID2;
		Agent* agent = ctx->agentManager.GetAgent(id);
		RenderUIBody(agent);
	}

	void AgentVisualizerLayer::OnUIRender(AppContext* ctx) {
		namespace RE = SDLCore::Render;
		typedef UI::UIKey Key;

		if (m_winID.IsInvalid())
			return;

		SDLCore::Input::SetWindow(m_winID);
		RE::SetWindowRenderer(m_winID);
		UI::SetContextWindow(m_UICtx, m_winID);
		UI::BindContext(m_UICtx);

		UI::BeginFrame(Key("root"), Style::commanRootTransparent);
		{
			UI::BeginFrame(Key("header"), m_styleHeader);
			{
				if (UIComp::DrawTabButton("tab_agent_1", "Agent 1", m_isAgent1Shown, 
					Style::commanBTNTabNormal, 
					Style::commanBTNTabActive, 
					m_player1 == PlayerType::PLAYER)) 
				{
					m_isAgent1Shown = true;
				}

				if (UIComp::DrawTabButton("tab_agent_2", "Agent 2", !m_isAgent1Shown, 
					Style::commanBTNTabNormal, 
					Style::commanBTNTabActive,
					m_player2 == PlayerType::PLAYER)) 
				{
					m_isAgent1Shown = false;
				}
			}
			UI::EndFrame();
		}
		UI::EndFrame();

		RE::Present();
	}

	void AgentVisualizerLayer::OnQuit(AppContext* ctx) {
		ctx->app->DeleteWindow(m_winID);
		UI::DestroyContext(m_UICtx);
		m_UICtx = nullptr;
	}

	LayerID AgentVisualizerLayer::GetLayerID() const {
		return LayerID::AGENT_VISUALIZER;
	}

	void AgentVisualizerLayer::SetupWindow(AppContext* ctx) {
		auto* win = ctx->app->CreateWindow(&m_winID, "Visualizer", 800, 500);
		win->SetWindowMinSize(600, 400);

		namespace RE = SDLCore::Render;
		RE::SetWindowRenderer(m_winID);
		RE::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
		RE::SetColor(25);
		RE::Clear();
		RE::Present();

		win->AddOnDestroy([&]() {
			auto* app = App::GetInstance();
			if (app)
				app->PopLayer(LayerID::AGENT_VISUALIZER);
		});
	}

	void AgentVisualizerLayer::RenderUIBody(const Agent* agent) {
		namespace RE = SDLCore::Render;
		if (!agent)
			return;

		RE::ResetTextParams();

		auto viewport = RE::GetViewport();
		float width = static_cast<float>(viewport.w);
		float height = static_cast<float>(viewport.h);

		Vector2 mousePos = SDLCore::Input::GetMousePosition();
		float reservedTop = m_headerHeightPixel;
		float usableHeight = height - reservedTop;

		RE::SetColor(25);
		RE::FillRect(0, reservedTop, width, usableHeight);

		float textPadding = 5;
		float horHeaderOffset = 20;
		float localHeaderHeight = m_headerHeight;
		RE::SetColor(35);
		RE::FillRect(0, reservedTop, width, localHeaderHeight);
		RE::SetColor(255);
		RE::SetTextSize(36.0f);

		std::string agentNameStr = "Agent: " + agent->GetName();
		RE::Text(agentNameStr, horHeaderOffset, reservedTop + localHeaderHeight * 0.5f);
		RE::SetColor(200);

		float headerHeight = RE::GetTextHeight();

		horHeaderOffset += textPadding + RE::GetTextWidth(agentNameStr);
		std::string matchesText = "Matches: " + std::to_string(agent->GetMatchesPlayed());
		RE::SetTextSize(16.0f);

		float currentTextHeight = RE::GetTextHeight();

		RE::SetTextAlign(SDLCore::Align::START, SDLCore::Align::CENTER);
		RE::Text(matchesText, horHeaderOffset, reservedTop + localHeaderHeight * 0.5f + headerHeight * 0.5f - currentTextHeight * 0.5f);

		std::string whiteMatchesText = "Played as white: " + std::to_string(agent->GetMatchesPlayedAsWhite());
		RE::Text(whiteMatchesText, horHeaderOffset, reservedTop + localHeaderHeight * 0.5f + headerHeight * 0.5f + currentTextHeight * 0.5f);

		RE::SetTextSize(36.0f);
		RE::SetTextAlign(SDLCore::Align::START);
		float copyBTNWidth = 250.0f;
		float copyBTNHeight = 48.0f;
		std::string copyText = (m_currentClipBoardCopyTimer > 0) ? "Copied" : "Copy Config";
		if (DrawButton(copyText, width - copyBTNWidth - 15.0f, reservedTop + localHeaderHeight - copyBTNHeight - 10.0f, copyBTNWidth, copyBTNHeight)) {
			auto* app = App::GetInstance();
			if (app) {
				app->SetClipboardText(agent->GetChessConfig());
				m_currentClipBoardCopyTimer = m_clipBoardCopyTime;
			}
		}

		if (m_currentClipBoardCopyTimer > 0)
			m_currentClipBoardCopyTimer -= SDLCore::Time::GetDeltaTimeSecF();

		float leftWidth = width * 0.35f;
		float rightWidth = width - leftWidth;
		float contentTop = reservedTop + localHeaderHeight;
		float contentHeight = height - contentTop;

		// Left panel (History)
		RE::SetColor(30);
		RE::FillRect(0, contentTop, leftWidth, contentHeight);
		bool insideLeftPanel = IsPointInRect(mousePos,
			leftWidth, contentTop, rightWidth, contentHeight);

		const auto& history = agent->GetMoveHistory();
		const auto& states = agent->GetNormilzedBoardStates();

		float entryHeight = 140.0f;
		m_historyScrollMax = std::max(0.0f, history.size() * entryHeight - contentHeight);
		if(insideLeftPanel)
			UpdateScroll(m_historyScrollOffset, m_historyScrollVelocity, m_historyScrollMax);

		RE::SetClipRect(0, static_cast<int>(contentTop), static_cast<int>(leftWidth), static_cast<int>(contentHeight));
		float y = contentTop - m_historyScrollOffset;
		for (const auto& entry : history) {
			RE::SetColor(45);
			RE::FillRect(10, y, leftWidth - 20, entryHeight - 10);

			DrawBoard(entry.first, 20, y + 10, entryHeight - 40);

			auto it = states.find(entry.first);
			if (it != states.end()) {
				const GameMove& gameMove = it->second.GetMove(entry.second);
				std::string moveText = ToChessNotation(gameMove.GetFrom())
					+ " -> "
					+ ToChessNotation(gameMove.GetTo());
				float textWidth = RE::GetTextWidth(moveText);
				RE::SetColor(255);
				RE::Text(moveText, std::min(130.0f, leftWidth - textWidth - 10.0f), y + 50);
			}
			y += entryHeight;
		}
		RE::ResetClipRect();

		// Right panel (States & Moves)
		RE::SetColor(28);
		RE::FillRect(leftWidth, contentTop, rightWidth, contentHeight);
		bool insideRightPanel = IsPointInRect(mousePos, 
			leftWidth, contentTop, rightWidth, contentHeight);

		// Layout constants
		float miniBoardSize = 60.0f;
		float moveBoxW = 80.0f;
		float moveBoxH = 80.0f;
		float listLineH = 42.0f;
		float sectionPad = 12.0f;
		float evalBarMaxW = 90.0f;
		float evalBarH = 10.0f;
		float panelX = leftWidth + 20.0f;
		float panelW = rightWidth - 40.0f;

		float estimatedH = 0.0f;
		for (const auto& [stateStr, boardState] : states) {
			int moveCount = static_cast<int>(boardState.GetPossibleMoves().size());
			int gridCols = std::max(1, static_cast<int>(panelW / (moveBoxW + 5.0f)));
			int gridRows = (moveCount + gridCols - 1) / gridCols;

			float stateLineH = RE::GetTextHeight();     
			float gridViewH = RE::GetTextHeight() + 2.0f
				+ gridRows * (moveBoxH + 5.0f);
			float listViewH = RE::GetTextHeight()
				+ moveCount * listLineH;
			float dividerH = 1.0f + sectionPad;

			estimatedH += m_padding
				+ stateLineH
				+ miniBoardSize + sectionPad
				+ gridViewH + sectionPad
				+ listViewH + sectionPad * 2
				+ dividerH;
		}

		m_detailScrollMax = std::max(0.0f, estimatedH - contentHeight);
		if(insideRightPanel)
			UpdateScroll(m_detailScrollOffset, m_detailScrollVelocity, m_detailScrollMax);

		RE::SetClipRect(static_cast<int>(leftWidth), static_cast<int>(contentTop),
			static_cast<int>(rightWidth), static_cast<int>(contentHeight));

		float detailY = contentTop - m_detailScrollOffset + m_padding;

		for (const auto& [stateStr, boardState] : states) {
			const auto& moves = boardState.GetPossibleMoves();
			int moveCount = static_cast<int>(moves.size());
			int gridCols = std::max(1, static_cast<int>(panelW / (moveBoxW + 5.0f)));

			RE::SetColor(255);
			RE::Text("State: " + stateStr, panelX, detailY);
			detailY += RE::GetTextHeight();

			DrawBoard(stateStr, panelX, detailY, miniBoardSize);
			detailY += miniBoardSize + sectionPad;

			RE::SetColor(255);
			RE::Text("Grid View", panelX, detailY);
			detailY += RE::GetTextHeight() + 2.0f;

			float cellW = moveBoxW + 5.0f;
			float cellH = moveBoxH + 5.0f;
			float boardSz = moveBoxW * 0.65f;
			float labelH = moveBoxH - boardSz - 4.0f;

			for (int i = 0; i < moveCount; ++i) {
				const GameMove& move = boardState.GetMove(i);
				float eval = move.GetEvaluation();

				int col = i % gridCols;
				int row = i / gridCols;
				float bx = panelX + col * cellW;
				float by = detailY + row * cellH;

				if (eval > 0.0f) RE::SetColor(30, 55, 30);
				else if (eval < 0.0f) 
					RE::SetColor(55, 30, 30);
				else                  
					RE::SetColor(40, 40, 40);
				RE::FillRect(bx, by, moveBoxW, moveBoxH);

				RE::SetColor(60);
				RE::SetStrokeWidth(1);
				RE::Rect(bx, by, moveBoxW, moveBoxH);

				float boardOffX = (moveBoxW - boardSz) * 0.5f;
				DrawBoard(stateStr, bx + boardOffX, by + 2.0f, boardSz);

				Vector2 from = move.GetFrom();
				float cellSize = boardSz / 3.0f;
				RE::SetColor(255, 220, 0, 120);
				RE::FillRect(bx + boardOffX + from.x * cellSize,
					by + 2.0f + from.y * cellSize,
					cellSize, cellSize);

				Vector2 to = move.GetTo();
				RE::SetColor(255, 255, 255, 80);
				RE::FillRect(bx + boardOffX + to.x * cellSize,
					by + 2.0f + to.y * cellSize,
					cellSize, cellSize);

				RE::SetTextSize(18.0f);
				std::string label = ToChessNotation(from) + "->" + ToChessNotation(to);
				float lw = RE::GetTextWidth(label);
				float lx = bx + (moveBoxW - lw) * 0.5f;
				RE::SetColor(220);
				RE::Text(label, lx, by + boardSz + 4.0f);
			}

			RE::SetTextSize(36.0f);

			int gridRows = (moveCount + gridCols - 1) / gridCols;
			detailY += gridRows * cellH + sectionPad;

			RE::SetColor(255);
			RE::Text("List View", panelX, detailY);
			detailY += RE::GetTextHeight();

			for (int i = 0; i < moveCount; ++i) {
				const GameMove& move = boardState.GetMove(i);
				float eval = move.GetEvaluation();
				float rowY = detailY + i * listLineH;
				float rowCenterY = rowY + listLineH * 0.5f;

				RE::SetColor(i % 2 == 0 ? 33 : 38);
				RE::FillRect(panelX, rowY, panelW, listLineH - 1.0f);

				Vector2 from = move.GetFrom();
				Vector2 to = move.GetTo();
				std::string moveStr = ToChessNotation(from) + " -> " + ToChessNotation(to);
				RE::SetColor(220);
				RE::Text(moveStr, panelX + 5.0f, rowCenterY - RE::GetTextHeight() * 0.5f);

				float barX = panelX + 200.0f;
				float barY = rowCenterY - evalBarH * 0.5f;
				float clampEval = std::max(-1.0f, std::min(1.0f, eval * 0.5f));

				RE::SetColor(25);
				RE::FillRect(barX, barY, evalBarMaxW, evalBarH);

				float midX = barX + evalBarMaxW * 0.5f;
				float halfW = std::abs(clampEval) * (evalBarMaxW * 0.5f);
				if (eval > 0.0f) {
					RE::SetColor(60, 180, 60);
					RE::FillRect(midX, barY, halfW, evalBarH);
				} else if(eval < 0.0f) {
					RE::SetColor(180, 60, 60);
					RE::FillRect(midX - halfW, barY, halfW, evalBarH);
				}

				RE::SetColor(80);
				RE::FillRect(midX - 1.0f, barY, 2.0f, evalBarH);

				std::string evalStr = (eval >= 0 ? "+" : "") +
					std::to_string(static_cast<int>(std::round(eval * 100.0f) / 100.0f * 100) / 100);
		
				if (eval > 0.0f)
					RE::SetColor(60, 180, 60);
				else if (eval < 0.0f)
					RE::SetColor(180, 60, 60);
				else
					RE::SetColor(40, 40, 40);


				RE::Text(evalStr, barX + evalBarMaxW + 6.0f, rowCenterY - RE::GetTextHeight() * 0.5f);
			}

			detailY += moveCount * listLineH + sectionPad * 2;

			// Divider between states
			RE::SetColor(50);
			RE::FillRect(panelX, detailY, panelW, 1.0f);
			detailY += sectionPad;
		}

		RE::ResetClipRect();
	}

	void AgentVisualizerLayer::UpdateScroll(float& offset, float& velocity, float maxScroll) {
		int dir = 0;
		if (SDLCore::Input::GetScrollDir(dir)) {
			velocity -= dir * m_scrollSpeed;
		}

		velocity -= velocity * m_scrollDrag * SDLCore::Time::GetDeltaTimeSecF();

		offset += velocity * SDLCore::Time::GetDeltaTimeSecF();

		if (offset < 0.0f) {
			offset = 0.0f;
			velocity = 0.0f;
		}

		if (offset > maxScroll) {
			offset = maxScroll;
			velocity = 0.0f;
		}
	}

	bool AgentVisualizerLayer::DrawButton(const std::string& text, float x, float y, float w, float h) {
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
		bool inside = (mPos.x >= x && mPos.x < (x + w)) && (mPos.y >= y && mPos.y < (y + h));

		return leftPressed && inside;
	}

	void AgentVisualizerLayer::DrawBoard(const std::string& state, float x, float y, float size) {
		namespace RE = SDLCore::Render;
		float cell = size / 3.0f;

		for (int i = 0; i < 9; ++i) {
			int row = i / 3;
			int col = i % 3;
			float cx = x + col * cell;
			float cy = y + row * cell;

			int value = state[i] - '0';
			if (value == 0) 
				RE::SetColor(50);
			else if (value == 1) 
				RE::SetColor(200, 80, 80);
			else if (value == 2) 
				RE::SetColor(80, 80, 200);

			RE::FillRect(cx, cy, cell, cell);

			// Board grid lines
			RE::SetColor(20);
			RE::SetStrokeWidth(2);
			RE::Rect(cx, cy, cell, cell);
		}
	}

	std::string AgentVisualizerLayer::ToChessNotation(const Vector2& pos) {
		char col = 'A' + static_cast<int>(pos.x);
		char row = '1' + (2 - static_cast<int>(pos.y));// flip y axis
		return std::string(1, col) + row;
	}

	bool AgentVisualizerLayer::IsPointInRect(const Vector2& mPos, float x, float y, float w, float h) {
		return (mPos.x > x && mPos.x < x + w) &&
			(mPos.y > y && mPos.y < y + h);
	}

}