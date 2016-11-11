// 和初始化有关的事件

#pragma once
#include "ModEvent.h"


namespace tml
{
	enum THInitEvent
	{
		OnInit = 0,				// 程序刚开始，Struct2插入Struct1前
		OnUninit,				// 程序准备结束，Struct1析构之前
		OnReadResource			// 读取资源，可替换资源，事件类型为ReadResourceEvent
	};

	class TML_API ReadResourceEvent final : public EventBase
	{
	public:
		const LPCSTR m_resName;
	private:
		// 设置为NULL则使用默认的资源，修改前要用THAPI::free释放之前的buffer，新的buffer要用THAPI::malloc分配
		void*& m_resBuffer;
		// 如果不为NULL要设置为buffer的尺寸
		DWORD*& m_pnBytesRead;

	public:
		ReadResourceEvent(LPCSTR resName, void*& resBuffer, DWORD*& pnBytesRead);
		void* GetResBuffer() { return m_resBuffer; }
		// 复制buffer内容到新的m_resBuffer，buffer请自行释放
		void SetResBuffer(void* buffer, size_t size);
	};


namespace THInit
{
	bool Init();
	bool Uninit();
}
}
