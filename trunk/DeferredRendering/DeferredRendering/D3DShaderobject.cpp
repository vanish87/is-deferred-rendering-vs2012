#include "D3DShaderobject.h"
#include "Context.h"

namespace MocapGE
{
	D3DShaderobject::D3DShaderobject(void)
	{
	}


	D3DShaderobject::~D3DShaderobject(void)
	{
	}

	void D3DShaderobject::LoadFxoFile( std::string file_name )
	{
		std::ifstream fin(file_name, std::ios::binary);

		if (!fin)PRINT("Cannot open Fxo File ");

		fin.seekg(0, std::ios_base::end);
		int size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		std::vector<char> compiledShader(size);

		fin.read(&compiledShader[0], size);
		fin.close();
		D3DRenderEngine* render_engine = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		HRESULT result = D3DX11CreateEffectFromMemory(&compiledShader[0], size,	0, render_engine->D3DDevice(), &fx_);
		if(FAILED(result))
			PRINT("Cannot Load Effect File");
	}

	void D3DShaderobject::SetVariable( std::string name )
	{
		effect_variable_[name] = fx_->GetVariableByName(name.c_str());
	}

	void D3DShaderobject::SetMatrixVariable( std::string name )
	{
		matrix_variable_[name] = fx_->GetVariableByName(name.c_str())->AsMatrix();
	}

	void D3DShaderobject::SetBool( std::string name)
	{
		scale_variable_[name] = fx_->GetVariableByName(name.c_str())->AsScalar();
	}

	void D3DShaderobject::SetBool( std::string name, bool value )
	{
		ID3DX11EffectScalarVariable* bool_var = this->GetBoolVariable(name);
		HRESULT res = bool_var->SetBool(value);
		if(FAILED(res))
			PRINT("Cannot SetBool");
	}
	void D3DShaderobject::SetMatrixVariable( std::string name, float4x4 & matrix )
	{
		//TODO: write a better solution for cast float4x4 to float*
		ID3DX11EffectMatrixVariable* mat_var = GetMatrixVariable(name);
		float *p = new float[matrix.size()];
		for(size_t i = 0; i< matrix.row(); i++)
			for(size_t j = 0; j < matrix.col(); j++)
				p[i*matrix.row() + j] = matrix[i][j];
		HRESULT res = mat_var->SetMatrix(p);
		if(FAILED(res))
			PRINT("Cannot set Shader Matrix");
		delete[] p;
	}

	void D3DShaderobject::SetVectorVariable( std::string name )
	{
		vector_variable_[name] = fx_->GetVariableByName(name.c_str())->AsVector();
	}

	void D3DShaderobject::SetVectorVariable( std::string name, float3 & vec3 )
	{
		ID3DX11EffectVectorVariable* vec3_var = GetVectorVariable(name);
		float data[3] = {vec3[0], vec3[1], vec3[2]};
		HRESULT res = vec3_var->SetFloatVector(data);
		if(FAILED(res))
			PRINT("Cannot SetFloatVector");
	}

	void D3DShaderobject::SetVectorVariable( std::string name, float4 & vec4 )
	{
		ID3DX11EffectVectorVariable* vec4_var = GetVectorVariable(name);
		float data[4] = {vec4[0], vec4[1], vec4[2], vec4[3]};
		HRESULT res = vec4_var->SetFloatVector(data);
		if(FAILED(res))PRINT("Fail to SetVectorVariable: " + name);
	}

	void D3DShaderobject::SetTechnique( std::string name )
	{
		tech_ = fx_->GetTechniqueByName(name.c_str());
	}

	size_t D3DShaderobject::GetPass()
	{
		D3DX11_TECHNIQUE_DESC tech_desc;
		HRESULT res = tech_->GetDesc( &tech_desc );
		if(FAILED(res))
			PRINT("Cannot get pass");
		return tech_desc.Passes;
	}

	void D3DShaderobject::Apply( size_t pass_index )
	{
		D3DRenderEngine* d3d_render_engine = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		HRESULT res = tech_->GetPassByIndex(pass_index)->Apply(0, d3d_render_engine->D3DDeviceImmContext());
		if(FAILED(res))
			PRINT("Cannot apply pass");
	}

	void D3DShaderobject::SetRawData( std::string name, void* data, uint32_t size )
	{
		ID3DX11EffectVariable* var = this->GetVariable(name);
		HRESULT res = var->SetRawValue(data, 0, size);
		if(FAILED(res))
			PRINT("Cannot set RawData");
	}

	void D3DShaderobject::SetReource( std::string name, RenderBuffer* data, uint32_t type )
	{

		ID3DX11EffectShaderResourceVariable* sr_var = this->GetShaderRourceVariable(name);
		if(data == 0)
		{
			HRESULT res = sr_var->SetResource(NULL);
			if(FAILED(res))
				PRINT("Cannot set Resource");
			return;
		}

		//TODO : Combine them together
		if(type == 0)//RenderBuffer
		{
			D3DRenderBuffer* d3d_render_buffer;
			d3d_render_buffer = static_cast<D3DRenderBuffer*>(data);
			HRESULT res = sr_var->SetResource(d3d_render_buffer->D3DShaderResourceView());
			if(FAILED(res))
				PRINT("Cannot set Resource");
		}
		else//Shader Resource
		{
			D3DShaderResourceView* d3d_srv = static_cast<D3DShaderResourceView*>(data);
			HRESULT res = sr_var->SetResource(d3d_srv->D3DSRV());
			if(FAILED(res))
				PRINT("Cannot set Resource");
		}
	}

	void D3DShaderobject::SetShaderResourceVariable( std::string name )
	{
		bool valid = fx_->GetVariableByName(name.c_str())->IsValid();
		if( !valid)
			PRINT("Cannot find Variables");
		shader_resource_variable_[name] = fx_->GetVariableByName(name.c_str())->AsShaderResource();
	}




}
