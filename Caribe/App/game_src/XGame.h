#pragma once
#include "XFramework/client/XEContent.h"
#include "XGameCommon.h"
//#include "XSceneBase.h"
//#include "Network/XNetworkDelegate.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XTimeoutMng.h"
#include "XFramework/XESceneMng.h"
#include "XSpot.h"
#include "_Wnd2/XWndH.h"
#include "_Wnd2/XWndEdit.h"
//#include "XAccount.h"
#include "XFramework/Game/XFLevelH.h"
#include "XPropHelpH.h"
//#include "XImage.h"
#include "XFramework/XFacebook.h"
#include "Sprite/XDelegateSprObj.h"
//#include "XOption.h"

class XWorld;
class XOption;
class XSockGameSvr;
class XSockLoginSvr;
class XEBaseScene;
class XAccount;
class XFLevel;
class XSceneBase;
class XGameWndAlert;
class XWndAlert;
//#define GAME_VER		__DATE__##__TIME__//_T("2014.10.7.c")

namespace XGAME {
	enum xtEditID {
		EID_NONE,
		EID_CHANGE_IP,
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
	enum xtAlertWorld {
		xAW_NONE,
		xAW_TRAIN_COMPLETE,
		xAW_RESEARCH_COMPLETE,
	};
	// 월드씬 진입하면 띄워야 하는 팝업류
	struct xAlertWorld {
		xtAlertWorld m_Type;
		ID m_snHero = 0;
		ID m_idParam = 0;
		int m_Level = 0;
		XGAME::xtTrain m_Train;
		_tstring m_strMsg;
		ID getid() const {
			return m_snHero;
		}
	};
};
namespace xHelp {
	class XSeq;
//	enum xtEvent;
}



//extern XAppLayout *LAYOUT;

class XGame : public XEContent, public XGameCommon//, public XNetworkDelegate
													, public XDelegateFontMng
													, public XDelegateLayout
													, public XEDelegateSceneMng
													, public XDelegateSpot
													, public XDelegateLevel
													, public XDelegateWnd
													, public XTimeoutDelegate
													, public XDelegateFacebook
													, public XDelegateWndEdit
													, public XDelegateSprObj
{
public:
	static XGame* sGet() {
		return s_pInstance;
	}
	struct xBrilliant {
		float dAng;
		ID idWnd;
		float scale;
		xBrilliant() {
			dAng = 0.f;
			idWnd = 0;
			scale = 1.f;
		}
		ID getid() {
			return idWnd;
		}
	};
	static _tstring s_strVerBuild;		// 빌드버전
	static std::string s_strGcmRegid;		// gcm 푸쉬용 regid
	static bool s_bLoaded;
	static bool sIsHaveSessionKey() {
		return !s_strSessionKey.empty();
	}
	static const _tstring& sGetSessionKey() {
		return s_strSessionKey;
	}
	static void sSetSessionKey( LPCTSTR szSessionKey ) {
		s_strSessionKey = szSessionKey;
	}
#ifdef _CHEAT
	xHelp::XSeq *m_pCurrSeq = nullptr;
	std::string m_strcNodeParticle;
	static _tstring s_strMouseSpr;		// 마우스커서에 달려서 테스트해볼수 있는 spr파일명
	XSprObj* m_psoMouse = nullptr;		// 테스트용
#endif // cheat
private:
	static XGame* s_pInstance;
	static _tstring s_strSessionKey;		// 임시
	XESceneMng *m_pSceneMng;
	ID m_idAccount;						// 임시 저장
	BOOL m_bNewAccount;			// 새로 계정을 생성시켜야 하는가.
	BOOL m_bLockPassword;			// 로그인서버로 접속했는데 계정이 비번걸려있는 상태다.
//	XSPAcc m_spAccount;
	XGAME::PURCHASE *m_pIncompletePurchase;	// 결제 처리가 끝나지 않은 결제정보
	XSprObj *m_psoBrilliant;
	XList<xBrilliant> m_listBrilliant;
	CTimer m_timerSec;			// 1초타이머
	CTimer m_timerMin;
	bool m_bLevelUp = false;			// 레벨업 이펙트
	XSprObj *m_psoWait = nullptr;
	bool m_bTodaysTip = false;		// 오늘의 팁 출력했는지 여부.
	XGuild *m_pGuild = nullptr;
	CTimer m_timerEmitter;
	int m_Count = 0;
	XList4<std::string> m_listAllowWnd;
	XList4<std::string> m_listSeq;
	XSurface *m_psfcBgPopup = nullptr;	// 팝업배경뒤에 어두운 그라뎅
	XOption* m_pOption = nullptr;
//	bool m_bFirst = false;			// 앱을 처음 실행시킨 상태인가.
	int m_cntEventOpening = 0;
	int m_cntRefBgDark = 0;			// bg_dark의 참조카운터
	XList4<XGAME::xAlertWorld> m_listAlertWorld;			// 월드씬 진입하면 띄워야할 알림들.
	void Init() {
		m_pSceneMng = nullptr;
		m_idAccount = 0;
		m_pIncompletePurchase = nullptr;
		m_psoBrilliant = nullptr;
	}
	void Destroy();
public:
	XSurface* m_psfcProfile = nullptr;		// 플레이어의 프로필 이미지
	XGame();
	virtual ~XGame() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( ID, idAccount );
	ID GetidAcc() const {
		return m_idAccount;
	}
	GET_SET_ACCESSOR_CONST( BOOL, bNewAccount );
	GET_SET_ACCESSOR_CONST( BOOL, bLockPassword );
//	GET_ACCESSOR( XSPAcc, spAccount );
	GET_ACCESSOR( XGAME::PURCHASE*, pIncompletePurchase );
	GET_SET_ACCESSOR( XSurface*, psfcBgPopup );
	GET_SET_ACCESSOR_CONST( bool, bLevelUp );
	GET_SET_ACCESSOR_CONST( bool, bTodaysTip );
	GET_ACCESSOR( XList4<std::string>&, listAllowWnd );
	GET_ACCESSOR( XESceneMng*, pSceneMng );
	GET_ACCESSOR_CONST( XSurface*, psfcProfile );
//	GET_SET_BOOL_ACCESSOR( bFirst );
	bool IsbFirst() const;
	inline bool GetbFirst() const {
		return IsbFirst();
	}
	void ClearidsAllow() {
		m_listAllowWnd.clear();
	}
// 	void SetidsAllowWnds( std::vector<std::string>& ary ) {
// 		m_listAllowWnd = ary;
// 	}
	void AddidsAllowWnd( std::vector<std::string>& ary ) {
		for( auto& elem : ary ) 
			m_listAllowWnd.Add( elem );
	}
	void AddidsAllowWnd( const std::string& idsAllowWnd ) {
		m_listAllowWnd.Add( idsAllowWnd );
	}
// 	void SetspAccount( XSPAcc spAccount ) {
// #ifdef _DEBUG
// 		XBREAK( m_spAccount != nullptr );
// #endif
// 		m_spAccount = spAccount;
// 	}
	XEBaseScene* GetCurrScene() {
		return m_pSceneMng->GetpScene();
	}
	GET_ACCESSOR( XOption*, pOption );
	//
	void DidFinishCreated() override;
// 	void Create();
	void CreateGameResource();
	void DestroyGameResource();
	void CopyLuaToWork();
	void TryReconnect();
	XWndAlert* RecvMsgBox( LPCTSTR szBuff, ID idText, LPCTSTR szTitle=nullptr );
	void RecvDuplicateLogout();
	int ProcessScene( float dt );
// 	void CreateGameSvrSocketAndTryConnect( BOOL bReconnect, std::string strIP, WORD port, DWORD param );
// 	void CreateLoginSvrSocketAndTryConnect( BOOL bReconnect, DWORD param );
	void CheckReconnect();
	void ProcessConnection();
	void DoReconnectForExistUser( float secDelay );
	//
	int Process( float dt ) override;
	void Draw() override;
	// handler
	void DrawDebugInfo( float x, float y, XCOLOR col, XBaseFontDat* pFontDat ) override;
	void OnLButtonDown( float x, float y ) override;
	void OnMouseMove( float x, float y ) override;
	void OnLButtonUp( float x, float y ) override;
	void OnRButtonDown( float x, float y ) override;
	void OnRButtonUp( float x, float y ) override;
	void OnCheatMode();
	void OnEndEditBox( int idEditField, const char *cStr );
	BOOL RestoreDevice();
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
// 	void DelegateConnect( XENetworkConnection *pConnect, DWORD param ) override;
// 	void DelegateNetworkError( XENetworkConnection *pConnect, XE::xtNError error ) override;
	virtual void OnDelegateLayoutPlaySound( ID idSnd );
	virtual void OnDelegateLayoutPlayMusic( LPCTSTR szBgm );
	//	virtual void OnDelegateScheduleEvent( XScheduler *pSchedule );
	void OnTimeout2( DWORD idPacket, ID idSubPacket ) override;
	virtual void OnResume();
	virtual void OnPause();
//	void OnFinishPatchClient();
	int DoAsyncBuyIAP( LPCTSTR szSku, const std::string& strcPayload, int price = 0 );
	inline int DoAsyncBuyIAP( const _tstring& strSku, const std::string& strcPayload, int price ) {
		return DoAsyncBuyIAP( strSku.c_str(), strcPayload, price );
	}
#ifdef _NEW_INAPP
#else
	void OnPurchaseFinishedConsume( BOOL bGoogle,
		const char *cJson,
		const char *cidProduct, 
		const char *cToken,
		const char *cPrice,
		const char *cOrderId,
		const char *cTransactDate );
#endif // not _NEW_INAPP
	void OnPurchaseError( const char *cErr );
	void OnPurchaseStart( const char *cSku );
	void DestroyIncompletePurchase() {
		SAFE_DELETE( m_pIncompletePurchase );
	}
	XEBaseScene* DelegateCreateScene( XESceneMng *pSceneMng, ID idScene, XSPSceneParam& spParam ) override;
	void DelegateOnDestroy( XEBaseScene *pScene );
	void DelegateOnDestroyAfter( ID idSceneDestroy, ID idSceneNext, XSPSceneParam spParam );
// 	// fsm
	void DelegateOnCalcTimer( XSpot *pBaseSpot, const XTimer2& timerCalc );
	DWORD OnDelegateGetMaxExp( const XFLevel *pLevel, int level, DWORD param1, DWORD param2 ) const override;
	int OnDelegateGetMaxLevel( const XFLevel *pLevel, DWORD param1, DWORD param2 ) const override;

	void AddBrilliant( ID idWnd, float scale = 1.f );
	void DelBrilliant( ID idWnd );
	xBrilliant* FindBilliant( ID idWnd );
	void ClearBrilliant() {
		m_listBrilliant.Clear();
	}
	void DrawBrilliant( xBrilliant *pBr, XWnd *pWnd );
	void FrameMoveBrilliant( float dt );
	void AddAlert( XWnd *pButt, ID idAct, const XE::VEC2& _vPos = XE::VEC2(-1) ) const;
	void DelAlert( XWnd *pButt, ID idAct ) const;
	virtual void DelegateBeforeDraw( XWnd *pWnd );
	void Update() override;
	void ProcessResearch( float dt );
	void ProcessTraining( float dt );
	int OnClickClearAcc( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickItemTooltip( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickSkillTooltip( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickHeroTooltip( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnClickTooltip( XWnd* pWnd ) override;
	int OnGotoAppStore( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnCreateFailedSocket();
	void ProcessAP( float dt );
	void OnOnline();
	int OnClickCreateItemFromTooltip( XWnd* pWnd, DWORD p1, DWORD p2 );
	void SetGreenAlert( XWnd *pParent, bool bShow, const XE::VEC2& vLocal = XE::VEC2(0), float scale = 1.f );
	XSceneBase* GetpScene();
	int OnGotoStart( XWnd* pWnd, DWORD p1, DWORD p2 );
	//////////////////////////////////////////////////////////////////////////
	// jw

	// 운영자 메세지
private:
	static std::list<_tstring> s_masterMessages;
	CTimer m_masterMessgeTimer;
	XWndTextString *m_wndMasterText = nullptr;
public:

	// 운영자 메세지
	static void sPushMasterMessage(_tstring& str);
	void MasterMessageStart();
#ifdef _CHEAT
	bool ToggleXuzhuMode();
	void WriteGoldLog( const char *cFormat, ... );
#endif // _CHEAT

	//길드
	GET_ACCESSOR(XGuild*, pGuild);
	void DelGuild();	//길드 삭제(탈퇴, 추방)
	void SetpGuild(XGuild* pGuild);
	void OnReload() override;
	void OnClickWnd( XWnd *pWnd ) override;
	bool _DoCutScene( const char* idsSeq ) {
		return DoCutScene( idsSeq );
	}
private:
	bool DoCutScene( const char* idsSeq );
	bool DoCutScene( const std::string& idsSeq ) {
		return DoCutScene( idsSeq.c_str() );
	}
public:
	void DestroySeq();
	void OnEndSeq( xHelp::XSeq *pSeq );
	void OnRecvEndSeq( const std::string& idsSeq );
	void DelSeqWnd( const std::string& idsSeq );
	void SetActive( bool bFlag );
	void DispatchEventToSeq( xHelp::xtEvent event );
	bool IsOutsideClickedAllowWnd( const XE::VEC2& vPos );
	inline bool DispatchEvent( XGAME::xtActionEvent event, DWORD dwParam ) {
		return DispatchEvent( event, dwParam, std::string(), xHelp::DEFAULT );
	}
	inline bool DispatchEvent( XGAME::xtActionEvent event, const std::string& strParam ) {
		return DispatchEvent( event, xHelp::DEFAULT, strParam, xHelp::DEFAULT );
	}
	inline bool DispatchEvent( XGAME::xtActionEvent event, DWORD dwParam, const std::string& strParam ) {
		return DispatchEvent( event, dwParam, strParam, xHelp::DEFAULT );
	}
	inline bool DispatchEvent( XGAME::xtActionEvent event, const std::string& strParam, DWORD dwParam2 ) {
		return DispatchEvent( event, xHelp::DEFAULT, std::string(), dwParam2 );
	}
	inline bool DispatchEvent( XGAME::xtActionEvent event, DWORD dwParam, DWORD dwParam2 ) {
		return DispatchEvent( event, dwParam, std::string(), dwParam2 );
	}
	bool DispatchEvent( XGAME::xtActionEvent event, DWORD dwParam, const std::string& strParam, DWORD dwParam2 );
// 	void DispatchEvent( XGAME::xtActionEvent event, DWORD dwParam, const std::string& strParam = std::string(), DWORD dwParam2 = 0 );
	int OnClickUnlockMenu( XWnd* pWnd, DWORD p1, DWORD p2 );
	std::string GetidsSeqWnd( const std::string& idsSeq );
	XWnd* GetpRootSeq() {
		return Find("root.seq");
	}
	bool IsPlayingSeq() {
		return m_listSeq.size() > 0;
	}
	XGameWndAlert* WndAlert( LPCTSTR format, ... );
	void OnFinishAppearPopup( XWndView *pView ) override;
	void OnClosePopup( const std::string& idsWnd ) override;
	void OnBeforeDrawByPopup( XWndView *pView );
	XWndDarkBg* CreateDarkBg() override;
//	void CreateUpdateTopResource( XWnd *pRoot );
	int OnClickStopTutorial( XWnd* pWnd, DWORD p1, DWORD p2 );
	void ReleaseBgDark();
	void DestroyTutorialStopButton();
//	int OnClickTest( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnRecvProfileImageByFacebook( const std::string& strFbUserId, DWORD* pImg, int w, int h, int bpp ) override;
	void DelegateFacebookCertResult( const char *cUserId, const char *cUsername, DWORD param ) override;
	void DoRequestProfileImgAllSpot();
	void DoRequestPlayerProfileImg( XSPAcc spAcc );
	void OnDelegateEnterEditBox( XWndEdit *pWndEdit, LPCTSTR szString, const _tstring& strOld ) override;
	void OnDestroyAfterByWnd( ID idWndChild, const std::string& idsChild ) override;
	int OnFinishLoadedByScene( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickCtrlWnd( XWnd* pWnd, DWORD p1, DWORD p2 );
	bool OnReceiveCallbackData( const xnReceiverCallback::xData& data ) override;
	std::string GetGCMRegId() const;
	void OnEventEtc( const xSpr::xEvent& event ) override;
	const std::string OnSelectLanguageKey() override;
	int OnClickTopGemUI( XWnd*, DWORD, DWORD );
	int OnClickGotoGemShop( XWnd*, DWORD, DWORD );
	bool ToggleBGM();
	bool ToggleSound();
	void OnAfterPropSerialize() override;
	int OnClickFillAPByCash( XWnd* pWnd, DWORD p1, DWORD p2 );
	GET_ACCESSOR( XList4<XGAME::xAlertWorld>&, listAlertWorld );		// 시범적으로 만든거라 const로 하지 않음.
#ifdef _CHEAT
	int OnClickDebugShowLog( XWnd* pWnd, DWORD p1, DWORD p2 );
#endif // _CHEAT
	// 	static bool sDoAlertCampaignByError( XGAME::xtError errCode );
// 	static void sDoPopupSpotWithStageObj( XSpot* pBaseSpot, XSPCampObj spCampObj, XSPStageObj spStageObj );
	int OnDebug( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnRecvResearchCompleted( XHero* pHero, ID idAbil, int point );
	void DoPopupTrainComplete( XGAME::xtTrain train, XHero* pHero, int level );
}; // class XGame


//////////////////////////////////////////////////////////////////////////
extern XGame *GAME;
extern XSockGameSvr *GAMESVR_SOCKET;			// 월드서버와의 커넥션
extern XSockLoginSvr *LOGINSVR_SOCKET;			// 로긴서버와의 커넥션
// #define ACCOUNT	XAccount::sGetPlayer()

XWorld* sGetpWorld();

