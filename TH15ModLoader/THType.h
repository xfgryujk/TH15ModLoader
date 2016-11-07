#pragma once

namespace THType
{
	struct LinkNode;
	struct Struct1;
	struct Struct1_Ren;
	struct Struct2;
	struct Struct3;
	struct EclClass;
	struct Struct4;
	struct EclExecuteStruct;
	struct Struct5;


	struct LinkNode
	{
		Struct2* pThisData;
		LinkNode* pNext;
		LinkNode* pPrev;
		LinkNode* unknown; // 貌似和next一样
	};

	struct Struct1
	{
		DWORD unknown1;
		DWORD unknown2;
		DWORD unknown3;
		DWORD unknown4;
		DWORD unknown5;
		LinkNode firstLogicNode;
		DWORD unknown6;
		Struct1_Ren struct1Ren;
		DWORD unknown7;
		LinkNode* pNextNode; // 遍历时候临时用
		DWORD unknown8;
	};

	struct Struct1_Ren
	{
		DWORD unknown1;
		DWORD unknown2;
		DWORD unknown3;
		DWORD unknown4;
		DWORD unknown5;
		LinkNode firstRenderNode;
	};

	struct Struct2
	{
		int order;
		DWORD unknownFlag;
		void* mainFunction; // 逻辑或渲染函数
		DWORD unknown1;
		void* unknownFunction;
		LinkNode linkNode;
		void* pStruct3;
	};
}
