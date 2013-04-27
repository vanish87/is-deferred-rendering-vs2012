#include "Mesh.h"
#include "Context.h"

namespace MocapGE
{
	Mesh::Mesh(void)
	{
	}


	Mesh::~Mesh(void)
	{
	}

// 	void Mesh::AddToScene()
// 	{
// 		Context::Instance().GetSceneManager().AddSceneObject(new SceneObject(this));
// 	}

	RenderLayout* Mesh::GetRenderLayout()
	{
		return render_layout_;
	}

	void Mesh::SetRenderParameters()
	{
		shader_object_->SetMatrixVariable("g_model_matrix", model_matrix_);
	}

	void Mesh::Render(int pass_index)
	{
		RenderEngine& render_engine = Context::Instance().GetRenderFactory().GetRenderEngine();
		render_engine.Render(render_layout_, shader_object_, pass_index);
	}

	void Mesh::EndRender()
	{
		//throw std::exception("The method or operation is not implemented.");
	}


}
