#pragma once

enum FONT_ALIGN : DWORD {
	FONT_ALIGN_LEFT = 0,
	FONT_ALIGN_RIGHT,
	FONT_ALIGN_CENTER,
};

class Font final
{
private:
	std::string font_family;
	float size;
	Gdiplus::FontStyle flags;
	uint32_t hash;

	static std::hash<std::string> hash_fn;
public:

	Font(const std::string fontName = "Arial, Arial, Helvetica, sans-serif", float fHeight = 14, Gdiplus::FontStyle styles = Gdiplus::FontStyleRegular)
		: font_family(fontName), size(fHeight), flags(styles)
	{
		hash = generateHash();
	}

	std::string getFontFamily() const { return font_family; }

	float getSize() const { return size; }


	uint32_t getFlags() const { return flags; }

	uint32_t generateHash() const
	{
		size_t hs = hash_fn(font_family);
		auto val = hs * static_cast<double>(size) + static_cast<int>(flags);
		uint32_t* rets = reinterpret_cast<uint32_t*>(&val);
		return rets[0] << rets[1];
	}

	uint32_t getHash() const
	{
		return hash;
	}

	void setFontFamily(const std::string fontName) { font_family = fontName; hash = generateHash(); }

	void setSize(float fHeight) { size = fHeight; hash = generateHash(); }

	void setFlags(Gdiplus::FontStyle styles) { flags = styles; hash = generateHash(); }
};

class FontData
{
	friend class Renderer;
protected:
	DirectX::XMFLOAT4A m_TexCoords[0x60];

	uint32_t	m_TexWidth;
	uint32_t	m_TexHeight;
	float		m_Spacing;
public:
	virtual ~FontData() {}

	static HRESULT CreateFontObjects(const Font& font, FontData* data, std::unique_ptr<uint32_t[]>& lpBitmapBits);

	void* __cdecl operator new(size_t count)
	{
#ifdef _DEBUG
		return _aligned_malloc_dbg(count, 16, __FILE__, __LINE__);
#else
		return _aligned_malloc(count, 16);
#endif
	}
	void __cdecl operator delete(void * object)
	{
#ifdef _DEBUG
		_aligned_free_dbg(object);
#else
		_aligned_free(object);
#endif
	}
};            

class Renderer
{
protected:
	DWORD m_colour;
	DWORD m_font_colour;
public:
	Renderer() {}
	virtual ~Renderer() {}

	virtual HRESULT Init() { return S_OK; }

	virtual bool Begin(const int fps = 20);
	virtual void End() {};
	virtual void Present() {};

	virtual void SetDrawColor(const DWORD _colour) { m_colour = _colour; }
	virtual void SetFontColor(const DWORD _colour) { m_font_colour = _colour; }

	virtual HRESULT LoadFont(const Font& font) { return E_FAIL; };
	virtual void FreeFont(const Font& font) {};

	virtual void XM_CALLCONV AddFilledRect(const DirectX::XMVECTOR &rect) = 0;

	void XM_CALLCONV AddFilledRect(const DirectX::XMFLOAT4A &rect) { AddFilledRect(DirectX::XMLoadFloat4A(&rect)); }
	void XM_CALLCONV AddFilledRect(const DirectX::XMFLOAT4 &rect) { AddFilledRect(DirectX::XMLoadFloat4(&rect)); }
	void XM_CALLCONV AddFilledRect(const DirectX::XMUINT4 &rect) { AddFilledRect(DirectX::XMLoadUInt4(&rect)); }
	void XM_CALLCONV AddFilledRect(const DirectX::XMINT4 &rect) { AddFilledRect(DirectX::XMLoadSInt4(&rect)); }

	virtual HRESULT AddText(const Font& font, float x, float y, float scale, const std::string& strText, DWORD flag=0) = 0;

	inline HRESULT AddText(const Font& font, float x, float y, const std::string& strText, DWORD flag = 0)
	{
		return AddText(font, x, y, 1.f, strText, flag);
	}

	DirectX::XMFLOAT2 MeasureText(const FontData* const data, const std::string& strText, float scale = 1.f);
};

template <typename T>
class VertexBuffer
{
protected:
	typedef T Vertex;
	Vertex* m_data;
	int32_t m_maxVertices;
	int32_t m_numVertices;
	bool m_needResize, open;

	inline void FinalizeBegin() { open = true; }
	inline void InitializeEnd() { open = false; }
	inline bool IsOpen() const { return open; }
public:
	VertexBuffer(int32_t maxVertices = 128)
		: m_numVertices(0), m_maxVertices(maxVertices), m_data(nullptr), m_needResize(true), open(false) {}

	inline int32_t GetMaxVertices() const { return m_maxVertices; }
	inline int32_t GetNumVertices() const { return m_numVertices; }
	inline int32_t GetSafeVertices() const { return min(m_numVertices, m_maxVertices); }
	inline void SetNumVerticies(int32_t val) { m_maxVertices = val; }
	inline bool ResizeRequired() const { return m_needResize || GetMaxVertices() < GetNumVertices(); }

	inline void XM_CALLCONV Add(const Vertex& vertex)
	{
		if (!open)
			throw std::runtime_error("Vertex Buffer frame is not open!");

		if (!(GetMaxVertices() >= (GetNumVertices() + 1)))
			*m_data++ = vertex;
		++m_numVertices;
	}

	inline void XM_CALLCONV Add(const Vertex* vertices, int32_t len)
	{
		if (!open)
			throw std::runtime_error("Vertex Buffer frame is not open!");

		if (GetMaxVertices() >= GetNumVertices() + len)
		{
			memcpy(m_data, vertices, len * sizeof(vertices[0]));
			m_data += len;
		}
		m_numVertices += len;
	}

	inline void XM_CALLCONV Add(const std::vector<Vertex>& vertices)
	{
		if (!open)
			throw std::runtime_error("Vertex Buffer frame is not open!");

		if (GetMaxVertices() >= GetNumVertices() + vertices.size())
		{
			memcpy(m_data, vertices.data(), vertices.size() * sizeof(vertices[0]));
			m_data += vertices.size();
		}
		m_numVertices += vertices.size();
	}

	virtual void Begin() = 0;
	virtual void End() = 0;
};
void FPSCheck(std::string& str);