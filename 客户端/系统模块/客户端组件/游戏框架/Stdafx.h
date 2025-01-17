#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows 95 和 Windows NT 4 或更高版本的功能。
#define WINVER 0x0501		// 将此更改为针对于 Windows 98 和 Windows 2000 或更高版本的合适的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows NT 4 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此更改为针对于 Windows 2000 或更高版本的合适的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0501 // 将此更改为针对于 Windows Me 或更高版本的合适的值。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 4.0 或更高版本的功能。
#define _WIN32_IE 0x0400	// 将此更改为针对于 IE 5.0 或更高版本的合适的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将为显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "../../开发库/Include/flypublicdefine.h"
#define HOME_WEB_URL "http://www.cctvdodo.com/FlyGameWeb"

#include "..\..\共享组件\界面控件\SkinControls.h"

extern void WriteLog(CString strFileName, CString strText);

//////////////////////////////////////////////////////////////////////////

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