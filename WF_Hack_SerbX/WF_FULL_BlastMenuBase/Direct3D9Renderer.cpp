#include "stdafx.h"

class Direct3D9BlockStateGuard final
{
private:
	ComPtr<IDirect3DStateBlock9>	m_state_block;
	ComPtr<IDirect3DDevice9>		m_device;
public:
	Direct3D9BlockStateGuard(ComPtr<IDirect3DDevice9>& device) : m_device(device)
	{
		Instalog::SystemFacades::ThrowIfFailed(m_device->CreateStateBlock(D3DSTATEBLOCKTYPE::D3DSBT_ALL, &m_state_block));
		Capture();
	}

	~Direct3D9BlockStateGuard()
	{
		Release();
	}

	inline void Capture()
	{
		m_state_block->Capture();
	}

	inline void Release()
	{
		m_state_block->Apply();
	}
};

class Direct3D9VertexManager final : public FontData, public VertexBuffer<Direct3DVertex>
{
	friend class Direct3D9Renderer;
protected:
	ComPtr<IDirect3DTexture9> m_Texture;
	ComPtr<IDirect3DVertexBuffer9> m_buffer;
	Direct3D9Renderer& m_parent_renderer;
public:
	Direct3D9VertexManager(Direct3D9Renderer& parent_renderer) : FontData(), m_parent_renderer(parent_renderer){}
	virtual ~Direct3D9VertexManager() {}

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

			Instalog::SystemFacades::ThrowIfFailed(m_parent_renderer.m_device->CreateVertexBuffer(GetMaxVertices() * sizeof(Vertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_buffer, nullptr));
			m_needResize = false;
		}

		Instalog::SystemFacades::ThrowIfFailed(m_buffer->Lock(0, 0, (void**)(&m_data), D3DLOCK_DISCARD));

		m_numVertices = 0;
		FinalizeBegin();
	}

	virtual void End()
	{
		InitializeEnd();

		Instalog::SystemFacades::ThrowIfFailed(m_buffer->Unlock());

		m_data = nullptr;
	}

	friend class Direct3D9Renderer;
};

Direct3D9Renderer::Direct3D9Renderer(ComPtr<IDirect3DDevice9>&& device)
	: Renderer(), m_device(device)
{
}

Direct3D9Renderer::~Direct3D9Renderer()
{

}

HRESULT Direct3D9Renderer::Init()
{
	ComPtr<ID3DBlob> p_blob;
	HRESULT hr = D3D10CompileShaderFromMemory(shader, strlen(shader), "VSMain", "vs_2_0", &p_blob);
	if (FAILED(hr))
		return hr;

	hr = m_device->CreateVertexShader(static_cast<const DWORD*>(p_blob->GetBufferPointer()), &m_vshader);
	if (FAILED(hr))
		return hr;

	D3DVERTEXELEMENT9 layout[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	hr = m_device->CreateVertexDeclaration(layout, &m_vertex_declaration);
	if (FAILED(hr))
		return hr;

	hr = D3D10CompileShaderFromMemory(shader, strlen(shader), "PSMain", "ps_3_0", &p_blob);
	if (FAILED(hr))
		return hr;

	hr = m_device->CreatePixelShader(static_cast<const DWORD*>(p_blob->GetBufferPointer()), &m_pshader);
	if (FAILED(hr))
		return hr;

	hr = m_device->CreateStateBlock(D3DSTATEBLOCKTYPE::D3DSBT_ALL, &m_state_block);
	if (FAILED(hr))
		return hr;

	hr = m_device->BeginStateBlock();
	if (FAILED(hr))
		return hr;
	{
		m_device->SetRenderState(D3DRS_ZENABLE, FALSE);
		// Blend Desc
		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
		m_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
		m_device->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);

		// Rasterizer Desc
		m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_device->SetRenderState(D3DRS_CLIPPING, TRUE);
		m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		m_device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, FALSE);
		m_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
		m_device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);

		// Texture
		m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		m_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		m_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	}
	
	hr = m_device->EndStateBlock(m_state_block.GetAddressOf());
	if (FAILED(hr))
		return hr;

	D3DVIEWPORT9 vp;
	UINT numViewports = 1;
	m_device->GetViewport(&vp);

	width = static_cast<float>(vp.Width);
	height = static_cast<float>(vp.Height);

	return S_OK;
}

bool Direct3D9Renderer::Begin(const int fps)
{
	if (!Renderer::Begin(fps))
		return false;

	if (m_state_block == nullptr)
		if (FAILED(Init()))
			return false;

	for each (auto& var in m_fonts)
	{
		var.second->Begin();
	}
	return true;
}

void Direct3D9Renderer::End()
{
	for each (auto& var in m_fonts)
	{
		var.second->End();
	}
}

void Direct3D9Renderer::Present()
{
	Direct3D9BlockStateGuard guard(m_device);

	m_state_block->Apply();
	m_device->SetVertexDeclaration(m_vertex_declaration.Get());
	//m_device->SetVertexShader(m_vshader.Get());
	m_device->SetPixelShader(m_pshader.Get());
	UINT stride = sizeof(Direct3DVertex);

	for each (auto& var in m_fonts)
	{
		auto& vertex_manager = var.second;
		m_device->SetStreamSource(0, vertex_manager->m_buffer.Get(), 0, stride);
		m_device->SetTexture(0, var.second->m_Texture.Get());
		m_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vertex_manager->GetSafeVertices() / 3);
	}
}

HRESULT Direct3D9Renderer::LoadFont(const Font& font)
{
	HRESULT hr = S_OK;
	uint32_t hash = font.getHash();

	if (m_fonts.find(hash) != m_fonts.end())
		return hr;

	std::unique_ptr<uint32_t[]> bitmap;
	std::unique_ptr<Direct3D9VertexManager> font_data = std::make_unique<Direct3D9VertexManager>(*this);
	
	if (FAILED(hr = FontData::CreateFontObjects(font, font_data.get(), bitmap)))
		return hr;


	ComPtr<IDirect3DTexture9> buftex;
	if (FAILED(hr = m_device->CreateTexture(font_data->m_TexWidth, font_data->m_TexHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &buftex, nullptr)))
		return hr;

	D3DLOCKED_RECT d3dlr;

	if (FAILED(hr = buftex->LockRect(0, &d3dlr, 0, D3DLOCK_DISCARD)))
		return hr;

	memcpy(d3dlr.pBits, bitmap.get(), font_data->m_TexWidth * font_data->m_TexHeight * sizeof(bitmap[0]));

	if (FAILED(hr = buftex->UnlockRect(0)))
		return hr;

	font_data->m_Texture = std::move(buftex);
	font_data->Begin();
	m_fonts.insert(std::pair<uint32_t, std::unique_ptr<Direct3D9VertexManager>>(hash, std::move(font_data)));
	return S_OK;
}

void Direct3D9Renderer::FreeFont(const Font& font)
{
	uint32_t hash = font.getHash();
	if (m_fonts.find(hash) == m_fonts.end())
		return;

	auto data = std::move(m_fonts[hash]);
	m_fonts.erase(hash);
}

void XM_CALLCONV Direct3D9Renderer::AddFilledRect(const DirectX::XMVECTOR &rect)
{
	if (FAILED(LoadFont(DEFAULT_FONT)))
		return;

	auto& data = m_fonts[DEFAULT_FONT.getHash()];

	register DirectX::XMFLOAT4A rec =
	{
		rect.m128_f32[0],
		rect.m128_f32[1],
		(rect.m128_f32[0] + rect.m128_f32[2]),
		(rect.m128_f32[1] + rect.m128_f32[3]),
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

HRESULT Direct3D9Renderer::AddText(const Font& font, float x, float y, float scale, const std::string& strText, DWORD flag)
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

	float fStartX = loc.x;

	for each(char c in strText)
	{
		if (c < 32 || c >= 128)
		{
			if (c == '\n')
			{
				loc.x = fStartX;
				loc.y += (data->m_TexCoords[c - 32].y - data->m_TexCoords[c - 32].w) * data->m_TexHeight * scale;
			}
			else
				continue;
		}

		c -= 33;

		loc.z = loc.x + ((data->m_TexCoords[c].z - data->m_TexCoords[c].x) * data->m_TexWidth  * scale);
		loc.w = loc.y + ((data->m_TexCoords[c].w - data->m_TexCoords[c].y) * data->m_TexHeight * scale);

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

			loc.x = loc.z + (0.7f * scale);
		}
		else
			loc.x += data->m_Spacing * scale;
	}
	return D3D_OK;
}