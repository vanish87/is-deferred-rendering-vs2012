#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#pragma once
#include "PreDec.h"
#include "Matrix.h"
#include "RenderElement.h"

namespace MocapGE
{
	class SceneObject
	{
	public:
		SceneObject(void);
		SceneObject(RenderElement* render_element)
			:render_element_(render_element),model_matrix_(render_element->model_matrix_), visiable_(true)
		{
			
		};
		~SceneObject(void);

		//do update aabb and set model_matrix equals to its render_element_'s
		virtual void Update();
		virtual RenderElement* & GetRenderElement();

		virtual void AddToScene();

		bool Visiable();
		void SetVisiable(bool visiable);

	protected:

		RenderElement* render_element_;
		float4x4 model_matrix_;

		bool visiable_;
	};

}

#endif

