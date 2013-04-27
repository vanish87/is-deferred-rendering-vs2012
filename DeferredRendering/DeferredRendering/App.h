#ifndef APP_H_
#define APP_H_

#pragma once

#include <string>

#include "PreDec.h"
#include <windows.h>

namespace MocapGE
{
	class App
	{
	public:
		App(void);
		~App(void);
		App(std::string app_name);
		void Initialize();
		void Run();

		Window* InitializeWindow( std::string app_name, RenderSetting render_setting );
		Window& GetWindow()
		{
			return *win_;
		}

		Camera* GetCamera();
		void LockCursor(bool lock);
		void SetCursor(int x, int y);

		virtual void OnKeyDown(WPARAM key_para)
		{
		}
		virtual void OnKeyUp(WPARAM key_para)
		{
		}
		virtual void OnMouseDown(WPARAM mouse_para,  int x, int y)
		{
		}
		virtual void OnMouseUp(WPARAM mouse_para,  int x, int y)
		{
		}
		virtual void OnMouseMove(WPARAM mouse_para,  int x, int y)
		{
		}

	private:
		std::string app_name_;
		Window* win_;
		bool cursor_locked_;

		static const int MAXIMUM_FRAME_RATE = 60;
		static const int UPDATE_INTERVAL = 1000 / MAXIMUM_FRAME_RATE;
		static const int MINIMUM_FRAME_RATE = 24;
		static const int MAX_CYCLES_PER_FRAME = MAXIMUM_FRAME_RATE / MINIMUM_FRAME_RATE ;


	protected:
		virtual void InitObjects()
		{
		}
		virtual void Update()
		{
		}
		virtual void ReleaseObjects()
		{
		}



	};
}
#endif

