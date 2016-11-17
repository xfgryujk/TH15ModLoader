// 东方本体程序用的结构、全局变量声明

#pragma once
#include "APIHeader.h"

struct IDirect3D9;
struct IDirect3DDevice9; 
struct IDirectInputA;
struct IDirectInputDeviceA;


namespace THAPI
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

	// 储存渲染链表，每帧被遍历调用
	struct Struct1_Ren
	{
		DWORD unknown1;
		DWORD unknown2;
		DWORD unknown3;
		DWORD unknown4;
		DWORD unknown5;
		LinkNode<Struct2> firstRenderNode;
	};

	// 储存逻辑链表，每帧被遍历调用
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
		LinkNode<Struct2>* pNextNode;		// 遍历时候临时用，逻辑、渲染链都用到
		DWORD unknown8;
	};

	// 每帧被遍历时mainFunction被调用
	struct Struct2 // size = 0x28
	{
		DWORD order;					// 也可能是类型、图层？这个决定在链表中的顺序
		DWORD flag;						// 已知没有0x2时不执行mainFunction，没有0x1时从Struct1断开后不free这个Struct2
		void* mainFunction;				// 逻辑或渲染函数，thiscall，参数为param
		void* onInsertToStruct1;		// 这个Struct2插入Struct1时执行，thiscall，参数为param
		void* onLogicEnd;				// 在逻辑链表中有可能执行，没见执行过，thiscall，参数为param
		LinkNode<Struct2> linkNode;
		void* param;					// 逻辑链中order为26时是g_pStage
	};

	// 每个关卡初始化一次的全局变量
	struct Stage // size = 0x190
	{
		DWORD unknown1;
		Struct2* pStruct2;
		Struct2* pStruct2Ren;
		BYTE unknown[368];
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
		float time;					// 每帧一直执行ECL直到EclContext.time < Ins.time，Ins23实际会减少EclContext.time
		DWORD eclFuncIndex;			// 当前在执行EclManager中哪个函数
		DWORD insOffset;			// 当前ins偏移量，相对于ECL函数头"ECLH"+16的位置
		BYTE stack[4096];			// ECL栈储存区
		DWORD stackTopOffset;		// 栈中下一个储存位置
		DWORD varOffset;			// 栈中局部变量的位置
		DWORD unknown1;
		Unit* pUnit;
		BYTE unknown2[460];
	};

	// 一条ECL指令
	struct Ins
	{
		DWORD time;				// 当前时间没到time则不执行，实际会转为float再比较
		WORD code;				// 指令号
		WORD size;				// 这个ins结构的大小
		WORD isRefArgFlag;		// 第几位为1代表第几个参数是变量引用
		BYTE flag;				// 一般为0xFF，与EclContext某成员与运算后决定是否执行这条指令，某成员一般为0x2
		BYTE argCount;			// 参数数
		DWORD usedStackSize;	// 一般为0，执行完这条ins后栈顶指针减去这个数
		int args[1];			// 可能是int或float，实际有多少个参数看argCount
	};

	struct Vec3
	{
		float x;
		float y;
		float z;
	};

	struct Position // size = 0x44
	{
		float x;
		float y;
		float z;
		DWORD unknown1;
		DWORD flag;
		DWORD unknown2;
		float angularVelocity;		// angle速度或直角坐标速度大小
		float angle;				// 极坐标角度或方向
		float radius;
		float radiusVelocity;		// radius速度
		float axisRotation;			// 坐标轴旋转角度，用于椭圆运动
		float zoomRatio;			// 旋转后X轴缩放比，用于椭圆运动
		DWORD unknown6;
		DWORD unknown7;
		DWORD unknown8;

		// 速度或原点坐标

		float movingArgX;
		float movingArgY;
		float movingArgZ;

		/*
		0直线，1静止，2圆，3椭圆，4未知
		=0时以angle和angularVelocity计算movingArgXYZ，然后xyz加上movingArgXYZ
		=2时以movingArgXYZ为原点，以angle、radius为极坐标计算xyz
		=3时先按圆计算，再把坐标轴旋转axisRotation后，X轴缩放zoomRatio倍，再映射回原坐标轴得到xyz
		*/
		DWORD movingMode : 4;
		DWORD padding : 28;
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
		float lastFinalX;
		float lastFinalY;
		float lastFinalZ;
		BYTE unknown1[40];
		float lastDeltaX;
		float lastDeltaY;
		float lastDeltaZ;
		DWORD unknown16;
		float finalX;				// 实际坐标 = 绝对坐标 + 相对坐标
		float finalY;
		float finalZ;
		BYTE unknown2[40];
		float deltaX;				// 改变的坐标值，在改变坐标后调用的一个函数赋值(0x4278D0)
		float deltaY;
		float deltaZ;
		DWORD unknown3;
		Position absolutePos;		// 绝对坐标
		Position relativePos;		// 相对坐标
		float HitBoxWidth;			// 被弹判定
		float HitBoxHeight;
		float BoundingBoxWidth;		// 体术判定
		float BoundingBoxHeight;
		DWORD unknown17;
		DWORD unknown18[16];
		BYTE unknown7[1076];
		Danmaku danmakus[16];
		DWORD unknown8[16];
		Vec3 RelativeBulletGenPoints[16]; // 相对发弹点
		Vec3 AbsoluteBulletGenPoints[16]; // 绝对发弹点
		float Width;
		float Height;
		float boundCenterX;			// 移动范围限制
		float boundCenterY;
		float boundWidth;
		float boundHeight;
		DWORD unknown10;
		DWORD life;
		DWORD maxLife;
		DWORD life3;				// 暂时未知
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

	// 一个EclContext执行一个ECL函数，可以有多个EclContext并行，关卡初始化时第一个单位执行main函数
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


	// 全局变量

	extern TML_API IDirect3D9*& g_pD3D;
	extern TML_API IDirect3DDevice9*& g_pD3DDevice;
	extern TML_API IDirectInputA*& g_pDInput;
	extern TML_API IDirectInputDeviceA*& g_pDInputDevice;

	extern TML_API HWND& g_hMainWnd;
	extern TML_API HINSTANCE& g_hInstance;

	extern TML_API Struct1*& g_pStruct1;
	extern TML_API Stage*& g_pStage;

	extern TML_API float& g_ticksPerFrame;			// 用来控制游戏速度
	
	extern TML_API DWORD& g_playerInputFlag;		// 当前用户输入了哪些键，在0x401F50这个函数设定，被order=1的Struct2调用
	extern TML_API int& g_playerLife;
	extern TML_API int& g_playerBomb;
}
