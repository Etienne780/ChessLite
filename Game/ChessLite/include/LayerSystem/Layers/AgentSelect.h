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

		AgentSelectMode m_selectMode = AgentSelectMode::AGENT_1_ONLY;

		int	m_activeSlot = 0;

		// Scroll tuning
		float m_scrollSpeed = 600.0f;
		float m_scrollDrag = 10.0f;

		float m_listScrollOffset = 0.0f;
		float m_listScrollVelocity = 0.0f;
		float m_headerheightPx = 250.0f;

		void RenderSelectionBar(AppContext* ctx, float x, float y, float w, float h);
		void RenderAgentList(AppContext* ctx, float x, float y, float w, float h);

		void UpdateScroll(float& offset, float& velocity, float maxScroll);
		bool IsPointInRect(const Vector2& p, float x, float y, float w, float h);
	};

}