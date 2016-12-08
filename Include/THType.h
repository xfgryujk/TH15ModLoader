// 东方本体程序用的结构、全局变量声明

#pragma once
#include "APIHeader.h"

struct IDirect3D9;
struct IDirect3DDevice9; 
struct IDirectInputA;
struct IDirectInputDeviceA;
struct IDirect3DTexture9;
struct IDirect3DVertexBuffer9;


namespace THAPI
{
	struct Module;
	struct Stage;
	struct EclManager;
	struct Unit;

#define CHECK_SIZE(T, size) static_assert(sizeof(T) == size, "Wrong size of " #T)

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
	CHECK_SIZE(LinkNode<void>, 0x10);

	struct ModulesLink
	{
		DWORD unknown1; // 这里unknown成员都没被使用过
		DWORD unknown2;
		DWORD unknown3;
		DWORD unknown4;
		DWORD unknown5;
		LinkNode<Module> firstLinkNode;
		DWORD unknown6;
	};
	CHECK_SIZE(ModulesLink, 0x28);

	// 储存逻辑、渲染链表，每帧被遍历调用
	struct ModulesLinks
	{
		ModulesLink logicLink;				// 逻辑链
		ModulesLink renderLink;				// 渲染链
		LinkNode<Module>* pNextNode;		// 遍历链表时临时储存下一个结点指针，逻辑、渲染链都用到
		BOOL hasUninit;						// ModulesLinks析构时这个赋值1并调用logicLink
	};
	CHECK_SIZE(ModulesLinks, 0x58);

	// 每帧被遍历时mainFunction被调用
	struct Module
	{
		DWORD order;					// 也可能是类型、图层？这个决定在链表中的顺序
		DWORD flag;						// 已知没有0x2时不执行mainFunction，没有0x1时从ModulesLinks断开后不free这个Module
		int(__fastcall* mainFunction)(void* param);				// 逻辑或渲染函数，thiscall，参数为param
		int(__fastcall* onInsertToModulesLinks)(void* param);	// 这个Module插入ModulesLinks时执行，thiscall，参数为param
		int(__fastcall* onLogicEnd)(void* param);				// 在逻辑链表中有可能执行，没见执行过，thiscall，参数为param
		LinkNode<Module> linkNode;
		void* param;					// 逻辑链中order为26时是g_pStage
	};
	CHECK_SIZE(Module, 0x28);


	// 每个关卡初始化一次的全局变量
	struct Stage
	{
		DWORD unknown1;
		Module* pLogicModule;
		Module* pRenderModule;
		BYTE unknown[368];
		EclManager* pEclManager;
		LinkNode<Unit>* pFirstUnitNode;
		LinkNode<Unit>* pLastUnitNode;
		LinkNode<Unit>* pFirstUnitNode_;
		DWORD nNumberOfUnits;
	};
	CHECK_SIZE(Stage, 0x190);

	// 负责读取、储存ECL
	struct EclManager
	{
		void* pVTable;
		DWORD nNumberOfEclBufs;			// 读取的ECL文件数
		DWORD nNumberOfFunctions;		// ECL函数数，包括include的
		BYTE* pEclBufs[32];				// 指向ECL文件buffer
		struct
		{
			char* name;
			BYTE* pBuf;					// 指向ECL函数头"ECLH"的地方
		}* pFunctions;
		BYTE unknown[4104];
	};
	CHECK_SIZE(EclManager, 0x1098);

	// ECL执行环境
	struct EclContext
	{
		float time;					// 每帧一直执行ECL直到EclContext.time < Ins.time，Ins23实际会减少EclContext.time
		int eclFuncIndex;			// 当前在执行EclManager中哪个函数，-1说明未找到或已执行完
		DWORD insOffset;			// 当前ins偏移量，相对于ECL函数头"ECLH"+16的位置
		BYTE stack[4096];			// ECL栈储存区
		DWORD stackTopOffset;		// 栈中下一个储存位置
		DWORD varOffset;			// 栈中局部变量的位置
		DWORD unknown1;
		Unit* pUnit;
		DWORD unknown2;
		BYTE executeInsFlag;		// 与Ins.flag与运算后决定是否执行这条指令，一般为0x2
		BYTE unknown3[455];
	};
	CHECK_SIZE(EclContext, 0x11E8);

	// 一条ECL指令
	struct Ins
	{
		DWORD time;				// 当前时间没到time则不执行，实际会转为float再比较
		WORD code;				// 指令号
		WORD size;				// 这个ins结构的大小
		WORD isRefArgFlag;		// 第几位为1代表第几个参数是变量引用
		BYTE flag;				// 一般为0xFF，与EclContext.executeInsFlag与运算后决定是否执行这条指令
		BYTE argCount;			// 参数数
		DWORD usedStackSize;	// 一般为0，执行完这条ins后栈顶指针减去这个数
		int args[1];			// 可能是int或float，实际有多少个参数看argCount
	};

	struct InitUnitArg
	{
		float x;
		float y;
		float z;
		int score;
		DWORD baseDropItemType;
		int maxLife;
		DWORD xMirrorFlag;			// 左右镜像
		BYTE unknown[56];
	};
	CHECK_SIZE(InitUnitArg, 0x54);

	struct Vec3
	{
		float x;
		float y;
		float z;
	};

	struct Position
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
		DWORD unknown3;

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
	CHECK_SIZE(Position, 0x44);

	struct Danmaku
	{
		BYTE unknown1[20];
		float direct;				// 方向
		float deltaRadBetweenWays;	// 每way间隔弧度
		DWORD flag;
		DWORD unknown2;
		float bulletGenDist;		// 发弹点距离BOSS的距离？
		BYTE unknown3[856];
	};
	CHECK_SIZE(Danmaku, 0x380);

	// 储存了坐标、弹幕等
	struct Enemy
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
		int score;
		int life;
		int maxLife;
		int life3;					// 暂时未知
		int lifeMul7;				// life乘7？
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
	CHECK_SIZE(Enemy, 0x4530);

	// 一个EclContext执行一个ECL函数，可以有多个EclContext并行，关卡初始化时第一个单位执行main函数
	struct Unit
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
	CHECK_SIZE(Unit, 0x574C);


	// 对应一个动画，也是ANM指令环境，执行一个script
	struct AnmContext
	{
		BYTE unknown1[24];
		DWORD renderMode;
		DWORD renderFlag;
		BYTE unknown2[8];
		DWORD anmIndex;			// AnmManager索引
		DWORD unknown3;
		DWORD scriptIndex;		// AnmManager中script索引
		DWORD insOffset;		// 当前ins偏移量
		BYTE unknown4[932];
		float matrix1[16]; //D3DXMATRIX matrix1;
		float matrix2[16]; //D3DXMATRIX matrix2;
		BYTE unknown5[232];
		DWORD id;				// 低13位是anmCtxIndex，高位是总分配的AnmCtx数
		DWORD anmCtxIndex;		// AnmCtx池索引
		DWORD unknown6;
		float time;				// 当前anm指令时间
		BYTE unknown7[32];
		LinkNode<AnmContext> linkNode;
		BYTE unknown8[120];
		float rotationX;
		float rotationY;
		float rotationZ;
	};
	CHECK_SIZE(AnmContext, 0x608);

	// 对应一个ANM文件
	struct AnmManager
	{
		DWORD index;
		char name[260];
		BYTE* pAnmBuf;					// ANM文件buffer
		AnmContext* pAnmCtxPrototypes;	// AnmContext原型数组，新的AnmContext从这里拷贝，大小=scriptCount
		DWORD imgCount;					// ANM文件中图片数
		DWORD scriptCount;				// ANM文件中总script数
		DWORD spriteCount;				// ANM文件中总sprite数
		void* pSprites;					// 大小=spriteCount
		BYTE** pScriptBufs;				// 指针数组，指向ANM指令开始的地方，大小=scriptCount
		struct Img
		{
			IDirect3DTexture9* pTexture;
			BYTE* pImgFileBuf;			// 外部图片文件的buffer
			DWORD imgFileBufSize;
			DWORD bytesPerPixel;
			BYTE* pImgBufInAnmFile;		// ANM文件中储存的图片buffer
			DWORD flag;
		}* pImgs;						// 大小=imgCount
		BYTE unknown[20];
	};
	CHECK_SIZE(AnmManager, 0x13C);

	struct AnmCtxNode
	{
		AnmContext anmCtx;
		LinkNode<AnmCtxNode> linkNode;
		BYTE unknown;		// 可能是已被分配的flag
		BYTE padding[3];
		DWORD index;
	};
	CHECK_SIZE(AnmCtxNode, 0x620);

	struct Vertex1
	{
		float x, y, z;
		float specular, diffuse;
		float tu, tv;
	};

	struct Vertex2
	{
		float x, y, z, rhw;
		float diffuse;
	};

	// 储存渲染用的资源和状态
	struct RenderEngine
	{
		void* pVTable;
		BYTE unknown[28];
		struct
		{
			DWORD anmIndex;
			DWORD imgIndex;
			DWORD srcX;
			DWORD srcY;
			DWORD srcWidth;
			DWORD srcHeight;
			DWORD dstX;
			DWORD dstY;
			DWORD dstWidth;
			DWORD dstHeight;
		} copyRenderResultArgs[4];			// 貌似会把每帧渲染结果拷贝？应该和截图有关
		DWORD newBulletCount;				// 也可能不是子弹数，暂时这么叫...
		DWORD unknown1;
		DWORD setStateCount;				// 每帧开始=0，调用渲染函数时+1
		DWORD drawCount;					// 每帧开始=0，调用渲染函数时+1
		BYTE unknown2[20];
		LinkNode<AnmCtxNode>* pAnmCtx1Link;
		LinkNode<AnmCtxNode>* unknown3;
		AnmCtxNode anmCtxNodes1[8191];
		DWORD nextAnmCtxNode2Index;			// 下一个分配的AnmCtxNode2索引
		DWORD totalAllocAnmCtx2Count;		// 和上面区别是这个不会被清0
		LinkNode<AnmCtxNode> anmCtxNode2Link;
		AnmCtxNode anmCtxNodes2[8191];		// AnmContext池，需要新的AnmContext优先从这里分配
		LinkNode<AnmCtxNode> anmCtxNode1Link;
		DWORD unknown4;
		AnmManager* pAnmManager[30];		// ANM资源
		BYTE unknown5[64];
		AnmContext unknown6;
		DWORD unknown7;

		// 这一部分都是关于D3D(固定渲染管线)状态的东西

		DWORD CurTextureFactor;				// D3DRS_TEXTUREFACTOR
		DWORD CurTextureID;					// 高8位是AnmManager索引，低8位是Img索引
		BYTE CurBlendMode;
		BYTE unknown8;
		BYTE CurVertexBufferType;			// 好像决定了用DrawPrimitive还是DrawPrimitiveUP吧？
		BYTE unknown9[3];
		BYTE CurSampleFilter;
		BYTE CurBlendOp;
		BYTE CurAddressU;
		BYTE CurAddressV;
		BYTE unknown10[6];
		IDirect3DVertexBuffer9* pD3DVertexBuffer;

		BYTE unknown11[80];
		DWORD rectPrimitiveCount;			// 三角片数量的一半，即矩形数量
		// 东方的优化策略：渲染函数只是向顶点缓冲区添加顶点，直到渲染状态改变了(比如换纹理)才真正渲染
		Vertex1 vertex1Buf[131072];			// 渲染时向这里添加顶点1
		Vertex1* pNextVertex1Buf;			// 指向下一个添加顶点1的位置
		Vertex1* pCurVertex1Buf;			// 指向当前准备渲染的顶点1缓冲的位置
		DWORD unknown12;
		Vertex2 vertex2Buf[32768];			// 渲染时向这里添加顶点2
		Vertex2* pNextVertex2Buf;			// 指向下一个添加顶点2的位置
		Vertex2* pCurVertex2Buf;			// 指向当前准备渲染的顶点2缓冲的位置
		AnmContext unknown13[42];
		DWORD topAnmCtxNode1Index;
		BYTE unknown14[8];
	};
	CHECK_SIZE(RenderEngine, 0x1CAF97C);
	
#pragma pack(pop)


	// 全局变量

	extern TML_API IDirect3D9*& g_pD3D;
	extern TML_API IDirect3DDevice9*& g_pD3DDevice;
	extern TML_API IDirectInputA*& g_pDInput;
	extern TML_API IDirectInputDeviceA*& g_pDInputDevice;

	extern TML_API HWND& g_hMainWnd;
	extern TML_API HINSTANCE& g_hInstance;

	extern TML_API ModulesLinks*& g_pModulesLinks;	// 储存逻辑链表和渲染链表，每帧被遍历调用
	extern TML_API RenderEngine*& g_pRenderEngine;	// 储存渲染用的资源和状态
	extern TML_API Stage*& g_pStage;				// 每个关卡初始化一次的全局变量

	extern TML_API float& g_ticksPerFrame;			// 用来控制游戏速度
	
	extern TML_API DWORD& g_pressedButton;			// 这一帧玩家按下了哪些键，在0x401F50这个函数设定，被order=1的Module调用
	extern TML_API DWORD& g_lastPressedButton;		// 上一帧玩家按下了哪些键
	extern TML_API DWORD& g_longPressedButton;		// 被长按的键
	extern TML_API DWORD& g_newPressedButton;		// 这一帧新按下的键
	extern TML_API DWORD& g_newReleasedButton;		// 这一帧新放开的键

	extern TML_API int& g_playerScore;
	extern TML_API int& g_playerMaxGetPoint;		// 最大得点
	extern TML_API int& g_playerLife;
	extern TML_API int& g_playerLifePiece;
	extern TML_API int& g_playerBomb;
	extern TML_API int& g_playerBombPiece;
	extern TML_API int& g_playerPower;				// 100-400
}
