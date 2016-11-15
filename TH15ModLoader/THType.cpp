#include "stdafx.h"
#include "THType.h"


namespace THAPI
{
	TML_API IDirect3D9*& g_pD3D = *(IDirect3D9**)0x4E77D4;
	TML_API IDirect3DDevice9*& g_pDevice = *(IDirect3DDevice9**)0x4E77D8;

	TML_API HWND& g_hMainWnd = *(HWND*)(0x519BB0 + 0x0);
	TML_API HINSTANCE& g_hInstance = *(HINSTANCE*)(0x519BB0 + 0x4);

	TML_API Struct1*& g_pStruct1 = *(Struct1**)0x4E9A54;
	TML_API Stage*& g_pStage = *(Stage**)0x4E9A80;

	TML_API int& g_playerLife = *(int*)0x4E7450;
	TML_API int& g_playerBomb = *(int*)0x4E745C;
}
