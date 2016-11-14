#include "stdafx.h"
#include "DllHijack.h"


namespace tml
{
	namespace
	{
		typedef void* (WINAPI* Direct3DCreate9Type)(UINT SDKVersion);
		Direct3DCreate9Type RealDirect3DCreate9 = NULL;

#pragma comment(linker, "/EXPORT:Direct3DCreate9=_MyDirect3DCreate9@4")
		extern "C" void* WINAPI MyDirect3DCreate9(UINT SDKVersion)
		{
			return RealDirect3DCreate9(SDKVersion);
		}
	}


	DllHijack::DllHijack()
	{
		_RPTF0(_CRT_WARN, "DllHijack初始化\n");

		char DllPath[MAX_PATH];
		GetSystemDirectory(DllPath, sizeof(DllPath));
		strcat_s(DllPath, "\\d3d9.dll");

		m_d3d9Module = LoadLibrary(DllPath);
		RealDirect3DCreate9 = (Direct3DCreate9Type)GetProcAddress(m_d3d9Module, "Direct3DCreate9");
	}

	DllHijack::~DllHijack()
	{
		_RPTF0(_CRT_WARN, "DllHijack析构\n");

		if (m_d3d9Module == NULL)
			return;

		if (FreeLibrary(m_d3d9Module))
		{
			m_d3d9Module = NULL;
			RealDirect3DCreate9 = NULL;
		}
	}

	bool DllHijack::IsReady()
	{
		return RealDirect3DCreate9 != NULL;
	}
}

