#ifndef SHADEROBJECT_H_
#define SHADEROBJECT_H_

#pragma once
#include "PreDec.h"

namespace MocapGE
{
	class ShaderObject
	{
	public:
		ShaderObject(void);
		~ShaderObject(void);

		virtual void SetVectorVariable(std::string) = 0;
		virtual void SetVectorVariable(std::string name, float3 & vec3) = 0;
		virtual void SetVectorVariable(std::string name, float4 & vec4) = 0;
		virtual void SetMatrixVariable(std::string name) = 0;
		virtual void SetMatrixVariable(std::string name, float4x4 & matrix) = 0;
		virtual void SetRawData(std::string name, void* data, uint32_t size) = 0;
		virtual void SetShaderResourceVariable(std::string name) = 0;
		virtual void SetReource(std::string name, RenderBuffer* data, uint32_t size) = 0;

		virtual void SetBool(std::string) = 0;
		virtual void SetBool(std::string, bool value) = 0;

		virtual void SetTechnique(std::string name) = 0;
		virtual size_t GetPass() = 0;
		virtual void Apply(size_t pass_index) = 0;
	};
}

#endif
