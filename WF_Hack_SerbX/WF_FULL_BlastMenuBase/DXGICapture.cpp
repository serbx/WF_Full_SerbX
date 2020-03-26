#include "stdafx.h"

HRESULT Direct3D10SwapChain(HMODULE d3d10_dll, HWND hWnd, ComPtr<IDXGISwapChain>& swap_chain, ComPtr<ID3D10Device>& device);
HRESULT Direct3D11SwapChain(HMODULE d3d11_dll, HWND hWnd, ComPtr<IDXGISwapChain>& swap_chain, ComPtr<ID3D11DeviceContext>& context);
#if (WINVER >= 0x0A00 )   
HRESULT Direct3D12SwapChain(HMODULE d3d12_dll, HWND hWnd, ComPtr<IDXGISwapChain>& swapchain, ComPtr<ID3D12CommandQueue>& commandqueue, ComPtr<ID3D12GraphicsCommandList>& commandlist);
#endif
LRESULT CALLBACK DLLWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

DXGICapture::DXGICapture()
{
#if (WINVER >= 0x0A00 )  
	lastCommandQueue = nullptr;
#endif
}

DXGICapture::~DXGICapture()
{
	FreeCapture();
}

HMODULE DXGICapture::GetCaptureModule()
{
	static const std::string d3d12_dll = Capture::SYSTEM_PATH + "\\d3d12.dll";
	static const std::string d3d11_dll = Capture::SYSTEM_PATH + "\\d3d11.dll";
	static const std::string d3d10_dll = Capture::SYSTEM_PATH + "\\d3d10.dll";

	static const std::string dlls[] = {
		d3d12_dll,
		d3d11_dll,
		d3d10_dll,
	};

	int count = _countof(dlls) - 1;
	HMODULE module;

	for (size_t i = 0; i < _countof(dlls); i++)
	{
		const auto& dll = dlls[i];
		module = GetModuleHandleA(dll.c_str());
		if (module != nullptr)
			break;
		else
			--count;
	}

	if (count < 0)
		type = DXGI_TYPE::DIRECT3D_UNKNOWN;
	else
	type = (DXGI_TYPE) count;
	return module;
}

HRESULT DXGICapture::TryCapture()
{
	HMODULE module = GetCaptureModule();

	if (module == nullptr)
	{
		// Throw Error, Module not could, cannot capture
		Instalog::SystemFacades::Win32Exception::ThrowFromLastError();
		return CO_E_DLLNOTFOUND;
	}
	HRESULT hr;

	ComPtr<IDXGISwapChain> swap_chain;
#if (WINVER >= 0x0A00 )    
	ComPtr<ID3D12GraphicsCommandList> command_list;
	ComPtr<ID3D12CommandQueue> command_queue;
#endif
	// Create window
	HWND hWnd;
	WNDCLASSEX wc;
	static const LPTSTR TITLE = TEXT("BlastMenuV3HookWindow");
	{
		wc =
		{
			sizeof(WNDCLASSEX), CS_CLASSDC, DLLWindowProc, 0L, 0L,
			g_hModule, NULL, NULL, NULL, NULL,
			TITLE, NULL
		};
		if (!RegisterClassEx(&wc))
			return E_FAIL;
		hWnd = CreateWindow(TITLE, TITLE,
			WS_OVERLAPPEDWINDOW, 100, 100, 1024, 768,
			NULL, NULL, wc.hInstance, NULL);
	}
	switch (type)
	{
	case DIRECT3D10:
	{
		ComPtr<ID3D10Device> device;
		hr =  Direct3D10SwapChain(module, hWnd, swap_chain, device);
		if (FAILED(hr))
		{
			goto destroy_window;

		}
		// May hook Device's DrawIndex here :P if you want
		break;
	}
	case DIRECT3D11:
	{
		ComPtr<ID3D11DeviceContext> device_context;
		hr =  Direct3D11SwapChain(module, hWnd, swap_chain, device_context);
		if (FAILED(hr))
		{
			goto destroy_window;
		}
		// May hook DeviceContext's DrawIndex here :P if you want
		break;
	}
#if (WINVER >= 0x0A00 )   
	case DIRECT3D12:
	{
		hr = Direct3D12SwapChain(module, hWnd, swap_chain, command_queue, command_list);
		if (FAILED(hr))
		{
			goto destroy_window;
		}

		uintptr_t* vtable = *(uintptr_t**)command_queue.Get();
		executecommandlists_hook = FuncHook(vtable[10], (uintptr_t)ExecuteCommandLists);
		executecommandlists_hook.hook();
		// May hook DeviceContext's DrawIndex here :P if you want
		break;
	}
#endif
	case DIRECT3D_UNKNOWN:
	default:
		throw std::runtime_error("Unknown DXGI Device! Contact Topblast for more information.");
	}

	uintptr_t *vtable = *(uintptr_t**)swap_chain.Get();
	present_hook = FuncHook(vtable[8], (uintptr_t)Present);
	resizebuffer_hook = FuncHook(vtable[13], (uintptr_t)ResizeBuffer);
	{
		ComPtr<IDXGISwapChain1> swap_chain1;
		if (SUCCEEDED(swap_chain->QueryInterface(IID_PPV_ARGS(&swap_chain1))))
		{
			vtable = *(uintptr_t**)swap_chain1.Get();
			present1_hook = FuncHook(vtable[22], (uintptr_t)Present1);
		}
	}
	{
		ComPtr<IDXGISwapChain3> swap_chain3;
		if (SUCCEEDED(swap_chain->QueryInterface(IID_PPV_ARGS(&swap_chain3))))
		{
			vtable = *(uintptr_t**)swap_chain3.Get();
			resizebuffer1_hook = FuncHook(vtable[39], (uintptr_t)ResizeBuffer1);
		}
	}

	present_hook.hook();
	resizebuffer_hook.hook();
destroy_window:
	DestroyWindow(hWnd);
	UnregisterClass(TITLE, wc.hInstance);
	return hr;
}

HRESULT DXGICapture::FreeCapture()
{
	present_hook.unhook();
	present1_hook.unhook();
	resizebuffer_hook.unhook();
	resizebuffer1_hook.unhook();
#if (WINVER >= 0x0A00 )   
	executecommandlists_hook.unhook();
#endif
	return S_OK;
}

std::unique_ptr<DXGICapture> DXGICapture::capture;
DXGICapture* DXGICapture::singleton()
{
	if (capture == nullptr)
	{
		capture = std::unique_ptr<DXGICapture>(new DXGICapture());
	}
	return capture.get();
}

void DXGICapture::destroy_singleton()
{
	DXGICapture::capture.reset();
}

void DXGICapture::DoPresent(IDXGISwapChain *swap)
{
	FPSCheck(framerate);

	auto& result = renderers.find(swap);
	if (result == renderers.cend())
	{
		do
		{
			// D3D 10
			{
				ComPtr<ID3D10Device> device;
				if (SUCCEEDED(swap->GetDevice(IID_PPV_ARGS(&device))))
				{
					renderers[swap].reset(new Direct3D10Renderer{ std::move(device) });
					break;
				}
			}
			// D3D 11
			{
				ComPtr<ID3D11Device> device;
				if (SUCCEEDED(swap->GetDevice(IID_PPV_ARGS(&device))))
				{
					renderers[swap].reset(new Direct3D11Renderer{ std::move(device) });
					break;
				}
			}
#if (WINVER >= 0x0A00 )   
			// D3D 12
			{
				ComPtr<ID3D12Device> device;
				ComPtr<IDXGISwapChain3> swap_chain;
				if (lastCommandQueue != nullptr
					&& SUCCEEDED(swap->GetDevice(IID_PPV_ARGS(&device)))
					&& SUCCEEDED(swap->QueryInterface(IID_PPV_ARGS(&swap_chain))))
				{
					renderers[swap].reset(new Direct3D12Renderer{ std::move(swap_chain), ComPtr<ID3D12CommandQueue>(lastCommandQueue) });
					break;
				}
			}
#endif
		} while (false);

		result = renderers.find(swap);
	}

	if (result != renderers.cend())
	{
		auto& renderer = (*result).second;
		if (renderer->Begin())
		{
			g_menu.Render(renderer.get());

			renderer->End();
		}
		renderer->Present();
	}
}

void DXGICapture::DoResizeBuffer(IDXGISwapChain *swap)
{
	auto& result = renderers.find(swap);
	if (result != renderers.cend())
	{
		result->second.reset();
		renderers.erase(swap);
	}
}

HRESULT WINAPI DXGICapture::Present(IDXGISwapChain *swap, UINT syncInterval, UINT flags)
{
	DXGICapture* capture = DXGICapture::singleton();

	capture->DoPresent(swap);

	return capture->present_hook.Call<HRESULT>(swap, syncInterval, flags);
}

HRESULT WINAPI DXGICapture::Present1(IDXGISwapChain *swap, UINT syncInterval, UINT flags, const DXGI_PRESENT_PARAMETERS *pPresentParameters)
{
	DXGICapture* capture = DXGICapture::singleton();

	capture->DoPresent(swap);

	return capture->present1_hook.Call<HRESULT>(swap, syncInterval, flags, pPresentParameters);
}

HRESULT WINAPI DXGICapture::ResizeBuffer(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags)
{
	DXGICapture* capture = DXGICapture::singleton();

	capture->DoResizeBuffer(swap);

	return DXGICapture::singleton()->resizebuffer_hook.Call<HRESULT>(swap, bufferCount, width, height, giFormat, flags);
}

HRESULT WINAPI DXGICapture::ResizeBuffer1(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags, const UINT *pCreationNodeMask, IUnknown *const *ppPresentQueue)
{
	DXGICapture* capture = DXGICapture::singleton();

	capture->DoResizeBuffer(swap);

	return DXGICapture::singleton()->resizebuffer1_hook.Call<HRESULT>(swap, bufferCount, width, height, giFormat, flags, pCreationNodeMask, ppPresentQueue);
}

#if (WINVER >= 0x0A00 )   
HRESULT WINAPI DXGICapture::ExecuteCommandLists(ID3D12CommandQueue *_this, UINT NumCommandLists, ID3D12CommandList *const *ppCommandLists)
{
	DXGICapture* capture = DXGICapture::singleton();
	/*
        D3D12_COMMAND_LIST_TYPE_DIRECT	= 0,
        D3D12_COMMAND_LIST_TYPE_BUNDLE	= 1,
        D3D12_COMMAND_LIST_TYPE_COMPUTE	= 2,
        D3D12_COMMAND_LIST_TYPE_COPY	= 3,
        D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE	= 4,
        D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS	= 5
		*/
	if (_this->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
		capture->lastCommandQueue = _this;
	HRESULT hr = capture->executecommandlists_hook.Call<HRESULT>(_this, NumCommandLists, ppCommandLists);

	return hr;
}
#endif


HRESULT Direct3D10SwapChain(HMODULE d3d10_dll, HWND hWnd, ComPtr<IDXGISwapChain>& swap_chain, ComPtr<ID3D10Device>& device)
{
	if (d3d10_dll == nullptr)
	{
		throw std::runtime_error("Invalid 'd3d10.dll' module!");
	}
		
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.Width = 2;
	swapDesc.BufferDesc.Height = 2;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hWnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.Windowed = TRUE;

	HRESULT hr = ModuleCall<HRESULT>(d3d10_dll, "D3D10CreateDeviceAndSwapChain", nullptr, D3D10_DRIVER_TYPE_NULL, nullptr, 0, D3D10_SDK_VERSION, &swapDesc, &swap_chain, &device);

	return hr;
}

HRESULT Direct3D11SwapChain(HMODULE d3d11_dll, HWND hWnd, ComPtr<IDXGISwapChain>& swap_chain, ComPtr<ID3D11DeviceContext>& context)
{
	if (d3d11_dll == nullptr)
	{
		throw std::runtime_error("Invalid 'd3d11.dll' module!");
	}

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.Width = 2;
	swapDesc.BufferDesc.Height = 2;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hWnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.Windowed = TRUE;

	D3D_FEATURE_LEVEL desiredLevels[7] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	D3D_FEATURE_LEVEL receivedLevel;

	ComPtr<ID3D10Device> device;
	
	HRESULT hr = ModuleCall<HRESULT>(d3d11_dll, "D3D11CreateDeviceAndSwapChain", nullptr, D3D_DRIVER_TYPE_NULL, nullptr, 0, desiredLevels, 7, D3D11_SDK_VERSION, &swapDesc, &swap_chain, &device, &receivedLevel, &context);

	return hr;
}

#if (WINVER >= 0x0A00 )   
HRESULT Direct3D12SwapChain(HMODULE d3d12_dll, HWND hWnd, ComPtr<IDXGISwapChain>& swapchain, ComPtr<ID3D12CommandQueue>& commandqueue, ComPtr<ID3D12GraphicsCommandList>& commandlist)
{
	if (d3d12_dll == nullptr)
	{
		throw std::runtime_error("Invalid 'd3d12.dll' module!");
	}
	HRESULT hr;

	ComPtr<IDXGIFactory4> factory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	if (FAILED(hr))
		return hr;

	ComPtr<IDXGIAdapter3> warpAdapter;
	hr = factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
	if (FAILED(hr))
		return hr;

	ComPtr<ID3D12Device> device;
	PFN_D3D12_CREATE_DEVICE pcreate = (PFN_D3D12_CREATE_DEVICE)GetProcAddress(d3d12_dll, "D3D12CreateDevice");
	if (pcreate == nullptr)
		throw Instalog::SystemFacades::Win32Exception::FromLastError();
	hr = pcreate(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	if (FAILED(hr))
		return hr;

	ComPtr<ID3D12CommandQueue> command_queue;
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&command_queue));
	if (FAILED(hr))
		return hr;

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.Width = 2;
	swapDesc.BufferDesc.Height = 2;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.OutputWindow = hWnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.Windowed = TRUE;

	ComPtr<IDXGISwapChain> swap_chain;
	ComPtr<ID3D12GraphicsCommandList> command_list;
	ComPtr<ID3D12CommandAllocator> command_allocator;

	hr = factory->CreateSwapChain(command_queue.Get(), &swapDesc, &swap_chain);
	if (FAILED(hr))
		return hr;

	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator));
	if (FAILED(hr))
		return hr;

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator.Get(), 0, IID_PPV_ARGS(&command_list));
	if (FAILED(hr))
		return hr;

	swapchain = swap_chain.Detach();
	commandlist = command_list.Detach();
	commandqueue = command_queue.Detach();

	return hr;
}
#endif
LRESULT CALLBACK DLLWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}