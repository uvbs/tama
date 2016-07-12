
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
class XSocketForGameSvr;

// This is DBAServer
class XMain : public XEServerMain //DBAgent
						, public XDelegateOption
{
public:
	XFps m_fpsFromGameSvr;
	XFps m_fpsToGameSvr;
	static void sCreateMemoryPool();
	static void sDestroyMemoryPool();

private:
	XEOption m_Env;
//	XSocketForGameSvr *m_pSocketForGameSvr;	// 클라이언트가 접속하는 소켓
	int m_Port;				// 게임서버들이 접속해올 포트
	std::string m_strPublicKey;		// 구글플레이용 공개키
	int m_maxPool = 10000;		// 메모리풀 크기
	void Init() {
//		m_pSocketForGameSvr = NULL;
		m_Port = 0;
	}
	void Destroy();
public:
	XMain() 
	: m_Env( this ) { Init(); }
	virtual ~XMain() { Destroy(); }
	//
//	GET_ACCESSOR( XSocketForGameSvr*, pSocketForGameSvr );
// 	std::shared_ptr<XSocketForGameSvr> GetpSocketForGameSvr() const;
	XSocketForGameSvr* GetpSocketForGameSvr() const;
	GET_ACCESSOR_CONST( const std::string&, strPublicKey );
//	GET_STRING_ACCESSOR( strIP );	
//	GET_ACCESSOR_CONST(ID, ServerID);
	GET_ACCESSOR_CONST( int, maxPool );

	//
	void Create( LPCTSTR szINI );
	//
	BOOL LoadINI( CToken &token ) override {
		XBREAK(1);
		return TRUE;
	}
	void DidFinishCreate() override;
	void Process() override;
	void ConsoleMessage( LPCTSTR szMsg ) override;
	XEContents* CreateContents();
	bool DelegateDidFinishLoad( const XParamObj2& param ) override;
	void UpdateSleepDBMng( const XEOption* pEnv );
	void UpdateSleepServer( const XEOption* pEnv, XEWinSocketSvr* pSocketSvr );
	void UpdateSleepMain( const XEOption* pEnv );
	void ReloadIni();
};

extern XMain			*MAIN;
//extern XDatabase		*DB;

