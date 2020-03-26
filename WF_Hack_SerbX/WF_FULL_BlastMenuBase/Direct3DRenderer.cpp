#include "stdafx.h"

const Font DEFAULT_FONT;

#pragma region Shaders
const char shader[] = {
	"struct PSInput																		 \n"
	"{																					 \n"
	"	float4 position : SV_POSITION;													 \n"
	"	float4 col : COLOR;																 \n"
	"	float2 uv : TEXCOORD;															 \n"
	"};																					 \n"
	"Texture2D g_texture : register(t0);												 \n"
	"SamplerState g_sampler : register(s1);												 \n"
	"PSInput VSMain(float4 position : POSITION, float4 col : COLOR, float4 uv : TEXCOORD)\n"
	"{																					 \n"
	"	PSInput result;																	 \n"
	"																					 \n"
	"	result.position = position;														 \n"
	"	result.col = float4(col.z, col.y, col.x, col.w);								 \n"
	"	result.uv = uv;																	 \n"
	"																					 \n"
	"	return result;																	 \n"
	"}																					 \n"
	"																					 \n"
	"float4 PSMain(PSInput input) : SV_TARGET											 \n"
	"{																					 \n"
	"	if(input.uv.x == 0 && input.uv.y == 0)											 \n"
	"		return input.col;															 \n"
	"	return g_texture.Sample(g_sampler, input.uv) * input.col;						 \n"
	"}																					 \n"
};

inline HMODULE FindD3DXModule(const TCHAR* dllName) noexcept
{
	TCHAR dll_file[MAX_PATH];
	HMODULE module = nullptr;
	for (int i = D3D_SDK_VERSION + D3D10_SDK_LAYERS_VERSION; i >= 29; i--)
	{
		wsprintf(dll_file, TEXT("%s_%d.dll"), dllName, i);
		if (module = GetModuleHandle(dll_file))
		{
			return module;
		}
	}
	return nullptr;
}

inline HMODULE FindOrLoadD3DXModule(const TCHAR* dllName) noexcept
{
	TCHAR dll_file[MAX_PATH];
	HMODULE module = nullptr;
	for (int i = D3D_SDK_VERSION + D3D10_SDK_LAYERS_VERSION; i >= 29; i--)
	{
		wsprintf(dll_file, TEXT("%s_%d.dll"), dllName, i);
		if (module = GetModuleHandle(dll_file))
		{
			return module;
		}
	}

	for (int i = D3D_SDK_VERSION + D3D10_SDK_LAYERS_VERSION; i >= 29; i--)
	{
		wsprintf(dll_file, TEXT("%s_%d.dll"), dllName, i);
		if (module = LoadLibrary(dll_file))
		{
			return module;
		}
	}
	return nullptr;
}

inline HMODULE FindOrLoadD3DCompilerModule() noexcept
{
	TCHAR dll_file[MAX_PATH];
	HMODULE module = nullptr;
	for (int i = 47; i >= 43; i--)
	{
		wsprintf(dll_file, TEXT("d3dcompiler_%d.dll"), i);
		if (module = GetModuleHandle(dll_file))
		{
			return module;
		}
	}

	for (int i = 47; i >= 43; i--)
	{
		wsprintf(dll_file, TEXT("d3dcompiler_%d.dll"), i);
		if (module = LoadLibrary(dll_file))
		{
			return module;
		}
	}
	return nullptr;
}

inline HRESULT WINAPI D3DCompileDx(LPCVOID pSrcData,
	SIZE_T SrcDataSize,
	LPCSTR pSourceName,
	CONST D3D_SHADER_MACRO* pDefines,
	ID3DInclude* pInclude,
	LPCSTR pEntrypoint,
	LPCSTR pTarget,
	UINT Flags1,
	UINT Flags2,
	ID3DBlob** ppCode,
	ID3DBlob** ppErrorMsgs)
{
	HMODULE module = FindOrLoadD3DCompilerModule();
	if (module == nullptr)
	{
		throw Instalog::SystemFacades::Win32Exception::FromLastError();
		return E_FAIL;
	}

	return ModuleCall<HRESULT>(module, "D3DCompile", pSrcData, SrcDataSize, pSourceName, pDefines, pInclude, pEntrypoint, pTarget,
		Flags1, Flags2, ppCode, ppErrorMsgs);
}

inline HRESULT WINAPI D3DX11CompileFromMemory(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
	LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2, void* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, HRESULT* pHResult)
{
	HMODULE module = FindOrLoadD3DXModule(TEXT("d3dx11"));
	if (module == nullptr)
	{
		HRESULT hr = D3DCompileDx(pSrcData, SrcDataLen, pFileName, pDefines, pInclude, pFunctionName, pProfile, Flags1, Flags2, ppShader, ppErrorMsgs);
		if (pHResult)
			*pHResult = hr;

		return hr;
	}

	return ModuleCall<HRESULT>(module, "D3DX11CompileFromMemory", pSrcData, SrcDataLen, pFileName, pDefines, pInclude, pFunctionName, pProfile, Flags1, Flags2, pPump, ppShader, ppErrorMsgs, pHResult);
}

inline HRESULT WINAPI D3DX10CompileFromMemory(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
	LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2, void* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, HRESULT* pHResult)
{
	HMODULE module = FindOrLoadD3DXModule(TEXT("d3dx10"));
	if (module == nullptr)
	{
		HRESULT hr = D3DCompileDx(pSrcData, SrcDataLen, pFileName, pDefines, pInclude, pFunctionName, pProfile, Flags1, Flags2, ppShader, ppErrorMsgs);
		if (pHResult)
			*pHResult = hr;

		return hr;
	}

	return ModuleCall<HRESULT>(module, "D3DX10CompileFromMemory", pSrcData, SrcDataLen, pFileName, pDefines, pInclude, pFunctionName, pProfile, Flags1, Flags2, pPump, ppShader, ppErrorMsgs, pHResult);
}

HRESULT D3D12CompileShaderFromMemory(const char* szdata, SIZE_T len, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> pErrorBlob;

	hr = D3DCompileDx(szdata, len, NULL, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

	return hr;
}

HRESULT D3D11CompileShaderFromMemory(const char* szdata, SIZE_T len, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> pErrorBlob;

	hr = D3DX11CompileFromMemory(szdata, len, NULL, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);

	if (FAILED(hr))
	{
		return hr;
	}

	return D3D_OK;
}

HRESULT D3D10CompileShaderFromMemory(const char* szdata, SIZE_T len, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> pErrorBlob;

	hr = D3DX10CompileFromMemory(szdata, len, NULL, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);

	return hr;
}

#pragma endregion
#if (WINVER >= 0x0A00 )   
HRESULT WINAPI D3D12LibSerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC *pRootSignature, D3D_ROOT_SIGNATURE_VERSION Version, ID3DBlob **ppBlob, ID3DBlob **ppErrorBlob)
{
	HMODULE module = GetModuleHandle(TEXT("d3d12.dll"));
	if (module == nullptr)
	{
		throw Instalog::SystemFacades::Win32Exception::FromLastError();
	}

	return ModuleCall<HRESULT>(module, "D3D12SerializeRootSignature", pRootSignature, Version, ppBlob, ppErrorBlob);
}
#endif