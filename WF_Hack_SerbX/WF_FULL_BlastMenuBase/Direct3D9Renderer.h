#pragma once

class Direct3D9VertexManager;

class Direct3D9Renderer final : public Renderer
{

private:
	ComPtr<IDirect3DDevice9> m_device;
	ComPtr<IDirect3DStateBlock9> m_state_block;
	ComPtr<IDirect3DVertexDeclaration9> m_vertex_declaration;

	float width, height;

	ComPtr<IDirect3DPixelShader9> m_pshader;
	ComPtr<IDirect3DVertexShader9> m_vshader;

	std::unordered_map<uint32_t, std::unique_ptr<Direct3D9VertexManager>> m_fonts;
public:
	Direct3D9Renderer(ComPtr<IDirect3DDevice9>&& device);
	virtual ~Direct3D9Renderer();

	virtual HRESULT Init();

	virtual bool Begin(const int fps = 25);
	virtual void End();
	virtual void Present();

	virtual HRESULT LoadFont(const Font& font);
	virtual void FreeFont(const Font& font);

	virtual void XM_CALLCONV AddFilledRect(const DirectX::XMVECTOR &rect);

	virtual HRESULT AddText(const Font& font, float x, float y, float scale, const std::string& strText, DWORD flag = 0);
private:
	friend class Direct3D9VertexManager;
};
