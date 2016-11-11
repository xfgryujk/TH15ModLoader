#include "stdafx.h"
#include "DllHijack.h"


namespace tml
{
namespace DllHijack
{
	namespace
	{
		HMODULE g_d3d9Module = NULL;
		typedef void* (WINAPI* Direct3DCreate9Type)(UINT SDKVersion);
		Direct3DCreate9Type RealDirect3DCreate9 = NULL;
	}


	bool Init()
	{
		char DllPath[MAX_PATH];
		GetSystemDirectory(DllPath, sizeof(DllPath));
		strcat_s(DllPath, "\\d3d9.dll");

		g_d3d9Module = LoadLibrary(DllPath);
		RealDirect3DCreate9 = (Direct3DCreate9Type)GetProcAddress(g_d3d9Module, "Direct3DCreate9");
		
		return RealDirect3DCreate9 != NULL;
	}

	bool Uninit()
	{
		if (g_d3d9Module == NULL)
			return true;

		BOOL result = FreeLibrary(g_d3d9Module);
		if (result)
		{
			g_d3d9Module = NULL;
			RealDirect3DCreate9 = NULL;
		}
		return result != FALSE;
	}
}
}

#pragma comment(linker, "/EXPORT:Direct3DCreate9=_MyDirect3DCreate9@4")
extern "C" void* WINAPI MyDirect3DCreate9(UINT SDKVersion)
{
	return tml::DllHijack::RealDirect3DCreate9(SDKVersion);
}
