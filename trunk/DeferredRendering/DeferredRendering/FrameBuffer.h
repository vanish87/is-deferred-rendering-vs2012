#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#pragma once

#include <vector>

#include "PreDec.h"
#include "RenderView.h"
#include "Viewport.h"

namespace MocapGE
{
	class FrameBuffer
	{
	public:
		FrameBuffer(void);
		FrameBuffer(uint32_t left, uint32_t top, uint32_t width, uint32_t height);
		~FrameBuffer(void);

		Camera* GetFrameCamera();
		void SetFrameCamera(Camera* camera);
		Viewport* GetViewport();
		void SetViewPort(Viewport* viewport);
		virtual void OnBind() = 0;
		virtual void AddRenderView(RenderView* render_view);
		virtual RenderView* GetRenderView(size_t index){return render_views_[index];};
		Texture* GetDepthTexture(){return depth_texture_;};


	protected:
		uint32_t	left_;
		uint32_t	top_;
		uint32_t	width_;
		uint32_t	height_;

		Viewport* viewport_;

		std::vector<RenderView*> render_views_;
		Texture* depth_texture_;
	};
}

#endif

