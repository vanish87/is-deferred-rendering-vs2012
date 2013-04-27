#include "MyApp.h"
#include "Context.h"
#include "Timer.h"

#include "StartMenu.h"

#include "D3DSkyDome.h"

using namespace MocapGE;
MyApp::MyApp(void) : App("The CentBebop Design")
{

}


MyApp::~MyApp(void)
{
}

void MyApp::InitObjects()
{
	//set up lights
	point_light_ = new PointLight();
	point_light_->SetPos(float3(50 ,0, 0));	
	//point_light_->AddToScene();

	spot_light_ = new SpotLight();
	spot_light_->SetPos(float3(10, 10, 0));
	spot_light_->SetDir(float3(0,0,0)-spot_light_->GetPos());
	spot_light_->SetInnerAngle(Math::PI / 24);
	spot_light_->SetOuterAngle(Math::PI / 14);
	spot_light_->AddToScene();

	D3DModel *model = new D3DModel();
	model->LoadFile("../Media/spacecraft_new.dae");
	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
	ship_ = new SceneObject(model);
	ship_->AddToScene();

	timer_ = new Timer();
	timer_->Retart();
	
}

void MyApp::ReleaseObjects()
{
	delete point_light_;
	delete spot_light_;
	delete timer_;
}

void MyApp::Update()
{
	//std::cout<<timer_->Time()<<"\r";

	//std::cout<<spot_light_->GetPos().x()<<"\r";
	//spot_light_->SetDir(float3(-0.5,-1,Math::Cos(timer_->Time()/1000.0f) * Math::PI / 6));
}

void MyApp::OnKeyDown( WPARAM key_para )
{

	
}

void MyApp::OnMouseDown( WPARAM mouse_para, int x, int y )
{
}

void MyApp::OnMouseMove( WPARAM mouse_para, int x, int y )
{

}

void MyApp::OnMouseUp( WPARAM mouse_para, int x, int y )
{
}


int main()
{
	 _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	Context::Instance().LoadConfig("E:/13Spring/IS/DeferredRendering/DeferredRendering/Config.xml");
	ContextConfig cfg = Context::Instance().GetConfigure();
	Context::Instance().Configure(cfg);

	MyApp app;
	app.Initialize();
	app.Run();



	_CrtDumpMemoryLeaks();
	return 0;
}

 