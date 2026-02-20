#pragma once
#include <functional>
#include <SDLCoreLib/SDLCoreUI.h>

#include "ResourcesManagement/ResourceLoader.h"
#include "LayerSystem/Layer.h"

namespace Layers {

	class StartLoadLayer : public Layer {
	public:
		StartLoadLayer() = default;
		~StartLoadLayer() override = default;

		void OnStart(AppContext* ctx) override;
		void OnUpdate(AppContext* ctx) override;
		void OnRender(AppContext* ctx) override;
		void OnUIRender(AppContext* ctx) override;
		void OnQuit(AppContext* ctx) override;
		LayerID GetLayerID() const override;

	private:
		struct LoadingSection {
			std::string name;
			int progress = 0;
			std::unique_ptr<ResourceLoader> loader;
			std::function<void(AppContext*, ResourceLoader&)> onFinished;

			LoadingSection(std::string&& _name,
				std::vector<ResourceRequest>&& _request,
				std::function<void(AppContext*, ResourceLoader&)> _onFinished = nullptr)
				: name(std::move(_name)),
				loader(std::make_unique<ResourceLoader>(std::move(_request))),
				onFinished(std::move(_onFinished)) {
			}
		};

		std::vector<LoadingSection> m_loadingSections;

		Vector2 m_RefDisplaySize{ 0.0f };
		Vector2 m_displaySize{ 0.0f };
		Vector2 m_windowSize{ 0.0f };

		float m_loadTimeMin = 0.2f;	// minimum hold time at 99%
		float m_loadTimeMax = 0.6f;	// maximum hold time at 99%
		float m_startDelay = 0.35f;	// delay before switching to main menu

		float m_loadTime = 0.0f;
		float m_currentLoadTime = 0.0f;
		float m_currentStartDelay = 0.0f;

		bool m_isHoldingSection = false;

		size_t m_currentSectionIndex = 0;
		bool m_allSectionsFinished = false;
		int m_progress = 0;
		std::string m_currentSectionName;

		void AddLoadingSectionAssets();
		void AddLoadingSectionData();
		void AddLoadingSection(std::string&& name, 
			std::vector<ResourceRequest>&& requests,
			std::function<void(AppContext*, ResourceLoader&)> onFinished);

		void RegisterData(AppContext* ctx, ResourceLoader& loader);
		void RegisterAgent(AppContext* ctx, const OTN::OTNObject& agentOTN);

		void RegistSkins(AppContext* ctx);
	};

}