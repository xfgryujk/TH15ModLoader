// 和初始化有关的事件

#pragma once
#include "ModEvent.h"
#include "THType.h"


namespace tml
{
	enum THInitEvent
	{
		OnInit = 0,						// 程序刚开始，ModulesLinks和另一个尺寸为0x1CAF97C的对象初始化完成后
		OnUninit,						// 程序准备结束，ModulesLinks析构之前
		OnReadResource,					// 读取资源，可替换资源，事件类型为ReadResourceEvent
		OnInsertModuleToLogicLink,		// 准备把某个Module插入逻辑链，事件类型为ModuleEvent，可取消
		OnInsertModuleToRenderLink,		// 准备把某个Module插入渲染链，事件类型为ModuleEvent，可取消
		OnDeleteModule,					// 准备把某个Module从逻辑链或渲染链断开和析构，事件类型为ModuleEvent，可取消
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

	class TML_API ModuleEvent : public EventBase
	{
	public:
		THAPI::Module& m_module;
		// 插入之前Module的order未初始化，应依赖这个判断
		const DWORD m_order;

		ModuleEvent(THAPI::Module& module, DWORD order);
		virtual ~ModuleEvent() = default;
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
		THAPI::InitUnitArg& m_arg;

		InitUnitEvent(LPCSTR eclFuncName, THAPI::InitUnitArg& arg);
	};

	class TML_API UnitEvent : public EventBase
	{
	public:
		THAPI::Unit& m_unit;

		UnitEvent(THAPI::Unit& unit);
		virtual ~UnitEvent() = default;
	};
}
