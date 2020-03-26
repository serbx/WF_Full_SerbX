#pragma once
#include <memory>
#include <map>

enum DXGI_TYPE
{
	DIRECT3D10 = 0,
	DIRECT3D11,
	DIRECT3D12,
	DIRECT3D_UNKNOWN,
};
class Renderer;
class DXGICapture : public Capture
{
	static HRESULT WINAPI Present(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
	static HRESULT WINAPI Present1(IDXGISwapChain *swap, UINT syncInterval, UINT flags, const DXGI_PRESENT_PARAMETERS *pPresentParameters);
	static HRESULT WINAPI ResizeBuffer(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);
	static HRESULT WINAPI ResizeBuffer1(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags, const UINT *pCreationNodeMask, IUnknown *const *ppPresentQueue);
#if (WINVER >= 0x0A00 )   // DirectX 12 
	static HRESULT WINAPI ExecuteCommandLists(ID3D12CommandQueue *_this, UINT NumCommandLists, ID3D12CommandList *const *ppCommandLists);
#endif
protected:
	std::map<IDXGISwapChain*, std::shared_ptr<Renderer>> renderers;

	FuncHook present_hook;
	FuncHook present1_hook;
	FuncHook resizebuffer_hook;
	FuncHook resizebuffer1_hook;

#if (WINVER >= 0x0A00 )   
	ID3D12CommandQueue* lastCommandQueue;
	FuncHook executecommandlists_hook;
#endif

	DXGI_TYPE type;

	void DoPresent(IDXGISwapChain *swap);
	void DoResizeBuffer(IDXGISwapChain *swap);

	static std::unique_ptr<DXGICapture> capture;

	DXGICapture();
public:
	virtual ~DXGICapture();
	virtual HMODULE GetCaptureModule();
	virtual HRESULT TryCapture();
	virtual HRESULT FreeCapture();

	static DXGICapture* singleton();
	static void destroy_singleton();
};
