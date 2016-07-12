#pragma once
#include "XFramework/server/XSocketClientInServer.h"

class XMain;
class XAhgoUser;
class XSAccount;
class XEWinConnectionInServer;


class XLoginConnection : public XESocketClientInServer
{
private:
	XList<XPacketMem*> m_listPacketToLogin;	// 로그인서버가 죽어서 못보내고있는 패킷뭉치들.
	void Init() {
	}
	void Destroy() {}
public:
	XLoginConnection();
	virtual ~XLoginConnection() { Destroy(); }
	
//	//Send
	void SendDidFinishLoad( void );
  void SendUnlockLoginForBattle( ID idDefender );

	void FlushSendPacket( void );
	BOOL GetSizeSendPacketBuffer( int *pOutNum, int *pOutBytes );
	//
	BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) override;
	void OnLogined( void ) override;
	void OnDisconnect( void ) override;
	void SendLoginAccountInfo( ID idAcc, ID idConnect, const _tstring& strUUID );
	void SendAccountIsAlreadyConnect( ID idConnectNew, ID idAcc );
	void SendSaveDummyComplete( ID idAcc );
	void SendGameSvrInfo( int numClients );
	void SendReload( const std::string& strTags );
private:
	// 외부에서 못사용하도록 바뀜
	BOOL Send( XPacket& ar ) override;
	void RecvExitGameSvr( XPacket& p );
	void RecvSVRConnectSuccess( XPacket& p );
	void RecvIsAleadyConnectAccount( XPacket& p );
	void RecvLoginAccountInfoLoad( XPacket& p );
	void RecvNewAccount( XPacket& p );
	void RecvNewDummyAccount( XPacket& p );
};

