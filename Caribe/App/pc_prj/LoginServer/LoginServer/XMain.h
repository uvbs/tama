#pragma once
#include "XFramework/server/XServerMain.h"
#include "Network/XNetworkDelegate.h"
#include "XFramework/XEOption.h"

namespace xHTTP {
	enum xtReqID {
		xREQ_NONE=0,
		xREQ_APPLE_VERIFY,
	};
};

class XDatabase;
class XSocketSvrForClient;
class XSocketForGameSvr;
class XGUserMng;
class XCUserMng;
class XDBUAccount;
class XPacket;
class XMain;
class XEOption;

class XMain : public XEServerMain
						, public XDelegateOption
#ifdef _DUMMY_GENERATOR
						, public std::enable_shared_from_this<XMain>
#endif // _DUMMY_GENERATOR
{
public:
//	static std::shared_ptr<XMain> s_spMain;
	XFps m_fpsToGameSvr;		// 게임서버로 로그인요청을 보내는 횟수
	XFps m_fpsFromGameSvr;	// 게임서버로부터 로그인요청결과를 받는 횟수.
	XFps m_fpsToClient;			// 클라이언트로부터 로그인결과를 보내는 횟수.
	XFps m_fpsFromClient;
public:
	static void sCreateMemoryPool( void );
	static void sDestroyMemoryPool( void );
private:
	int m_cntCreateAcc;						///< 더미 제네레이터. 생성할 계정수
	int m_cntSave;							///< 더미 생성 완료 카운터
	XSocketSvrForClient *m_pSocketForClient;		// 클라이언트가 접속하는 소켓
	XSocketForGameSvr *m_pSocketForGameSvr;	// 게임서버가 접속하는 소켓
	XEOption m_Env;

	int			m_maxGSvrConnect;				// 최대 접속 게임서버 수
	int			m_maxConnect;						// 최대 접속 클라 수
	int			m_Port;								// 클라이언트가 접속할 포트
	int			m_portGameSvr;						// 게임서버가 접속할 포트
	std::string	m_strPublicKey;						// 구글플레이용 공개키
	bool m_bServerOpen = true;
#ifdef _DUMMY_GENERATOR
	XVector<_tstring> m_aryNames;
	void LoadDummyNames();
#endif // _DUMMY_GENERATOR

	void Init() {
		m_pSocketForClient = NULL;
		m_pSocketForGameSvr = NULL;
		m_Port = 0;
		m_portGameSvr = 0;
		m_maxConnect = 0;
		m_maxGSvrConnect = 0;
		m_cntCreateAcc = 0;
		m_cntSave = 0;
	}

	void Destroy();
public:
	XMain() 
	: m_Env( this ) { Init(); }
	virtual ~XMain() { Destroy(); }
	//
	GET_ACCESSOR_CONST(bool, bServerOpen);
	GET_ACCESSOR( XSocketSvrForClient*, pSocketForClient );
	GET_ACCESSOR( XSocketForGameSvr*, pSocketForGameSvr );
	inline XSocketForGameSvr* const GetpSocketForGameSvrConst() const {
		return m_pSocketForGameSvr;
	}
	GET_ACCESSOR_CONST( int, maxConnect );
	GET_ACCESSOR_CONST( int, maxGSvrConnect );	
	GET_ACCESSOR_CONST( const std::string&, strPublicKey );
	int StartCreateDummy( int numCreate ) {
		m_cntCreateAcc = numCreate;
		m_cntSave = 0;
		return m_cntCreateAcc;
	}
	int IsFinishCreateDummy() const {
		return m_cntSave == 0;
	}
	//
	void Create( LPCTSTR szINI );
	void CreateSocketForClient( void );
	LPCTSTR GetGameSvrIPAndPort( WORD *pOutPort );
	//
	virtual BOOL LoadINI( CToken &token ) { 
		XBREAK(1);
		return TRUE; 
	}
	virtual void DidFinishCreate( void );
	virtual void Process( void );
	virtual void ConsoleMessage( LPCTSTR szMsg );
	XEContents* CreateContents( void );
#ifdef _DUMMY_GENERATOR
	void cbCreateNewAccount( XDatabase *pDB,
									const _tstring& strUUID,
									const _tstring& strName,
									const _tstring& strFbId,
									int cntCreateAcc );
	void OnResultCreateNewAccount( ID idAcc, const _tstring& strName, int cntCreateAcc );
	std::shared_ptr<XMain> GetThis() {
		return shared_from_this();
	}
#endif // _DUMMY_GENERATOR
	void Release() {}	// DBMng2를 위해 만듬.
	void Retain() {}	// DBMng2를 위해 만듬.
	void __SendForXE( XPacket& p )	{}	// DBMng2를 위해 만듬.
	void OnArriveSaveDummy();
#ifdef _XPROFILE
	void OnFinishProfile( XEProfile* pProfile ) override;
#endif // _XPROFILE
	bool DelegateDidFinishLoad( const XParamObj2& param ) override;
	void ReloadIni();
	void UpdateSleepDBMng( const XEOption* pEnv );
	void UpdateSleepServer( const XEOption* pEnv, XEWinSocketSvr* pSocketSvr );
	void UpdateSleepMain( const XEOption* pEnv );
};

extern std::shared_ptr<XMain> MAIN;

