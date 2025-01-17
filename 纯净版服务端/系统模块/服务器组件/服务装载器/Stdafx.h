#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 标头中排除不常使用的资料
#endif

#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x0501		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0501		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0501 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0400	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类

#include <afxdtctl.h>		// Internet Explorer 4 公共控件的 MFC 支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 公共控件的 MFC 支持
#endif // _AFX_NO_AFXCMN_SUPPORT

//////////////////////////////////////////////////////////////////////////

//公共头文件
#include "..\..\公共文件\GlobalDef.h"
#include "..\..\公共文件\GlobalService.h"

//组件头文件
#include "..\内核引擎\KernelEngineHead.h"
#include "..\游戏服务\GameServiceExport.h"
#include "..\..\共享组件\公共服务\ComService.h"
#include "..\..\共享组件\界面控件\SkinControls.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
#pragma comment (lib,"../../链接库/ComService.lib")
#pragma comment (lib,"../ ../链接库/KernelEngine.lib")
#else
#pragma comment (lib,"../../链接库/ComServiceD.lib")
#pragma comment (lib,"../../链接库/KernelEngineD.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////