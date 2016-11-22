#include "stdafx.h"
#include "THType.h"


namespace THAPI
{
#define DEF_TH_VARIABLE_T(type, name, address) TML_API decltype(name) name = *(type*)address
#define DEF_TH_VARIABLE(name, address) DEF_TH_VARIABLE_T(decltype(name + 0), name, address)


	DEF_TH_VARIABLE_T(IDirect3D9*, g_pD3D, 0x4E77D4);
	DEF_TH_VARIABLE_T(IDirect3DDevice9*, g_pD3DDevice, 0x4E77D8);
	DEF_TH_VARIABLE_T(IDirectInputA*, g_pDInput, 0x4E77DC);
	DEF_TH_VARIABLE_T(IDirectInputDeviceA*, g_pDInputDevice, 0x44E77F0);

	DEF_TH_VARIABLE(g_hMainWnd, 0x519BB0);
	DEF_TH_VARIABLE(g_hInstance, 0x519BB4);

	DEF_TH_VARIABLE(g_pStruct1, 0x4E9A54);
	DEF_TH_VARIABLE(g_pStage, 0x4E9A80);

	DEF_TH_VARIABLE(g_ticksPerFrame, 0x4E73E8);

	DEF_TH_VARIABLE(g_pressedButton, 0x4E6D10);
	DEF_TH_VARIABLE(g_lastPressedButton, 0x4E6D14);
	DEF_TH_VARIABLE(g_longPressedButton, 0x4E6D18);
	DEF_TH_VARIABLE(g_newPressedButton, 0x4E6D1C);
	DEF_TH_VARIABLE(g_newReleasedButton, 0x4E6D20);

	DEF_TH_VARIABLE(g_playerScore, 0x4E740C);
	DEF_TH_VARIABLE(g_playerMaxGetPoint, 0x4E7434);
	DEF_TH_VARIABLE(g_playerLife, 0x4E7450);
	DEF_TH_VARIABLE(g_playerLifePiece, 0x4E7454);
	DEF_TH_VARIABLE(g_playerBomb, 0x4E745C);
	DEF_TH_VARIABLE(g_playerBombPiece, 0x4E7460);
	DEF_TH_VARIABLE(g_playerPower, 0x4E7440);
}
