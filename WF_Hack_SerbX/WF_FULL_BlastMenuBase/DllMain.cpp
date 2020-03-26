#include "stdafx.h"
#include "VirtualHook.h"
#include "Classes.h"
#include "pe_earser.h"
//#include "Menu.h"

FuncHook hook;

DWORD WINAPI InitThread(LPVOID);
std::atomic_bool running;
uintptr_t initThreadHandle;
HMODULE g_hModule;

using UpdateHook = bool(__stdcall*)(ISystem*, int, int);
UpdateHook pUpdate;
VMTHookManager* vmtUpdate = new VMTHookManager;
//FuncHook pUpdate;

void OnExit() noexcept;

 bool __stdcall Update(ISystem* pSystem, int updateFlags, int nPauseMode)
{
	RunChees();
	return pUpdate(pSystem, updateFlags, nPauseMode);
}
SSGE* pxSSGE = pxSSGE->GetSSGE();



BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	//NewNtFlushInstructionCache = (RealNtFlushInstructionCache)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtFlushInstructionCache");

	int m_iSeed;
    DisableThreadLibraryCalls(hModule);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		
		//Erase_PE_Header((DWORD)GetModuleHandleA("SX.dll"));
		//Erase_PEB_Module("SX.dll");
		//InitializeMutation();
		g_hModule = hModule;

		//uintptr_t* vtbl = (uintptr_t*)pxSSGE->getSystem();
		//pUpdate = FuncHook(vtbl[4], (uintptr_t)Update);

		std::atexit(OnExit);
		initThreadHandle = _beginthreadex(nullptr, 0, (_beginthreadex_proc_type)InitThread, hModule, 0, nullptr);
		_beginthreadex(0, 0, (_beginthreadex_proc_type)AKK, 0, 0, 0);
		_beginthreadex(0, 0, (_beginthreadex_proc_type)ara, 0, 0, 0);
		//_beginthreadex(0, 0, (_beginthreadex_proc_type)F_WSN, 0, 0, 0);

        vmtUpdate->bInitialize((PDWORD64*)pxSSGE->getSystem());
	    pUpdate = (UpdateHook)vmtUpdate->dwHookMethod((DWORD64)Update, 4);

		FreeLibrary(g_hModule);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		OnExit();
	}

	return TRUE;
}

Capture* capture = nullptr;
extern DWORD WINAPI InitThread(LPVOID module);
__declspec(safebuffers) DWORD WINAPI InitThread(LPVOID module)
{
	//Sleep(18000);
	int count = 0;
	running = true;
	HMODULE hModule = (HMODULE)module;

	

	Capture* captures[] = {
		Direct3D9Capture::singleton(),
		DXGICapture::singleton(),
	};

	do
	{
		capture = captures[count++ % _countof(captures)];
	} while (capture->GetCaptureModule() == nullptr && running);

	if (!running)
		return 0;
	HRESULT hr = S_OK;
	do
	{
		Sleep(100);
		hr = capture->TryCapture();
	} while (FAILED(hr));
	if (FAILED(hr))
	{
		Instalog::SystemFacades::ThrowFromHResult(hr);
		return 0;
	}

	BuildMenu();

	while (running)
	{
		RunStuff();
		Sleep(50);
	}

	capture->FreeCapture();

	return 0;
}

void OnExit() noexcept
{
	if (running)
	{
		running = false;
		WaitForSingleObject((HANDLE)initThreadHandle, INFINITE);
		CloseHandle((HANDLE)initThreadHandle);

		try
		{
			Direct3D9Capture::destroy_singleton();
			DXGICapture::destroy_singleton();
		}
		catch (std::exception&)
		{

		}
	}
}