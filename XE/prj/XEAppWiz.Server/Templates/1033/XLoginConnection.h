#pragma once
#include "server/XWinSocketClient.h"

class XMain;
class XAhgoUser;
class XSAccount;
class XEWinConnectionInServer;


class XLoginConnection : public XEWinSocketClient
{
public:
	static XLoginConnection* sGet() {
		XBREAK( s_pSingleton == nullptr );	// sCreateSingleton을 XMain::DoConnectLoginServer에 넣을것.
		return s_pSingleton;
	}
	static void sCreateSingleton( const std::string& strIP, WORD port, XNetworkDelegate *pDelegate );
	static void sDestroySingleton();
private:
	static XLoginConnection* s_pSingleton;
private:
	XList<XPacketMem*> m_listPacketToLogin;	// 로그인서버가 죽어서 못보내고있는 패킷뭉치들.
	void Destroy() {}
public:
	XLoginConnection( const std::string& strIP, WORD port, XNetworkDelegate *pDelegate, DWORD param=0 );
	virtual ~XLoginConnection() { Destroy(); }
	//
	void FlushSendPacket( void );
	BOOL GetSizeSendPacketBuffer( int *pOutNum, int *pOutBytes );
	//
	BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) override;
	void OnConnect( void ) override;
	void OnDisconnect( void ) override;
	void SendDidFinishLoad( void );
	// send/recv
//	void RecvSVRConnectSuccess( XPacket& p );
	void RecvIsAleadyConnectAccount ( XPacket& p );
	void SendLoginAccountInfo( ID idAcc, ID idConnect );
	void RecvLoginAccountInfoLoad ( XPacket& p );
	void RecvNewAccount( XPacket& p );
	void SendAccountIsAlreadyConnect( ID idConnectNew, ID idConnectAlready, ID idAcc );
private:
	// 외부에서 못사용하도록 바뀜
	BOOL Send( XPacket& ar ) override;

};

