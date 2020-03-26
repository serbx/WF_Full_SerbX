#pragma once
class Direct3D10VertexManager;

class Direct3D10Renderer final : public Renderer
{

private:
	ComPtr<ID3D10Device> m_device;
	ComPtr<ID3D10BlendState> m_blend_state;
	ComPtr<ID3D10InputLayout> m_input_layout;
	ComPtr<ID3D10RasterizerState> m_rast_state;

	float width, height;

	ComPtr<ID3D10PixelShader> m_pshader;
	ComPtr<ID3D10VertexShader> m_vshader;

	std::unordered_map<uint32_t, std::unique_ptr<Direct3D10VertexManager>> m_fonts;
public:
	Direct3D10Renderer(ComPtr<ID3D10Device>&& device);
	virtual ~Direct3D10Renderer();

	virtual HRESULT Init();

	virtual bool Begin(const int fps = 25);
	virtual void End();
	virtual void Present();

	virtual HRESULT LoadFont(const Font& font);
	virtual void FreeFont(const Font& font);

	virtual void XM_CALLCONV AddFilledRect(const DirectX::XMVECTOR &rect);

	virtual HRESULT AddText(const Font& font, float x, float y, float scale, const std::string& strText, DWORD flag = 0);
private:
	friend class Direct3D10VertexManager;
};
