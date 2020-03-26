#include "stdafx.h"

class Direct3DBlockStateGuard final
{
private:
	ComPtr<ID3D11BlendState>		m_pUILastBlendState;
	float							m_LastBlendFactor[4];
	UINT							m_LastBlendMask;
	UINT							m_LastStencilRef;
	ComPtr<ID3D11InputLayout>		m_LastInputLayout;
	D3D11_PRIMITIVE_TOPOLOGY		m_LastTopology;
	ID3D11Buffer*					m_LastBuffers[8];
	UINT							m_LastStrides[8];
	UINT							m_LastOffsets[8];
	ComPtr<ID3D11PixelShader>		m_LastPSShader;
	ComPtr<ID3D11VertexShader>		m_LastVSShader;
	ComPtr<ID3D11GeometryShader>	m_LastGSShader;
	ComPtr<ID3D11DepthStencilState>	m_LastDepthState;
	ComPtr<ID3D11RasterizerState>	m_pUILastRasterizerState;
	ComPtr<ID3D11DeviceContext>		m_context;
public:
	Direct3DBlockStateGuard(ComPtr<ID3D11DeviceContext>& context) : m_context(context)
	{
		Capture();
	}

	~Direct3DBlockStateGuard()
	{
		Release();
	}

	void Capture()
	{
		m_context->OMGetBlendState(&m_pUILastBlendState, m_LastBlendFactor, &m_LastBlendMask);
		m_context->RSGetState(&m_pUILastRasterizerState);
		m_context->OMGetDepthStencilState(&m_LastDepthState, &m_LastStencilRef);
		m_context->IAGetInputLayout(&m_LastInputLayout);
		m_context->IAGetPrimitiveTopology(&m_LastTopology);
		m_context->IAGetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);
		m_context->PSGetShader(&m_LastPSShader, NULL, 0);
		m_context->GSGetShader(&m_LastGSShader, NULL, 0);
		m_context->VSGetShader(&m_LastVSShader, NULL, 0);
	}

	inline void Release()
	{
		m_context->OMSetBlendState(m_pUILastBlendState.Get(), m_LastBlendFactor, m_LastBlendMask);
		m_context->RSSetState(m_pUILastRasterizerState.Get());
		m_context->OMSetDepthStencilState(m_LastDepthState.Get(), m_LastStencilRef);
		m_context->IASetInputLayout(m_LastInputLayout.Get());
		m_context->IASetPrimitiveTopology(m_LastTopology);
		m_context->IASetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);
		m_context->PSSetShader(m_LastPSShader.Get(), NULL, 0);
		m_context->GSSetShader(m_LastGSShader.Get(), NULL, 0);
		m_context->VSSetShader(m_LastVSShader.Get(), NULL, 0);
	}
};

class Direct3D11VertexManager final : public FontData, public VertexBuffer<Direct3DVertex>
{
	friend class Direct3D11Renderer;
protected:
	ComPtr<ID3D11Buffer> m_buffer;
	ComPtr<ID3D11ShaderResourceView> m_Texture;
	Direct3D11Renderer& m_parent_renderer;
public:
	Direct3D11VertexManager(Direct3D11Renderer& parent_renderer) : FontData(), m_parent_renderer(parent_renderer) {}
	virtual ~Direct3D11VertexManager() {}

	virtual void Begin()
	{
		if (ResizeRequired())
		{
			m_buffer.Reset();

			if (m_numVertices > m_maxVertices)
			{
				m_maxVertices = m_numVertices;
			}

			D3D11_BUFFER_DESC bufdesc = CD3D11_BUFFER_DESC(GetMaxVertices() * sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

			Instalog::SystemFacades::ThrowIfFailed(m_parent_renderer.m_device->CreateBuffer(&bufdesc, nullptr, &m_buffer));
			m_needResize = false;
		}

		D3D11_MAPPED_SUBRESOURCE map;
		Instalog::SystemFacades::ThrowIfFailed(m_parent_renderer.m_context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map));

		m_data = (Vertex*)map.pData;
		m_numVertices = 0;
		FinalizeBegin();
	}

	virtual void End()
	{
		InitializeEnd();

		m_parent_renderer.m_context->Unmap(m_buffer.Get(), 0);

		m_data = nullptr;
	}
};

Direct3D11Renderer::Direct3D11Renderer(ComPtr<ID3D11Device>&& device)
	: Renderer(), m_device(device)
{
	
}

Direct3D11Renderer::~Direct3D11Renderer()
{

}

HRESULT Direct3D11Renderer::Init()
{
	ComPtr<ID3DBlob> p_blob;

	HRESULT hr = D3D11CompileShaderFromMemory(shader, strlen(shader), "VSMain", "vs_4_0", &p_blob);
	if (FAILED(hr))
		return hr;

	hr = m_device->CreateVertexShader(p_blob->GetBufferPointer(), p_blob->GetBufferSize(), nullptr, &m_vshader);
	if (FAILED(hr))
		return hr;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	const uint32_t numElements = ARRAYSIZE(layout);

	hr = m_device->CreateInputLayout(layout, numElements, p_blob->GetBufferPointer(), p_blob->GetBufferSize(), &m_input_layout);
	if (FAILED(hr))
		return hr;

	hr = D3D11CompileShaderFromMemory(shader, strlen(shader), "PSMain", "ps_4_0", &p_blob);
	if (FAILED(hr))
		return hr;

	hr = m_device->CreatePixelShader(p_blob->GetBufferPointer(), p_blob->GetBufferSize(), nullptr, &m_pshader);
	if (FAILED(hr))
		return hr;

	D3D11_BLEND_DESC blenddesc;
	blenddesc.AlphaToCoverageEnable = FALSE;
	blenddesc.IndependentBlendEnable = FALSE;
	blenddesc.RenderTarget[0].BlendEnable = TRUE;
	blenddesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blenddesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blenddesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blenddesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blenddesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blenddesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blenddesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	
	hr = m_device->CreateBlendState(&blenddesc, &m_blend_state);
	if (FAILED(hr))
		return hr;

	D3D11_RASTERIZER_DESC rastdesc = CD3D11_RASTERIZER_DESC(D3D11_FILL_SOLID, D3D11_CULL_NONE, false, 0, 0.f, 0.f, false, false, false, false);

	hr = m_device->CreateRasterizerState(&rastdesc, &m_rast_state);
	if (FAILED(hr))
		return hr;
	
	m_device->GetImmediateContext(&m_context);

	D3D11_VIEWPORT vp;
	UINT numViewports = 1;
	m_context->RSGetViewports(&numViewports, &vp);

	width = vp.Width;
	height = vp.Height;

	return S_OK;
}

bool Direct3D11Renderer::Begin(const int fps)
{
	if (!Renderer::Begin(fps))
		return false;
	if (m_context == nullptr)
		if (FAILED(Init()))
			return false;

	for each (auto& var in m_fonts)
	{
		var.second->Begin();
	}
	return true;
}

void Direct3D11Renderer::End()
{
	for each (auto& var in m_fonts)
	{
		var.second->End();
	}
}

void Direct3D11Renderer::Present()
{
	Direct3DBlockStateGuard guard(m_context);

	m_context->RSSetState(m_rast_state.Get());
	m_context->IASetInputLayout(m_input_layout.Get());

	UINT stride = sizeof(Direct3DVertex);
	UINT offset = 0;
	m_context->OMSetBlendState(m_blend_state.Get(), NULL, 0xFFFFFFFF);
	m_context->PSSetShader(m_pshader.Get(), NULL, 0);
	m_context->VSSetShader(m_vshader.Get(), NULL, 0);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for each (auto& var in m_fonts)
	{
		auto& vertex_manager = var.second;
		m_context->IASetVertexBuffers(0, 1, vertex_manager->m_buffer.GetAddressOf(), &stride, &offset);
		m_context->PSSetShaderResources(0, 1, vertex_manager->m_Texture.GetAddressOf());
		m_context->Draw(vertex_manager->GetSafeVertices(), 0);
	}
}

HRESULT Direct3D11Renderer::LoadFont(const Font& font)
{
	HRESULT hr = S_OK;
	uint32_t hash = font.getHash();

	if (m_fonts.find(hash) != m_fonts.end())
		return hr;

	std::unique_ptr<uint32_t[]> bitmap;
	std::unique_ptr<Direct3D11VertexManager> font_data = std::make_unique<Direct3D11VertexManager>(*this);

	if (FAILED(hr = FontData::CreateFontObjects(font, font_data.get(), bitmap)))
		return hr;

	ComPtr<ID3D11Texture2D> buftex;
	D3D11_TEXTURE2D_DESC texdesc = CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R8G8B8A8_UNORM, font_data->m_TexWidth, font_data->m_TexHeight, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = bitmap.get();
	data.SysMemPitch = font_data->m_TexWidth * sizeof(uint32_t);
	data.SysMemSlicePitch = 0;

	if (FAILED(hr = m_device->CreateTexture2D(&texdesc, &data, &buftex)))
		return hr;

	if (FAILED(hr = m_device->CreateShaderResourceView(buftex.Get(), nullptr, &font_data->m_Texture)))
		return hr;

	font_data->Begin();
	m_fonts.insert(std::pair<uint32_t, std::unique_ptr<Direct3D11VertexManager>>(hash, std::move(font_data)));
	return S_OK;
}

void Direct3D11Renderer::FreeFont(const Font& font)
{
	uint32_t hash = font.getHash();
	if (m_fonts.find(hash) == m_fonts.end())
		return;

	auto data = std::move(m_fonts[hash]);
	m_fonts.erase(hash);
}

void XM_CALLCONV Direct3D11Renderer::AddFilledRect(const DirectX::XMVECTOR &rect)
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

HRESULT Direct3D11Renderer::AddText(const Font& font, float x, float y, float scale, const std::string& strText, DWORD flag)
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

			loc.x = loc.z + 0.7f * scalex;
		}
		else
			loc.x += data->m_Spacing * scalex;
	}
	return D3D_OK;
}