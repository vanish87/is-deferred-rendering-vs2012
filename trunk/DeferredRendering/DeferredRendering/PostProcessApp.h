#pragma once

#include "App.h"

#include "D3DModel.h"

class PostProcessApp:
	public MocapGE:: App
{
public:
	PostProcessApp(void);
	~PostProcessApp(void);

	void InitObjects();
	void ReleaseObjects();
	void Update();

	virtual void OnKeyDown( WPARAM key_para );

	virtual void OnMouseDown( WPARAM mouse_para, int x, int y );
	virtual void OnMouseMove( WPARAM mouse_para, int x, int y );
	virtual void OnMouseUp( WPARAM mouse_para, int x, int y );

private:
	MocapGE::PostProcess* pp_;

	MocapGE::PointLight* point_light;
	MocapGE::D3DModel* pluto_;
	MocapGE::SceneObject* scene_pluto_;

	MocapGE::float3 cam_pos;

};

