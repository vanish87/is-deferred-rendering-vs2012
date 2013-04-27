#ifndef LIGHT_H_
#define LIGHT_H_

#pragma once
#include "PreDec.h"
#include "Camera.h"
#include "TypeAndFormat.h"

namespace MocapGE
{
	class Light
	{
	public:
		~Light(void);

		explicit Light(LightType light_type)
			:light_type_(light_type)
		{
			light_color_ = float4(1, 1, 1, 1);
			virtual_camera_ = new Camera(1);
		}

		void SetType(LightType type);
		LightType GetType(){return light_type_;};
		void SetColor(float4 color);
		float4 GetColor(){return light_color_;};

		Camera* GetCamera(){return virtual_camera_;};
		void SetCamera(Camera* camera);

		void AddToScene();

	private:
		virtual void UpdateCamera() = 0;

	protected:
		float4 light_color_;
		LightType light_type_;

		Camera* virtual_camera_;
	};

	class PointLight : public Light
	{
	public:
		PointLight(void);
		~PointLight(void);

		float3 GetPos(){return pos_;};
		void SetPos(float3 pos);

		virtual void UpdateCamera();

	private:
		float3 pos_;
	};

	class SpotLight : public Light
	{
	public:
		SpotLight(void);
		~SpotLight(void);

		float3 GetPos(){return pos_;};
		void SetPos(float3 pos);

		float3 GetDir(){return dir_;};
		void SetDir(float3 dir);

		float GetOuterAngle(){return outer_angle_;};
		void SetOuterAngle(float outer_angle);

		float GetInnerAngle(){return inner_angle_;};
		void SetInnerAngle(float inner_angle);

		virtual void UpdateCamera();

	private:
		float3 pos_;
		float3 dir_;
		
		float outer_angle_;
		float inner_angle_;


	};
}

#endif
