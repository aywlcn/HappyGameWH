#ifndef GLOBAL_DEF_HEAD_FILE
#define GLOBAL_DEF_HEAD_FILE

#pragma once
#include "GlobalProperty.h"

//////////////////////////////////////////////////////////////////////////
//公共定义

#define MAX_CHAIR						100								//最大椅子
#define MAX_CHAIR_NORMAL				8								//最大人数

#define MAX_ANDROID						256								//最大机器
#define MAX_CHAT_LEN					128								//聊天长度
#define LIMIT_CHAT_TIMES				1200							//限时聊天

//////////////////////////////////////////////////////////////////////////////////
//数据库定义

#define DB_ERROR 					-1  								//处理失败
#define DB_SUCCESS 					0  									//处理成功
#define DB_NEEDMB 					18 									//处理失败
#define DB_PASSPORT					19									//处理失败

/////////////////////////////////////////////////////////////////////////////////////////
//宏定义

//端口定义
#define PORT_VIDEO_SERVER				7600								//视频服务器
#define PORT_LOGON_SERVER				9001								//登陆服务器
#define PORT_CENTER_SERVER				9010								//中心服务器

//网络数据定义
#define SOCKET_VER						0x66								//网络版本
#define SOCKET_BUFFER					8192								//网络缓冲
#define SOCKET_PACKET					(SOCKET_BUFFER-sizeof(CMD_Head)-2*sizeof(DWORD))

/////////////////////////////////////////////////////////////////////////////////////////

//内核命令码
#define MDM_KN_COMMAND					0									//内核命令
#define SUB_KN_DETECT_SOCKET			1									//检测命令
#define SUB_KN_SHUT_DOWN_SOCKET			2									//中断网络

//网络内核
struct CMD_Info
{
	BYTE								cbVersion;							//版本标识
	BYTE								cbCheckCode;						//效验字段
	WORD								wPacketSize;						//数据大小
};

//网络命令
struct CMD_Command
{
	WORD								wMainCmdID;							//主命令码
	WORD								wSubCmdID;							//子命令码
};

//网络包头
struct CMD_Head
{
	CMD_Info							CmdInfo;							//基础结构
	CMD_Command							CommandInfo;						//命令信息
};

//网络包缓冲
struct CMD_Buffer
{
	CMD_Head							Head;								//数据包头
	BYTE								cbBuffer[SOCKET_PACKET];			//数据缓冲
};

//检测结构信息
struct CMD_KN_DetectSocket
{
	DWORD								dwSendTickCount;					//发送时间
	DWORD								dwRecvTickCount;					//接收时间
};

/////////////////////////////////////////////////////////////////////////////////////////

//IPC 数据定义
#define IPC_VER							0x0001								//IPC 版本
#define IPC_IDENTIFIER					0x0001								//标识号码
#define IPC_PACKAGE						4096								//最大 IPC 包
#define IPC_BUFFER						(sizeof(IPC_Head)+IPC_PACKAGE)		//缓冲长度

//IPC 数据包头
struct IPC_Head
{
	WORD								wVersion;							//IPC 版本
	WORD								wDataSize;							//数据大小
	WORD								wMainCmdID;							//主命令码
	WORD								wSubCmdID;							//子命令码
};

//IPC 缓冲结构
struct IPC_Buffer
{
	IPC_Head							Head;								//数据包头
	BYTE								cbBuffer[IPC_PACKAGE];				//数据缓冲
};

//////////////////////////////////////////////////////////////////////////

//长度宏定义
#define TYPE_LEN						32									//种类长度
#define KIND_LEN						32									//类型长度
#define STATION_LEN						32									//站点长度
#define SERVER_LEN						32									//房间长度
#define MODULE_LEN						32									//进程长度

//性别定义
#define GENDER_NULL						0									//未知性别
#define GENDER_BOY						1									//男性性别
#define GENDER_GIRL						2									//女性性别

//游戏类型
#define GAME_GENRE_SCORE				0x0001								//点值类型
#define GAME_GENRE_GOLD					0x0002								//金币类型
#define GAME_GENRE_MATCH				0x0004								//比赛类型
#define GAME_GENRE_EDUCATE				0x0008								//训练类型

//游戏类型结构
struct tagGameType
{
	WORD								wSortID;							//排序号码
	WORD								wTypeID;							//种类号码
	TCHAR								szTypeName[TYPE_LEN];				//种类名字
};

//游戏名称结构
struct tagGameKind
{
	WORD								wSortID;							//排序号码
	WORD								wTypeID;							//类型号码
	WORD								wKindID;							//名称号码
	DWORD								dwMaxVersion;						//最新版本
	DWORD								dwOnLineCount;						//在线数目
	TCHAR								szKindName[KIND_LEN];				//游戏名字
	TCHAR								szProcessName[MODULE_LEN];			//进程名字
};

//游戏站点结构
struct tagGameStation
{
	WORD								wSortID;							//排序号码
	WORD								wKindID;							//名称号码
	WORD								wJoinID;							//挂接号码
	WORD								wStationID;							//站点号码
	TCHAR								szStationName[STATION_LEN];			//站点名称
};

//游戏房间列表结构
struct tagGameServer
{
	WORD								wSortID;							//排序号码
	WORD								wKindID;							//名称号码
	WORD								wServerID;							//房间号码
	WORD								wStationID;							//站点号码
	WORD								wServerPort;						//房间端口
	DWORD								dwServerAddr;						//房间地址
	DWORD								dwOnLineCount;						//在线人数
	TCHAR								szServerName[SERVER_LEN];			//房间名称
};

//游戏级别结构
struct tagLevelItem
{
	LONG								lLevelScore;						//级别积分
	WCHAR								szLevelName[16];					//级别描述
};
//////////////////////////////////////////////////////////////////////////

//用户状态定义
#define US_NULL							0x00								//没有状态
#define US_FREE							0x01								//站立状态
#define US_SIT							0x02								//坐下状态
#define US_READY						0x03								//同意状态
#define US_LOOKON						0x04								//旁观状态
#define US_PLAY							0x05								//游戏状态
#define US_OFFLINE						0x06								//断线状态

//长度宏定义
#define NAME_LEN						32									//名字长度
#define PASS_LEN						33									//密码长度
#define EMAIL_LEN						32									//邮箱长度
#define GROUP_LEN						32									//社团长度
#define COMPUTER_ID_LEN					33									//机器序列
#define UNDER_WRITE_LEN					32									//个性签名

//用户积分信息
struct tagUserScore
{
	LONG								lScore;								//用户分数
	LONG								lGameGold;							//游戏金币//
	LONG								lInsureScore;						//存储金币
	LONG								lWinCount;							//胜利盘数
	LONG								lLostCount;							//失败盘数
	LONG								lDrawCount;							//和局盘数
	LONG								lFleeCount;							//断线数目
	LONG								lExperience;						//用户经验
};

//用户状态信息
struct tagUserStatus
{
	WORD								wTableID;							//桌子号码
	WORD								wChairID;							//椅子位置
	BYTE								cbUserStatus;						//用户状态
};

//用户基本信息结构
struct tagUserInfoHead
{
	//用户属性
	WORD								wFaceID;							//头像索引
	DWORD								dwUserID;							//用户 I D
	DWORD								dwGameID;							//游戏 I D
	DWORD								dwGroupID;							//社团索引
	DWORD								dwUserRight;						//用户等级
	LONG								lLoveliness;						//用户魅力
	DWORD								dwMasterRight;						//管理权限

	//用户属性
	BYTE								cbGender;							//用户性别
	BYTE								cbMemberOrder;						//会员等级
	BYTE								cbMasterOrder;						//管理等级

	//用户状态
	WORD								wTableID;							//桌子号码
	WORD								wChairID;							//椅子位置
	BYTE								cbUserStatus;						//用户状态

	//用户积分
	tagUserScore						UserScoreInfo;						//积分信息

	//扩展信息
	//LONG								lInsureScore;						//消费金币
	//LONG								lGameGold;							//游戏金币
	DWORD								dwCustomFaceVer;					//上传头像
	DWORD								dwPropResidualTime[PROPERTY_COUNT];	//道具时间
};

//用户信息结构
struct tagUserData
{
	//用户属性
	WORD								wFaceID;							//头像索引
	DWORD								dwCustomFaceVer;					//上传头像
	DWORD								dwUserID;							//用户 I D
	DWORD								dwGroupID;							//社团索引
	DWORD								dwGameID;							//用户 I D
	DWORD								dwUserRight;						//用户等级
	LONG								lLoveliness;						//用户魅力
	DWORD								dwMasterRight;						//管理权限
	TCHAR								szName[NAME_LEN];					//用户名字
	TCHAR								szGroupName[GROUP_LEN];				//社团名字
	TCHAR								szUnderWrite[UNDER_WRITE_LEN];		//个性签名

	//用户属性
	BYTE								cbGender;							//用户性别
	BYTE								cbMemberOrder;						//会员等级
	BYTE								cbMasterOrder;						//管理等级

	//用户积分
	LONG								lInsureScore;						//消费金币
	LONG								lGameGold;							//游戏金币
	LONG								lScore;								//用户分数
	LONG								lWinCount;							//胜利盘数
	LONG								lLostCount;							//失败盘数
	LONG								lDrawCount;							//和局盘数
	LONG								lFleeCount;							//断线数目
	LONG								lExperience;						//用户经验

	//用户状态
	WORD								wTableID;							//桌子号码
	WORD								wChairID;							//椅子位置
	BYTE								cbUserStatus;						//用户状态

	//其他信息
	BYTE								cbCompanion;						//用户关系
	DWORD								dwPropResidualTime[PROPERTY_COUNT];	//道具时间
};

//////////////////////////////////////////////////////////////////////////

//机器序列号结构
struct tagClientSerial
{
	DWORD								dwSystemVer;						//系统版本
	DWORD								dwComputerID[3];					//机器序列
};

//配置缓冲结构
struct tagOptionBuffer
{
	BYTE								cbBufferLen;						//数据长度
	BYTE								cbOptionBuf[32];					//设置缓冲
};

//////////////////////////////////////////////////////////////////////////

//加密密钥
const DWORD g_dwPacketKey=0xA55AA55A;

//发送映射
const BYTE g_SendByteMap[256]=
{
	0x70,0x2F,0x40,0x5F,0x44,0x8E,0x6E,0x45,0x7E,0xAB,0x2C,0x1F,0xB4,0xAC,0x9D,0x91,
	0x0D,0x36,0x9B,0x0B,0xD4,0xC4,0x39,0x74,0xBF,0x23,0x16,0x14,0x06,0xEB,0x04,0x3E,
	0x12,0x5C,0x8B,0xBC,0x61,0x63,0xF6,0xA5,0xE1,0x65,0xD8,0xF5,0x5A,0x07,0xF0,0x13,
	0xF2,0x20,0x6B,0x4A,0x24,0x59,0x89,0x64,0xD7,0x42,0x6A,0x5E,0x3D,0x0A,0x77,0xE0,
	0x80,0x27,0xB8,0xC5,0x8C,0x0E,0xFA,0x8A,0xD5,0x29,0x56,0x57,0x6C,0x53,0x67,0x41,
	0xE8,0x00,0x1A,0xCE,0x86,0x83,0xB0,0x22,0x28,0x4D,0x3F,0x26,0x46,0x4F,0x6F,0x2B,
	0x72,0x3A,0xF1,0x8D,0x97,0x95,0x49,0x84,0xE5,0xE3,0x79,0x8F,0x51,0x10,0xA8,0x82,
	0xC6,0xDD,0xFF,0xFC,0xE4,0xCF,0xB3,0x09,0x5D,0xEA,0x9C,0x34,0xF9,0x17,0x9F,0xDA,
	0x87,0xF8,0x15,0x05,0x3C,0xD3,0xA4,0x85,0x2E,0xFB,0xEE,0x47,0x3B,0xEF,0x37,0x7F,
	0x93,0xAF,0x69,0x0C,0x71,0x31,0xDE,0x21,0x75,0xA0,0xAA,0xBA,0x7C,0x38,0x02,0xB7,
	0x81,0x01,0xFD,0xE7,0x1D,0xCC,0xCD,0xBD,0x1B,0x7A,0x2A,0xAD,0x66,0xBE,0x55,0x33,
	0x03,0xDB,0x88,0xB2,0x1E,0x4E,0xB9,0xE6,0xC2,0xF7,0xCB,0x7D,0xC9,0x62,0xC3,0xA6,
	0xDC,0xA7,0x50,0xB5,0x4B,0x94,0xC0,0x92,0x4C,0x11,0x5B,0x78,0xD9,0xB1,0xED,0x19,
	0xE9,0xA1,0x1C,0xB6,0x32,0x99,0xA3,0x76,0x9E,0x7B,0x6D,0x9A,0x30,0xD6,0xA9,0x25,
	0xC7,0xAE,0x96,0x35,0xD0,0xBB,0xD2,0xC8,0xA2,0x08,0xF3,0xD1,0x73,0xF4,0x48,0x2D,
	0x90,0xCA,0xE2,0x58,0xC1,0x18,0x52,0xFE,0xDF,0x68,0x98,0x54,0xEC,0x60,0x43,0x0F
};

//接收映射
const BYTE g_RecvByteMap[256]=
{
	0x51,0xA1,0x9E,0xB0,0x1E,0x83,0x1C,0x2D,0xE9,0x77,0x3D,0x13,0x93,0x10,0x45,0xFF,
	0x6D,0xC9,0x20,0x2F,0x1B,0x82,0x1A,0x7D,0xF5,0xCF,0x52,0xA8,0xD2,0xA4,0xB4,0x0B,
	0x31,0x97,0x57,0x19,0x34,0xDF,0x5B,0x41,0x58,0x49,0xAA,0x5F,0x0A,0xEF,0x88,0x01,
	0xDC,0x95,0xD4,0xAF,0x7B,0xE3,0x11,0x8E,0x9D,0x16,0x61,0x8C,0x84,0x3C,0x1F,0x5A,
	0x02,0x4F,0x39,0xFE,0x04,0x07,0x5C,0x8B,0xEE,0x66,0x33,0xC4,0xC8,0x59,0xB5,0x5D,
	0xC2,0x6C,0xF6,0x4D,0xFB,0xAE,0x4A,0x4B,0xF3,0x35,0x2C,0xCA,0x21,0x78,0x3B,0x03,
	0xFD,0x24,0xBD,0x25,0x37,0x29,0xAC,0x4E,0xF9,0x92,0x3A,0x32,0x4C,0xDA,0x06,0x5E,
	0x00,0x94,0x60,0xEC,0x17,0x98,0xD7,0x3E,0xCB,0x6A,0xA9,0xD9,0x9C,0xBB,0x08,0x8F,
	0x40,0xA0,0x6F,0x55,0x67,0x87,0x54,0x80,0xB2,0x36,0x47,0x22,0x44,0x63,0x05,0x6B,
	0xF0,0x0F,0xC7,0x90,0xC5,0x65,0xE2,0x64,0xFA,0xD5,0xDB,0x12,0x7A,0x0E,0xD8,0x7E,
	0x99,0xD1,0xE8,0xD6,0x86,0x27,0xBF,0xC1,0x6E,0xDE,0x9A,0x09,0x0D,0xAB,0xE1,0x91,
	0x56,0xCD,0xB3,0x76,0x0C,0xC3,0xD3,0x9F,0x42,0xB6,0x9B,0xE5,0x23,0xA7,0xAD,0x18,
	0xC6,0xF4,0xB8,0xBE,0x15,0x43,0x70,0xE0,0xE7,0xBC,0xF1,0xBA,0xA5,0xA6,0x53,0x75,
	0xE4,0xEB,0xE6,0x85,0x14,0x48,0xDD,0x38,0x2A,0xCC,0x7F,0xB1,0xC0,0x71,0x96,0xF8,
	0x3F,0x28,0xF2,0x69,0x74,0x68,0xB7,0xA3,0x50,0xD0,0x79,0x1D,0xFC,0xCE,0x8A,0x8D,
	0x2E,0x62,0x30,0xEA,0xED,0x2B,0x26,0xB9,0x81,0x7C,0x46,0x89,0x73,0xA2,0xF7,0x72
};

//////////////////////////////////////////////////////////////////////////

#endif