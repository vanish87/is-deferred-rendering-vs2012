#include "D3DRenderEngine.h"


namespace MocapGE
{
	D3DRenderEngine::D3DRenderEngine(void)
	{		
		cur_frame_buffer_ = nullptr;
	}


	D3DRenderEngine::~D3DRenderEngine(void)
	{
	}

	void D3DRenderEngine::InitRenderWindow( std::string const & name, RenderSetting const & render_setting )
	{
		RenderEngine::InitRenderWindow(name, render_setting);

		HRESULT result;
		UINT create_device_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		result = D3D11CreateDevice(
				0,                 // default adapter
				md3dDriverType,
				0,                 // no software device
				create_device_flags, 
				0, 0,              // default feature level array
				D3D11_SDK_VERSION,
				&d3d_device_,
				&d3d_feature_level_,
				&d3d_imm_context_);

		if( FAILED(result) )
		{
			PRINT("D3D11CreateDevice Failed.");
		}

		if( d3d_feature_level_ != D3D_FEATURE_LEVEL_11_0 )
		{
			PRINT("Direct3D Feature Level 11 unsupported.");
		}
		UINT msaa_quality = 0;
		result = d3d_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaa_quality);
		if(FAILED(result) )
		{
			PRINT("msaa_quality Failed.");
		}

		DXGI_SWAP_CHAIN_DESC swap_chain_desc;
		swap_chain_desc.BufferDesc.Width  = render_setting_.width;
		swap_chain_desc.BufferDesc.Height = render_setting_.height;
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Use 4X MSAA? 
		if( render_setting_.msaa4x )
		{
			swap_chain_desc.SampleDesc.Count   = 4;
			swap_chain_desc.SampleDesc.Quality = msaa_quality-1;
		}
		// No MSAA
		else
		{
			swap_chain_desc.SampleDesc.Count   = 1;
			swap_chain_desc.SampleDesc.Quality = 0;
		}

		swap_chain_desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.BufferCount  = 1;
		swap_chain_desc.OutputWindow = Context::Instance().AppInstance().GetWindow().GetWnd();
		swap_chain_desc.Windowed     = true;
		swap_chain_desc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
		swap_chain_desc.Flags        = 0;

		IDXGIDevice* dxgi_device = 0;
		result = d3d_device_->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device);
		if( FAILED(result) )
		{
			PRINT("dxgiDevice Failed.");
		}

		IDXGIAdapter* dxgi_adapter = 0;
		result = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter);
		if( FAILED(result) )
		{
			PRINT("dxgiAdapter Failed.");
		}

		IDXGIFactory* dxgiFactory = 0;
		result = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
		if( FAILED(result) )
		{
			PRINT("dxgiFactory Failed.");
		}

		result = dxgiFactory->CreateSwapChain(d3d_device_, &swap_chain_desc, &d3d_swap_chain);
		if( FAILED(result) )
		{
			PRINT("d3d_swap_chain Failed.");
		}

		dxgi_device->Release();
		dxgi_adapter->Release();
		dxgiFactory->Release();


		this->OnResize();



	}


	void D3DRenderEngine::Render(RenderLayout* render_layout, ShaderObject* shader_object, int pass_index)
	{
		HRESULT result;
		//Get view and Projection Matrix
		D3DFrameBuffer* d3d_frame_buffer;
		d3d_frame_buffer= static_cast<D3DFrameBuffer*>(cur_frame_buffer_);
		float4x4 view_mat = d3d_frame_buffer->GetFrameCamera()->GetViewMatirx();
		float4x4 proj_mat = d3d_frame_buffer->GetFrameCamera()->GetProjMatrix();
		float3 camera_pos = d3d_frame_buffer->GetFrameCamera()->GetPos();
		float3 camera_at =  d3d_frame_buffer->GetFrameCamera()->GetLookAt();

		//Make sure every Shader has a constant named view_proj_matrix
		shader_object->SetMatrixVariable("g_view_proj_matrix", view_mat*proj_mat);
		shader_object->SetMatrixVariable("g_view_matrix",view_mat);
		shader_object->SetMatrixVariable("g_inv_proj_matrix", Math::Inverse(proj_mat));
		shader_object->SetMatrixVariable("g_inv_view_matrix", Math::Inverse(view_mat));
		shader_object->SetVectorVariable("g_eye_pos", camera_pos);
		

		D3DShaderobject* d3d_shader_object = static_cast<D3DShaderobject*>(shader_object);
		//size_t pass = d3d_shader_object->GetPass();

		
		//IASetInputLayout
		D3DRenderLayout* d3d_rl = static_cast<D3DRenderLayout*>(render_layout);
		std::vector<VertexUsage> vertex_layout = d3d_rl->GetInputLayout();
		D3D11_INPUT_ELEMENT_DESC *input_layout_desc = new D3D11_INPUT_ELEMENT_DESC[vertex_layout.size()]; 
		for (size_t i = 0; i < vertex_layout.size(); i++)
		{
			input_layout_desc[i].SemanticIndex = 0;
			input_layout_desc[i].InputSlot = 0;
			input_layout_desc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			input_layout_desc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			input_layout_desc[i].InstanceDataStepRate = 0;
			switch (vertex_layout[i])
			{
			case VU_POSITION:
				input_layout_desc[i].SemanticName = "POSITION";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VU_NORMAL:
				input_layout_desc[i].SemanticName = "NORMAL";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VU_TEXCOORD:
				input_layout_desc[i].SemanticName = "TEXCOORD";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case VU_TANGENT:
				input_layout_desc[i].SemanticName = "TANGENT";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VU_BINORMAL:
				input_layout_desc[i].SemanticName = "BINORMAL";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VU_COLOR:
				input_layout_desc[i].SemanticName = "COLOR";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				input_layout_desc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				break;
			default:
				break;
			}


		}
		// Create the input layout
		D3DX11_PASS_DESC pass_desc;
		result = d3d_shader_object->GetTechnique()->GetPassByIndex(pass_index)->GetDesc( &pass_desc );
		if(FAILED(result))PRINT("Cannot Get Pass Desc");
		ID3D11InputLayout* input_layout;
		result = d3d_device_->CreateInputLayout(input_layout_desc, vertex_layout.size(), pass_desc.pIAInputSignature, 
			pass_desc.IAInputSignatureSize, &input_layout);
		if(FAILED(result))PRINT("Cannot Create Input Layout");
		d3d_imm_context_->IASetInputLayout(input_layout);
		input_layout->Release();
		delete[] input_layout_desc;

		//IASetPrimitiveTopology
		PrimitiveType pri_type = d3d_rl->GetPrimitive();
		switch (pri_type)
		{
		case MocapGE::PT_POINTLIST:
			d3d_imm_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			break;
		case MocapGE::PT_TRIANGLELIST:
			d3d_imm_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		case MocapGE::PT_LINELIST:
			d3d_imm_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			break;
		default:
			break;
		}
		{							
			//IASetVertexBuffers
			D3DRenderBuffer* d3d_vertex_buffer = static_cast<D3DRenderBuffer*>(d3d_rl->GetBuffer(VBU_VERTEX));
			uint32_t stride = d3d_rl->GetVertexSize();
			uint32_t offset = 0;
			d3d_imm_context_->IASetVertexBuffers(0, 1, &(d3d_vertex_buffer->D3DBuffer()), &stride, &offset);

			//IASetIndexBuffer	
			D3DRenderBuffer* d3d_index_buffer = static_cast<D3DRenderBuffer*>(d3d_rl->GetBuffer(VBU_INDEX));
			d3d_imm_context_->IASetIndexBuffer(d3d_index_buffer->D3DBuffer(), DXGI_FORMAT_R32_UINT, 0);
			
			//SetShaderPara
			//DrawIndexed
			d3d_shader_object->Apply(pass_index);
			uint32_t index_count = d3d_rl->GetIndexCount();	
			d3d_imm_context_->DrawIndexed(index_count, 0, 0);
		}

	}

	void D3DRenderEngine::SwapBuffers()
	{
		d3d_swap_chain->Present(0, 0);
	}

	void D3DRenderEngine::OnResize()
	{
		HRESULT result;
		D3DFrameBuffer* d3d_frame_buffer;
		if(cur_frame_buffer_ == nullptr)
		{
			cur_frame_buffer_ = Context::Instance().GetRenderFactory().MakeFrameBuffer(render_setting_);
		    d3d_frame_buffer= static_cast<D3DFrameBuffer*>(cur_frame_buffer_);
		}
		else
		{
			//check it
			d3d_frame_buffer= static_cast<D3DFrameBuffer*>(cur_frame_buffer_);
			d3d_frame_buffer->D3DRTView()->D3DRTV()->Release();
			d3d_frame_buffer->D3DDSView()->D3DDSV()->Release();
		}

		//TODO : Use new size of window to resize FrameBuffer
		result = d3d_swap_chain->ResizeBuffers(1, render_setting_.width, render_setting_.height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		if(FAILED(result))
			PRINT("ResizeBuffer Failed!");

		ID3D11Texture2D* back_buffer;
		ID3D11RenderTargetView* render_target_view;// = d3d_frame_buffer->D3DRTView()->D3DRTV();
		result = d3d_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
		if(FAILED(result))
			PRINT("GetBuffer Failed!");
		result = d3d_device_->CreateRenderTargetView(back_buffer, 0, &render_target_view);
		if(FAILED(result))
			PRINT("CreateRenderTargetView Failed!");
		back_buffer->Release();

		D3D11_TEXTURE2D_DESC depth_stencil_desc;
	
		depth_stencil_desc.Width     = render_setting_.width;
		depth_stencil_desc.Height    = render_setting_.height;
		depth_stencil_desc.MipLevels = 1;
		depth_stencil_desc.ArraySize = 1;
		depth_stencil_desc.Format    = DXGI_FORMAT_R24G8_TYPELESS;



		if( render_setting_.msaa4x )
		{
			UINT msaa_quality = 0;
			result = d3d_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaa_quality);
			if(FAILED(result) )
			{
				PRINT("msaa_quality Failed.");
			}
			depth_stencil_desc.SampleDesc.Count   = 4;
			depth_stencil_desc.SampleDesc.Quality = msaa_quality-1;
		}
		// No MSAA
		else
		{
			depth_stencil_desc.SampleDesc.Count   = 1;
			depth_stencil_desc.SampleDesc.Quality = 0;
		}

		//TODO : Create Texture RTV through RenderFactory, as well as FrameBuffer->Onbind()
		depth_stencil_desc.Usage          = D3D11_USAGE_DEFAULT;
		depth_stencil_desc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depth_stencil_desc.CPUAccessFlags = 0; 
		depth_stencil_desc.MiscFlags      = 0;

		ID3D11Texture2D* depth_stencil_buffer;// = d3d_frame_buffer->D3DDSBuffer()->D3DTexture();
		ID3D11DepthStencilView*	depth_stencil_view;// = d3d_frame_buffer->D3DDSView()->D3DDSV();

		result = d3d_device_->CreateTexture2D(&depth_stencil_desc, 0, &depth_stencil_buffer);
		if(FAILED(result))
			PRINT("depth_stencil create Failed!");

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;

		result = d3d_device_->CreateDepthStencilView(depth_stencil_buffer, &dsvd, &depth_stencil_view);
		if(FAILED(result))
			PRINT("depth_stencil_view create Failed!");

		D3DRenderTargetView* d3d_rtv = new D3DRenderTargetView();
		d3d_rtv->SetD3DRTV(render_target_view);
		d3d_frame_buffer->AddRenderView(d3d_rtv);
		static_cast<D3DTexture2D*>(d3d_frame_buffer->GetDepthTexture())->SetD3DTexture(depth_stencil_buffer);
		d3d_frame_buffer->GetDepthTexture()->SetUsage(TU_DEPTH_SR);
		d3d_frame_buffer->D3DDSView()->SetD3DDSV(depth_stencil_view);

		this->BindFrameBuffer(d3d_frame_buffer);


		// Set up the description of the stencil state.
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depth_on_ = new D3DRenderState(depthStencilDesc);

		D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
		depthDisabledStencilDesc.DepthEnable = false;
		depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthDisabledStencilDesc.StencilEnable = true;
		depthDisabledStencilDesc.StencilReadMask = 0xFF;
		depthDisabledStencilDesc.StencilWriteMask = 0xFF;
		depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depth_off_ = new D3DRenderState(depthDisabledStencilDesc);

		D3D11_RASTERIZER_DESC wireframeDesc;
		ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
		wireframeDesc.FillMode = D3D11_FILL_SOLID;
		wireframeDesc.CullMode = D3D11_CULL_BACK;
		wireframeDesc.FrontCounterClockwise = false;
		wireframeDesc.DepthClipEnable = true;
		//wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;

		wire_frame_ = new D3DRenderState(wireframeDesc);


		depthDisabledStencilDesc.DepthEnable = false;
		depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthDisabledStencilDesc.StencilEnable = true;
		depthDisabledStencilDesc.StencilReadMask = 0xFF;
		depthDisabledStencilDesc.StencilWriteMask = 0xFF;
		depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		cull_depth_ = new D3DRenderState(depthDisabledStencilDesc);

		D3D11_RASTERIZER_DESC cull_Desc;
		ZeroMemory(&cull_Desc, sizeof(D3D11_RASTERIZER_DESC));
		cull_Desc.FillMode = D3D11_FILL_SOLID;
		cull_Desc.CullMode = D3D11_CULL_NONE;
		cull_Desc.FrontCounterClockwise = false;
		cull_Desc.DepthClipEnable = true;
		cull_Desc.DepthBias = 0;
		cull_Desc.DepthBiasClamp = 0.0f;
		cull_Desc.DepthClipEnable = true;
		cull_Desc.FillMode = D3D11_FILL_SOLID;
		cull_Desc.FrontCounterClockwise = false;
		cull_Desc.MultisampleEnable = false;
		cull_Desc.ScissorEnable = false;
		cull_Desc.SlopeScaledDepthBias = 0.0f;
		cull_back_ = new D3DRenderState(cull_Desc);

	}

	DXGI_FORMAT D3DRenderEngine::MapFormat( Format format )
	{
		switch (format)
		{
		case A8_U:
			return DXGI_FORMAT_A8_UNORM;
			break;
		case R8_U:
			return DXGI_FORMAT_R8_UNORM;
			break;
		case R8G8B8A8_U:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case R32G32B32A32_F:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		default:
			return DXGI_FORMAT_R32G32B32A32_UINT;
			break;
		}
	}

	void D3DRenderEngine::BindFrameBuffer( FrameBuffer* const & fb )
	{
		cur_frame_buffer_ = fb;
		fb->OnBind();
	}

	void D3DRenderEngine::RenderFrameBegin()
	{
		//Clear Frame Buffer
		float color[4] = {0.0f,0.0f,0.0f,1.0f};
		D3DFrameBuffer* d3d_frame_buffer;
		d3d_frame_buffer= static_cast<D3DFrameBuffer*>(cur_frame_buffer_);
		for(size_t i = 0; i< d3d_frame_buffer->D3DRTViewSize(); i++)
			d3d_imm_context_->ClearRenderTargetView(d3d_frame_buffer->D3DRTView(i)->D3DRTV(), color);
		d3d_imm_context_->ClearDepthStencilView(d3d_frame_buffer->D3DDSView()->D3DDSV(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);	


	}

	void D3DRenderEngine::RenderFrameEnd()
	{

		// Cleanup (aka make the runtime happy)
		d3d_imm_context_->VSSetShader(0, 0, 0);
		d3d_imm_context_->GSSetShader(0, 0, 0);
		d3d_imm_context_->PSSetShader(0, 0, 0);
		d3d_imm_context_->OMSetRenderTargets(0, 0, 0);
		ID3D11ShaderResourceView* nullSRV[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		d3d_imm_context_->VSSetShaderResources(0, 8, nullSRV);
		d3d_imm_context_->PSSetShaderResources(0, 8, nullSRV);
		d3d_imm_context_->CSSetShaderResources(0, 8, nullSRV);
		ID3D11UnorderedAccessView *nullUAV[1] = {0};
		d3d_imm_context_->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}

	void D3DRenderEngine::SetDeferredRenderingState()
	{
		ID3D11DepthStencilState* depth_state= depth_off_->GetDepthStencilState();
		d3d_imm_context_->OMSetDepthStencilState(depth_state, 1);
	}

	void D3DRenderEngine::SetNormalState()
	{
		ID3D11DepthStencilState* depth_state= depth_on_->GetDepthStencilState();
		d3d_imm_context_->OMSetDepthStencilState(depth_state, 1);

		ID3D11RasterizerState* res_state = wire_frame_->GetRasterizerState();
		d3d_imm_context_->RSSetState(res_state);

	}

	void D3DRenderEngine::TrunoffCull()
	{
		ID3D11DepthStencilState* depth_state= cull_depth_->GetDepthStencilState();
		d3d_imm_context_->OMSetDepthStencilState(depth_state, 1);

		ID3D11RasterizerState* res_state = cull_back_->GetRasterizerState();
		d3d_imm_context_->RSSetState(res_state);
	}





}

