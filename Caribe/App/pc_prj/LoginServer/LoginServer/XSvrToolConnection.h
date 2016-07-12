#pragma once
#if 0
#include <stdio.h>
#include <WinSock2.h>
#include "XFramework/server/XWinConnection.h"

#pragma warning( disable : 4250 )	//  warning C4250: 'xxxx' : ������ ���� 'xxxxx'��(��) ����մϴ�.

class XLAccount;
class XUser;


class XSvrToolConnection :	public XEWinConnectionInServer
{
	int m_numConnectedClient;		// �� ���Ӽ����� �����ִ� ������ ��.
	WORD m_GameSvrPort;			// �� Ŀ�ؼǿ� ����Ǿ��ִ� ���Ӽ����� Ŭ���̾�Ʈ ���� ��Ʈ.
	WORD wReserved;
//	WORD m_GSvrID;				// ���Ӽ��� ���̵�-XEWinConnectionInServer::m_idConnectUserDef �� ��ü��.
	char m_cIPExternal[ 16 ];			// Ŭ���̾�Ʈ�� �˷��� this���Ӽ����� �ܺ� ������`

	void Init() {	
		m_numConnectedClient = 0;
		m_GameSvrPort = 0;
		m_cIPExternal[0] = 0;
//		m_GSvrID = 0;
	}
	void Destroy() {	}

	GET_SET_ACCESSOR( int, numConnectedClient );
	SET_ACCESSOR( WORD, GameSvrPort );
public:
	XSvrToolConnection( SOCKET socket, LPCTSTR szIP );
	virtual ~XSvrToolConnection() { Destroy(); }
	//
	GET_ACCESSOR( const char*, cIPExternal );
	GET_ACCESSOR( WORD, GameSvrPort );
	WORD GetGSvrID( void ) {
		return (WORD)GetidConnectUserDef();
	}
	void SetGSvrID( WORD widGSvr ) {
		SetidConnectUserDef( (DWORD)widGSvr );
	}
	//	
	void RecvGameSvrInfo( XPacket& p );
	void RecvDidFinishLoad( XPacket& p );	
	//
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p );
	virtual void OnError( xtError error, DWORD param1, DWORD param2 );
	// Delegate
};

#pragma warning ( default : 4250 )

#endif // 0