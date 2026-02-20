#pragma once
#include <SDLCoreLib/SDLCoreUI.h>
#include "AI/AgentManager.h"

#include "LayerSystem/Layer.h"

namespace Layers {

	class AgentSelect : public Layer {
	public:
		AgentSelect(AgentSelectMode mode);
		~AgentSelect() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnLateRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		SDLCore::UI::UIStyle m_styleTitle;
		SDLCore::UI::UIStyle m_styleHeader;
		float m_headerheightPx = 250.0f;

		AgentSelectMode m_selectMode = AgentSelectMode::AGENT_1_ONLY;

		int m_activeSlot = 0;
		float m_listScrollOffset = 0.0f;
		float m_listScrollVelocity = 0.0f;
		float m_scrollSpeed = 600.0f;
		float m_scrollDrag = 10.0f;
		float m_saveTimer = 0.0f;
		const float m_saveTimerDuration = 2.0f;

		bool m_isAddingAgent = false;
		std::string m_newAgentName = "";
		float m_cursorBlinkTimer = 0.0f;

		void RenderSelectionBar(AppContext* ctx, float x, float y, float w, float h);
		void RenderAddBar(AppContext* ctx, float x, float y, float w, float h);
		void RenderAgentList(AppContext* ctx, float x, float y, float w, float h);
		void UpdateScroll(float& offset, float& velocity, float maxScroll);
		bool DrawButton(const std::string& text, float x, float y, float w, float h);
		bool IsPointInRect(const Vector2& mPos, float x, float y, float w, float h);
	};

}