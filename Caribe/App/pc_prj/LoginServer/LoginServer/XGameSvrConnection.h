#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include "XFramework/server/XWinConnection.h"

#pragma warning( disable : 4250 )	//  warning C4250: 'xxxx' : ������ ���� 'xxxxx'��(��) ����մϴ�.

class XLAccount;
class XUser;


class XGameSvrConnection :	public XEWinConnectionInServer
{
	//XSPLock m_spLock;
	int m_numConnectedClient;		// �� ���Ӽ����� �����ִ� ������ ��.
	WORD m_GameSvrPort;			// �� Ŀ�ؼǿ� ����Ǿ��ִ� ���Ӽ����� Ŭ���̾�Ʈ ���� ��Ʈ.
	WORD wReserved;
//	WORD m_GSvrID;				// ���Ӽ��� ���̵�-XEWinConnectionInServer::m_idConnectUserDef �� ��ü��.
	std::string m_strcIPExternal;			// Ŭ���̾�Ʈ�� �����ؾ��� ���Ӽ��� ������

	void Init() {	
		m_numConnectedClient = 0;
		m_GameSvrPort = 0;
//		m_cIPExternal[0] = 0;
//		m_GSvrID = 0;
	}
	void Destroy() {	}
	SET_ACCESSOR( WORD, GameSvrPort );
public:
	XGameSvrConnection( SOCKET socket, LPCTSTR szIP );
	virtual ~XGameSvrConnection() { Destroy(); }
	//
//	GET_ACCESSOR_CONST( const char*, cIPExternal );
	GET_ACCESSOR_CONST( const std::string&, strcIPExternal );
	GET_ACCESSOR_CONST( WORD, GameSvrPort );
	GET_ACCESSOR_CONST( int, numConnectedClient );
	//GET_ACCESSOR( XSPLock&, spLock );
	inline WORD GetGSvrID() const {
		return (WORD)GetidConnectUserDef();
	}
	inline ID GetidSvr() const {
		return GetidConnectUserDef();
	}
	inline void SetGSvrID( WORD widGSvr ) {
		SetidConnectUserDef( (DWORD)widGSvr );
	}
  bool IsOnlineUser( ID idAcc );
	//	
	void RecvGameSvrInfo( XPacket& p );
	void RecvDidFinishLoad( XPacket& p );
	void RecvAccountReadyForConnect( XPacket& p );
	void RecvAccountReadyForNewConnect( XPacket& p );
	void RecvSaveDummyComplete( XPacket& p );
	void RecvDuplicateConnectProcess(XPacket& p);
  void RecvUnlockLoginForBattle( XPacket& p );
  void SendLoginAccountInfo( bool bSessionKeyLogin, ID idConnect, ID idAcc );
	//
	BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) override;
	void OnError( xtError error, DWORD param1, DWORD param2 ) override;
	void SendAccountIsAlreadyConnect( ID idConnect, ID idAcc );
	void SendNewAccount( ID idConnect, ID idAcc, const _tstring& strUUID, const _tstring& strNickName, const _tstring& strFbUserId );
#ifdef _DUMMY_GENERATOR
	void SendNewDummyAccount( ID idAcc, const _tstring& strName, int cntCreate );
#endif // _DUMMY_GENERATOR
	// Delegate
  // callback
private:
	void SendExitGameSvr();
	XSPGameSvrConnect GetThis() {
		return std::static_pointer_cast<XGameSvrConnection>( XEWinConnectionInServer::GetThis() );
	}
	void RecvReload( XPacket& p );
};

#pragma warning ( default : 4250 )

