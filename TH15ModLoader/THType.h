#pragma once

namespace THType
{
	struct Struct2;
	struct Stage;
	struct EclManager;
	struct Unit;


#pragma pack(push)
#pragma pack(1)
	template<class T>
	struct LinkNode
	{
		T* pThisData;
		LinkNode* pNext;
		LinkNode* pPrev;
		LinkNode* unknown; // 貌似和next一样
	};

	struct Struct1_Ren
	{
		DWORD unknown1;
		DWORD unknown2;
		DWORD unknown3;
		DWORD unknown4;
		DWORD unknown5;
		LinkNode<Struct2> firstRenderNode;
	};

	struct Struct1 // size = 0x58
	{
		DWORD unknown1;
		DWORD unknown2;
		DWORD unknown3;
		DWORD unknown4;
		DWORD unknown5;
		LinkNode<Struct2> firstLogicNode;
		DWORD unknown6;
		Struct1_Ren struct1Ren;
		DWORD unknown7;
		LinkNode<Struct2>* pNextNode; // 遍历时候临时用
		DWORD unknown8;
	};

	struct Struct2 // size = 0x28
	{
		int order; // 也可能是类型，这个决定在链表中的顺序
		DWORD unknownFlag;
		void* mainFunction; // 逻辑或渲染函数，参数为pStruct3
		DWORD unknown1;
		void* unknownFunction; // 参数为pStruct3
		LinkNode<Struct2> linkNode;
		void* pStruct3; // order为26时是g_pStage
	};

	// 每个关卡初始化一次的全局变量
	struct Stage // size = 0x190
	{
		DWORD unknown1;
		Struct2* pStruct2;
		BYTE unknown[372];
		EclManager* pEclManager;
		LinkNode<Unit>* pFirstUnitNode;
		LinkNode<Unit>* pLastUnitNode;
		LinkNode<Unit>* pFirstUnitNode_;
		int nNumberOfUnits;
	};

	// 负责读取、储存ECL
	struct EclManager // size = 0x1098
	{
		void* pVTable;
		int nNumberOfEclBufs; // 读取的ECL文件数
		int nNumberOfFunctions; // ECL函数数，包括include的
		BYTE* pEclBufs[32]; // 指向ECL文件buffer
		struct
		{
			char* pName;
			BYTE* pBuf; // 指向ECL函数头"ECLH"的地方
		}* pFunctions;
		BYTE unknown[4104];
	};

	// ECL执行环境
	struct EclContext // size = 0x11E8
	{
		float nextTime; // ECL执行到这个时间停止
		int eclFuncIndex; // 当前在执行EclManager中哪个函数
		int insOffset; // 当前ins偏移量，相对于ECL函数头"ECLH"+16的位置
		BYTE stack[4096]; // ECL栈储存区
		int stackTopOffset; // 栈中下一个储存位置
		int varOffset; // 栈中局部变量的位置
		DWORD unknown1;
		Unit* pUnit;
		BYTE unknown[460];
	};

	struct Struct5 // size = 0x4530
	{
		BYTE unknown[17712];
	};

	// 一个单位可以有多个子单位，一个单位执行一个ECL函数，关卡初始化时第一个单位执行main函数
	struct Unit // size = 0x574C
	{
		void* pVTable;
		DWORD unknown1;
		DWORD unknown2;
		EclContext* pCurEclContext;
		EclContext eclContext;
		EclManager* pEclManager;
		LinkNode<EclContext> firstEclContext;
		Struct5 struct5;
		DWORD unknown3;
		DWORD unknown4;
		DWORD unknown5;
		DWORD unknown6;
	};
#pragma pack(pop)
}
