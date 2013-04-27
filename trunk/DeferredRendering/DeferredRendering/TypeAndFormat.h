#ifndef TYPEANDFORMAT_H_
#define TYPEANDFORMAT_H_

#pragma once

#include "PreDec.h"
#include "Vector.h"

namespace MocapGE
{
	enum StateOP
	{
		SOP_PUSH,
		SOP_POP,
	};

	enum Format
	{
		A8_U,
		R8_U,
		R8G8B8A8_U,
		R32G32B32A32_U,
		R32G32B32A32_F,
		R24G8_TYPELESS
	};

	enum TextureType
	{
		TEXTURE1D,
		TEXTURE2D,
		TEXTURECUBE
	};
	enum TextureUsage
	{
		TU_SHADER_RES,
		TU_RENDER_TARGET,
		TU_SHADER_CONST,
		TU_STRUCTURED_BUFFER,
		TU_SR_RT,
		TU_DEPTH_SR

	};

	enum BufferUsage
	{
		BU_VERTEX,
		BU_INDEX,
		BU_SHADER_RES,
		BU_SHADER_CONST,
		BU_STRUCTURED_BUFFER,
		BU_SR_SB //shader resource | structured buffer

	};

	enum AccessType
	{
		AT_CPU_READ,
		AT_CPU_WRITE,
		AT_GPU_READ,
		AT_GPU_WRITE
	};
	enum PrimitiveType
	{
		PT_POINTLIST,
		PT_TRIANGLELIST,
		PT_LINELIST
	};
	enum VertexUsage
	{
		VU_POSITION,
		VU_NORMAL,
		VU_DIFFUSE,
		VU_SPECULAR,
		VU_TEXCOORD,
		VU_TANGENT,
		VU_BINORMAL,
		VU_BLENDINDEX,
		VU_BLENDWEIGHT,
		VU_COLOR
	};
	enum VertexBufferUsage
	{
		VBU_ANIMATION,
		VBU_INDEX,
		VBU_VERTEX
	};

	struct InitData
	{
		void const * data;
		uint32_t row_pitch;
		uint32_t slice_pitch;
	};

	struct Material
	{
		float4 ambient;
		float4 diffuse;
		float4 specular;
		float shininess;
	};

	//make sure the same order in fx file
	//because d3d effect packing data in vec4
	struct LightStruct
	{
		float3 position;	
		int type;
		float3 direction;
		int dummy;
		float4 color;
		float2 inner_outer;
	};

	//formal struct for loading and vertex buffer 
	//you can map vertexbuffer(RenderBuffer) and return a variable of this type to accessing vertex data of GPU
	struct VertexType
	{
		float3 position;
		//float4 color;
		float3 normal;
		float2 uv;
		float3 tangent;
		float3 bitangent;
	};

	enum LightType
	{
		LT_POINT,
		LT_SPOT,
		LT_DERECTIONAL
	};

	enum RenderStateType
	{
		RST_RASTERIZER,
		RST_DEPTH,
		RST_BLEND
	};

}

#endif