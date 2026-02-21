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

		bool m_isInputBarActive = false;
		bool m_isAddingAgent = false;
		std::string m_inputBarText = "";
		float m_cursorBlinkTimer = 0.0f;

		std::string m_sortedAgentsSearchInput;
		std::vector<Agent> m_cachedSortedAgents;

		void RenderSelectionBar(AppContext* ctx, float x, float y, float w, float h);
		void RenderInputBar(AppContext* ctx, float x, float y, float w, float h);
		void RenderAgentList(AppContext* ctx, float x, float y, float w, float h);
		
		const std::vector<Agent>& SortAgents(
			const std::string& searchInput, const std::unordered_map<AgentID, Agent>& agents);
		std::vector<std::string> SplitTokens(const std::string& input);

		template<typename T, typename ConvertFunc>
		bool EvaluateNumberFilter(const std::string& token, 
			T value, ConvertFunc convertFunc, int& outScore);
		bool EvaluateIntFilter(const std::string& token, int value, int& outScore);
		bool EvaluateFloatFilter(const std::string& token, float value, int& outScore);

		void UpdateScroll(float& offset, float& velocity, float maxScroll);
		bool DrawButton(const std::string& text, float x, float y, float w, float h);
		bool IsPointInRect(const Vector2& mPos, float x, float y, float w, float h);
		std::string ToLower(std::string str);
	};

}