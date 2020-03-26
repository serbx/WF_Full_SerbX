#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <array>
#include <string>
#include <locale>
#include <codecvt>
#include <chrono>
#include <future>
#include <unordered_map>
#include <stdint.h>
#include <stdexcept>
#include <wrl/client.h>

#include <Windows.h>
#include <gdiplus.h>

#pragma comment (lib, "gdiplus.lib")

#include <ntverp.h>

#if (WINVER >= 0x0A00 )          
#include <d3d12.h>
#include <dxgi1_4.h>
#include "d3dx12.h"
#endif
#include <d3d11.h>
#include <d3d10.h>
#include <d3d9.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using namespace Microsoft::WRL;

#include "Win32Exception.h"
#include "FuncHook.h"
#include "Capture.h"
#include "DXGICapture.h"
#include "Direct3D9Capture.h"
#include "Renderer.h"
#include "Direct3DRenderer.h"
#include "Direct3D9Renderer.h"
#include "Direct3D10Renderer.h"
#include "Direct3D11Renderer.h"
#if (WINVER >= 0x0A00 )      
#include "Direct3D12Renderer.h"
#endif

#include "Menu.h"
#include "Main.h"
#include "Classes.h"

//#include "c/VirtualizerSDK.h"
//#pragma comment(lib, "C/Lib/COFF/VirtualizerSDK64.lib")



extern HMODULE g_hModule;

extern D3DVIEWPORT9 viewport;



