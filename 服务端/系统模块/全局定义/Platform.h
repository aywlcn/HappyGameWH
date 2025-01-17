#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//包含文件
#include <iostream>
//定义文件
#include "Macro.h"
#include "Define.h"

//结构文件
#include "Struct.h"
#include "Packet.h"
#include "Property.h"

//模板文件
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"
#include "util/Util.h"
#include "Log.h"

//////////////////////////////////////////////////////////////////////////////////

//程序版本
#define VERSION_FRAME				PROCESS_VERSION(0,0,1)				//框架版本
#define VERSION_PLAZA				PROCESS_VERSION(0,0,1)				//大厅版本
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(0,0,1)				//手机版本
#define VERSION_MOBILE_IOS			PROCESS_VERSION(0,0,1)				//手机版本

//版本定义
#define VERSION_EFFICACY			0									//效验版本
#define VERSION_FRAME_SDK			INTERFACE_VERSION(0,1)				//框架版本

//////////////////////////////////////////////////////////////////////////////////
//发布版本

#ifndef _DEBUG

//平台常量
const TCHAR szProduct[]=TEXT("网狐棋牌精简平台");							//产品名字
const TCHAR szPlazaClass[]=TEXT("WHSZCYWLQPGamePlaza");						//广场类名
const TCHAR szProductKey[]=TEXT("WHSZCYWLQPGamePlatform");					//产品主键

//地址定义
const TCHAR szCookieUrl[]=TEXT("http://");					//记录地址
const TCHAR szLogonServer[]=TEXT("ry.foxuc.net");						//登录地址
const TCHAR szPlatformLink[]=TEXT("http://");				//平台网站
const TCHAR szValidateKey[]=TEXT("RYSyncLoginKey");						//验证密钥
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //验证地址 

#else

//////////////////////////////////////////////////////////////////////////////////
//内测版本

//平台常量
const TCHAR szProduct[]=TEXT("网狐棋牌精华平台");						//产品名字
const TCHAR szPlazaClass[]=TEXT("WHJHGamePlaza");						//广场类名
const TCHAR szProductKey[]=TEXT("WHJHGamePlatform");					//产品主键

//地址定义
const TCHAR szCookieUrl[]=TEXT("http://ry.foxuc.net");					//记录地址
const TCHAR szLogonServer[]=TEXT("ry.foxuc.net");						//登录地址
const TCHAR szPlatformLink[]=TEXT("http://ry.foxuc.net");				//平台网站
const TCHAR szValidateKey[]=TEXT("RYSyncLoginKey");						//验证密钥
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //验证地址 

#endif

//////////////////////////////////////////////////////////////////////////////////

//数据库名
const TCHAR szPlatformDB[]=TEXT("QPPlatformDB");						//平台数据库
const TCHAR szAccountsDB[]=TEXT("QPAccountsDB");						//用户数据库
const TCHAR szTreasureDB[]=TEXT("QPTreasureDB");						//财富数据库
const TCHAR szGameMatchDB[]=TEXT("QPGameMatchDB");					//比赛数据库
const TCHAR szExerciseDB[]=TEXT("QPEducateDB"); 						//练习数据库
const TCHAR szGameScoreDB[]=TEXT("QPGameScoreDB");					//练习数据库

//////////////////////////////////////////////////////////////////////////////////

//授权信息
const TCHAR szCompilation[]=TEXT("ED56BE63-3026-465B-9DFC-17F595145F3D");

//////////////////////////////////////////////////////////////////////////////////

#endif