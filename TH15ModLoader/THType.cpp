#include "stdafx.h"
#include "THType.h"


namespace THAPI
{
	TML_API IDirect3D9*& g_pD3D = *(IDirect3D9**)0x4E77D4;
	TML_API IDirect3DDevice9*& g_pDevice = *(IDirect3DDevice9**)0x4E77D8;
	TML_API IDirectInputA*& g_pDInput = *(IDirectInputA**)0x4E77DC;
	TML_API IDirectInputDeviceA*& g_pDInputDevice = *(IDirectInputDeviceA**)0x44E77F0;

	TML_API HWND& g_hMainWnd = *(HWND*)0x519BB0;
	TML_API HINSTANCE& g_hInstance = *(HINSTANCE*)0x519BB4;

	TML_API Struct1*& g_pStruct1 = *(Struct1**)0x4E9A54;
	TML_API Stage*& g_pStage = *(Stage**)0x4E9A80;

	TML_API float& g_ticksPerFrame = *(float*)0x4E73E8;

	TML_API DWORD& g_playerInputFlag = *(DWORD*)0x4E6D10;
	TML_API int& g_playerLife = *(int*)0x4E7450;
	TML_API int& g_playerBomb = *(int*)0x4E745C;
}
