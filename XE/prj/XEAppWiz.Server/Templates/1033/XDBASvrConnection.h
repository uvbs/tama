#pragma once
#include "server/XWinSocketClient.h"
#include "XGameUser.h"

class XSAccount;
class XEWinConnectionInServer;
class XDBASvrConnection;

class XDBASvrConnection : public XEWinSocketClient
{
public:
	static XDBASvrConnection* sGet() {
		XBREAK( s_pSingleton == nullptr );	// sCreateSingleton을 XMain::DoConnectDBAServer에 넣을것.
		return s_pSingleton;
	}
	static void sCreateSingleton( const std::string& strIP, WORD port, XNetworkDelegate *pDelegate );
	static void sDestroySingleton();
private:
	static XDBASvrConnection* s_pSingleton; 
private:
	struct xCALLBACK {
		ID idClientConnect;
		std::function<void(XGameUser*, XPacket&)> callbackFunc;
		xCALLBACK() {
			idClientConnect = 0;
		}
	};
	// snKey, callback
	std::map<ID,xCALLBACK> m_mapCallback;
	template<typename F>
	void AddRecv( ID idPacket, F func ) {}
	BOOL CallRecvCallback( ID idKey, XPacket& p );
private:
	XList<XPacketMem*> m_listPacketToLogin;	// 로그인서버가 죽어서 못보내고있는 패킷뭉치들.
	void Init() {}
	void Destroy() {}
public:
	XDBASvrConnection( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, DWORD param=0 );
	virtual ~XDBASvrConnection() { Destroy(); }
	//
	template<typename F>
	ID AddResponse( ID idKey, XGameUser *pOwner, F func ) {
		std::map<ID,xCALLBACK>::iterator itor;
		itor = m_mapCallback.find( idKey );
		if( itor == m_mapCallback.end() ) {
			xCALLBACK callback;
			callback.idClientConnect = pOwner->GetidConnect();
			callback.callbackFunc = std::bind( func, std::placeholders::_1, std::placeholders::_2 );
			m_mapCallback[ idKey ] = callback;
		}
		return idKey;
	}
	void FlushSendPacket();
	BOOL GetSizeSendPacketBuffer( int *pOutNum, int *pOutBytes );
	//
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) override;
	virtual void OnConnect() override;
	virtual void OnDisconnect() override;
private:
	// 외부에서 못사용하도록 바뀜.
	virtual BOOL Send( XPacket& ar ) override;
public:
	// send/recv
//	void SendDidFinishLoad();	
	void SendUpdateAccountLogout( ID idConnectNew, ID idConnectAlready, ID idAcc );
	void SendUserLog(ID idAccount, _tstring strUserName, int Logtype, _tstring strLog);
// 	void RecvSuccessRegisterAccount( XPacket& p );
// 	void RecvFailedRegisterAccount( XPacket& p );
//	void RecvLoginAccountInfo( XPacket& p );
	void RecvLoginAccountInfoLoad ( XPacket& p );	
//	void RecvNewAccountForUUID( XPacket& p );	
	void RecvAccountDuplicateProcess(XPacket& p);
#ifdef _xCONNECT_DUPLICATE
	void RecvAlreadyConnectUser ( XPacket& p);
#endif
	void SendAppleVerify( ID idAcc, LPCTSTR szSku, LPCTSTR szReceipt, LPCTSTR szPurchaseInfo );
	void SendSaveAccountInfo( XSAccount *pAccount, BOOL bReq, 
														ID idConnectClient,	// 로긴서버로부터온 클라이언트 커넥션 아이디
														ID idReqPacket=0, int cnt = 0 );
	void RecvSaveAccountInfo( XPacket& p );
	ID SendUpdateLogin(ID idaccount, ID GameServerID, _tstring lastconnectip);
	ID SendAccountNickNameDuplicateCheck(ID idaccount, _tstring strAccountNickName, ID idPacket);
	ID SendAccountNameDuplicateCheck(ID idaccount, _tstring strAccountName, ID idPacket);
	ID SendAccountNameRegist(ID idaccount, _tstring strAccountName, _tstring strPassword, ID idPacket);
	ID SendAccountNameRegistCheck(ID idaccount, _tstring strAccountName, _tstring strPassword, ID idPacket);
	ID SendAccountNickNameChange(ID idaccount, _tstring strAccountNickName, ID idPacket);
	ID SendGooglePayload(ID idaccount, int producttype, _tstring strProductID, ID idClientConnection, ID idPacket);
	ID SendGoogleBuyCashVerify(ID idaccount, int producttype, _tstring strProductID, _tstring strReceipt, ID idClientConnection, ID idPacket);	
	ID SendAppleBuyCashVerify(ID idaccount, _tstring strProductID, _tstring strReceipt, _tstring strPurchaseInfo, ID idClientConnection, ID idPacket);
	void SendLoginAccountInfoLoad( bool bSessionKeyLogin, ID idConnect, ID idAcc );
	//
};

