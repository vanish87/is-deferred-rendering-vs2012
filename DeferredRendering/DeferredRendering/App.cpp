#include "App.h"
#include "Context.h"
#include "Window.h"

namespace MocapGE
{
	App::App(void)
	{
	}

	App::App( std::string app_name )
		:app_name_(app_name), cursor_locked_(false)
	{
		Context::Instance().SetAppInstance(*this);

		ContextConfig config = Context::Instance().GetConfigure();
		win_ = this->InitializeWindow(app_name_, config.render_setting);

		Context::Instance().Configure(config);
	}


	App::~App(void)
	{
	}

	void App::Initialize()
	{
		ContextConfig config = Context::Instance().GetConfigure();
		Context::Instance().GetRenderFactory().GetRenderEngine().InitRenderWindow(app_name_ , config.render_setting);

		//init deferred rendering here
		if(config.render_setting.deferred_rendering)
			Context::Instance().GetRenderFactory().GetRenderEngine().InitDeferredRendering(config.render_setting);

		this->InitObjects();
		Context::Instance().GetRenderFactory().GetRenderEngine().InitLights();

	}

	void App::Run()
	{
		MSG msg = {0};
		DWORD next_game_tick;
		int loops;
		next_game_tick = GetTickCount();

		while( WM_QUIT != msg.message )
		{
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				loops = 0;
				//while( GetTickCount() > next_game_tick && loops < MAX_CYCLES_PER_FRAME ) 
				{
					this->Update();//do user's update

					Context::Instance().GetStateManager().Update();
					Context::Instance().GetSceneManager().Update();


					next_game_tick += UPDATE_INTERVAL;
					loops++;
				}
				Context::Instance().GetSceneManager().Flush();

				if(cursor_locked_)
				{
					ContextConfig config = Context::Instance().GetConfigure();
					POINT pt; 
					pt.x = config.render_setting.width / 2;
					pt.y = config.render_setting.height/ 2;
					ClientToScreen(win_->GetWnd(), &pt); 
					::SetCursorPos(pt.x, pt.y);
				}

			}
		}
	}

	Window* App::InitializeWindow( std::string app_name, RenderSetting render_setting )
	{
		return new Window(app_name,render_setting);
	}

	Camera* App::GetCamera()
	{
		return Context::Instance().GetRenderFactory().GetRenderEngine().CurrentFrameBuffer()->GetFrameCamera();
	}

	void App::LockCursor( bool lock )
	{
		cursor_locked_ = lock;
	}

	void App::SetCursor( int x, int y )
	{
		::SetCursorPos(x, y);
	}

}
