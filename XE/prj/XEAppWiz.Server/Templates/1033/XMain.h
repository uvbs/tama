#pragma once
#include "server/XServerMain.h"
#include "Network/XNetworkDelegate.h"

namespace xHTTP {
	enum xtReqID {
		xREQ_NONE=0,
		xREQ_APPLE_VERIFY,
	};
};

class XDatabase;
class XSocketForClient;

class XMain : public XEServerMain
						, public XNetworkDelegate
						, public XCrypto
{
public:
	static void sCreateMemoryPool();
	static void sDestroyMemoryPool();
public:
	XMain() {}
	virtual ~XMain() { Destroy(); }
	//
	GET_ACCESSOR( XSocketForClient*, pSocketForClient );
	GET_ACCESSOR( int, maxConnect );
	GET_ACCESSOR( const std::string&, strPublicKey );
	GET_ACCESSOR( const std::string&, strIPLoginSvr );
	GET_ACCESSOR( const std::string&, strIPDBASvr );
	GET_ACCESSOR( const std::string&, strIPExternal );
	GET_ACCESSOR( WORD, GSvrID );
	int GetServerID(){ return m_GSvrID; }
	//
	void Create( LPCTSTR szINI ) override;
	BOOL LoadINI( CToken &token ) override;
	void DidFinishCreate() override;
	void DoConnectLoginServer( void );
	void DoConnectDBAServer();
	void DelegateConnect( XENetworkConnection *pConnect, DWORD param );
	void Process() override;
	void ConsoleMessage( LPCTSTR szMsg ) override;
	XEContents* CreateContents();
private:
	XSocketForClient *m_pSocketForClient = nullptr;	// 클라이언트가 접속하는 소켓
	int m_maxConnect = 0;		// 최대 접속 클라 수
	std::string m_strPublicKey;		// 구글플레이용 공개키
	WORD m_PortLoginSvr = 0;			// 로그인서버 포트
	WORD m_PortDBASvr = 0;			// DBA서버 포트
	WORD m_PortForClient = 0;	// 클라이언트가 접속할 포트
	WORD m_GSvrID = 0;				// 게임서버들의 아이디
	std::string m_strIPLoginSvr;	// 로그인 서버 아이피
	std::string m_strIPDBASvr;		// DBA서버 아이피
	std::string m_strIPExternal;	// 클라가 접속해올 외부아이피
private:
	void Destroy();
};

extern XMain			*MAIN;

