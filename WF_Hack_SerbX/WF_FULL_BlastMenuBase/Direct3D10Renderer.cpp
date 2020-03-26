#include "stdafx.h"

class Direct3D10BlockStateGuard final
{
private:
	ComPtr<ID3D10BlendState>		m_pUILastBlendState;
	float							m_LastBlendFactor[4];
	UINT							m_LastBlendMask;
	UINT							m_LastStencilRef;
	ComPtr<ID3D10InputLayout>		m_LastInputLayout;
	D3D10_PRIMITIVE_TOPOLOGY		m_LastTopology;
	ID3D10Buffer*					m_LastBuffers[8];
	UINT							m_LastStrides[8];
	UINT							m_LastOffsets[8];
	ComPtr<ID3D10PixelShader>		m_LastPSShader;
	ComPtr<ID3D10VertexShader>		m_LastVSShader;
	ComPtr<ID3D10GeometryShader>	m_LastGSShader;
	ComPtr<ID3D10DepthStencilState>	m_LastDepthState;
	ComPtr<ID3D10RasterizerState>	m_pUILastRasterizerState;
	ComPtr<ID3D10Device>			m_device;
public:
	Direct3D10BlockStateGuard(ComPtr<ID3D10Device>& device) : m_device(device)
	{
		Capture();
	}

	~Direct3D10BlockStateGuard()
	{
		Release();
	}

	void Capture()
	{
		m_device->OMGetBlendState(&m_pUILastBlendState, m_LastBlendFactor, &m_LastBlendMask);
		m_device->RSGetState(&m_pUILastRasterizerState);
		m_device->OMGetDepthStencilState(&m_LastDepthState, &m_LastStencilRef);
		m_device->IAGetInputLayout(&m_LastInputLayout);
		m_device->IAGetPrimitiveTopology(&m_LastTopology);
		m_device->IAGetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);
		m_device->PSGetShader(&m_LastPSShader);
		m_device->GSGetShader(&m_LastGSShader);
		m_device->VSGetShader(&m_LastVSShader);
	}

	inline void Release()
	{
		m_device->OMSetBlendState(m_pUILastBlendState.Get(), m_LastBlendFactor, m_LastBlendMask);
		m_device->RSSetState(m_pUILastRasterizerState.Get());
		m_device->OMSetDepthStencilState(m_LastDepthState.Get(), m_LastStencilRef);
		m_device->IASetInputLayout(m_LastInputLayout.Get());
		m_device->IASetPrimitiveTopology(m_LastTopology);
		m_device->IASetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);
		m_device->PSSetShader(m_LastPSShader.Get());
		m_device->GSSetShader(m_LastGSShader.Get());
		m_device->VSSetShader(m_LastVSShader.Get());
	}
};

class Direct3D10VertexManager final : public FontData, public VertexBuffer<Direct3DVertex>
{
	friend class Direct3D10Renderer;
protected:
	ComPtr<ID3D10Buffer> m_buffer;
	ComPtr<ID3D10ShaderResourceView> m_Texture;
	Direct3D10Renderer& m_parent_renderer;
public:
	Direct3D10VertexManager(Direct3D10Renderer& parent_renderer) : FontData(), m_parent_renderer(parent_renderer){}
	virtual ~Direct3D10VertexManager() {}

	virtual void Begin()
	{

		if (ResizeRequired())
		{
			m_buffer.Reset();

			if (m_numVertices > m_maxVertices)
			{
				m_maxVertices = m_numVertices;
			}

			D3D10_BUFFER_DESC bufdesc = CD3D10_BUFFER_DESC(GetMaxVertices() * sizeof(Vertex), D3D10_BIND_VERTEX_BUFFER, D3D10_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);

			Instalog::SystemFacades::ThrowIfFailed(m_parent_renderer.m_device->CreateBuffer(&bufdesc, nullptr, &m_buffer));
			m_needResize = false;
		}

		Instalog::SystemFacades::ThrowIfFailed(m_buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)(&m_data)));

		m_numVertices = 0;
		FinalizeBegin();
	}

	virtual void End()
	{
		InitializeEnd();

		m_buffer->Unmap();

		m_data = nullptr;
	}
};

Direct3D10Renderer::Direct3D10Renderer(ComPtr<ID3D10Device>&& device)
	: Renderer(), m_device(device)
{

}

Direct3D10Renderer::~Direct3D10Renderer()
{

}

HRESULT Direct3D10Renderer::Init()
{
	ComPtr<ID3DBlob> p_blob;

	HRESULT hr = D3D10CompileShaderFromMemory(shader, strlen(shader), "VSMain", "vs_4_0", &p_blob);
	if (FAILED(hr))
		return hr;

	hr = m_device->CreateVertexShader(p_blob->GetBufferPointer(), p_blob->GetBufferSize(), &m_vshader);
	if (FAILED(hr))
		return hr;

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	const uint32_t numElements = ARRAYSIZE(layout);

	hr = m_device->CreateInputLayout(layout, numElements, p_blob->GetBufferPointer(), p_blob->GetBufferSize(), &m_input_layout);
	if (FAILED(hr))
		return hr;

	hr = D3D10CompileShaderFromMemory(shader, strlen(shader), "PSMain", "ps_4_0", &p_blob);
	if (FAILED(hr))
		return hr;

	hr = m_device->CreatePixelShader(p_blob->GetBufferPointer(), p_blob->GetBufferSize(), &m_pshader);
	if (FAILED(hr))
		return hr;

	D3D10_BLEND_DESC blenddesc;
	ZeroMemory(&blenddesc, sizeof(blenddesc));
	blenddesc.AlphaToCoverageEnable = FALSE;
	blenddesc.BlendEnable[0] = TRUE;
	blenddesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	blenddesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	blenddesc.BlendOp = D3D10_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D10_BLEND_SRC_ALPHA;
	blenddesc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
	blenddesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;

	hr = m_device->CreateBlendState(&blenddesc, &m_blend_state);
	if (FAILED(hr))
		return hr;

	D3D10_RASTERIZER_DESC rastdesc = { D3D10_FILL_SOLID, D3D10_CULL_NONE, false, 0, 0.f, 0.f, false, false, false, false };

	hr = m_device->CreateRasterizerState(&rastdesc, &m_rast_state);
	if (FAILED(hr))
		return hr;
	
	D3D10_VIEWPORT vp;
	UINT numViewports = 1;
	m_device->RSGetViewports(&numViewports, &vp);

	width = static_cast<float>(vp.Width);
	height = static_cast<float>(vp.Height);

	return S_OK;
}

bool Direct3D10Renderer::Begin(const int fps)
{
	if (!Renderer::Begin(fps))
		return false;

	if (m_rast_state == nullptr)
		if (FAILED(Init()))
			return false;
			
	for each (auto& var in m_fonts)
	{
		var.second->Begin();
	}
	return true;
}

void Direct3D10Renderer::End()
{
	for each (auto& var in m_fonts)
	{
		var.second->End();
	}
}

void Direct3D10Renderer::Present()
{
	Direct3D10BlockStateGuard guard(m_device);

	m_device->RSSetState(m_rast_state.Get());
	m_device->IASetInputLayout(m_input_layout.Get());

	UINT stride = sizeof(Direct3DVertex);
	UINT offset = 0;
	m_device->OMSetBlendState(m_blend_state.Get(), NULL, 0xFFFFFFFF);
	m_device->PSSetShader(m_pshader.Get());
	m_device->VSSetShader(m_vshader.Get());
	m_device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	for each (auto& var in m_fonts)
	{
		auto& vertex_manager = var.second;
		m_device->IASetVertexBuffers(0, 1, vertex_manager->m_buffer.GetAddressOf(), &stride, &offset);
		m_device->PSSetShaderResources(0, 1, vertex_manager->m_Texture.GetAddressOf());
		m_device->Draw(vertex_manager->GetSafeVertices(), 0);
	}
}

HRESULT Direct3D10Renderer::LoadFont(const Font& font)
{
	HRESULT hr = S_OK;
	uint32_t hash = font.getHash();

	if (m_fonts.find(hash) != m_fonts.end())
		return hr;

	std::unique_ptr<uint32_t[]> bitmap;
	std::unique_ptr<Direct3D10VertexManager> font_data = std::make_unique<Direct3D10VertexManager>(*this);

	if (FAILED(hr = FontData::CreateFontObjects(font, font_data.get(), bitmap)))
		return hr;

	ComPtr<ID3D10Texture2D> buftex;
	D3D10_TEXTURE2D_DESC texdesc = CD3D10_TEXTURE2D_DESC(DXGI_FORMAT_R8G8B8A8_UNORM, font_data->m_TexWidth, font_data->m_TexHeight, 1, 1, D3D10_BIND_SHADER_RESOURCE, D3D10_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);

	D3D10_SUBRESOURCE_DATA data;
	data.pSysMem = bitmap.get();
	data.SysMemPitch = font_data->m_TexWidth * sizeof(uint32_t);
	data.SysMemSlicePitch = 0;

	if (FAILED(hr = m_device->CreateTexture2D(&texdesc, &data, &buftex)))
		return hr;

	if (FAILED(hr = m_device->CreateShaderResourceView(buftex.Get(), nullptr, &font_data->m_Texture)))
		return hr;

	font_data->Begin();
	m_fonts.insert(std::pair<uint32_t, std::unique_ptr<Direct3D10VertexManager>>(hash, std::move(font_data)));
	return S_OK;
}

void Direct3D10Renderer::FreeFont(const Font& font)
{
	uint32_t hash = font.getHash();
	if (m_fonts.find(hash) == m_fonts.end())
		return;

	auto data = std::move(m_fonts[hash]);
	m_fonts.erase(hash);
}

void XM_CALLCONV Direct3D10Renderer::AddFilledRect(const DirectX::XMVECTOR &rect)
{
	if (FAILED(LoadFont(DEFAULT_FONT)))
		return;

	auto data = m_fonts[DEFAULT_FONT.getHash()].get();

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

HRESULT Direct3D10Renderer::AddText(const Font& font, float x, float y, float scale, const std::string& strText, DWORD flag)
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

			loc.x = loc.z + 0.7f * scalex * scale;
		}
		else
			loc.x += data->m_Spacing * scalex * scale;
	}
	return D3D_OK;
}