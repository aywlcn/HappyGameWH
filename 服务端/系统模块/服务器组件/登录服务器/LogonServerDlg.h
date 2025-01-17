#ifndef LOGON_SERVER_SERVER_DLG_HEAD_FILE
#define LOGON_SERVER_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"
#include ".\flashaccredit.h"

//////////////////////////////////////////////////////////////////////////////////

//配置对话框
class CSystemOptionDlg : public CDialog
{
	//构造函数
public:
	//构造程序
	CSystemOptionDlg();
	//析构函数
	virtual ~CSystemOptionDlg();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//控件子类化
	virtual void DoDataExchange(CDataExchange * pDX);
	//确定函数
	virtual void OnOK();

	//消息映射
	DECLARE_MESSAGE_MAP()
};


//主对话框
class CLogonServerDlg : public CDialog, public IServiceUnitsSink
{
	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元
	CTraceServiceControl			m_TraceServiceControl;				//追踪窗口
	CFlashAccredit                  m_FlashAccredit;                    //Flash网络

	//函数定义
public:
	//构造函数
	CLogonServerDlg();
	//析构函数
	virtual ~CLogonServerDlg();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();
	//取消函数
	virtual VOID OnCancel();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//服务接口
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);

	//按钮消息
protected:
	//启动服务
	VOID OnBnClickedStartService();
	//停止服务
	VOID OnBnClickedStopService();
	//系统配置
	void OnBnClickedInitService();

	//消息映射
public:
	//关闭询问
	BOOL OnQueryEndSession();
	//网络事件
	LRESULT OnNetworkEvent(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif