#include "DeferredRendering.h"
#include "D3DModel.h"


namespace MocapGE
{
	static const float4x4 TEXCOOD_TRANS = 
		float4x4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	DeferredRendering::DeferredRendering( RenderSetting& render_setting )
	{
		gbuffer_ = Context::Instance().GetRenderFactory().MakeFrameBuffer(render_setting);
		lighting_buffer_ = Context::Instance().GetRenderFactory().MakeFrameBuffer(render_setting);
		//make a full screen qua for lighting pass
		VertexType* vb = new VertexType[6];
		uint32_t* ib = new uint32_t[6];
		//clock wise
		vb[0].position = float3(-1, +1, 1);
		vb[1].position = float3(+1, +1, 1);
		vb[2].position = float3(-1, -1, 1);
		vb[3].position = float3(+1, -1, 1);
		vb[4].position = float3(-1, -1, 1);
		vb[5].position = float3(+1, +1, 1);

		ib[0] = 0;
		ib[1] = 1;
		ib[2] = 2;
		ib[3] = 3;
		ib[4] = 4;
		ib[5] = 5;

		//call MakeRenderLayout
		RenderLayout* render_layout = Context::Instance().GetRenderFactory().MakeRenderLayout();
		//call MakeRenderBuffer(Vertex)
		InitData init_data;
		init_data.data = vb;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* vertex_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ, BU_VERTEX, 6 ,sizeof(VertexType));
		//delete[] vb;
		//call MakeRenderBuffer(Index)
		init_data.data = ib;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* index_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ, BU_INDEX, 6, sizeof(uint32_t));
		//delete[] ib;

		//add VertexBuffer to renderlayout;
		render_layout->AddBuffer(vertex_buffer, sizeof(VertexType));
		//add IndexBuffer to renderlayout;
		render_layout->AddBuffer(index_buffer, 6);
		//set Primitivetype of renderlayout;
		render_layout->SetPrimitive(PT_TRIANGLELIST);
		//set Input layout Semi
		std::vector<VertexUsage> inputlayout;
		inputlayout.push_back(VU_POSITION);
		render_layout->SetInputLayout(inputlayout);
		float4x4 model_matrix;
		Math::Identity(model_matrix);
		fullscreen_mesh_ = new MocapGE::Mesh("full screen quad", render_layout, model_matrix, vb, ib);

		//According to config file
		for(size_t i=0; i< render_setting.gbuffer_size; i++)
		{
			//create render target
			Texture* texture_2d = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
				1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);
			gbuffer_tex_.push_back(texture_2d);
			//Add to gbuffer
			RenderView* render_view = Context::Instance().GetRenderFactory().MakeRenderView(texture_2d, 1, 0);
			RenderBuffer* shader_resource = Context::Instance().GetRenderFactory().MakeRenderBuffer(texture_2d, AT_GPU_READ, BU_SHADER_RES);
			AddGBuffer(render_view);
			AddGBuffer(shader_resource);
		}

		//init shadow blur buffer
		shadow_map_buffer_ = Context::Instance().GetRenderFactory().MakeFrameBuffer(render_setting);
		shadow_depth_ = shadow_map_buffer_->GetDepthTexture();
		shadow_linear_depth_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);


		linearize_shadow_map_so_ = new D3DShaderobject();
		linearize_shadow_map_so_->LoadFxoFile("FxFiles/LinearizeDepthPostProcess.cso");
		linearize_shadow_map_so_->SetTechnique("PPTech");

		linearize_shadow_map_pp_ = new PostProcess();
		linearize_shadow_map_pp_->SetPPShader(linearize_shadow_map_so_);
		linearize_shadow_map_pp_->SetInput(shadow_depth_, 0);
		linearize_shadow_map_pp_->SetOutput(shadow_linear_depth_, 0);

		shadow_blur_X_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);
		shadow_blur_Y_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);

		shadow_map_blur_so_ = new D3DShaderobject();
		shadow_map_blur_so_->LoadFxoFile("FxFiles/GaussianBlurXFilterPostProcess.cso");
		shadow_map_blur_so_->SetTechnique("PPTech");

		shadow_map_xblur_pp_ = new PostProcess();
		shadow_map_xblur_pp_->SetPPShader(shadow_map_blur_so_);
		shadow_map_xblur_pp_->SetInput(shadow_linear_depth_, 0);
		shadow_map_xblur_pp_->SetOutput(shadow_blur_X_, 0);

		shadow_map_blur_so_->LoadFxoFile("FxFiles/GaussianBlurYFilterPostProcess.cso");
		shadow_map_blur_so_->SetTechnique("PPTech");

		shadow_map_yblur_pp_ = new PostProcess();
		shadow_map_yblur_pp_->SetPPShader(shadow_map_blur_so_);
		shadow_map_yblur_pp_->SetInput(shadow_blur_X_, 0);
		shadow_map_yblur_pp_->SetOutput(shadow_blur_Y_, 0);

		shadow_blur_srv_ = Context::Instance().GetRenderFactory().MakeRenderBuffer(shadow_blur_Y_, AT_GPU_READ, BU_SHADER_RES);


		shadow_blur_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, 512, 512,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);
		//shadow_blur_buffer_->AddRenderView(Context::Instance().GetRenderFactory().MakeRenderView(shadow_blur_, 1, 0));

		//inti shadowing buffer
		shadowing_buffer_ = Context::Instance().GetRenderFactory().MakeFrameBuffer(render_setting);
		Texture* shadowing_texture_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);
		RenderView* render_view = Context::Instance().GetRenderFactory().MakeRenderView(shadowing_texture_, 1, 0);
		shadowing_buffer_->AddRenderView(render_view);
		shadowing_srv_ = Context::Instance().GetRenderFactory().MakeRenderBuffer(shadowing_texture_, AT_GPU_READ, BU_SHADER_RES);

		//init SSDO
		ssdo_so_ = new D3DShaderobject();;
		ssdo_so_->LoadFxoFile("FxFiles/SSDO.cso");
		ssdo_so_->SetTechnique("PPTech");

		D3DModel *random_tex_dummy = new D3DModel();
		noise_tex_ = random_tex_dummy->LoadTexture("../Media/noise.png");
		delete random_tex_dummy;

		occlusion_tex_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);
		occlusion_blur_tex_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);
		occlusion_blur_X_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);
		occlusion_blur_srv_ = Context::Instance().GetRenderFactory().MakeRenderBuffer(occlusion_blur_tex_, AT_GPU_READ, BU_SHADER_RES);

		ssdo_pp_ = new PostProcess();
		ssdo_pp_->SetPPShader(ssdo_so_);
		//position
		ssdo_pp_->SetInput(gbuffer_tex_[2], 0);
		//normal
		ssdo_pp_->SetInput(gbuffer_tex_[0], 1);
		ssdo_pp_->SetInput(noise_tex_, 2 );
		ssdo_pp_->SetOutput(occlusion_tex_, 0);

		occlusion_blur_so_ = new D3DShaderobject();
		occlusion_blur_so_->LoadFxoFile("FxFiles/GaussianBlurXFilterPostProcess.cso");
		occlusion_blur_so_->SetTechnique("PPTech");

		occlusion_xblur_pp_ = new PostProcess();
		occlusion_xblur_pp_->SetPPShader(occlusion_blur_so_);
		occlusion_xblur_pp_->SetInput(occlusion_tex_, 0);
		occlusion_xblur_pp_->SetOutput(occlusion_blur_X_, 0);

		occlusion_blur_so_->LoadFxoFile("FxFiles/GaussianBlurYFilterPostProcess.cso");
		occlusion_blur_so_->SetTechnique("PPTech");

		occlusion_yblur_pp_ = new PostProcess();
		occlusion_yblur_pp_->SetPPShader(occlusion_blur_so_);
		occlusion_yblur_pp_->SetInput(occlusion_blur_X_, 0);
		occlusion_yblur_pp_->SetOutput(occlusion_blur_tex_, 0);

		
		
		//init lighting buffer
		Texture* texture_2d = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);
		render_view = Context::Instance().GetRenderFactory().MakeRenderView(texture_2d, 1, 0);
		RenderBuffer* shader_resource = Context::Instance().GetRenderFactory().MakeRenderBuffer(texture_2d, AT_GPU_READ, BU_SHADER_RES);
		AddLightingBuffer(render_view);
		AddLightingBuffer(shader_resource);

		linearize_depth_so_ = new D3DShaderobject();
		linearize_depth_so_->LoadFxoFile("FxFiles/LinearizeDepthPostProcess.cso");
		linearize_depth_so_->SetTechnique("PPTech");

		linearize_depth_pp_ = new PostProcess();
		linearize_depth_pp_->SetPPShader(linearize_depth_so_);

		depth_tex_ = gbuffer_->GetDepthTexture();
		linear_depth_tex_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, render_setting.width, render_setting.height,
			1, 1, R32G32B32A32_F, render_setting.msaa4x ==1 ? 4 : 1, 0, AT_GPU_WRITE, TU_SR_RT);

		linearize_depth_pp_->SetInput(depth_tex_, 0);
		linearize_depth_pp_->SetOutput(linear_depth_tex_, 0);

		depth_srv_ = Context::Instance().GetRenderFactory().MakeRenderBuffer(linear_depth_tex_, AT_GPU_READ, BU_SHADER_RES); 

		//back camera
		back_buffer_ = Context::Instance().GetRenderFactory().GetRenderEngine().CurrentFrameBuffer();
		back_frame_camera_= back_buffer_->GetFrameCamera();

	}


	DeferredRendering::~DeferredRendering(void)
	{
	}

	void DeferredRendering::AddGBuffer( RenderView* render_target_view )
	{
		gbuffer_->AddRenderView(render_target_view);
	}

	void DeferredRendering::AddGBuffer( RenderBuffer* shader_resource_view )
	{
		gbuffer_srv_.push_back(shader_resource_view);
	}

	Mesh* DeferredRendering::GetFullscreenQuad()
	{
		return fullscreen_mesh_;
	}

	void DeferredRendering::AddLightingBuffer( RenderView* render_view )
	{
		lighting_buffer_->AddRenderView(render_view);
	}

	void DeferredRendering::AddLightingBuffer( RenderBuffer* shader_resource)
	{
		lighting_srv_ = shader_resource;
	}

	void DeferredRendering::Update()
	{
		//do DR here
/*
			//Deferred Shading
			//---------------------------------------------------------------------------
			render_engine->SetNormalState();
			//bind gbuffer
			FrameBuffer* back_buffer = render_engine->CurrentFrameBuffer();
			Camera* back_frame_camera = back_buffer->GetFrameCamera();
			render_engine->GetGBuffer()->SetFrameCamera(back_frame_camera);
			render_engine->BindFrameBuffer(render_engine->GetGBuffer());

			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameBegin();
			std::vector<RenderElement*>::iterator re;
			for(re = render_list_.begin() ; re < render_list_.end(); re++)
			{
				(*re)->SetRenderParameters();
				//Render to Gbuffer
				//(*re)->GetShaderObject()->Apply(0);
				(*re)->Render(0);
				(*re)->EndRender();
			}

			
			//bind screen buffer
			render_engine->BindFrameBuffer(back_buffer);
			render_engine->SetDeferredRenderingState();
			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameBegin();
			//set lights parameters
			std::vector<Light*> lights = Context::Instance().GetSceneManager().GetLights();
			RenderBuffer* lights_buffer = Context::Instance().GetRenderFactory().GetRenderEngine().GetLightsBuufer();
			LightStruct* l = static_cast<LightStruct*>(lights_buffer->Map(AT_CPU_WRITE));
			for (size_t i =0; i< lights.size(); i++)
			{
				l[i].color = lights[i]->GetColor();
				l[i].position = static_cast<PointLight*>(lights[i])->GetPos();
			}
			lights_buffer->UnMap();

			//set gbuffer as input textures
			ShaderObject* shader_object = render_list_[0]->GetShaderObject();
			std::vector<RenderBuffer*> gbuffer_srv = render_engine->GetGBufferSRV();	
			FrameBuffer* gbuffer = render_engine->GetGBuffer();
			RenderBuffer* depth_srv = Context::Instance().GetRenderFactory().MakeRenderBuffer(gbuffer->GetDepthTexture(), AT_GPU_READ, BU_SHADER_RES); 
			shader_object->SetReource("position_tex", gbuffer_srv[3], 1);
			shader_object->SetReource("diffuse_tex", gbuffer_srv[1], 1);
			shader_object->SetReource("specular_tex", gbuffer_srv[2], 1);
			shader_object->SetReource("normal_tex", gbuffer_srv[0], 1);
			//do lighting
			Mesh* quad = render_engine->GetFullscreenQuad();
			//Set Shader file for quad
			quad->SetShaderObject(shader_object);
			quad->SetRenderParameters();
			//quad->GetShaderObject()->Apply(1);
			quad->Render(1);
			quad->EndRender();

			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameEnd();
			Context::Instance().GetRenderFactory().GetRenderEngine().SwapBuffers();
			//End of Deferred Shading
			//---------------------------------------------------------------------------
*/


		RenderEngine* render_engine = &Context::Instance().GetRenderFactory().GetRenderEngine();
		std::vector<RenderElement*> render_list = Context::Instance().GetSceneManager().GetRenderList();
		ShaderObject* shader_object = render_list[0]->GetShaderObject();

		//Deferred Lighting
			//pass 0
			render_engine->SetNormalState();
			//bind gbuffer
			gbuffer_->SetFrameCamera(back_frame_camera_);
			render_engine->BindFrameBuffer(gbuffer_);

			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameBegin();
			std::vector<RenderElement*>::iterator re;
			for(re = render_list.begin() ; re < render_list.end(); re++)
			{
				(*re)->SetRenderParameters();
				//Render to Gbuffer
				//(*re)->GetShaderObject()->Apply(0);
				(*re)->Render(0);
				(*re)->EndRender();
			}

			linearize_depth_pp_->Apply();

			//pass 1
			//bind lighting buffer
			lighting_buffer_->SetFrameCamera(back_frame_camera_);
			render_engine->BindFrameBuffer(lighting_buffer_);
			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameBegin();
			//set lights parameters
			std::vector<Light*> lights = Context::Instance().GetSceneManager().GetLights();
			LightStruct* light_buffer = new LightStruct[lights.size()];
			float4x4 view_mat = back_frame_camera_->GetViewMatirx();
			float4x4 invtrans_view_mat = Math::InverTranspose(view_mat);
			float4x4 shadow_trans_mat;
			float4x4 light_view_proj;
			LightType type;

			for (size_t i =0; i< lights.size(); i++)
			{

				light_buffer[i].color = lights[i]->GetColor();
				type = lights[i]->GetType();
				switch (type)
				{
				case LT_POINT:
					{
						light_buffer[i].type = LT_POINT;
						light_buffer[i].position = Math::Transform(static_cast<PointLight*>(lights[i])->GetPos(), view_mat);
						light_buffer[i].direction = float3(0, 0, 0);
						light_buffer[i].inner_outer = float2(0, 0);
						break;
					}
				case LT_SPOT:
					{
						light_buffer[i].type = LT_SPOT;
						light_buffer[i].position = Math::Transform(static_cast<SpotLight*>(lights[i])->GetPos(), view_mat);
						light_buffer[i].direction = Math::TransformNormal(static_cast<SpotLight*>(lights[i])->GetDir(), invtrans_view_mat);
						float outer = static_cast<SpotLight*>(lights[i])->GetOuterAngle();
						float inner = static_cast<SpotLight*>(lights[i])->GetInnerAngle();
						light_buffer[i].inner_outer = float2(Math::Cos(inner), Math::Cos(outer));
						break;
					}
				default:
					break;
				}
				//LightStruct* l = &light_buffer[i];
				shader_object->SetRawData("light", &light_buffer[i], sizeof(LightStruct));
				Camera* sm_camera = lights[i]->GetCamera();
				float4x4 view_proj_mat = sm_camera->GetViewMatirx() * sm_camera->GetProjMatrix();
				//TODO : handle point light
				//Shadowing spot
				if(type == LT_SPOT)
				{

					//std::cout<<static_cast<SpotLight*>(lights[i])->GetDir().x()<<" "<<static_cast<SpotLight*>(lights[i])->GetDir().y()<<" "<<static_cast<SpotLight*>(lights[i])->GetDir().z()<<"\r";
					
					shadow_map_buffer_->SetFrameCamera(sm_camera);
					render_engine->BindFrameBuffer(shadow_map_buffer_);
					render_engine->SetNormalState();

					Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameBegin();
					std::vector<RenderElement*>::iterator re;
					for(re = render_list.begin() ; re < render_list.end(); re++)
					{
						//set shadowing 
						(*re)->GetShaderObject()->SetTechnique("Shadowing");
						(*re)->SetRenderParameters();
						(*re)->Render(0);
						(*re)->EndRender();
						//reset
						(*re)->GetShaderObject()->SetTechnique("GbufferTech");
					}
					Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameEnd();
					linearize_shadow_map_pp_->Apply();
					shadow_map_xblur_pp_->Apply();
					shadow_map_yblur_pp_->Apply();
				}

				
				

				ssdo_pp_->SetCamera(back_frame_camera_);
				ssdo_pp_->Apply();
				occlusion_xblur_pp_->Apply();
 				occlusion_yblur_pp_->Apply();


				//set gbuffer as input textures		
				shader_object->SetReource("depth_tex", depth_srv_, 1);
				shader_object->SetReource("normal_tex", gbuffer_srv_[0], 1);
				shader_object->SetReource("shadow_map_tex", shadow_blur_srv_, 1);
				shader_object->SetReource("blur_occlusion_tex", occlusion_blur_srv_, 1);

				//shader_object->SetReource("lighting_tex", lighting_srv_, 1);
				//do lighting
				//Set Shader file for quad
				//std::cout<<lights[i]->GetCamera()->GetPos().x()<<" "<<lights[i]->GetCamera()->GetPos().y()<<" "<<lights[i]->GetCamera()->GetPos().z()<<"\r";
/*
				std::cout<<view_proj_mat[0][0]<<" "<<view_proj_mat[0][1]<<" "<<view_proj_mat[0][2]<<"\n";
				std::cout<<view_proj_mat[1][0]<<" "<<view_proj_mat[1][1]<<" "<<view_proj_mat[1][2]<<"\n";
				std::cout<<view_proj_mat[2][0]<<" "<<view_proj_mat[2][1]<<" "<<view_proj_mat[2][2]<<"\n\n";*/
				lighting_buffer_->SetFrameCamera(back_frame_camera_);
				render_engine->BindFrameBuffer(lighting_buffer_);
				//render_engine->RenderFrameBegin();
				render_engine->SetDeferredRenderingState();
				shader_object->SetMatrixVariable("g_shadow_transform", shadow_trans_mat);
				shader_object->SetMatrixVariable("g_light_view_proj", view_proj_mat);

				fullscreen_mesh_->SetShaderObject(shader_object);
				fullscreen_mesh_->SetRenderParameters();
				fullscreen_mesh_->Render(1);
				fullscreen_mesh_->EndRender();
			}

			delete[] light_buffer;

			//pass 2
			render_engine->BindFrameBuffer(back_buffer_);
			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameBegin();
			shader_object->SetReource("lighting_tex", lighting_srv_, 1);
			shader_object->SetReource("diffuse_tex", gbuffer_srv_[1], 1);
			//Set Shader file for quad
			fullscreen_mesh_->SetShaderObject(shader_object);
			fullscreen_mesh_->SetRenderParameters();
			//quad->GetShaderObject()->Apply(1);
			fullscreen_mesh_->Render(2);
			fullscreen_mesh_->EndRender();




			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameEnd();
			Context::Instance().GetRenderFactory().GetRenderEngine().SwapBuffers();
	}


}
