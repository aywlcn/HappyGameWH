#ifndef GAME_FRAME_HEAD_FILE
#define GAME_FRAME_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//全局头文件
#include "..\..\公共文件\GlobalDef.h"
#include "..\..\公共文件\GlobalField.h"
#include "..\..\公共文件\GlobalFrame.h"

//接口头文件
#include "..\..\组件接口\IUnknownEx.h"

//模板库
#include "..\..\模板库\Template.h"

//组件头文件
#include "..\游戏等级\GameRank.h"
#include "..\头像组件\UserFace.h"
#include "..\关系管理\Companion.h"
#include "..\信道模块\ChannelModule.h"
#include "..\客户端共享\ClientShare.h"
#include "..\..\共享组件\界面控件\SkinControls.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef GAME_FRAME_CLASS
	#ifdef  GAME_FRAME_DLL
		#define GAME_FRAME_CLASS _declspec(dllexport)
	#else
		#define GAME_FRAME_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define GAME_FRAME_DLL_NAME	TEXT("GameFrame.dll")					//组件 DLL 名字
#else
	#define GAME_FRAME_DLL_NAME	TEXT("GameFrame.dll")					//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//结构体定义

//房间属性
struct tagServerAttribute
{
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	WORD							wGameGenre;							//游戏类型
	WORD							wChairCount;						//椅子数目
	TCHAR							szKindName[KIND_LEN];				//类型名字
	TCHAR							szServerName[SERVER_LEN];			//房间名称
};

//////////////////////////////////////////////////////////////////////////

#define VER_IClientKernel INTERFACE_VERSION(3,1)
static const GUID IID_IClientKernel={0x850581d,0x1d7a,0x4137,0xa4,0xb7,0xf0,0x3,0x36,0x1c,0x0,0xed};

//客户端内核
interface IClientKernel : public IUnknownEx
{
	//驱动接口
public:
	//设置接口
	virtual bool __cdecl SetMessageProxy(IUnknownEx * pIUnknownEx)=NULL;
	//内核配置
	virtual bool __cdecl InitClientKernel(LPCTSTR lpszComLine, IUnknownEx * pIUnknownEx)=NULL;

	//信息接口
public:
	//离开状态
	virtual bool __cdecl GetExitFlags()=NULL;
	//旁观用户
	virtual bool __cdecl IsLookonMode()=NULL;
	//允许旁观
	virtual bool __cdecl IsAllowLookon()=NULL;
	//房间属性
	virtual const tagServerAttribute * __cdecl GetServerAttribute()=NULL;

	//状态接口
public:
	//游戏状态
	virtual BYTE __cdecl GetGameStatus()=NULL;
	//游戏状态
	virtual void __cdecl SetGameStatus(BYTE bGameStatus)=NULL;

	//用户接口
public:
	//自己位置
	virtual WORD __cdecl GetMeChairID()=NULL;
	//获取自己
	virtual const tagUserData * __cdecl GetMeUserInfo()=NULL;
	//获取玩家
	virtual const tagUserData * __cdecl GetUserInfo(WORD wChairID)=NULL;

	//发送接口
public:
	//显示消息
	virtual int __cdecl ShowMessageBox(LPCTSTR pszMessage, UINT nType)=NULL;
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool __cdecl SendProcessData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendProcessData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)=NULL;

	//功能接口
public:
	//发送准备
	virtual bool __cdecl SendUserReady(void * pBuffer, WORD wDataSize)=NULL;
	//发送聊天
	virtual bool __cdecl SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)=NULL;

	//定时器接口
public:
	//定时器位置
	virtual WORD __cdecl GetTimeChairID()=NULL;
	//设置定时器
	virtual bool __cdecl SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse)=NULL;
	//删除定时器
	virtual bool __cdecl KillGameTimer(UINT nTimerID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IClientKernelSink INTERFACE_VERSION(1,1)
static const GUID IID_IClientKernelSink={0xebda1f6e,0x3ce2,0x448d,0x99,0x5,0x87,0x6e,0x52,0x71,0x6b,0x7f};

//内核钩子接口
interface IClientKernelSink : public IUnknownEx
{
	//驱动接口
public:
	//获取窗口
	virtual HWND __cdecl GetFrameWnd()=NULL;
	//重置状态
	virtual void __cdecl ResetGameFrame()=NULL;
	//关闭窗口
	virtual void __cdecl CloseGameFrame(bool bForceClose)=NULL;

	//内核事件
public:
	//时间消息
	virtual bool __cdecl OnEventTimer(WORD wChairID, UINT nElapse, UINT nTimerID)=NULL;
	//网络消息
	virtual bool __cdecl OnEventSocket(const CMD_Command & Command, const void * pBuffer, WORD wDataSize)=NULL;
	//进程消息
	virtual bool __cdecl OnEventProcess(const IPC_Head & Head, const void * pBuffer, WORD wDataSize, HWND hWndSend)=NULL;

	//游戏事件
public:
	//游戏状态
	virtual bool __cdecl OnEventGameScene(BYTE cbGameStation, bool bLookonOther, void * pBuffer, WORD wDataSize)=NULL;
	//聊天信息
	virtual bool __cdecl OnUserChatMessage(tagUserData * pSendUserData, tagUserData * pRecvUserData, LPCTSTR pszChatMessage, COLORREF crColor)=NULL;
	//用户进入
	virtual void __cdecl OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser)=NULL;
	//用户离开
	virtual void __cdecl OnEventUserLeft(tagUserData * pUserData, WORD wChairID, bool bLookonUser)=NULL;
	//用户积分
	virtual void __cdecl OnEventUserScore(tagUserData * pUserData, WORD wChairID, bool bLookonUser)=NULL;
	//用户状态
	virtual void __cdecl OnEventUserStatus(tagUserData * pUserData, WORD wChairID, bool bLookonUser)=NULL;
	//时间更新
	virtual void __cdecl OnEventTimerKilled(WORD wChairID, UINT nTimerID)=NULL;
	//旁观状态
	virtual void __cdecl OnEventLookonChanged(bool bLookonUser, void * pBuffer, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IGameFrameView INTERFACE_VERSION(1,1)
static const GUID IID_IGameFrameView={0x22d2598,0x624f,0x46f9,0xa6,0xde,0xbc,0x85,0xf4,0xc8,0x12,0x11};

//游戏视图接口
interface IGameFrameView : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////

//游戏内核对象辅助类
class CClientKernelHelper : public CTempldateHelper<IClientKernel>
{
	//函数定义
public:
	//构造函数
	CClientKernelHelper(void) : CTempldateHelper<IClientKernel>(IID_IClientKernel,
		VER_IClientKernel,GAME_FRAME_DLL_NAME,TEXT("CreateClientKernel"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

//导出类头文件
#ifndef GAME_FRAME_DLL
	#include "ThreadDraw.h"
	#include "GameFrameApp.h"
	#include "GameFrameDlg.h"
	#include "GameFrameView.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif