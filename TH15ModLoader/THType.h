// 东方本体程序用的结构、常量声明

#pragma once

namespace tml
{
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
		LinkNode<Struct2>* pNextNode;		// 遍历时候临时用
		DWORD unknown8;
	};

	struct Struct2 // size = 0x28
	{
		DWORD order;					// 也可能是类型、图层？这个决定在链表中的顺序
		DWORD flag;						// 已知没有0x2时不执行mainFunction
		void* mainFunction;				// 逻辑或渲染函数，参数为param
		void* onInsertToStruct1;		// 这个Struct2插入Struct1时执行，参数为param
		void* onLogicEnd;				// 在逻辑链表中有可能执行，没见执行过，参数为param
		LinkNode<Struct2> linkNode;
		void* param;					// order为26时是g_pStage
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
		DWORD nNumberOfUnits;
	};

	// 负责读取、储存ECL
	struct EclManager // size = 0x1098
	{
		void* pVTable;
		DWORD nNumberOfEclBufs;			// 读取的ECL文件数
		DWORD nNumberOfFunctions;			// ECL函数数，包括include的
		BYTE* pEclBufs[32];				// 指向ECL文件buffer
		struct
		{
			char* pName;
			BYTE* pBuf;					// 指向ECL函数头"ECLH"的地方
		}* pFunctions;
		BYTE unknown[4104];
	};

	// ECL执行环境
	struct EclContext // size = 0x11E8
	{
		float nextTime;				// ECL执行到这个时间停止
		DWORD eclFuncIndex;			// 当前在执行EclManager中哪个函数
		DWORD insOffset;			// 当前ins偏移量，相对于ECL函数头"ECLH"+16的位置
		BYTE stack[4096];			// ECL栈储存区
		DWORD stackTopOffset;		// 栈中下一个储存位置
		DWORD varOffset;			// 栈中局部变量的位置
		DWORD unknown1;
		Unit* pUnit;
		BYTE unknown[460];
	};

	// 一条ECL指令
	struct Ins
	{
		float time;
		WORD code;				// 指令号
		WORD size;				// 这个ins结构的大小
		WORD isRefArgFlag;		// 第几位为1代表第几个参数是变量引用
		WORD flag2;
		DWORD usedStackSize;	// 执行完这条ins后栈顶指针减去这个数
		BYTE unknown[3];
		DWORD args[1];			// 可能是整数或浮点数，实际有多少个参数看size
	};

	struct BulletGenPoint
	{
		float x;
		float y;
		DWORD unknown;
	};

	struct Danmaku // size = 0x380
	{
		BYTE unknown1[20];
		float direct;				// 方向
		float deltaRadBetweenWays;	// 每way间隔弧度
		DWORD flag;
		DWORD unknown2;
		float bulletGenDist;		// 发弹点距离BOSS的距离？
		BYTE unknown3[856];
	};

	// 储存了坐标、弹幕等
	struct Enemy // size = 0x4530
	{
		BYTE unknown1[43];
		float finalX;				// 实际坐标 = 绝对坐标 + 相对坐标
		float finalY;
		float finalZ;
		BYTE unknown2[40];
		float deltaX;				// 改变的坐标值，在改变坐标后调用的一个函数赋值(0x4278D0)
		float deltaY;
		float deltaZ;
		DWORD unknown3;
		float absoluteX;			// 绝对坐标
		float absoluteY;
		float absoluteZ;
		DWORD unknown4;
		DWORD flag1;
		BYTE unknown5[30];
		float relativeX;			// 相对坐标
		float relativeY;
		float relativeZ;
		BYTE unknown6[56];
		float HitBoxWidth;			// 被弹判定
		float HitBoxHeight;
		float BoundingBoxWidth;		// 体术判定
		float BoundingBoxHeight;
		BYTE unknown7[1144];
		Danmaku danmakus[16];
		DWORD unknown8[16];
		BulletGenPoint RelativeBulletGenPoints[16]; // 相对发弹点
		BulletGenPoint AbsoluteBulletGenPoints[16]; // 绝对发弹点
		BYTE unknown9[8];
		float boundCenterX;			// 移动范围限制
		float boundCenterY;
		float boundWidth;
		float boundHeight;
		DWORD unknown10;
		DWORD life1;				// 赋值了这3个，暂时不知道各个的意义
		DWORD life2;
		DWORD life3;
		DWORD lifeMul7;				// life乘7？
		BYTE unknown11[12];
		DWORD baseDropItemType;		// 基础掉落
		DWORD dropItemCount[16];	// 索引是类型
		float dropWidth;			// 掉落区域
		float dropHeight;
		BYTE unknown12[132];
		DWORD flag;					// 见wiki
		BYTE unknown13[16];
		float protectRange;			// 弹幕保护范围的平方，自机这范围内不生成弹幕
		BYTE unknown14[1088];
		Unit* pUnit;
		BYTE unknown15[112];
		Enemy* thiz;
	};

	// 一个单位可以有多个子单位，一个单位执行一个ECL函数，关卡初始化时第一个单位执行main函数
	struct Unit // size = 0x574C
	{
		void* pVTable;							// =0x4CC2E8，第一个虚函数用来执行100以上的ins
		DWORD unknown1;
		DWORD unknown2;
		EclContext* pCurEclContext;				// 指向当前遍历到的EclContext
		EclContext eclContext;
		EclManager* pEclManager;
		LinkNode<EclContext> firstEclContext;
		Enemy enemy;
		DWORD unknown3;
		DWORD unknown4;
		DWORD unknown5;
		DWORD unknown6;
	};
#pragma pack(pop)
}
}
