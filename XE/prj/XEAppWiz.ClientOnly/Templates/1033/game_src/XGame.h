#pragma once

#include "client/XEContent.h"
#include "XGameCommon.h"
#include "XEBaseScene.h"
#include "Network/XNetworkDelegate.h"
#include "XAppLayout.h"
#include "client/XTimeoutMng.h"
#include "XESceneMng.h"


#define GAME_VER		_T("2014.05.30")

namespace XGAME {
	enum xtEditID {
		EID_NONE,
		EID_CHANGE_IP,
	};
	// FSM 상태
	enum xtFSMConnectState {
		xFS_NONE,
		xFS_CONNECT_GAMESVR,	// 게임서버로 연결시도
		xFS_RECONNECT_GAMESVR,	// 게임서버로 재연결시도
		xFS_SUCCESS_LOGIN,		// 게임서버로 연결이 끝난 상황
		xFS_DISCONNECT_GAMESVR,	// 게임서버와 연결이 끊긴후 대기시간
		xFS_GOTO_START,			// 접속을 끊고 게임 초기화면으로 돌아감
	};
	// FSM 이벤트
	enum xtFSMEvent {
		xFE_NONE,
		xFE_INIT,
		xFE_PROCESS,
		xFE_UNINIT,
		xFE_ETC
	};
	// 미결제 처리를 위한 구조체(나중에 객체화 시켜서 프레임워크로 들어갈것)
	struct PURCHASE {
		//		long long usTime;
		_tstring strJson;
		_tstring strTransactDate;	// 결제시간
		_tstring strSku;
		_tstring strToken;
		_tstring strOrderId;
		float price;
		BOOL bGoogle;
		PURCHASE() {
			bGoogle = FALSE;
			//			usTime = 0;
			price = 0;
		};
	};
};

extern XAppLayout *LAYOUT;

class XSockGameSvr;
class XEBaseScene;
class XGame : public XEContent, public XGameCommon, public XNetworkDelegate,
													public XDelegateFontMng,
													public XDelegateLayout,
													public XEDelegateSceneMng
{
public:
private:
	XESceneMng *m_pSceneMng;
	XGAME::PURCHASE *m_pIncompletePurchase;	// 결제 처리가 끝나지 않은 결제정보
	_tstring m_strNickname;
	void Init() {
		m_pSceneMng = NULL;
		m_pIncompletePurchase = FALSE;
	}
	void Destroy();
public:
	XGame();
	virtual ~XGame() { Destroy(); }
	//
	GET_ACCESSOR( XGAME::PURCHASE*, pIncompletePurchase );
	GET_SET_TSTRING_ACCESSOR( strNickname );
	//
	void Create( void );
	void CreateGameResource( void );
	void DestroyGameResource( void );
	void CopyLuaToWork( void );
	BOOL LoadTextTable( void );
	int ProcessScene( float dt );
	//
	virtual int Process( float dt );
	virtual void Draw( void );
	// handler
	virtual void OnLButtonDown( float x, float y );
	virtual void OnMouseMove( float x, float y );
	virtual void OnLButtonUp( float x, float y );
	virtual void OnRButtonDown( float x, float y );
	virtual void OnRButtonUp( float x, float y );
	virtual void OnCheatMode( void );
	virtual void OnEndEditBox( int idEditField, const char *cStr );
	virtual BOOL RestoreDevice( void );
	//
	int OnSoundDown( XWnd *pWnd, DWORD, DWORD );
	int OnExitApp( XWnd *pWnd, DWORD, DWORD );
	int OnEndHelpSeq( XWnd *pWnd, DWORD, DWORD );
	int OnIpConfig( XWnd *pWnd, DWORD, DWORD );
	int OnErrorDownload( XWnd *pWnd, DWORD, DWORD );
	int OnGotoLogin( XWnd *pWnd, DWORD p1, DWORD p2 );
	int OnEnterIP( XWnd *pWnd, DWORD p1, DWORD p2 );
	int OnDeveloperMode( XWnd *pWnd, DWORD p1, DWORD p2 );
	int OnQuestClicked( XWnd *pWnd, DWORD p1, DWORD p2 );
	XBaseFontDat* OnDelegateCreateFontDat( XFontMng *pFontMng, LPCTSTR szFont, float sizeFont );
	virtual void OnDelegateLayoutPlaySound( ID idSnd );
	virtual void OnDelegateLayoutPlayMusic( LPCTSTR szBgm );
	//	virtual void OnDelegateScheduleEvent( XScheduler *pSchedule );
	//	virtual void OnTimeout( DWORD idPacket );
	virtual void OnResume( void );
	virtual void OnPause( void );
	void OnFinishPatchClient( void );
	int DoAsyncBuyIAP( LPCTSTR szSku );
	void OnPurchaseFinishedConsume( BOOL bGoogle,
		const char *cJson,
		const char *cidProduct, 
		const char *cToken,
		const char *cPrice,
		const char *cOrderId,
		const char *cTransactDate );
	void OnPurchaseError( const char *cErr );
	void OnPurchaseStart( const char *cSku );
	void DestroyIncompletePurchase( void ) {
		SAFE_DELETE( m_pIncompletePurchase );
	}
	virtual XEBaseScene* DelegateCreateScene( XESceneMng *pSceneMng, ID idScene, XGAME::xSPM_BASE* pParam );
};
extern XGame *GAME;


