#ifndef CONTEXT_H_
#define CONTEXT_H_

#pragma once

#include <string>

#include "PreDec.h"
#include "Configure.h"
#include "XMLParser.h"
#include "RenderFactory.h"
#include "SceneManager.h"
#include "StateManager.h"
//#include "ResourceLoader.h" // find in Deprecated Files folder

namespace MocapGE
{
	class Context
	{
	public:
		//TODO : make constructor to be protected: because of singleton pattern
		Context(void);
		~Context(void);

		static Context& Instance();

		void LoadConfig(std::string const & cfg_file);

		void Configure(ContextConfig const & cfg);
		ContextConfig const & GetConfigure() const;

		RenderFactory& GetRenderFactory();
		SceneManager& GetSceneManager();
		StateManager& GetStateManager();
		//ResourceLoader& GetResourceLoader();

		void SetAppInstance(App &app)
		{
			app_ = &app;
		}

		App& AppInstance()
		{
			return *app_;
		}
	private:
		ContextConfig context_config_;

		App*		app_;

		RenderFactory*	render_factory_;
		SceneManager* scene_manager_;
		StateManager* state_manager_;
		//ResourceLoader* resource_loader_;
	};
}

#endif

