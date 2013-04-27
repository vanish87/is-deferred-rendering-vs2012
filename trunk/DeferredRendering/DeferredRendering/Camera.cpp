#include "Camera.h"

namespace MocapGE
{
	Camera::Camera(void)
	{
		this->SetView(float3(0,0,-1), float3(0,0,0), float3(0,1,0));
		this->SetProjection(Math::PI/4, 1 , 1, 1000);
		//TODO : Init frustum here
		frustum_;
	}

	Camera::Camera(float aspect)
	{
		this->SetView(float3(0,0,-1), float3(0,0,0), float3(0,1,0));
		this->SetProjection(Math::PI/4, aspect , 1, 1000);
		frustum_;
	}

	Camera::~Camera(void)
	{
	}

	void Camera::SetView( float3 eye, float3 at, float3 up )
	{
		eye_ = eye;
		at_ = at;
		up_ = up;

		view_matrix_ = Math::LookAtLH(eye_, at_, up_);
	}

	void Camera::SetProjection( float fovy, float aspect, float zn, float zf )
	{
		fovy_ = fovy;
		aspect_ = aspect;
		zn_ = zn;
		zf_ = zf;

		proj_matrix_ = Math::PerspectiveFovLH(fovy_, aspect_, zn_, zf_);
	}

	float3 Camera::GetLookAt()
	{
		return at_;
	}

	float3 Camera::GetUp()
	{
		return up_;
	}

	void Camera::Pitch( float angle )
	{
		float3 dir =  at_ - eye_;
		float3 left = Math::Cross(dir,up_);
		left = Math::Normalize(left);

		float4x4 mat;
		Math::Identity(mat);
		Math::RotationAxis(mat, left, angle);

		up_ = Math::TransformNormal(up_, mat);
		at_ = Math::Transform(at_, mat);

		view_matrix_ = Math::LookAtLH(eye_, at_, up_);
	}

	void Camera::Yaw( float angle )
	{
		float3 dir =  at_ - eye_;
		float4x4 mat;
		Math::Identity(mat);
		Math::RotationAxis(mat, up_, angle);

		dir = Math::TransformNormal(dir, mat);
		dir = Math::Normalize(dir);
		at_ = eye_ + dir;

		view_matrix_ = Math::LookAtLH(eye_, at_, up_);
	}

	void Camera::Roll( float angle )
	{
		float3 dir =  at_ - eye_;
		float4x4 mat;
		Math::Identity(mat);
		Math::RotationAxis(mat, dir, angle);

		up_ = Math::TransformNormal(up_, mat);

		view_matrix_ = Math::LookAtLH(eye_, at_, up_);
	}


}
