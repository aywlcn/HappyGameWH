#pragma once

//////////////////////////////////////////////////////////////////////////
//MFC �ļ�

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

#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <AfxCmn.h>
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
#pragma comment (lib,"../../���ӿ�/ComService.lib")
#pragma comment (lib,"../ ../���ӿ�/KernelEngine.lib")
#else
#pragma comment (lib,"../../���ӿ�/ComServiceD.lib")
#pragma comment (lib,"../../���ӿ�/KernelEngineD.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////