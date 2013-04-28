// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Picking and Assembling gameplay</summary>

#pragma once
#include "App.h"
#include "Vector.h"
#include "D3DModel.h"
#include <vector>


class MyApp :
	public MocapGE:: App
{
public:
	MyApp(void);
	~MyApp(void);
	void InitObjects();
	void ReleaseObjects();
	void Update();

	virtual void OnKeyDown( WPARAM key_para );

	virtual void OnMouseDown( WPARAM mouse_para, int x, int y );
	virtual void OnMouseMove( WPARAM mouse_para, int x, int y );
	virtual void OnMouseUp( WPARAM mouse_para, int x, int y );

private:
	MocapGE::PointLight* point_light_;
	MocapGE::SpotLight* spot_light_;

	MocapGE::SceneObject* ship_;

	MocapGE::Timer* timer_;

	bool first_person_;
	MocapGE::float3 cam_pos_;
	MocapGE::float3 cam_look_;

	float pitch_angle_;
	float speed_;


};

