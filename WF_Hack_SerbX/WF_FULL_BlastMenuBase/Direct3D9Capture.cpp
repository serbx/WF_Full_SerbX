#include "stdafx.h"

Direct3D9Capture::Direct3D9Capture()
{
}

Direct3D9Capture::~Direct3D9Capture()
{
	FreeCapture();
}

HMODULE Direct3D9Capture::GetCaptureModule()
{
	static const std::string d3d9_dll = Capture::SYSTEM_PATH + "\\d3d9.dll";

	return GetModuleHandleA(d3d9_dll.c_str());
}

HRESULT Direct3D9Capture::TryCapture()
{
	HMODULE module = GetCaptureModule();

	if (module == nullptr)
	{
		return CO_E_DLLNOTFOUND;
	}
	HRESULT hr;

	ComPtr<IDirect3D9Ex> d3d9ex;
	hr = ModuleCall<HRESULT>(module, "Direct3DCreate9Ex", D3D_SDK_VERSION, &d3d9ex);
	if (FAILED(hr))
		return hr;

	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(pp));
	pp.Windowed = 1;
	pp.SwapEffect = D3DSWAPEFFECT_FLIP;
	pp.BackBufferFormat = D3DFMT_A8R8G8B8;
	pp.BackBufferCount = 1;
	pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	ComPtr<IDirect3DDevice9Ex> deviceEx;
	hr = d3d9ex->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, NULL, &deviceEx);
	if (FAILED(hr))
		return hr;

	uintptr_t* vtbl = *(uintptr_t**)deviceEx.Get();
	endscene_hook = FuncHook(vtbl[42], (uintptr_t)EndScene);
	reset_hook = FuncHook(vtbl[16], (uintptr_t)Reset);
	endscene_hook.hook();
	reset_hook.hook();
	return S_OK;
}

HRESULT Direct3D9Capture::FreeCapture()
{
	endscene_hook.unhook();
	reset_hook.unhook();
	return S_OK;
}

std::unique_ptr<Direct3D9Capture> Direct3D9Capture::capture;
Direct3D9Capture* Direct3D9Capture::singleton()
{
	if (capture == nullptr)
	{
		capture = std::unique_ptr<Direct3D9Capture>(new Direct3D9Capture());
	}
	return capture.get();
}

void Direct3D9Capture::destroy_singleton()
{
	Direct3D9Capture::capture.reset();
}
D3DVIEWPORT9 viewport;

void Direct3D9Capture::DoPresent(IDirect3DDevice9 *device)
{
	FPSCheck(framerate);
	
	std::shared_ptr<Renderer> renderer = nullptr;

	auto& result = renderers.find(device);
	if (result == renderers.cend())
	{
		renderers[device].reset(new Direct3D9Renderer{ ComPtr<IDirect3DDevice9>(device) });
		renderer = renderers[device];
	}
	else
	{
		renderer = result->second;
	}

	if (renderer)
	{
		device->GetViewport(&viewport);
		if (renderer->Begin(45))
		{
			g_menu.Render(renderer.get());

			
			ShowNMS(renderer.get());
			//ShowStateXX(renderer.get());

			renderer->End();
		}
		renderer->Present();
	}
}

void Direct3D9Capture::DoReset(IDirect3DDevice9 *device)
{
	auto& result = renderers.find(device);
	if (result != renderers.cend())
	{
		result->second.reset();
		renderers.erase(device);
	}
}

HRESULT WINAPI Direct3D9Capture::EndScene(IDirect3DDevice9 * device)
{
	Direct3D9Capture* capture = Direct3D9Capture::singleton();

	capture->DoPresent(device);
	
	return capture->endscene_hook.Call<HRESULT>(device);
}

HRESULT WINAPI Direct3D9Capture::Reset(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *params)
{
	Direct3D9Capture* capture = Direct3D9Capture::singleton();
	
	capture->DoReset(device);
	
	return capture->reset_hook.Call<HRESULT>(device, params);
}