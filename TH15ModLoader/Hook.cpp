#include "stdafx.h"
#include "Hook.h"


namespace tml
{
	namespace
	{
#pragma pack(push)
#pragma pack(1)
		struct JmpCode
		{
		private:
			const BYTE jmpCode = 0xE9; // JMP指令的机器码，近跳转为E9，可跳至同一个段的范围内的地址
			uintptr_t relativeAddress; // 相对地址 = 目标地址 - 下一条指令地址 = 目标地址 - 当前地址 - JMP指令长度

		public:
			JmpCode(void* srcAddr, void* dstAddr)
			{
				setAddress(srcAddr, dstAddr);
			}

			void setAddress(void* srcAddr, void* dstAddr)
			{
				relativeAddress = (uintptr_t)dstAddr - (uintptr_t)srcAddr - sizeof(JmpCode); // JMP指令长度 = sizeof(JmpCode) = 5 (32位)
			}
		};
#pragma pack(pop)
	}

	InlineHook::InlineHook(void* originalFunction, void* hookFunction, SIZE_T hookLength, bool enable) :
		m_originalFunction(originalFunction),
		m_hookFunction(hookFunction),
		m_hookLength(hookLength),
		m_newEntry(VirtualAlloc(NULL, hookLength + sizeof(JmpCode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
	{
		if (m_newEntry == NULL)
			return;

		// 保存originalFunction开头的指令，originalFunction开头前hookLength字节是完整的指令，如果截断了会出错

		// 保存originalFunction开头前hookLength字节
		memcpy(m_newEntry, m_originalFunction, m_hookLength);
		// 从newEntry开头第hookLength + 1字节跳到originalFunction开头第hookLength + 1字节的指令
		JmpCode code((BYTE*)m_newEntry + hookLength, (BYTE*)originalFunction + hookLength);
		memcpy((BYTE*)m_newEntry + hookLength, &code, sizeof(code));

		if (enable)
			Enable();
	}

	InlineHook::~InlineHook()
	{
		Disable();

		// 释放newEntry
		if (m_newEntry != NULL)
			VirtualFree(m_newEntry, 0, MEM_RELEASE);
	}

	bool InlineHook::Enable()
	{
		if (m_newEntry == NULL)
			return false;
		if (m_isEnabled)
			return true;

		// 改变虚拟内存保护，使originalFunction开头可写，用WriteProcessMemory写内存的话好像不用改保护
		DWORD oldProtect, oldProtect2;
		if (!VirtualProtect(m_originalFunction, m_hookLength, PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		// 从originalFunction开头跳到hookFunction开头的指令
		JmpCode code(m_originalFunction, m_hookFunction);
		// 修改originalFunction开头的指令
		memcpy(m_originalFunction, &code, sizeof(code));

		// 恢复虚拟内存保护
		VirtualProtect(m_originalFunction, m_hookLength, oldProtect, &oldProtect2);

		m_isEnabled = true;
		return true;
	}

	bool InlineHook::Disable()
	{
		if (m_newEntry == NULL)
			return false;
		if (!m_isEnabled)
			return true;

		// 改变虚拟内存保护，使originalFunction开头可写
		DWORD oldProtect, oldProtect2;
		if (!VirtualProtect(m_originalFunction, m_hookLength, PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		// 恢复originalFunction开头的指令
		memcpy(m_originalFunction, m_newEntry, m_hookLength);

		// 恢复虚拟内存保护
		VirtualProtect(m_originalFunction, m_hookLength, oldProtect, &oldProtect2);

		m_isEnabled = false;
		return true;
	}

	bool InlineHook::IsEnabled()
	{
		return m_isEnabled;
	}
}
