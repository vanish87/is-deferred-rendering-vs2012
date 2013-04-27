#include "PostProcessApp.h"

#include "Context.h"
#include "PostProcess.h"
#include "D3DShaderobject.h"

using namespace MocapGE;
PostProcessApp::PostProcessApp(void)
	 : App("Post Process Example")
{
}


PostProcessApp::~PostProcessApp(void)
{
}
void PostProcessApp::InitObjects()
{
	float4x4 sacle_mat, trans_mat;

	pluto_ = new D3DModel();
	pluto_->LoadFile("..\\Media\\pluto.dae");
	pluto_->LoadShaderFile("..\\FxFiles\\DeferredLighting.fxo");
	Math::Scale(sacle_mat, 1);
	Math::Translate(trans_mat, 0, 0, 0);
	pluto_->SetModelMatrix(sacle_mat* trans_mat);
	scene_pluto_ = new SceneObject(pluto_);
	scene_pluto_->AddToScene();

	Camera* camera = Context::Instance().AppInstance().GetCamera();

	//set init pos of camera, make it from back and upper position of ship
	cam_pos = float3(0,0,-5);
	float3 cam_at(0,0,0);
	camera->SetView(cam_pos, cam_at, float3(0,1,0));

	point_light = new PointLight();
	point_light->SetPos(float3(50,10,10));
	point_light->AddToScene();
}

void PostProcessApp::ReleaseObjects()
{
}

void PostProcessApp::Update()
{
	//pp_->Apply();
}

void PostProcessApp::OnKeyDown( WPARAM key_para )
{
	switch (key_para) 
	{ 
	case 'S':
		{
			Camera* camera = Context::Instance().AppInstance().GetCamera();

			//set init pos of camera, make it from back and upper position of ship
			cam_pos = float3(cam_pos.x(), cam_pos.y()-0.01,cam_pos.z());
			float3 cam_at(0,0,0);
			camera->SetView(cam_pos, cam_at, float3(0,1,0));
		}
	}
}

void PostProcessApp::OnMouseDown( WPARAM mouse_para, int x, int y )
{
}

void PostProcessApp::OnMouseMove( WPARAM mouse_para, int x, int y )
{

}

void PostProcessApp::OnMouseUp( WPARAM mouse_para, int x, int y )
{
}

/*
int main()
{
	Context::Instance().LoadConfig("E:\\12Fall\\Independent Study\\IS-Deferred Rendering\\IS-Deferred Rendering\\Config.xml");
	ContextConfig cfg = Context::Instance().GetConfigure();
	Context::Instance().Configure(cfg);

	PostProcessApp app;
	app.Initialize();
	app.Run();

	return 0;
}*/
