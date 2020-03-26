#include "stdafx.h"
#if (WINVER >= 0x0A00)
class Direct3D12VertexManager final : public FontData, public VertexBuffer<Direct3DVertex>
{
	friend class Direct3D12Renderer;
protected:
	ComPtr<ID3D12Resource> m_buffer;
	ComPtr<ID3D12Resource> m_Texture;
	D3D12_VERTEX_BUFFER_VIEW m_vector_view;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_srv_desc_cpu_handle;
	CD3DX12_GPU_DESCRIPTOR_HANDLE m_srv_desc_gpu_handle;

	Direct3D12Renderer& m_parent_renderer;
public:
	Direct3D12VertexManager(Direct3D12Renderer& parent_renderer) : FontData(), m_parent_renderer(parent_renderer) {}
	virtual ~Direct3D12VertexManager() {}

	virtual void Begin()
	{
		if (ResizeRequired())
		{
			m_buffer.Reset();

			if (m_numVertices > m_maxVertices)
			{
				m_maxVertices = m_numVertices;
			}

			D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(GetMaxVertices() * sizeof(Vertex));

			Instalog::SystemFacades::ThrowIfFailed(
				m_parent_renderer.m_device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_buffer)));
			m_needResize = false;
		}


		D3D12_RANGE rng = { 0, 0 };
		Instalog::SystemFacades::ThrowIfFailed(m_buffer->Map(0, &rng, reinterpret_cast<void**>(&m_data)));

		m_numVertices = 0;
		FinalizeBegin();
	}

	virtual void End()
	{
		InitializeEnd();
		D3D12_RANGE rng = { 0, GetSafeVertices() * sizeof(Vertex) };
		m_buffer->Unmap(0, &rng);

		m_data = nullptr;
		m_vector_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
		m_vector_view.StrideInBytes = sizeof(Vertex);
		m_vector_view.SizeInBytes = GetSafeVertices() * sizeof(Vertex);
	}

	friend class Direct3D12Renderer;
};

Direct3D12Renderer::Direct3D12Renderer(ComPtr<IDXGISwapChain3>&& swap_chain, ComPtr<ID3D12CommandQueue>&& command_queue)
	: Renderer(), m_swap_chain(swap_chain), m_command_queue(command_queue), m_initialized(false)
{
	m_swap_chain->GetDevice(IID_PPV_ARGS(&m_device));
}

Direct3D12Renderer::~Direct3D12Renderer()
{
	WaitForCompletion();

	m_command_lists.clear();
	m_command_allocators.clear();
	m_render_views.clear();
	m_frame_fences.clear();
	m_fonts.clear();
}

HRESULT Direct3D12Renderer::Init()
{
	HRESULT hr = S_OK;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	m_swap_chain->GetDesc1(&swapChainDesc);
	m_frame_count = swapChainDesc.BufferCount;
	width = static_cast<float>(swapChainDesc.Width);
	height = static_cast<float>(swapChainDesc.Height);

	if (!CreateFrameFences()) return E_FAIL;
	if (!CreateCommandList()) return E_FAIL;
	if (!CreateRenderTargets()) return E_FAIL;
	if (!CreateRootSignature()) return E_FAIL;
	if (!CreatePipelineStateObjects()) return E_FAIL;

	m_initialized = true;
	return S_OK;
}

bool Direct3D12Renderer::CreateCommandList()
{
	m_command_lists.resize(m_frame_count);
	m_command_allocators.resize(m_frame_count);
	for (uint32_t index = 0; index < m_frame_count; index++)
	{
		Instalog::SystemFacades::ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_command_allocators[index])));

		Instalog::SystemFacades::ThrowIfFailed(
			m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_command_allocators[index].Get(), nullptr,
				IID_PPV_ARGS(&m_command_lists[index])));
		m_command_lists[index]->Close();
	}
	return true;
}

bool Direct3D12Renderer::CreateRenderTargets()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	m_swap_chain->GetDesc1(&swapChainDesc);

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 200;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	Instalog::SystemFacades::ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_desc_heap_srv)));

	m_desc_size_srv = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_srv_desc_cpu_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_desc_heap_srv->GetCPUDescriptorHandleForHeapStart());
	m_srv_desc_gpu_handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_desc_heap_srv->GetGPUDescriptorHandleForHeapStart());

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = m_frame_count;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	Instalog::SystemFacades::ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_desc_heap_rtv)));

	m_desc_size_rtv = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_desc_heap_rtv->GetCPUDescriptorHandleForHeapStart());
	m_render_views.resize(m_frame_count);

	// Create a RTV for each frame.
	for (UINT buffer_index = 0; buffer_index < m_render_views.size(); buffer_index++) {
		Instalog::SystemFacades::ThrowIfFailed(m_swap_chain->GetBuffer(buffer_index, IID_PPV_ARGS(&m_render_views[buffer_index])));

		m_device->CreateRenderTargetView(m_render_views[buffer_index].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_desc_size_rtv);
	}
	return true;
}

bool Direct3D12Renderer::CreateFrameFences()
{
	m_fence_value = 0;

	m_frame_fences.resize(m_frame_count);
	for (uint32_t i = 0; i < m_frame_count; ++i)
	{
		FrameFence& ff = m_frame_fences[i];
		Instalog::SystemFacades::ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&ff.fence)));

		ff.fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		ff.fence_value = 0;
	}
	return true;
}

bool Direct3D12Renderer::CreateRootSignature()
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData,
		sizeof(featureData)))) {
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	CD3DX12_DESCRIPTOR_RANGE range[1];
	range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParam[1];
	rootParam[0].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// rootParam[1].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);// D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC

	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 1;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParam), rootParam, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Instalog::SystemFacades::ThrowIfFailed(D3D12LibSerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));

	Instalog::SystemFacades::ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_root_signature)));

	return true;
}

bool Direct3D12Renderer::CreatePipelineStateObjects()
{
	ComPtr<ID3D12Resource> buffer;
	Instalog::SystemFacades::ThrowIfFailed(m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&buffer)));

	ComPtr<ID3DBlob> pvs_blob;
	ComPtr<ID3DBlob> pps_blob;

	Instalog::SystemFacades::ThrowIfFailed(D3D12CompileShaderFromMemory(shader, strlen(shader), "VSMain", "vs_5_0", &pvs_blob));

	Instalog::SystemFacades::ThrowIfFailed(D3D12CompileShaderFromMemory(shader, strlen(shader), "PSMain", "ps_5_0", &pps_blob));


	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_BLEND_DESC blenddesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	blenddesc.AlphaToCoverageEnable = FALSE;
	blenddesc.IndependentBlendEnable = FALSE;
	blenddesc.RenderTarget[0].BlendEnable = TRUE;
	blenddesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blenddesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	blenddesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	blenddesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_root_signature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(pvs_blob.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pps_blob.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = blenddesc;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	Instalog::SystemFacades::ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipeline_state)));

	return true;
}

void Direct3D12Renderer::SignalQueue()
{
	FrameFence& ff = m_frame_fences[m_frame_index];
	Instalog::SystemFacades::ThrowIfFailed(m_command_queue->Signal(ff.fence.Get(), m_fence_value));
	ff.fence_value = m_fence_value;
	m_fence_value++;
}

bool Direct3D12Renderer::Begin(const int fps)
{
	if (!Renderer::Begin(fps))
		return false;
	if (!m_initialized)
		if (FAILED(Init()))
			return false;

	for(uint32_t index = 0; index < m_frame_count; index++)
		Instalog::SystemFacades::ThrowIfFailed(m_command_allocators[index]->Reset());

	m_frame_index = this->m_swap_chain->GetCurrentBackBufferIndex();

	for each (auto& var in m_fonts)
	{
		var.second->Begin();
	}
	return true;
}

void Direct3D12Renderer::End()
{
	for each (auto& var in m_fonts)
	{
		var.second->End();
	}

	for (uint32_t index = 0; index < m_frame_count; index++)
	{
		auto& command_list = m_command_lists[index];
		Instalog::SystemFacades::ThrowIfFailed(m_command_allocators[index]->Reset());

		Instalog::SystemFacades::ThrowIfFailed(command_list->Reset(m_command_allocators[index].Get(), m_pipeline_state.Get()));

		command_list->SetPipelineState(m_pipeline_state.Get());
		command_list->SetGraphicsRootSignature(m_root_signature.Get());

		ID3D12DescriptorHeap* ppHeaps[] = { m_desc_heap_srv.Get() };
		command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		command_list->SetGraphicsRootDescriptorTable(0, m_desc_heap_srv->GetGPUDescriptorHandleForHeapStart());

		D3D12_VIEWPORT vp;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		D3D12_RECT scissor;
		scissor.right = static_cast<LONG>(width);
		scissor.bottom = static_cast<LONG>(height);

		command_list->RSSetScissorRects(1, &scissor);
		command_list->RSSetViewports(1, &vp);
		command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_render_views[index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_desc_heap_rtv->GetCPUDescriptorHandleForHeapStart(), index, m_desc_size_rtv);
		command_list->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		for each (auto& var in m_fonts)
		{
			auto& vertex_manager = var.second;
			command_list->SetGraphicsRootDescriptorTable(0, vertex_manager->m_srv_desc_gpu_handle);
			command_list->IASetVertexBuffers(0, 1, &vertex_manager->m_vector_view);
			command_list->DrawInstanced(vertex_manager->GetSafeVertices(), 1, 0, 0);
		}

		command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_render_views[index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		command_list->Close();
	}

}
void Direct3D12Renderer::Present()
{
	m_frame_index = m_swap_chain->GetCurrentBackBufferIndex();

	WaitForFence();

	ID3D12CommandList* ppCommandLists[] = { m_command_lists[m_frame_index].Get() };
	m_command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	SignalQueue();
}

void Direct3D12Renderer::WaitForCompletion()
{
	Instalog::SystemFacades::ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_completion_fence.fence)));
	
	m_completion_fence.fence_value = 1;

	m_completion_fence.fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_completion_fence.fence_event == nullptr)
	{
		Instalog::SystemFacades::Win32Exception::ThrowFromLastError();
		return;
	}

	const uint64_t fence = m_completion_fence.fence_value;
	Instalog::SystemFacades::ThrowIfFailed(m_command_queue->Signal(m_completion_fence.fence.Get(), fence));
	if (m_completion_fence.fence->GetCompletedValue() < fence)
	{
		m_completion_fence.fence->SetEventOnCompletion(fence, m_completion_fence.fence_event);
		WaitForSingleObject(m_completion_fence.fence_event, INFINITE);
	}
}

void Direct3D12Renderer::WaitForFence()
{
	FrameFence& ff = m_frame_fences[m_frame_index];

	const uint64_t fence = ff.fence_value;
	if (ff.fence->GetCompletedValue() < fence)
	{
		ff.fence->SetEventOnCompletion(fence, ff.fence_event);
		WaitForSingleObject(ff.fence_event, INFINITE);
	}
}

HRESULT Direct3D12Renderer::LoadFont(const Font& font)
{
	HRESULT hr = S_OK;
	uint32_t hash = font.getHash();

	if (m_fonts.find(hash) != m_fonts.end())
		return hr;

	std::unique_ptr<uint32_t[]> bitmap;
	std::unique_ptr<Direct3D12VertexManager> font_data = std::make_unique<Direct3D12VertexManager>(*this);

	if (FAILED(hr = FontData::CreateFontObjects(font, font_data.get(), bitmap)))
		return hr;

	ComPtr<ID3D12Resource> buftex;
	ComPtr<ID3D12Resource> textureUploadHeap;

	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Width = font_data->m_TexWidth;
	texDesc.Height = font_data->m_TexHeight;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	texDesc.DepthOrArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	font_data->Begin();

	if (FAILED(hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&buftex))))
		return hr;

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(buftex.Get(), 0, 1);

	if (FAILED(hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureUploadHeap))))
		return hr;
	WaitForFence();
	hr = m_command_lists[m_frame_index]->Reset(m_command_allocators[m_frame_index].Get(), m_pipeline_state.Get());
	if (FAILED(hr))
		return hr;

	D3D12_SUBRESOURCE_DATA data;
	data.pData = bitmap.get();
	data.RowPitch = font_data->m_TexWidth * sizeof(uint32_t);
	data.SlicePitch = data.RowPitch * font_data->m_TexHeight;
	UpdateSubresources(m_command_lists[m_frame_index].Get(), buftex.Get(), textureUploadHeap.Get(), 0, 0, 1, &data);
	m_command_lists[m_frame_index]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buftex.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	hr = m_command_lists[m_frame_index]->Close();
	if (FAILED(hr))
		return hr;

	ID3D12CommandList* ppCommandLists[] = { m_command_lists[m_frame_index].Get() };
	m_command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	SignalQueue();
	WaitForFence();

	font_data->m_Texture = buftex.Detach();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	m_device->CreateShaderResourceView(font_data->m_Texture.Get(), &srvDesc, m_srv_desc_cpu_handle);
	font_data->m_srv_desc_cpu_handle = m_srv_desc_cpu_handle;
	font_data->m_srv_desc_gpu_handle = m_srv_desc_gpu_handle;
	m_srv_desc_cpu_handle.Offset(m_desc_size_srv);
	m_srv_desc_gpu_handle.Offset(m_desc_size_srv);

	//m_command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	// SignalQueue();
	m_fonts.insert(std::pair<uint32_t, std::unique_ptr<Direct3D12VertexManager>>(hash, std::move(font_data)));
	return S_OK;
}

void Direct3D12Renderer::FreeFont(const Font& font)
{
	uint32_t hash = font.getHash();
	if (m_fonts.find(hash) == m_fonts.end())
		return;

	auto data = std::move(m_fonts[hash]);
	m_fonts.erase(hash);
}

void XM_CALLCONV Direct3D12Renderer::AddFilledRect(const DirectX::XMVECTOR &rect)
{
	if (FAILED(LoadFont(DEFAULT_FONT)))
		return;

	auto& data = m_fonts[DEFAULT_FONT.getHash()];

	const float scalex = 1 / width * 2.f;
	const float scaley = 1 / height * 2.f;
	register DirectX::XMFLOAT4A rec =
	{
		(rect.m128_f32[0] + rect.m128_f32[2]) * scalex - 1.f,
		1.f - (rect.m128_f32[1] + rect.m128_f32[3]) * scaley,
		rect.m128_f32[0] * scalex - 1.f,
		1.f - rect.m128_f32[1] * scaley
	};

	register Direct3DVertex v[6] =
	{
		{ DirectX::XMFLOAT2(rec.x, rec.w), m_colour },
	{ DirectX::XMFLOAT2(rec.x, rec.y), m_colour },
	{ DirectX::XMFLOAT2(rec.z, rec.w), m_colour },

	{ DirectX::XMFLOAT2(rec.z, rec.y), m_colour },
	{ DirectX::XMFLOAT2(rec.z, rec.w), m_colour },
	{ DirectX::XMFLOAT2(rec.x, rec.y), m_colour }
	};
	data->Add(v, ARRAYSIZE(v));
}

HRESULT Direct3D12Renderer::AddText(const Font& font, float x, float y, float scale, const std::string& strText, DWORD flag)
{
	HRESULT hr = S_OK;
	if (FAILED(hr = LoadFont(font)))
		return hr;

	auto data = m_fonts[font.getHash()].get();

	DirectX::XMFLOAT4A loc;
	DirectX::XMFLOAT2 size;
	switch (flag)
	{
	case FONT_ALIGN_LEFT:
		loc = { x, y , 0, 0 };
		break;
	case FONT_ALIGN_CENTER:
		size = MeasureText(data, strText, scale);
		loc = { x - (size.x / 2.f), y, 0, 0 };
		break;
	case FONT_ALIGN_RIGHT:
		size = MeasureText(data, strText, scale);
		loc = { x - size.x, y, 0, 0 };
		break;
	}

	const float fStartX = loc.x;
	const float scalex = 1 / width * 2.f;
	const float scaley = 1 / height * 2.f;

	loc = { loc.x  * scalex - 1.f, 1.f - loc.y * scaley, loc.z, loc.w };

	for each(char c in strText)
	{
		if (c < 32 || c >= 128)
		{
			if (c == '\n')
			{
				loc.x = fStartX;
				loc.y += (data->m_TexCoords[c - 32].y - data->m_TexCoords[c - 32].w) * data->m_TexHeight * scaley * scale;
			}
			else
				continue;
		}

		c -= 33;

		loc.z = loc.x + ((data->m_TexCoords[c].z - data->m_TexCoords[c].x) * data->m_TexWidth * scalex * scale);
		loc.w = loc.y + ((data->m_TexCoords[c].y - data->m_TexCoords[c].w) * data->m_TexHeight * scaley * scale);

		if (c != -1)
		{
			Direct3DVertex v[6];
			v[0] = { DirectX::XMFLOAT2(loc.x, loc.w), m_font_colour, DirectX::XMFLOAT2(data->m_TexCoords[c].x, data->m_TexCoords[c].w) };
			v[1] = { DirectX::XMFLOAT2(loc.x, loc.y), m_font_colour, DirectX::XMFLOAT2(data->m_TexCoords[c].x, data->m_TexCoords[c].y) };
			v[2] = { DirectX::XMFLOAT2(loc.z, loc.w), m_font_colour, DirectX::XMFLOAT2(data->m_TexCoords[c].z, data->m_TexCoords[c].w) };
			v[3] = { DirectX::XMFLOAT2(loc.z, loc.y), m_font_colour, DirectX::XMFLOAT2(data->m_TexCoords[c].z, data->m_TexCoords[c].y) };
			v[4] = v[2];
			v[5] = v[1];
			data->Add(v, 6);

			loc.x += (loc.z - loc.x) + 0.7f * scalex * scale;
		}
		else
			loc.x += data->m_Spacing * scalex * scale;
	}
	return D3D_OK;
}
#endif