#include "StdAfx.h"
#include "TraceService.h" //
#include "TCPNetworkEngine.h"

#include "../../消息定义/pb/NullPmd.pb.h"
#include "../../消息定义/pb/NullPmd.pb.cc"
//////////////////////////////////////////////////////////////////////////
//宏定义

#define TIME_BREAK_READY			9000L								//中断时间
#define TIME_BREAK_CONNECT			4000L								//中断时间
#define TIME_DETECT_SOCKET			20000L								//监测时间

//////////////////////////////////////////////////////////////////////////

//动作定义
#define QUEUE_SEND_REQUEST			1									//发送标识
#define QUEUE_SAFE_CLOSE			2									//安全关闭
#define QUEUE_ALLOW_BATCH			3									//允许群发
#define QUEUE_DETECT_SOCKET			4									//检测连接

//发送请求结构
struct tagSendDataRequest
{
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	DWORD							dwSocketID;							//连接索引
	WORD							wDataSize;							//数据大小
	BYTE							cbSendBuf[SOCKET_BUFFER];			//发送缓冲
};

//设置群发
struct tagAllowBatchSend
{
	DWORD							dwSocketID;							//连接索引
	BYTE							cbAllow;							//允许标志
};

//安全关闭
struct tagSafeCloseSocket
{
	DWORD							dwSocketID;							//连接索引
};

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLapped::COverLapped(enOperationType OperationType) : m_OperationType(OperationType)
{
	memset(&m_WSABuffer, 0, sizeof(m_WSABuffer));
	memset(&m_OverLapped, 0, sizeof(m_OverLapped));
}

//析构函数
COverLapped::~COverLapped()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLappedSend::COverLappedSend() : COverLapped(OperationType_Send)
{
	m_WSABuffer.len = 0;
	m_WSABuffer.buf = (char *)m_cbBuffer;
}

//析构函数
COverLappedSend::~COverLappedSend()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerSocketItem::CServerSocketItem(WORD wIndex, IServerSocketItemSink * pIServerSocketItemSink)
		: m_wIndex(wIndex), m_pIServerSocketItemSink(pIServerSocketItemSink)
{
	m_wRountID = 1;
	m_wRecvSize = 0;
	m_cbSendRound = 0;
	m_cbRecvRound = 0;
	m_bNotify = true;
	m_bRecvIng = false;
	m_bCloseIng = false;
	m_bAllowBatch = true;
	m_dwSendXorKey = 0;
	m_dwRecvXorKey = 0;
	m_dwClientAddr = 0;
	m_dwConnectTime = 0;
	m_dwSendTickCount = 0;
	m_dwRecvTickCount = 0;
	m_dwSendPacketCount = 0;
	m_dwRecvPacketCount = 0;
	m_hSocket = INVALID_SOCKET;

	m_connectType = CHECKING;
}

//析够函数
CServerSocketItem::~CServerSocketItem(void)
{
	//删除空闲重叠 IO
	INT_PTR iCount = m_OverLappedSendFree.GetCount();
	for (INT_PTR i = 0; i < iCount; i++) delete m_OverLappedSendFree[i];
	m_OverLappedSendFree.RemoveAll();

	//删除活动重叠 IO
	iCount = m_OverLappedSendActive.GetCount();
	for (INT_PTR i = 0; i < iCount; i++) delete m_OverLappedSendActive[i];
	m_OverLappedSendActive.RemoveAll();

	return;
}

//随机映射
WORD CServerSocketItem::SeedRandMap(WORD wSeed)
{
	DWORD dwHold = wSeed;
	return (WORD)((dwHold = dwHold * 241103L + 2533101L) >> 16);
}

//映射发送数据
BYTE CServerSocketItem::MapSendByte(BYTE const cbData)
{
	BYTE cbMap = g_SendByteMap[(BYTE)(cbData+m_cbSendRound)];
	m_cbSendRound += 3;
	return cbMap;
}

//映射接收数据
BYTE CServerSocketItem::MapRecvByte(BYTE const cbData)
{
	BYTE cbMap = g_RecvByteMap[cbData] - m_cbRecvRound;
	m_cbRecvRound += 3;
	return cbMap;
}

//获取发送结构
COverLappedSend * CServerSocketItem::GetSendOverLapped()
{
	//寻找空闲结构
	COverLappedSend * pOverLappedSend = NULL;
	INT_PTR nFreeCount = m_OverLappedSendFree.GetCount();
	if (nFreeCount > 0)
	{
		pOverLappedSend = m_OverLappedSendFree[nFreeCount-1];
		ASSERT(pOverLappedSend != NULL);
		m_OverLappedSendFree.RemoveAt(nFreeCount - 1);
		m_OverLappedSendActive.Add(pOverLappedSend);
		pOverLappedSend->m_WSABuffer.len = 0;
		ZeroMemory(pOverLappedSend->m_cbBuffer, sizeof(pOverLappedSend->m_cbBuffer));
		return pOverLappedSend;
	}

	//新建发送结构
	try
	{
		pOverLappedSend = new COverLappedSend;
		ASSERT(pOverLappedSend != NULL);
		m_OverLappedSendActive.Add(pOverLappedSend);
		return pOverLappedSend;
	}
	catch (...) { }
	return NULL;
}

//绑定对象
DWORD CServerSocketItem::Attach(SOCKET hSocket, DWORD dwClientAddr)
{
	//效验数据
	ASSERT(dwClientAddr != 0);
	ASSERT(m_bRecvIng == false);
	ASSERT(IsValidSocket() == false);
	ASSERT(hSocket != INVALID_SOCKET);

	//设置变量
	m_bNotify = false;
	m_bRecvIng = false;
	m_bCloseIng = false;
	m_hSocket = hSocket;
	m_dwClientAddr = dwClientAddr;
	m_dwRecvTickCount = GetTickCount();
	m_dwConnectTime = (DWORD)time(NULL);
	m_connectType = CHECKING;
	//发送通知
	m_pIServerSocketItemSink->OnSocketAcceptEvent(this);

	return GetSocketID();
}

//发送函数
bool CServerSocketItem::SendData(WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//效验状态
	if (m_bCloseIng == true) return false;
	if (m_wRountID != wRountID) return false;
	if (m_dwRecvPacketCount == 0) return false;
	if (IsValidSocket() == false) return false;

	try
	{
		switch (m_connectType) {
		case WEBSOCKET: {
			NullPmd::message message;
			message.Clear();
			NullPmd::head* head = message.mutable_head();
			NullPmd::command* command = head->mutable_command();
			NullPmd::info* info = head->mutable_info();

			command->set_mainid(wMainCmdID);
			command->set_subid(wSubCmdID);
			info->set_cbversion(SOCKET_VER);

			int buffsize = message.ByteSize();
			if (buffsize > SOCKET_PACKET) {
				throw TEXT("发送包太大");
			}
			std::shared_ptr<char> sendData(new char[buffsize]);
			if (message.SerializeToArray(sendData.get(), buffsize) == false) {
				throw TEXT("Serialize error");
			}

			char cbSendData[SOCKET_BUFFER];
			int encodesize = zl::net::ws::encodeFrame(zl::net::ws::WsFrameType::WS_BINARY_FRAME, sendData.get(), buffsize, cbSendData, SOCKET_BUFFER);

			SendRawData(cbSendData, encodesize);
			break;
		}
		case WINSOCKET: {
			//寻找发送结构
			COverLappedSend * pOverLappedSend = GetSendOverLapped();
			ASSERT(pOverLappedSend != NULL);
			if (pOverLappedSend == NULL) return false;

			//构造数据
			CMD_Head * pHead = (CMD_Head *)pOverLappedSend->m_cbBuffer;
			pHead->CommandInfo.wMainCmdID = wMainCmdID;
			pHead->CommandInfo.wSubCmdID = wSubCmdID;
			WORD wSendSize = EncryptBuffer(pOverLappedSend->m_cbBuffer, sizeof(CMD_Head), sizeof(pOverLappedSend->m_cbBuffer));
			pOverLappedSend->m_WSABuffer.len = wSendSize;

			//发送数据
			if (m_OverLappedSendActive.GetCount() == 1)
			{
				DWORD dwThancferred = 0;
				int iRetCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
				if ((iRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
				{
					OnSendCompleted(pOverLappedSend, 0L);
					return false;
				}
			}
			break;
		}
		}
	}
	catch (LPCTSTR pszMessage) {
		// 错误信息
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("SendData异常:wMainCmdID:%d,wSubCmdID:%d,%s"), wMainCmdID, wSubCmdID, pszMessage);
		CTraceService::TraceString(szString, TraceLevel_Exception);
		return false;
	}
	catch (...)
	{
		// 错误信息
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("SendData异常:wMainCmdID:%d,wSubCmdID:%d"), wMainCmdID, wSubCmdID);
		CTraceService::TraceString(szString, TraceLevel_Exception);
		CloseSocket(m_wRountID);
		return false;
	}
	return true;
}

//发送函数
bool CServerSocketItem::SendData(void * pData, WORD wDataSize, WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//效验参数
	ASSERT(wDataSize <= SOCKET_BUFFER);

	//效验状态
	if (m_bCloseIng == true) return false;
	if (m_wRountID != wRountID) return false;
	if (m_dwRecvPacketCount == 0) return false;
	if (IsValidSocket() == false) return false;
	if (wDataSize > SOCKET_BUFFER) return false;

	try
	{
		switch (m_connectType) {
		case WEBSOCKET: {
			if (wDataSize == 0) {
				return SendData(wMainCmdID, wSubCmdID, wRountID);
			}

			NullPmd::message message;
			message.Clear();
			NullPmd::head* head = message.mutable_head();
			NullPmd::command* command = head->mutable_command();
			NullPmd::info* info = head->mutable_info();

			command->set_mainid(wMainCmdID);
			command->set_subid(wSubCmdID);
			message.set_data((char*)pData);
			info->set_wpacketsize(wDataSize);
			info->set_cbversion(SOCKET_VER);

			int buffsize = message.ByteSize();
			if (buffsize > SOCKET_PACKET) {
				throw TEXT("发送包太大");
			}
			std::shared_ptr<char> sendData(new char[buffsize]);
			if (message.SerializeToArray(sendData.get(), buffsize) == false) {
				throw TEXT("Serialize error");
			}

			char cbSendData[SOCKET_BUFFER];
			int encodesize = zl::net::ws::encodeFrame(zl::net::ws::WsFrameType::WS_BINARY_FRAME, sendData.get(), buffsize, cbSendData, SOCKET_BUFFER);

			SendRawData(cbSendData, encodesize);
			break;
		}
		case WINSOCKET: {
			//寻找发送结构
			COverLappedSend * pOverLappedSend = GetSendOverLapped();
			ASSERT(pOverLappedSend != NULL);
			if (pOverLappedSend == NULL) return false;

			//构造数据
			CMD_Head * pHead = (CMD_Head *)pOverLappedSend->m_cbBuffer;
			pHead->CommandInfo.wMainCmdID = wMainCmdID;
			pHead->CommandInfo.wSubCmdID = wSubCmdID;
			if (wDataSize > 0)
			{
				ASSERT(pData != NULL);
				memcpy(pHead + 1, pData, wDataSize);
			}
			WORD wSendSize = EncryptBuffer(pOverLappedSend->m_cbBuffer, sizeof(CMD_Head) + wDataSize, sizeof(pOverLappedSend->m_cbBuffer));
			pOverLappedSend->m_WSABuffer.len = wSendSize;

			//发送数据
			if (m_OverLappedSendActive.GetCount() == 1)
			{
				DWORD dwThancferred = 0;
				int iRetCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
				if ((iRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
				{
					OnSendCompleted(pOverLappedSend, 0L);
					return false;
				}
			}
			break;
		}
		}
	}
	catch (LPCTSTR pszMessage) {
		// 错误信息
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("SendData 异常:wMainCmdID:%d,wSubCmdID:%d,%s"), wMainCmdID, wSubCmdID,pszMessage);
		CTraceService::TraceString(szString, TraceLevel_Exception);
		return false;
	}
	catch (...)
	{
		// 错误信息
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("SendData 异常:wMainCmdID:%d,wSubCmdID:%d"), wMainCmdID, wSubCmdID);
		CTraceService::TraceString(szString, TraceLevel_Exception);
		CloseSocket(m_wRountID);
		return false;
	}
	return true;
}

//投递接收
bool CServerSocketItem::RecvData()
{
	//效验变量
	ASSERT(m_bRecvIng == false);
	ASSERT(m_hSocket != INVALID_SOCKET);

	//判断关闭
	if (m_bCloseIng == true)
	{
		if (m_OverLappedSendActive.GetCount() == 0) CloseSocket(m_wRountID);
		return false;
	}

	//接收数据
	m_bRecvIng = true;
	DWORD dwThancferred = 0, dwFlags = 0;
	int iRetCode = WSARecv(m_hSocket, &m_OverLappedRecv.m_WSABuffer, 1, &dwThancferred, &dwFlags, &m_OverLappedRecv.m_OverLapped, NULL);
	if ((iRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
	{
		m_bRecvIng = false;
		CloseSocket(m_wRountID);
		return false;
	}

	return true;
}

//关闭连接
bool CServerSocketItem::CloseSocket(WORD wRountID)
{
	//状态判断
	if (m_wRountID != wRountID) return false;

	//关闭连接
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	//判断关闭
	if ((m_bRecvIng == false) && (m_OverLappedSendActive.GetCount() == 0)) OnCloseCompleted();

	return true;
}

//设置关闭
bool CServerSocketItem::ShutDownSocket(WORD wRountID)
{
	return true;

	//状态判断
	if (m_wRountID != wRountID) return false;
	if (m_hSocket == INVALID_SOCKET) return false;

	//设置变量
	if (m_bCloseIng == false)
	{
		m_bCloseIng = true;
		//if (m_OverLappedSendActive.GetCount() == 0) CloseSocket(wRountID);
	}

	return true;
}

//允许群发
bool CServerSocketItem::AllowBatchSend(WORD wRountID, bool bAllowBatch)
{
	//状态判断
	if (m_wRountID != wRountID) return false;
	if (m_hSocket == INVALID_SOCKET) return false;

	//设置变量
	m_bAllowBatch = bAllowBatch;

	return true;
}

//重置变量
void CServerSocketItem::ResetSocketData()
{
	//效验状态
	ASSERT(m_hSocket == INVALID_SOCKET);

	//重置数据
	m_wRountID++;
	m_wRecvSize = 0;
	m_cbSendRound = 0;
	m_cbRecvRound = 0;
	m_dwSendXorKey = 0;
	m_dwRecvXorKey = 0;
	m_dwClientAddr = 0;
	m_dwConnectTime = 0;
	m_dwSendTickCount = 0;
	m_dwRecvTickCount = 0;
	m_dwSendPacketCount = 0;
	m_dwRecvPacketCount = 0;

	//状态变量
	m_bNotify = true;
	m_bRecvIng = false;
	m_bCloseIng = false;
	m_bAllowBatch = true;
	m_OverLappedSendFree.Append(m_OverLappedSendActive);
	m_OverLappedSendActive.RemoveAll();
	m_connectType = CHECKING;
	return;
}

//发送完成函数
bool CServerSocketItem::OnSendCompleted(COverLappedSend * pOverLappedSend, DWORD dwThancferred)
{
	//效验变量
	ASSERT(pOverLappedSend != NULL);
	ASSERT(m_OverLappedSendActive.GetCount() > 0);
	ASSERT(pOverLappedSend == m_OverLappedSendActive[0]);

	//释放发送结构
	m_OverLappedSendActive.RemoveAt(0);
	m_OverLappedSendFree.Add(pOverLappedSend);

	//设置变量
	if (dwThancferred != 0) m_dwSendTickCount = GetTickCount();

	//判断关闭
	if (m_hSocket == INVALID_SOCKET)
	{
		m_OverLappedSendFree.Append(m_OverLappedSendActive);
		m_OverLappedSendActive.RemoveAll();
		CloseSocket(m_wRountID);
		return true;
	}

	//继续发送数据
	if (m_OverLappedSendActive.GetCount() > 0)
	{
		DWORD dwThancferred = 0;
		pOverLappedSend = m_OverLappedSendActive[0];
		ASSERT(pOverLappedSend != NULL);
		int iRetCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
		if ((iRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			m_OverLappedSendFree.Append(m_OverLappedSendActive);
			m_OverLappedSendActive.RemoveAll();
			CloseSocket(m_wRountID);
			return false;
		}
		return true;
	}

	//判断关闭
	if (m_bCloseIng == true)
		CloseSocket(m_wRountID);

	return true;
}

//接收完成函数
bool CServerSocketItem::OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred)
{
	//效验数据
	ASSERT(m_bRecvIng == true);

	//设置变量
	m_bRecvIng = false;
	m_dwRecvTickCount = GetTickCount();

	//判断关闭
	if (m_hSocket == INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//接收数据
	int iRetCode = recv(m_hSocket, (char *)m_cbRecvBuf + m_wRecvSize, sizeof(m_cbRecvBuf) - m_wRecvSize, 0);
	if (iRetCode <= 0)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//接收完成
	m_wRecvSize += iRetCode;

	if (m_connectType == CHECKING) {
		if (CheckIsWinSocket() == true) {
			m_connectType = WINSOCKET;
		}
		else {
			m_connectType = HANDSHAKING;
		}
	}

	//处理数据
	try
	{
		switch (m_connectType) {
		case HANDSHAKING: {
			const char kDoubleCRLF[] = "\r\n\r\n";
			char* recvChar = (char*)m_cbRecvBuf;
			const char* pos = std::search(recvChar, recvChar + m_wRecvSize, kDoubleCRLF, kDoubleCRLF + 4);
			if (pos == recvChar + m_wRecvSize) {
				return RecvData();
			}
			zl::net::ByteBuffer handshakeBuffer;
			handshakeBuffer.write(recvChar, m_wRecvSize);
			bool success = handshake(&handshakeBuffer);
			if (success == true) {
				m_connectType = WEBSOCKET;
				m_wRecvSize = 0;
			}
			else {
				throw TEXT("握手失败");
			}
			break;
		}
		case WINSOCKET: {
			BYTE cbBuffer[SOCKET_BUFFER];
			CMD_Head * pHead = (CMD_Head *)m_cbRecvBuf;
			while (m_wRecvSize >= sizeof(CMD_Head))
			{
				//效验数据
				WORD wPacketSize = pHead->CmdInfo.wPacketSize;
				if (wPacketSize > SOCKET_BUFFER) throw TEXT("数据包超长");
				if (wPacketSize < sizeof(CMD_Head)) throw TEXT("数据包非法");
				if (pHead->CmdInfo.cbVersion != SOCKET_VER) throw TEXT("数据包版本错误");
				if (m_wRecvSize < wPacketSize) break;

				//提取数据
				CopyMemory(cbBuffer, m_cbRecvBuf, wPacketSize);
				WORD wRealySize = CrevasseBuffer(cbBuffer, wPacketSize);
				ASSERT(wRealySize >= sizeof(CMD_Head));
				m_dwRecvPacketCount++;

				//解释数据
				WORD wDataSize = wRealySize - sizeof(CMD_Head);
				void * pDataBuffer = cbBuffer + sizeof(CMD_Head);
				CMD_Command Command = ((CMD_Head *)cbBuffer)->CommandInfo;

				//内核命令
				if (Command.wMainCmdID == MDM_KN_COMMAND)
				{
					switch (Command.wSubCmdID)
					{
					case SUB_KN_DETECT_SOCKET:	//网络检测
					{
						break;
					}
					default:
					{
						throw TEXT("非法命令码");
					}
					}
				}
				else
				{
					//消息处理
					m_pIServerSocketItemSink->OnSocketReadEvent(Command, pDataBuffer, wDataSize, this);
				}

				//删除缓存数据
				m_wRecvSize -= wPacketSize;
				MoveMemory(m_cbRecvBuf, m_cbRecvBuf + wPacketSize, m_wRecvSize);
			}
			break;
		}
		case WEBSOCKET: {
			if (HandleWebsocketRecv() == false) {
				throw TEXT("接收数据异常");
			}
			break;
		}
		}
	}
	catch (LPCTSTR pszMessage) {
		// 错误信息
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("OnRecvCompleted 异常:%s"), pszMessage);
		CTraceService::TraceString(szString, TraceLevel_Exception);
		CloseSocket(m_wRountID);
		return false;
	}
	catch (...)
	{
		CloseSocket(m_wRountID);
		return false;
	}

	return RecvData();
}

//关闭完成通知
bool CServerSocketItem::OnCloseCompleted()
{
	//效验状态
	ASSERT(m_hSocket == INVALID_SOCKET);
	ASSERT(m_OverLappedSendActive.GetCount() == 0);

	//关闭事件
	ASSERT(m_bNotify == false);
	if (m_bNotify == false)
	{
		m_bNotify = true;
		m_pIServerSocketItemSink->OnSocketCloseEvent(this);
	}

	//状态变量
	ResetSocketData();

	return true;
}

//加密数据
WORD CServerSocketItem::EncryptBuffer(BYTE pcbDataBuffer[], WORD wDataSize, WORD wBufferSize)
{
	WORD i = 0;
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_Head));
	ASSERT(wDataSize <= (sizeof(CMD_Head) + SOCKET_BUFFER));
	ASSERT(wBufferSize >= (wDataSize + 2*sizeof(DWORD)));

	//调整长度
	WORD wEncryptSize = wDataSize - sizeof(CMD_Info), wSnapCount = 0;
	if ((wEncryptSize % sizeof(DWORD)) != 0)
	{
		wSnapCount = sizeof(DWORD) - wEncryptSize % sizeof(DWORD);
		memset(pcbDataBuffer + sizeof(CMD_Info) + wEncryptSize, 0, wSnapCount);
	}

	//效验码与字节映射
	BYTE cbCheckCode = 0;
	for (i = sizeof(CMD_Info); i < wDataSize; i++)
	{
		cbCheckCode += pcbDataBuffer[i];
		pcbDataBuffer[i] = MapSendByte(pcbDataBuffer[i]);
	}

	//填写信息头
	CMD_Head * pHead = (CMD_Head *)pcbDataBuffer;
	pHead->CmdInfo.cbCheckCode = ~cbCheckCode + 1;
	pHead->CmdInfo.wPacketSize = wDataSize;
	pHead->CmdInfo.cbVersion = SOCKET_VER;

	//加密数据
	DWORD dwXorKey = m_dwSendXorKey;
	WORD * pwSeed = (WORD *)(pcbDataBuffer + sizeof(CMD_Info));
	DWORD * pdwXor = (DWORD *)(pcbDataBuffer + sizeof(CMD_Info));
	WORD wEncrypCount = (wEncryptSize + wSnapCount) / sizeof(DWORD);
	for (i = 0; i < wEncrypCount; i++)
	{
		*pdwXor++ ^= dwXorKey;
		dwXorKey = SeedRandMap(*pwSeed++);
		dwXorKey |= ((DWORD)SeedRandMap(*pwSeed++)) << 16;
		dwXorKey ^= g_dwPacketKey;
	}

	//设置变量
	m_dwSendPacketCount++;
	m_dwSendXorKey = dwXorKey;

	return wDataSize;
}

//解密数据
WORD CServerSocketItem::CrevasseBuffer(BYTE pcbDataBuffer[], WORD wDataSize)
{
	WORD i = 0;
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_Head));
	ASSERT(((CMD_Head *)pcbDataBuffer)->CmdInfo.wPacketSize == wDataSize);

	//调整长度
	WORD wSnapCount = 0;
	if ((wDataSize % sizeof(DWORD)) != 0)
	{
		wSnapCount = sizeof(DWORD) - wDataSize % sizeof(DWORD);
		memset(pcbDataBuffer + wDataSize, 0, wSnapCount);
	}

	//提取密钥
	if (m_dwRecvPacketCount == 0)
	{
		ASSERT(wDataSize >= (sizeof(CMD_Head) + sizeof(DWORD)));
		if (wDataSize < (sizeof(CMD_Head) + sizeof(DWORD))) throw TEXT("数据包解密长度错误");
		m_dwRecvXorKey = *(DWORD *)(pcbDataBuffer + sizeof(CMD_Head));
		m_dwSendXorKey = m_dwRecvXorKey;
		MoveMemory(pcbDataBuffer + sizeof(CMD_Head), pcbDataBuffer + sizeof(CMD_Head) + sizeof(DWORD),
		           wDataSize - sizeof(CMD_Head) - sizeof(DWORD));
		wDataSize -= sizeof(DWORD);
		((CMD_Head *)pcbDataBuffer)->CmdInfo.wPacketSize -= sizeof(DWORD);
	}

	//解密数据
	DWORD dwXorKey = m_dwRecvXorKey;
	DWORD * pdwXor = (DWORD *)(pcbDataBuffer + sizeof(CMD_Info));
	WORD  * pwSeed = (WORD *)(pcbDataBuffer + sizeof(CMD_Info));
	WORD wEncrypCount = (wDataSize + wSnapCount - sizeof(CMD_Info)) / 4;
	for (i = 0; i < wEncrypCount; i++)
	{
		if ((i == (wEncrypCount - 1)) && (wSnapCount > 0))
		{
			BYTE * pcbKey = ((BYTE *) & m_dwRecvXorKey) + sizeof(DWORD) - wSnapCount;
			CopyMemory(pcbDataBuffer + wDataSize, pcbKey, wSnapCount);
		}
		dwXorKey = SeedRandMap(*pwSeed++);
		dwXorKey |= ((DWORD)SeedRandMap(*pwSeed++)) << 16;
		dwXorKey ^= g_dwPacketKey;
		*pdwXor++ ^= m_dwRecvXorKey;
		m_dwRecvXorKey = dwXorKey;
	}

	//效验码与字节映射
	CMD_Head * pHead = (CMD_Head *)pcbDataBuffer;
	BYTE cbCheckCode = pHead->CmdInfo.cbCheckCode;;
	for (i = sizeof(CMD_Info); i < wDataSize; i++)
	{
		pcbDataBuffer[i] = MapRecvByte(pcbDataBuffer[i]);
		cbCheckCode += pcbDataBuffer[i];
	}
	if (cbCheckCode != 0) throw TEXT("数据包效验码错误");

	return wDataSize;
}

bool CServerSocketItem::isWebSocket() {
	return m_connectType == WEBSOCKET;
}

bool CServerSocketItem::isWinSocket() {
	return m_connectType == WINSOCKET;
}

bool CServerSocketItem::HandleWebsocketRecv() {
	using namespace zl::net::ws;

	if (m_wRecvSize > SOCKET_BUFFER) {
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("error,receive data len > SOCKET_TCP_BUFFER,%d>32768"), (int)m_wRecvSize);
		CTraceService::TraceString(szString, TraceLevel_Exception);
		return false;
	}

	try {
		while (m_wRecvSize > 0) {
			char outbuf[SOCKET_BUFFER];
			int outlen = 0;
			int frameSize = 0;
			WsFrameType type = decodeFrame((char*)m_cbRecvBuf, m_wRecvSize, outbuf, &outlen, &frameSize);

			switch (type) {
			case WS_INCOMPLETE_TEXT_FRAME:
			case WS_INCOMPLETE_BINARY_FRAME:
			case WS_INCOMPLETE_FRAME: {
				return true;
			}
			case WS_TEXT_FRAME:
			case WS_BINARY_FRAME: {
				m_dwRecvPacketCount++;

				CMD_Command Command;
				NullPmd::message message;
				if (message.ParseFromArray(outbuf, outlen) == false) {
					throw TEXT("parse error");
				}
				NullPmd::head head = message.head();
				NullPmd::command command = head.command();
				NullPmd::info info = head.info();

				if (info.cbversion() != SOCKET_VER) {
					CString aa;
					aa.Format(TEXT("0x%x"), info.cbversion());
					CTraceService::TraceString(aa, TraceLevel_Exception);
					throw TEXT("数据包版本不匹配");
				}

				Command.wMainCmdID = command.mainid();
				Command.wSubCmdID = command.subid();
				//内核命令
				if (Command.wMainCmdID == MDM_KN_COMMAND) {
					switch (Command.wSubCmdID) {
					case SUB_KN_DETECT_SOCKET: {	//网络检测
						SendData(MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET, m_wRountID);
						break;
					}
					default: {
						throw TEXT("非法命令码");
					}
					}
				}
				else {
					//消息处理
					m_pIServerSocketItemSink->OnSocketReadEvent(Command, (void*)message.data().c_str(), message.data().size(), this);
				}
				break;
			}
			case WS_PING_FRAME: {
				//printf ("receive ping frame,framesize:%d\n", frameSize);
				break;
			}
			case WS_PONG_FRAME: {
				// printf ("receive pong frame,framesize:%d\n", frameSize);
				break;
			}
			case WS_CLOSE_FRAME: {
				//printf ("receive close frame\n");
				CTraceService::TraceString("收到WS_CLOSE_FRAME消息", TraceLevel_Normal);
				CloseSocket(m_wRountID);
				return false;
			}
			default: {
				//LOG_WARN(_T("receive unknow frame,close socket,type:%d"), (int)type);
				CTraceService::TraceString("收到未知消息", TraceLevel_Normal);
				CloseSocket(m_wRountID);
				return false;
			}
			}

			//删除缓存数据
			m_wRecvSize -= frameSize;
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf + frameSize, m_wRecvSize);
		}
	}
	catch (LPCTSTR pszMessage) {
		// 错误信息
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("HandleWebsocketRecv 异常:%s"), pszMessage);
		CTraceService::TraceString(szString, TraceLevel_Exception);
		return false;
	}
	catch (...) {
		// 错误信息
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("Index=%ld，RountID=%ld，HandleWebsocketRecv 发生异常"), m_wIndex, m_wRountID);
		CTraceService::TraceString(szString, TraceLevel_Exception);
		CloseSocket(m_wRountID);
		return false;
	}

	return true;
}

bool CServerSocketItem::CheckIsWinSocket() {
	char* recvChar = (char*)m_cbRecvBuf;
	std::istringstream stream(recvChar);
	std::string reqType;
	std::getline(stream, reqType);
	if (reqType.substr(0, 4) == "GET ")
	{
		return false;
	}
	return true;
}

bool CServerSocketItem::handshake(zl::net::ByteBuffer* byteBuffer) {
	zl::net::HttpContext context;
	if (!context.parseRequest(byteBuffer) || context.request().method() != zl::net::HttpGet) { // 解析失败 或者 不是Get请求
		CTraceService::TraceString(TEXT("parse handshake error,send close header"), TraceLevel_Exception);
		std::string msg = "HTTP/1.1 400 Bad Request\r\n\r\n";
		SendRawData(msg.c_str(), msg.size());
		return false;
	}

	ASSERT(context.gotAll());
	zl::net::HttpRequest& req = context.request();
	std::string query = req.query();
	if (query.empty() == false) {
		query = query.substr(1);
	}
	std::string key = req.getHeader(zl::net::ws::kSecWebSocketKeyHeader);
	std::string answer = zl::net::ws::makeHandshakeResponse(key.c_str(), req.getHeader(zl::net::ws::kSecWebSocketProtocolHeader));
	SendRawData(answer.c_str(), answer.size());
	return true;
}

bool CServerSocketItem::SendRawData(const char* data, int len) {
	try
	{
		//寻找发送结构
		COverLappedSend * pOverLappedSend = GetSendOverLapped();
		ASSERT(pOverLappedSend != NULL);
		if (pOverLappedSend == NULL) return false;

		// 变量定义
		WORD wSourceLen = (WORD)pOverLappedSend->m_WSABuffer.len;

		// 附加数据
		if (len > 0)
		{
			ASSERT(data != NULL);
			CopyMemory(pOverLappedSend->m_cbBuffer + wSourceLen, data, len);
		}

		pOverLappedSend->m_WSABuffer.len += len;
		
		//发送数据
		if (m_OverLappedSendActive.GetCount() == 1)
		{
			DWORD dwThancferred = 0;
			int iRetCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
			if ((iRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
			{
				OnSendCompleted(pOverLappedSend, 0L);
				return false;
			}
		}
	}
	catch (...)
	{
		// 错误信息
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("Index=%ld，RountID=%ld，SendRawData 发生异常"), m_wIndex, m_wRountID);
		CTraceService::TraceString(szString, TraceLevel_Exception);
		CloseSocket(m_wRountID);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerSocketRSThread::CServerSocketRSThread(void)
{
	m_hCompletionPort = NULL;
}

//析构函数
CServerSocketRSThread::~CServerSocketRSThread(void)
{
}

//配置函数
bool CServerSocketRSThread::InitThread(HANDLE hCompletionPort)
{
	ASSERT(hCompletionPort != NULL);
	m_hCompletionPort = hCompletionPort;
	return true;
}

//运行函数
bool CServerSocketRSThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort != NULL);

	//变量定义
	DWORD dwThancferred = 0;
	OVERLAPPED * pOverLapped = NULL;
	COverLapped * pSocketLapped = NULL;
	CServerSocketItem * pServerSocketItem = NULL;

	//等待完成端口
	BOOL bSuccess = GetQueuedCompletionStatus(m_hCompletionPort, &dwThancferred, (PULONG_PTR) & pServerSocketItem, &pOverLapped, INFINITE);
	if ((bSuccess == FALSE) && (GetLastError() != ERROR_NETNAME_DELETED)) return false;
	if ((pServerSocketItem == NULL) && (pOverLapped == NULL)) return false;

	//处理操作
	ASSERT(pOverLapped != NULL);
	ASSERT(pServerSocketItem != NULL);
	pSocketLapped = CONTAINING_RECORD(pOverLapped, COverLapped, m_OverLapped);
	switch (pSocketLapped->GetOperationType())
	{
		case OperationType_Recv:	//SOCKET 数据读取
		{
			COverLappedRecv * pOverLappedRecv = (COverLappedRecv *)pSocketLapped;
			CThreadLock lock(pServerSocketItem->GetSignedLock());
			pServerSocketItem->OnRecvCompleted(pOverLappedRecv, dwThancferred);
			break;
		}
		case OperationType_Send:	//SOCKET 数据发送
		{
			COverLappedSend * pOverLappedSend = (COverLappedSend *)pSocketLapped;
			CThreadLock lock(pServerSocketItem->GetSignedLock());
			pServerSocketItem->OnSendCompleted(pOverLappedSend, dwThancferred);
			break;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSocketAcceptThread::CSocketAcceptThread(void)
{
	m_hCompletionPort = NULL;
	m_pTCPSocketManager = NULL;
	m_hListenSocket = INVALID_SOCKET;
}

//析构函数
CSocketAcceptThread::~CSocketAcceptThread(void)
{
}

//配置函数
bool CSocketAcceptThread::InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CTCPNetworkEngine * pTCPSocketManager)
{
	ASSERT(hCompletionPort != NULL);
	ASSERT(pTCPSocketManager != NULL);
	ASSERT(hListenSocket != INVALID_SOCKET);
	m_hListenSocket = hListenSocket;
	m_hCompletionPort = hCompletionPort;
	m_pTCPSocketManager = pTCPSocketManager;
	return true;
}

//运行函数
bool CSocketAcceptThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort != NULL);
	ASSERT(m_pTCPSocketManager != NULL);

	//设置变量
	SOCKADDR_IN	SocketAddr;
	CServerSocketItem * pServerSocketItem = NULL;
	SOCKET hConnectSocket = INVALID_SOCKET;
	int nBufferSize = sizeof(SocketAddr);

	try
	{
		//监听连接
		hConnectSocket = WSAAccept(m_hListenSocket, (SOCKADDR *) & SocketAddr, &nBufferSize, NULL, NULL);
		if (hConnectSocket == INVALID_SOCKET) return false;

		//获取连接
		pServerSocketItem = m_pTCPSocketManager->ActiveSocketItem();
		if (pServerSocketItem == NULL) throw TEXT("申请连接对象失败");

		//激活对象
		CThreadLock lock(pServerSocketItem->GetSignedLock());
		pServerSocketItem->Attach(hConnectSocket, SocketAddr.sin_addr.S_un.S_addr);
		CreateIoCompletionPort((HANDLE)hConnectSocket, m_hCompletionPort, (ULONG_PTR)pServerSocketItem, 0);
		pServerSocketItem->RecvData();
	}
	catch (...)
	{
		//清理对象
		ASSERT(pServerSocketItem == NULL);
		if (hConnectSocket != INVALID_SOCKET)	closesocket(hConnectSocket);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSocketDetectThread::CSocketDetectThread(void)
{
	m_dwTickCount = 0;;
	m_pTCPSocketManager = NULL;
}

//析构函数
CSocketDetectThread::~CSocketDetectThread(void)
{
}

//配置函数
bool CSocketDetectThread::InitThread(CTCPNetworkEngine * pTCPSocketManager)
{
	//效验参数
	ASSERT(pTCPSocketManager != NULL);

	//设置变量
	m_dwTickCount = 0L;
	m_pTCPSocketManager = pTCPSocketManager;

	return true;
}

//运行函数
bool CSocketDetectThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_pTCPSocketManager != NULL);

	//设置间隔
	Sleep(500);
	m_dwTickCount += 500L;

	//检测连接
	if (m_dwTickCount > 20000L)
	{
		m_dwTickCount = 0L;
		m_pTCPSocketManager->DetectSocket();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkEngine::CTCPNetworkEngine(void)
{
	m_bService = false;
	m_wListenPort = 0;
	m_dwLastDetect = 0;
	m_wMaxSocketItem = 0;
	m_hCompletionPort = NULL;
	m_hServerSocket = INVALID_SOCKET;

	return;
}

//析构函数
CTCPNetworkEngine::~CTCPNetworkEngine(void)
{
	//停止服务
	ConcludeService();

	//释放存储连接
	CServerSocketItem * pSocketItem = NULL;
	for (INT_PTR i = 0; i < m_StorageSocketItem.GetCount(); i++)
	{
		pSocketItem = m_StorageSocketItem[i];
		ASSERT(pSocketItem != NULL);
		SafeDelete(pSocketItem);
	}
	m_StorageSocketItem.RemoveAll();

	return;
}

//接口查询
void * __cdecl CTCPNetworkEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPNetworkEngine, Guid, dwQueryVer);
	QUERYINTERFACE(IQueueServiceSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPNetworkEngine, Guid, dwQueryVer);
	return NULL;
}

//设置接口
bool __cdecl CTCPNetworkEngine::SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx)
{
	//状态判断
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("网络引擎处于服务状态，绑定操作忽略"), TraceLevel_Exception);
		return false;
	}

	//设置接口
	if (m_QueueServiceEvent.SetQueueServiceSink(pIUnknownEx) == false)
	{
		CTraceService::TraceString(TEXT("网络引擎与触发服务绑定失败"), TraceLevel_Exception);
		return false;
	}

	return true;
}

//设置参数
bool __cdecl CTCPNetworkEngine::SetServiceParameter(WORD wServicePort, WORD wMaxConnect)
{
	//效验状态
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("网络引擎处于服务状态，端口绑定操作忽略"), TraceLevel_Exception);
		return false;
	}

	//判断参数
	if (wServicePort == 0)
	{
		CTraceService::TraceString(TEXT("网络端口错误，端口绑定操作失败"), TraceLevel_Exception);
		return false;
	}

	//设置变量
	m_wListenPort = wServicePort;
	m_wMaxSocketItem = wMaxConnect;
	return true;
}

//启动服务
bool __cdecl CTCPNetworkEngine::StartService()
{
	DWORD i = 0;
	//效验状态
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("网络引擎重复启动，启动操作忽略"), TraceLevel_Warning);
		return true;
	}

	//判断端口
	if (m_wListenPort == 0)
	{
		CTraceService::TraceString(TEXT("网络引擎监听端口无效"), TraceLevel_Exception);
		return false;
	}

	//绑定对象
	if (m_SendQueueService.SetQueueServiceSink(QUERY_ME_INTERFACE(IUnknownEx)) == false)
	{
		CTraceService::TraceString(TEXT("网络引擎与触发服务绑定失败"), TraceLevel_Exception);
		return false;
	}

	try
	{
		//获取系统信息
		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);
		DWORD dwThreadCount = SystemInfo.dwNumberOfProcessors;

		//建立完成端口
		m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, SystemInfo.dwNumberOfProcessors);
		if (m_hCompletionPort == NULL) throw TEXT("网络引擎完成端口创建失败");

		//建立监听SOCKET
		struct sockaddr_in SocketAddr;
		memset(&SocketAddr, 0, sizeof(SocketAddr));
		SocketAddr.sin_addr.s_addr = INADDR_ANY;
		SocketAddr.sin_family = AF_INET;
		SocketAddr.sin_port = htons(m_wListenPort);
		m_hServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_hServerSocket == INVALID_SOCKET) throw TEXT("网络引擎监听 SOCKET 创建失败");
		int iErrorCode = bind(m_hServerSocket, (SOCKADDR*) & SocketAddr, sizeof(SocketAddr));
		if (iErrorCode == SOCKET_ERROR) throw TEXT("网络引擎监听端口被占用，端口绑定失败");
		iErrorCode = listen(m_hServerSocket, 200);
		if (iErrorCode == SOCKET_ERROR) throw TEXT("网络引擎监听端口被占用，端口监听失败");

		//启动发送队列
		bool bSuccess = m_SendQueueService.StartService();
		if (bSuccess == false) throw TEXT("网络引擎发送队列服务启动失败");

		//建立读写线程
		for (i = 0; i < dwThreadCount; i++)
		{
			CServerSocketRSThread * pServerSocketRSThread = new CServerSocketRSThread();
			if (pServerSocketRSThread == NULL) throw TEXT("网络引擎读写线程服务创建失败");
			bSuccess = pServerSocketRSThread->InitThread(m_hCompletionPort);
			if (bSuccess == false) throw TEXT("网络引擎读写线程服务配置失败");
			m_SocketRSThreadArray.Add(pServerSocketRSThread);
		}

		//建立应答线程
		bSuccess = m_SocketAcceptThread.InitThread(m_hCompletionPort, m_hServerSocket, this);
		if (bSuccess == false) throw TEXT("网络引擎网络监听线程服务配置");

		//运行读写线程
		for (i = 0; i < dwThreadCount; i++)
		{
			CServerSocketRSThread * pServerSocketRSThread = m_SocketRSThreadArray[i];
			ASSERT(pServerSocketRSThread != NULL);
			bSuccess = pServerSocketRSThread->StartThread();
			if (bSuccess == false) throw TEXT("网络引擎读写线程服务启动失败");
		}

		//网络检测线程
		m_SocketDetectThread.InitThread(this);
		bSuccess = m_SocketDetectThread.StartThread();
		if (bSuccess == false) throw TEXT("网络引检测线程服务启动失败");

		//运行应答线程
		bSuccess = m_SocketAcceptThread.StartThread();
		if (bSuccess == false) throw TEXT("网络引擎监听线程服务启动失败");

		//设置变量
		m_bService = true;
	}
	catch (LPCTSTR pszError)
	{
		CTraceService::TraceString(pszError, TraceLevel_Exception);
		return false;
	}

	return true;
}

//停止服务
bool __cdecl CTCPNetworkEngine::ConcludeService()
{
	//设置变量
	m_bService = false;
	m_dwLastDetect = 0L;

	//停止检测线程
	m_SocketDetectThread.ConcludeThread(INFINITE);

	//终止应答线程
	if (m_hServerSocket != INVALID_SOCKET)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket = INVALID_SOCKET;
	}
	m_SocketAcceptThread.ConcludeThread(INFINITE);

	//停止发送队列
	m_SendQueueService.ConcludeService();

	//释放读写线程
	INT_PTR nCount = m_SocketRSThreadArray.GetCount(), i = 0;
	if (m_hCompletionPort != NULL)
	{
		for (i = 0; i < nCount; i++) PostQueuedCompletionStatus(m_hCompletionPort, 0, NULL, NULL);
	}
	for (i = 0; i < nCount; i++)
	{
		CServerSocketRSThread * pSocketThread = m_SocketRSThreadArray[i];
		ASSERT(pSocketThread != NULL);
		pSocketThread->ConcludeThread(INFINITE);
		SafeDelete(pSocketThread);
	}
	m_SocketRSThreadArray.RemoveAll();

	//关闭完成端口
	if (m_hCompletionPort != NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	//关闭连接
	CServerSocketItem * pServerSocketItem = NULL;
	for (i = 0; i < m_ActiveSocketItem.GetCount(); i++)
	{
		pServerSocketItem = m_ActiveSocketItem[i];
		ASSERT(pServerSocketItem != NULL);
		pServerSocketItem->CloseSocket(pServerSocketItem->GetRountID());
		pServerSocketItem->ResetSocketData();
	}
	m_FreeSocketItem.Append(m_ActiveSocketItem);
	m_ActiveSocketItem.RemoveAll();

	m_QueueServiceEvent.SetQueueServiceSink(NULL);

	return true;
}

//应答消息
bool CTCPNetworkEngine::OnSocketAcceptEvent(CServerSocketItem * pServerSocketItem)
{
	//效验数据
	ASSERT(pServerSocketItem != NULL);
	if (NULL == pServerSocketItem) return false;

	//投递消息
	if (m_bService == false) return false;
	m_QueueServiceEvent.PostNetworkAcceptEvent(pServerSocketItem->GetSocketID(), pServerSocketItem->GetClientAddr());

	return true;
}

//网络读取消息
bool CTCPNetworkEngine::OnSocketReadEvent(CMD_Command Command, void * pBuffer, WORD wDataSize, CServerSocketItem * pServerSocketItem)
{
	//效验数据
	ASSERT(pServerSocketItem != NULL);
	if (NULL == pServerSocketItem) return false;

	//效验状态
	if (m_bService == false) return false;
	m_QueueServiceEvent.PostNetworkReadEvent(pServerSocketItem->GetSocketID(), Command, pBuffer, wDataSize);

	return true;
}

//网络关闭消息
bool CTCPNetworkEngine::OnSocketCloseEvent(CServerSocketItem * pServerSocketItem)
{
	//效验参数
	ASSERT(pServerSocketItem != NULL);
	if (NULL == pServerSocketItem) return false;

	try
	{
		//效验状态
		if (m_bService == false) return false;

		//计算时间
		WORD wIndex = pServerSocketItem->GetIndex();
		WORD wRountID = pServerSocketItem->GetRountID();
		DWORD dwClientAddr = pServerSocketItem->GetClientAddr();
		DWORD dwConnectTime = pServerSocketItem->GetConnectTime();
		//////////////////////////////////////////////////////////////////////////,这里要调整
		m_QueueServiceEvent.PostNetworkCloseEvent(pServerSocketItem->GetSocketID() , dwClientAddr, dwConnectTime);

		//释放连接
		FreeSocketItem(pServerSocketItem);
	}
	catch (...) {}

	return true;
}

//通知回调函数（发送队列线程调用）
void __cdecl CTCPNetworkEngine::OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize)
{
	switch (wIdentifier)
	{
		case QUEUE_SEND_REQUEST:		//发送请求
		{
			//效验数据
			tagSendDataRequest * pSendDataRequest = (tagSendDataRequest *)pBuffer;
			ASSERT(wDataSize >= (sizeof(tagSendDataRequest) - sizeof(pSendDataRequest->cbSendBuf)));
			ASSERT(wDataSize == (pSendDataRequest->wDataSize + sizeof(tagSendDataRequest) - sizeof(pSendDataRequest->cbSendBuf)));

			//群发数据
			if (pSendDataRequest->dwSocketID == 0)
			{
				//获取活动项
				{
					CThreadLock lcok(m_CriticalSection);
					m_TempSocketItem.RemoveAll();
					m_TempSocketItem.Copy(m_ActiveSocketItem);
				}

				//循环发送数据
				CServerSocketItem * pServerSocketItem = NULL;
				for (INT_PTR i = 0; i < m_TempSocketItem.GetCount(); i++)
				{
					pServerSocketItem = m_TempSocketItem[i];
					ASSERT(pServerSocketItem != NULL);
					CThreadLock lock(pServerSocketItem->GetSignedLock());
					if (pServerSocketItem->IsAllowBatch())//第一次不允许？？
					{
						pServerSocketItem->SendData(pSendDataRequest->cbSendBuf, pSendDataRequest->wDataSize, pSendDataRequest->wMainCmdID,
						                            pSendDataRequest->wSubCmdID, pServerSocketItem->GetRountID());
					}
				}
			}
			else
			{
				//单项发送
				CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(pSendDataRequest->dwSocketID));
				CThreadLock lock(pServerSocketItem->GetSignedLock());
				pServerSocketItem->SendData(pSendDataRequest->cbSendBuf, pSendDataRequest->wDataSize, pSendDataRequest->wMainCmdID,
				                            pSendDataRequest->wSubCmdID, HIWORD(pSendDataRequest->dwSocketID));
			}

			break;
		}
		case QUEUE_SAFE_CLOSE:		//安全关闭
		{
			//效验数据
			ASSERT(wDataSize == sizeof(tagSafeCloseSocket));
			tagSafeCloseSocket * pSafeCloseSocket = (tagSafeCloseSocket *)pBuffer;

			//安全关闭
			try
			{
				CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(pSafeCloseSocket->dwSocketID));
				CThreadLock lock(pServerSocketItem->GetSignedLock());
				pServerSocketItem->ShutDownSocket(HIWORD(pSafeCloseSocket->dwSocketID));
			}
			catch (...)
			{
			}
			break;
		}
		case QUEUE_ALLOW_BATCH:		//允许群发
		{
			//效验数据
			ASSERT(wDataSize == sizeof(tagAllowBatchSend));
			tagAllowBatchSend * pAllowBatchSend = (tagAllowBatchSend *)pBuffer;

			//设置群发
			CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(pAllowBatchSend->dwSocketID));
			CThreadLock lock(pServerSocketItem->GetSignedLock());
			pServerSocketItem->AllowBatchSend(HIWORD(pAllowBatchSend->dwSocketID), pAllowBatchSend->cbAllow ? true : false);

			break;
		}
		case QUEUE_DETECT_SOCKET:	//检测连接
		{
			//获取活动项
			{
				CThreadLock lcok(m_CriticalSection);
				m_TempSocketItem.RemoveAll();
				m_TempSocketItem.Copy(m_ActiveSocketItem);
			}

			//构造数据
			CMD_KN_DetectSocket DetectSocket;
			ZeroMemory(&DetectSocket, sizeof(DetectSocket));

			//变量定义
			WORD wRountID = 0;
			DWORD dwNowTickCount = GetTickCount();
			DWORD dwBreakTickCount = __max(dwNowTickCount - m_dwLastDetect, TIME_BREAK_READY);

			//设置变量
			m_dwLastDetect = GetTickCount();

			//检测连接
			for (INT_PTR i = 0; i < m_TempSocketItem.GetCount(); i++)
			{
				//变量定义
				CServerSocketItem * pServerSocketItem = m_TempSocketItem[i];
				DWORD dwRecvTickCount = pServerSocketItem->GetRecvTickCount();
				CThreadLock lock(pServerSocketItem->GetSignedLock());

				//间隔检查
				if (dwRecvTickCount >= dwNowTickCount) continue;

				//检测连接
				if (pServerSocketItem->IsReadySend() == true)
				{
					if ((dwNowTickCount - dwRecvTickCount) > dwBreakTickCount)
					{
						pServerSocketItem->CloseSocket(pServerSocketItem->GetRountID());
						continue;
					}
				}
				else
				{
					if ((dwNowTickCount - dwRecvTickCount) > TIME_BREAK_CONNECT)
					{
						pServerSocketItem->CloseSocket(pServerSocketItem->GetRountID());
						continue;
					}
				}

				//发送数据
				if (pServerSocketItem->IsReadySend() == true)
				{
					wRountID = pServerSocketItem->GetRountID();
					DetectSocket.dwSendTickCount = GetTickCount();
					pServerSocketItem->SendData(&DetectSocket, sizeof(DetectSocket), MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET, wRountID);
				}
			}

			break;
		}
		default:
		{
			ASSERT(FALSE);
		}
	}

	return;
}

//获取空闲对象
CServerSocketItem * CTCPNetworkEngine::ActiveSocketItem()
{
	CThreadLock lock(m_CriticalSection);

	//获取空闲对象
	CServerSocketItem * pServerSocketItem = NULL;
	if (m_FreeSocketItem.GetCount() > 0)
	{
		INT_PTR nItemPostion = m_FreeSocketItem.GetCount() - 1;
		pServerSocketItem = m_FreeSocketItem[nItemPostion];
		ASSERT(pServerSocketItem != NULL);
		m_FreeSocketItem.RemoveAt(nItemPostion);
		m_ActiveSocketItem.Add(pServerSocketItem);
	}

	//创建新对象
	if (pServerSocketItem == NULL)
	{
		WORD wStorageCount = (WORD)m_StorageSocketItem.GetCount();
		if (wStorageCount < m_wMaxSocketItem)
		{
			try
			{
				pServerSocketItem = new CServerSocketItem(wStorageCount, this);
				if (pServerSocketItem == NULL) return NULL;
				m_StorageSocketItem.Add(pServerSocketItem);
				m_ActiveSocketItem.Add(pServerSocketItem);
			}
			catch (...)
			{
				return NULL;
			}
		}
	}

	return pServerSocketItem;
}

//获取连接对象
CServerSocketItem * CTCPNetworkEngine::EnumSocketItem(WORD wIndex)
{
	CThreadLock lock(m_CriticalSection);
	if (wIndex < m_StorageSocketItem.GetCount())
	{
		CServerSocketItem * pServerSocketItem = m_StorageSocketItem[wIndex];
		ASSERT(pServerSocketItem != NULL);
		return pServerSocketItem;
	}
	return NULL;
}

//释放连接对象
bool CTCPNetworkEngine::FreeSocketItem(CServerSocketItem * pServerSocketItem)
{
	//效验参数
	ASSERT(pServerSocketItem != NULL);

	//释放对象
	CThreadLock lock(m_CriticalSection);
	INT_PTR nActiveCount = m_ActiveSocketItem.GetCount();
	for (int i = 0; i < nActiveCount; i++)
	{
		if (pServerSocketItem == m_ActiveSocketItem[i])
		{
			m_ActiveSocketItem.RemoveAt(i);
			m_FreeSocketItem.Add(pServerSocketItem);
			return true;
		}
	}

	//释放失败
	ASSERT(FALSE);
	return false;
}

//检测连接
bool __cdecl CTCPNetworkEngine::DetectSocket()
{
	return m_SendQueueService.AddToQueue(QUEUE_DETECT_SOCKET, NULL, 0);
}


//发送函数
bool __cdecl CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID)
{
	//效益状态
	ASSERT(m_bService == true);
	if (m_bService == false) return false;

	//构造数据
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID = wMainCmdID;
	SendRequest.wSubCmdID = wSubCmdID;
	SendRequest.dwSocketID = dwSocketID;
	SendRequest.wDataSize = 0;

	//加入发送队列
	WORD wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuf);
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST, &SendRequest, wSendSize);
}

//发送函数
bool __cdecl CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//效益状态
	ASSERT(m_bService == true);
	if (m_bService == false) return false;

	//效益数据
	ASSERT((wDataSize + sizeof(CMD_Head)) <= SOCKET_BUFFER);
	if ((wDataSize + sizeof(CMD_Head)) > SOCKET_BUFFER) return false;

	//构造数据
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID = wMainCmdID;
	SendRequest.wSubCmdID = wSubCmdID;
	SendRequest.dwSocketID = dwSocketID;
	SendRequest.wDataSize = wDataSize;
	if (wDataSize > 0)
	{
		ASSERT(pData != NULL);
		ZeroMemory(SendRequest.cbSendBuf, sizeof(SendRequest.cbSendBuf));
		CopyMemory(SendRequest.cbSendBuf, pData, wDataSize);
	}

	//加入发送队列
	WORD wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuf) + wDataSize;
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST, &SendRequest, wSendSize);
}

//批量发送
bool __cdecl CTCPNetworkEngine::SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//效益状态
	if (m_bService == false) return false;

	//效益数据
	ASSERT((wDataSize + sizeof(CMD_Head)) <= SOCKET_BUFFER);
	if ((wDataSize + sizeof(CMD_Head)) > SOCKET_BUFFER) return false;

	//构造数据
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID = wMainCmdID;
	SendRequest.wSubCmdID = wSubCmdID;
	SendRequest.dwSocketID = 0;
	SendRequest.wDataSize = wDataSize;
	if (wDataSize > 0)
	{
		ASSERT(pData != NULL);
		CopyMemory(SendRequest.cbSendBuf, pData, wDataSize);
	}

	//加入发送队列
	WORD wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuf) + wDataSize;
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST, &SendRequest, wSendSize);
}

//关闭连接
bool __cdecl CTCPNetworkEngine::CloseSocket(DWORD dwSocketID)
{
	CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(dwSocketID));
	if (pServerSocketItem == NULL) return false;
	CThreadLock lock(pServerSocketItem->GetSignedLock());
	return pServerSocketItem->CloseSocket(HIWORD(dwSocketID));
}

//设置关闭
bool __cdecl CTCPNetworkEngine::ShutDownSocket(DWORD dwSocketID)
{
	tagSafeCloseSocket SafeCloseSocket;
	SafeCloseSocket.dwSocketID = dwSocketID;
	return m_SendQueueService.AddToQueue(QUEUE_SAFE_CLOSE, &SafeCloseSocket, sizeof(SafeCloseSocket));
}

//允许群发
bool __cdecl CTCPNetworkEngine::AllowBatchSend(DWORD dwSocketID, bool bAllow)
{
	tagAllowBatchSend AllowBatchSendNode;
	AllowBatchSendNode.dwSocketID = dwSocketID;
	AllowBatchSendNode.cbAllow = bAllow;
	return m_SendQueueService.AddToQueue(QUEUE_ALLOW_BATCH, &AllowBatchSendNode, sizeof(tagAllowBatchSend));
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateTCPNetworkEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CTCPNetworkEngine * pTCPSocketEngine = NULL;
	try
	{
		pTCPSocketEngine = new CTCPNetworkEngine();
		if (pTCPSocketEngine == NULL) throw TEXT("创建失败");
		void * pObject = pTCPSocketEngine->QueryInterface(Guid, dwInterfaceVer);
		if (pObject == NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pTCPSocketEngine);
	return NULL;
}



//////////////////////////////////////////////////////////////////////////
