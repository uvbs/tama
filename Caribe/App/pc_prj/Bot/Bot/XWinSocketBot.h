#pragma once
#include "Network/win32/XWinSocketClient.h"
#include "XBotObj.h"

struct xConn {
	std::string m_strIP;
	SOCKET m_Socket;
	DWORD m_dwKey;
	char* m_pBuffer;
	OVERLAPPED* m_pOp;
	WORD m_port;
};

class XBotObj;
class XWinSocketBot : public XEWinSocketClient
{
	XBotObj *m_pBotObj;
	OVERLAPPED m_op;
	//
	void Init() {
		m_pBotObj = NULL;
		memset( &m_op, 0, sizeof(m_op) );
	}
	void Destroy() {}
protected:
public:
	XWinSocketBot( XBotObj* pOwner
							, XNetworkDelegate *pDelegate
							, int sizeQueue )
		: XEWinSocketClient( pDelegate, sizeQueue, 0 ) {
			Init();
			m_pBotObj = pOwner;
	}
	virtual ~XWinSocketBot() { Destroy(); }
	//
	GET_SET_ACCESSOR( XBotObj*, pBotObj );
	BOOL TryConnect() override;
	void tWSARecv();
};

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/02 13:05
*****************************************************************/
class XIOCPMng
{
	enum { MAX_THREAD_WORK = 5
			 , MAX_THREAD_CONN  = 10};
public:
	static std::shared_ptr<XIOCPMng>& sGet();
	static void sDestroyInstance();
public:
	XIOCPMng() { Init(); }
	virtual ~XIOCPMng() { Destroy(); }
	//
	BOOL RegisterSocket( XWinSocketBot *pSocketClient );
	BOOL RegisterSocket( const xConn& conn );
	void WorkThread();
	void ConnectThread();
	void PushConnectQ( const xConn& conn );
private:
	static std::shared_ptr<XIOCPMng> s_spInstance;
	HANDLE m_hIOCP = 0;
	HANDLE m_hThread[ MAX_THREAD_WORK ];		// work thread
	HANDLE m_hThreadConn[ MAX_THREAD_WORK ];		// connect thread
	XSharedObj<std::queue<xConn>> m_shoQConn;
	XLock m_lockIOCP;
private:
	void Init() {
		XCLEAR_ARRAY( m_hThread );
		XCLEAR_ARRAY( m_hThreadConn );
	}
	void Destroy();
	BOOL Create();
	HANDLE CreateWorkThread();
	void tWSARecv( const xConn& conn );
}; // class XIOCPMng

