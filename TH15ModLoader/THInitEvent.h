// 和初始化有关的事件

#pragma once
#include "ModEvent.h"
#include "THType.h"


namespace tml
{
	enum THInitEvent
	{
		OnInit = 0,						// 程序刚开始，Struct1和另一个尺寸为0x1CAF97C的对象初始化完成后
		OnUninit,						// 程序准备结束，Struct1析构之前
		OnReadResource,					// 读取资源，可替换资源，事件类型为ReadResourceEvent
		OnInsertStruct2ToStruct1,		// 准备把某个Struct2插入Struct1，事件类型为Struct2Event，可取消
		OnInsertStruct2ToStruct1Ren,	// 准备把某个Struct2插入Struct1_Ren，事件类型为Struct2Event，可取消
		OnDeleteStruct2,				// 准备把某个Struct2从Struct1或Struct1_Ren断开和析构，事件类型为Struct2Event，可取消
		OnInitStage,					// 关卡初始化后，事件类型为StageEvent
		OnUninitStage,					// 准备析构关卡，事件类型为StageEvent
		OnInitUnit,						// 准备初始化和插入单位到Stage，事件类型为InitUnitEvent，可取消
		OnUninitUnit					// 单位死亡，准备从Stage断开和析构，事件类型为UnitEvent，可取消
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

	class TML_API Struct2Event : public EventBase
	{
	public:
		THAPI::Struct2& m_struct2;
		// 插入之前Struct2的order未初始化，应依赖这个判断
		const DWORD m_order;

		Struct2Event(THAPI::Struct2& struct2, DWORD order);
		virtual ~Struct2Event() = default;
	};

	class TML_API StageEvent final : public EventBase
	{
	public:
		THAPI::Stage& m_stage;

		StageEvent(THAPI::Stage& stage);
	};

	class TML_API InitUnitEvent final : public EventBase
	{
	public:
		// 单位执行的ECL函数名
		const LPCSTR m_eclFuncName;
		// 单位初始化用的参数
		void* const m_pArg;

		InitUnitEvent(LPCSTR eclFuncName, void* pArg);
	};

	class TML_API UnitEvent final : public EventBase
	{
	public:
		THAPI::Unit& m_unit;

		UnitEvent(THAPI::Unit& unit);
	};
}
