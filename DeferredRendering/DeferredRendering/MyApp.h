// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Picking and Assembling gameplay</summary>

#pragma once
#include "App.h"
#include "Vector.h"
#include "D3DModel.h"
#include <vector>


class StartMenu;
class ResembleState;
class GamingState;

class Picking;
class Cannon;

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

	typedef std::vector<Cannon*> PartList;

	MocapGE::PointLight* point_light_;
	MocapGE::SpotLight* spot_light_;

	MocapGE::Timer* timer_;


};

