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
	model->LoadFile("../Media/sponza.dae");
	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
	ship_ = new SceneObject(model);
	ship_->AddToScene();

	timer_ = new Timer();
	timer_->Retart();
	
	first_person_ = false;
	pitch_angle_ = 0;
	speed_ = 0.5;
	Camera* camera = Context::Instance().AppInstance().GetCamera();
	cam_pos_ = float3(-5,0,0);
	cam_look_ = float3(1,0,0);
	camera->SetView(cam_pos_, cam_look_, float3(0,1,0));
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
	if(first_person_)
	{
		Camera* camera = Context::Instance().AppInstance().GetCamera();
		camera->SetView(cam_pos_, cam_look_, float3(0,1,0));
	}
	//std::cout<<spot_light_->GetPos().x()<<"\r";
	//spot_light_->SetDir(float3(-0.5,-1,Math::Cos(timer_->Time()/1000.0f) * Math::PI / 6));
}

void MyApp::OnKeyDown( WPARAM key_para )
{
	switch (key_para)
	{
	case 'F':
		first_person_ = true;
		break;
	default:
		break;
	}
	if(first_person_)
	{
		Camera* camera = Context::Instance().AppInstance().GetCamera();
		float3 up_ = camera->GetUp();
		float4x4 mat;

		float3 dir =  cam_look_ - cam_pos_;
		float3 left = Math::Cross(dir,up_);
		switch (key_para)
		{
		case 'E':
			left = Math::Normalize(left);

			pitch_angle_ = -Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'C':
			left = Math::Normalize(left);

			pitch_angle_ = Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'X':
			left = float3(0,1,0);

			pitch_angle_ = Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'Z':
			left = float3(0,1,0);

			pitch_angle_ = -Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'W':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(dir);
			dir = dir*speed_;
			cam_look_ = cam_look_ + dir;
			cam_pos_ = cam_pos_ + dir;
			break;
		case 'S':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(dir);
			dir = dir*speed_;
			cam_look_ = cam_look_ - dir;
			cam_pos_ = cam_pos_ - dir;
			break;

		case 'A':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(left);
			dir = dir*speed_;
			cam_look_ = dir + cam_look_;
			cam_pos_ = cam_pos_ + dir;
			break;
		case 'D':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(left);
			dir = dir*speed_;
			cam_look_ = cam_look_ - dir;
			cam_pos_ = cam_pos_ - dir;
			break;
		case 'P':
			speed_ +=0.1;
			break;		
		case 'O':
			speed_ -=0.1;
			break;

		default:
			break;
		}
	}
	
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

 