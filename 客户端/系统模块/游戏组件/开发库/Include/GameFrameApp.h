#ifndef GAME_FRAME_APP_HEAD_FILE
#define GAME_FRAME_APP_HEAD_FILE

#pragma once

#include "GameFrameWnd.h"
#include "GameFrameView.h"
#include "GameFrameEngine.h"
#include "GameFrameAvatar.h"
#include "GameFrameService.h"

//其他文件
#include "IDispimp.h"       
#include "Custsite.h"       

//////////////////////////////////////////////////////////////////////////////////

//应用程序
class GAME_FRAME_CLASS CGameFrameApp : public CWinApp
{
	//变量定义
public:
	CImpIDispatch *					m_pIDispatch;						//自动接口
	CCustomOccManager *				m_pCustomOccManager;				//管理对象

	//变量定义
protected:
	CGameFrameWnd *					m_pGameFrameWnd;					//框架窗口
	CGameFrameEngine *				m_pGameFrameEngine;					//框架引擎

	//平台数据
protected:
	CGlobalWebLink					m_GlobalWebLink;					//链接信息

	//游戏引擎
protected:
	CD3DSound						m_D3DSound;							//声音对象
	CGameFrameAvatar				m_GameFrameAvatar;					//头像组件
	CGameFrameService				m_GameFrameService;					//框架服务	

	//组件变量
public:
	CPlatformResourceHelper			m_PlatformResourceModule;			//资源组件

	//函数定义
public:
	//构造函数
	CGameFrameApp();
	//析构函数
	virtual ~CGameFrameApp();

	//重载函数
public:
	//运行函数
	virtual int Run();
	//配置函数
	virtual BOOL InitInstance();
	//退出函数
	virtual BOOL ExitInstance();

	//继承函数
public:
	//创建框架
	virtual CGameFrameWnd * GetGameFrameWnd();
	//创建引擎
	virtual CGameFrameEngine * GetGameFrameEngine(DWORD dwSDKVersion)=NULL;

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////


#endif