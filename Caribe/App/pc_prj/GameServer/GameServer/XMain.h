#pragma once
#include "XFramework/server/XServerMain.h"
// #include "XLoginConnection.h"
// #include "XDBASvrConnection.h"
#include "Network/XNetworkDelegate.h"
#include "etc/XTimer2.h"
#include "XFramework/Game/XFLevel.h"
#include "XEnv.h"

namespace xHTTP {
	enum xtReqID {
		xREQ_NONE=0,
		xREQ_APPLE_VERIFY,
	};
};

class XLoginConnection;
class XDatabase;
class XSocketSvrForClient;
// This is GameServer
class XMain : public XEServerMain, public XNetworkDelegate ,public XCrypto
{
public:
	struct xResponseTime {
		ID m_idAcc = 0;
		CTimer m_timer;
	};
	static XMain* sGet() {
		return s_pInstance;
	}
	static void sCreateMemoryPool( void );
	static void sDestroyMemoryPool( void );	
	XFps m_fpsFromLogin;		// 로그인서버로부터 오는 건수
	XFps m_fpsToLogin;			// 로그인서버로 보내는 건수
	XFps m_fpsFromDBASave;
	XFps m_fpsFromDBADupl;
	XFps m_fpsFromDBALoad;
	XFps m_fpsToDBA;			// 0:load 1:save 2:logout
	XFps m_fpsFromClient;		// 초당 클라이언트 로그인수.
	XFps m_fpsToClientOk;		// 로그인 성공
	XFps m_fpsToClientRetry;		// 계정이 아직안와서 재시도 시킨 횟수
	XFps m_fpsDisconnect;		// 초당 끊어진 계정
	std::unordered_map<ID, xResponseTime> m_mapResponse;
	double m_dLoad = 0;
	double m_cntLoad = 0;
#ifdef _DUMMY_GENERATOR
	XVector<_tstring> m_aryNames;
	void LoadDummyNames();
#endif // _DUMMY_GENERATOR
	//
private:
// 	WORD m_PortLoginSvr;			// 로그인서버 포트
// 	WORD m_PortDBASvr;			// DBA서버 포트
// 	WORD m_PortForClient;	// 클라이언트가 접속할 포트
// 	WORD m_GSvrID;				// 게임서버들의 아이디
// 
// 	std::string m_strIPLoginSvr;	// 로그인 서버 아이피
// 	std::string m_strIPDBASvr;		// DBA서버 아이피
// 	std::string m_strIPExternal;	// 클라가 접속해올 외부아이피

	XSocketSvrForClient *m_pSocketForClient;	// 클라이언트가 접속하는 소켓
// 	int m_maxConnect;		// 최대 접속 클라 수
// 	std::string m_strPublicKey;		// 구글플레이용 공개키
	CTimer m_timerRank;		// 랭킹용 
	void Init() {
// 		m_PortLoginSvr = 0;
// 		m_PortDBASvr = 0;
// 		m_PortForClient = 0;
// 		m_GSvrID = 0;
		m_pSocketForClient = NULL;
// 		m_maxConnect = 0;
	}
	void Destroy();
public:
	XMain() { Init(); s_pInstance = this; }
	virtual ~XMain() { Destroy(); }
	//	
	GET_ACCESSOR( XSocketSvrForClient*, pSocketForClient );
// 	GET_ACCESSOR( int, maxConnect );
// 	GET_ACCESSOR( WORD, GSvrID );
//	int GetServerID(){ return m_GSvrID; }
	int GetServerID() const {
		return XEnv::sGet()->GetGSvrID();
	}
// 	GET_ACCESSOR_CONST( const std::string&, strPublicKey );
// 	GET_STRING_ACCESSOR( strIPLoginSvr );	// const char* GetstrIPExternal();
// 	GET_STRING_ACCESSOR( strIPDBASvr );
// 	GET_STRING_ACCESSOR( strIPExternal );

	//Servers Connection
	void DoConnectLoginServer( void );
	void DoConnectDBAServer( void );
	//
	void Create( LPCTSTR szINI );
	//
	BOOL LoadINI( CToken &token ) override {
		XBREAK(1);		// deprecated
		return TRUE;
	}
	virtual void DidFinishCreate( void );
	virtual void Process( void );
	virtual void ConsoleMessage( LPCTSTR szMsg );
	virtual void DelegateConnect( XENetworkConnection *pConnect, DWORD param );
	DWORD OnDelegateGetMaxExp( const XFLevel *pLevel, int level, DWORD param1, DWORD param2 ) const;
	int OnDelegateGetMaxLevel( const XFLevel *pLevel, DWORD param1, DWORD param2 ) const;
	XEContents* CreateContents( void );
	int GetPublicKeyWithEncrypted( XCrypto* pCryptObj, BYTE* pOut, const int sizeMax );
	void UpdateSleepServer( const XEOption* pEnv, XEWinSocketSvr* pSocketSvr );
	void UpdateSleepMain( const XEOption* pEnv );
	void ReloadIni();
private:
	static XMain* s_pInstance;
};

extern XMain					*MAIN;
extern XLoginConnection		*LOGIN_SVR;
//extern XDBASvrConnection	*DBA_SVR; 
#define DBA_SVR  XDBASvrConnection::sGet()
//extern XDatabase		*DB;

