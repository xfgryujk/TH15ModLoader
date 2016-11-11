#include "stdafx.h"
#include "Hook.h"


namespace Hook
{
#pragma pack(push)
#pragma pack(1)
	struct JmpCode
	{
	private:
		const BYTE jmpCode = 0xE9; // JMP指令的机器码，近跳转为E9，可跳至同一个段的范围内的地址
		DWORD relativeAddress; // 相对地址 = 目标地址 - 下一条指令地址 = 目标地址 - 当前地址 - JMP指令长度

	public:
		JmpCode(DWORD srcAddr, DWORD dstAddr)
		{
			setAddress(srcAddr, dstAddr);
		}

		void setAddress(DWORD srcAddr, DWORD dstAddr)
		{
			relativeAddress = dstAddr - srcAddr - 5; // JMP指令长度 = sizeof(JmpCode) = 5
		}
	};
#pragma pack(pop)


	bool HookInlineHook(void* originalFunction, void* hookFunction, void** newFunction, SIZE_T hookLength)
	{
		// 改变虚拟内存保护，使originalFunction开头可写，用WriteProcessMemory写内存的话好像不用改保护
		DWORD oldProtect, oldProtect2;
		if (!VirtualProtect(originalFunction, hookLength, PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		// 保存originalFunction开头的指令，originalFunction开头前hookLength字节是完整的指令，如果截断了会出错
		BYTE* newFunctionCode = (BYTE*)VirtualAlloc(NULL, hookLength + sizeof(JmpCode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (newFunctionCode == NULL)
		{
			VirtualProtect(originalFunction, hookLength, oldProtect, &oldProtect2);
			return false;
		}
		// 保存originalFunction开头前hookLength字节
		memcpy(newFunctionCode, originalFunction, hookLength);
		// 从newFunctionCode开头第hookLength + 1字节跳到originalFunction开头第hookLength + 1字节的指令
		JmpCode code((DWORD)newFunctionCode + hookLength, (DWORD)originalFunction + hookLength);
		memcpy(newFunctionCode + hookLength, &code, sizeof(code));
		*(BYTE**)newFunction = newFunctionCode;

		// 从originalFunction开头跳到hookFunction开头的指令
		code.setAddress((DWORD)originalFunction, (DWORD)hookFunction);
		// 修改originalFunction开头的指令
		memcpy(originalFunction, &code, sizeof(code));

		// 恢复虚拟内存保护
		VirtualProtect(originalFunction, hookLength, oldProtect, &oldProtect2);

		return true;
	}

	bool UnhookInlineHook(void* originalFunction, void** newFunction, SIZE_T hookLength)
	{
		if (*newFunction == NULL)
			return true;

		// 改变虚拟内存保护，使originalFunction开头可写
		DWORD oldProtect, oldProtect2;
		if (!VirtualProtect(originalFunction, hookLength, PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;
		// 恢复originalFunction开头的指令
		memcpy(originalFunction, *(BYTE**)newFunction, hookLength);
		// 恢复虚拟内存保护
		VirtualProtect(originalFunction, hookLength, oldProtect, &oldProtect2);

		// 释放newFunction
		VirtualFree(*(void**)newFunction, 0, MEM_RELEASE);
		*(void**)newFunction = NULL;

		return true;
	}
}
