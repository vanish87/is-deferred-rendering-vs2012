#ifndef RENDERENGINE_H_
#define RENDERENGINE_H_

#pragma once

#include <string>

#include "PreDec.h"
#include "Context.h"
#include "Configure.h"
#include "DeferredRendering.h"
#include "FrameBuffer.h"
#include "SceneObject.h"
#include "RenderBuffer.h"

namespace MocapGE
{
	class RenderEngine
	{
	public:
		RenderEngine(void);
		~RenderEngine(void);
		virtual void InitRenderWindow(std::string const & name, RenderSetting const & render_setting);
		RenderSetting& GetRenderSetting(){return render_setting_;};
		void InitLights();

		virtual void BindFrameBuffer(FrameBuffer* const & frame_buffer) = 0;
		virtual FrameBuffer* CurrentFrameBuffer(){return cur_frame_buffer_;};
		//virtual void BindSOBuffers(RenderLayoutPtr const & rl) = 0;
		//virtual void Render(ShaderObject const & shader, RenderLayout const & rl) = 0;

		virtual void RenderFrameBegin() = 0;
		virtual void Render(RenderLayout* render_layout, ShaderObject* shader_object, int pass_index) = 0;
		virtual void RenderFrameEnd() = 0;
		virtual void SwapBuffers() = 0;
		virtual void OnResize() = 0;
		//RenderBuffer* & GetLightsBuufer(){return light_buffer_;};
		
		//for Deferred Rendering
		void InitDeferredRendering( RenderSetting render_setting );
		//TODO : Use ShaderState to set State
		virtual void SetDeferredRenderingState() = 0;
		virtual void SetNormalState() = 0;
		virtual void TrunoffCull() = 0;

/*
		FrameBuffer* GetGBuffer();
		std::vector<RenderBuffer*> GetGBufferSRV();
		FrameBuffer* GetLightingBuffer();
		RenderBuffer* GetLightingBufferSRV();
		Mesh* GetFullscreenQuad();*/

		DeferredRendering* GetDeferredRendering(){return deferred_rendering_;};
	protected:
		std::string name_;
		RenderSetting render_setting_;

		FrameBuffer* cur_frame_buffer_;
		//RenderBuffer* light_buffer_;

		DeferredRendering* deferred_rendering_;

	};
}


#endif