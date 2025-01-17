#pragma once

//////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif						

#ifndef _WIN32_WINDOWS	
#define _WIN32_WINDOWS 0x0501
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#define _AFX_ALL_WARNINGS

#include <afxwin.h> 
#include <afxext.h> 
#include <afxdisp.h>

#include <afxdtctl.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif

//////////////////////////////////////////////////////////////////////////

//系统头文件
#include "Afxmt.h"
#include "AfxHtml.h"

//全局头文件
#include "..\..\公共文件\GlobalDef.h"
#include "..\..\公共文件\GlobalField.h"
#include "..\..\公共文件\GlobalFrame.h"
#include "..\..\公共文件\GlobalRight.h"

//接口头文件
#include "..\..\组件接口\IUnknownEx.h"
#include "..\..\组件接口\IGamePlaza.h"

//命令头文件
#include "..\..\消息定义\CMD_Game.h"
#include "..\..\消息定义\CMD_Plaza.h"

//模板库
#include "..\..\模板库\Template.h"

//组件头文件
#include "..\头像组件\UserFace.h"
#include "..\下载组件\DownLoad.h"
#include "..\游戏等级\GameRank.h"
#include "..\关系管理\Companion.h"
#include "..\信道模块\ChannelModule.h"
#include "..\客户端共享\ClientShare.h"
#include "..\..\共享组件\公共服务\ComService.h"
#include "..\..\共享组件\网络组件\SocketModule.h"
#include "..\..\共享组件\界面控件\SkinControls.h"

#include "GamePlaza.h"

//#define HOME_WEB_URL  "http://www.cctvdodo.com/FlyGameWeb/"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../链接库/Release/Ansi/ComService.lib")
#pragma comment (lib,"../../链接库/Release/Ansi/SkinControls.lib")
#pragma comment (lib,"../../链接库/Release/Ansi/ClientShare.lib")
#pragma comment (lib,"../../链接库/Release/Ansi/ChannelModule.lib")
#else
#pragma comment (lib,"../../链接库/Release/Unicode/ComService.lib")
#pragma comment (lib,"../../链接库/Release/Unicode/SkinControls.lib")
#pragma comment (lib,"../../链接库/Release/Unicode/ClientShare.lib")
#pragma comment (lib,"../../链接库/Release/Unicode/ChannelModule.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../链接库/Debug/Ansi/ComService.lib")
#pragma comment (lib,"../../链接库/Debug/Ansi/SkinControls.lib")
#pragma comment (lib,"../../链接库/Debug/Ansi/ClientShare.lib")
#pragma comment (lib,"../../链接库/Debug/Ansi/ChannelModule.lib")
#else
#pragma comment (lib,"../../链接库/Debug/Unicode/ComService.lib")
#pragma comment (lib,"../../链接库/Debug/Unicode/SkinControls.lib")
#pragma comment (lib,"../../链接库/Debug/Unicode/ClientShare.lib")
#pragma comment (lib,"../../链接库/Debug/Unicode/ChannelModule.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////
