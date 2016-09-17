#pragma once
//#include "XWindow.h"
#include "etc/Timer.h"
//#include "../XDrawGraph.h"
#include "XFramework/client/XTimeoutMng.h"
#include "XFramework/Game/XFLevel.h"
//#include "XWorld.h"
#include "XPropUnit.h"
//#include "XPropTech.h"
#include "XBaseItem.h"
#include "XAccount.h"
#include "XFramework/client/XEContent.h"
//#include "XPropCamp.h"
#include "XPropHero.h"
//#include "XPropBgObjH.h"
#include "XStruct.h"
#include "xscenebase.h"
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWndCtrls.h"
//#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndSprObj.h"
#include "_Wnd2/XWndImage.h"

namespace xHelp {
	class XOrderDialog;
}

#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

#ifndef _CLIENT
#error "only allow client build"
#endif // not CLIENT

#define STATIC_DESTROY_WND(IDS)		static void sDestroyWnd() { XE::GetGame()->DestroyWndByIdentifier(IDS); }

//class XAppLayout;
class XWndResIcon;
class XQuestObj;
class XSceneWorld;
class XGameWndAlert;
class XStatistic;
class XLegionObj;
class XSpot;
class XSpotCastle;
class XSpotJewel;
class XSpotSulfur;
class XSpotMandrake;
class XSpotNpc;
class XSpotVisit;
class XSpotCampaign;
class XSpotCash;
class XSpotDaily;
class XSpotSpecial;
class XSpotCommon;
class XSceneTitle;
class XWndScrollView;
//extern XAppLayout *LAYOUT;

namespace XSKILL {
class XSkillDat;
};

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/11/24 19:52
*****************************************************************/
class XWndLayerTest : public XWndPopup
{
public:
	XWndLayerTest( LPCTSTR szXml, const char* cNode );
	virtual ~XWndLayerTest() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	int m_Test = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndLayerTest


class XGameWndAlert : public XWndAlert
{
	BOOL m_bTouchNC;
	void Init() {
		m_bTouchNC = FALSE;
	}
	void Destroy() {}
public:
	XGameWndAlert();
	XGameWndAlert( LPCTSTR szLayout, const char *cKey, bool bDummy );
	XGameWndAlert( const _tstring& strXml, const std::string& strKey ) 
		: XGameWndAlert( strXml.c_str(), strKey.c_str(), true ) {}
	XGameWndAlert( LPCTSTR szText, LPCTSTR szTitle=nullptr, XWnd::xtAlert type=XWnd::xCLOSE, XCOLOR col=XCOLOR_WHITE );
	XGameWndAlert( const _tstring& strText, LPCTSTR szTitle=nullptr, XWnd::xtAlert type=XWnd::xCLOSE, XCOLOR col=XCOLOR_WHITE )
		: XGameWndAlert( strText.c_str(), szTitle, type, col ) {}
	XGameWndAlert( float w, float h );
	virtual ~XGameWndAlert() { Destroy(); }
	void OnNCLButtonDown( float lx, float ly );
	void OnNCLButtonUp( float lx, float ly );
	void OnNCMouseMove( float lx, float ly );
};
// #define	XWND_ALERT( F...) { \
// 	if( GAME->GetpScene() ) \
// 		GAME->GetpScene()->WndAlert( __VA_ARGS__ ); \
// 		else \
// 		GAME->WndAlert( __VA_ARGS__ ); \
// }
#define	XWND_ALERT(F,...) \
	( GAME->GetpScene() )? GAME->GetpScene()->WndAlert( _T(F), ##__VA_ARGS__ ) : GAME->WndAlert( _T(F), ##__VA_ARGS__ );

#define	XWND_ALERT_T(F,...) \
	( GAME->GetpScene() )? GAME->GetpScene()->WndAlert( F, ##__VA_ARGS__ ) : GAME->WndAlert( F, ##__VA_ARGS__ );

#define	XWND_ALERT_YESNO( IDS, F,...) \
	( GAME->GetpScene() )? \
		GAME->GetpScene()->WndAlertYesNo( IDS, _T(F), ##__VA_ARGS__ ) :\
		GAME->WndAlert( _T(F), ##__VA_ARGS__ );
#define	XWND_ALERT_YESNO_T( IDS, F,...) \
	( GAME->GetpScene() )? \
		GAME->GetpScene()->WndAlertYesNo( IDS, F, ##__VA_ARGS__ ) :\
		GAME->WndAlert( F, ##__VA_ARGS__ );

#define	XWND_ALERT_OKCANCEL( IDS, F,...) \
	( GAME->GetpScene() )? \
		GAME->GetpScene()->WndAlertOkCancel( IDS, _T(F), ##__VA_ARGS__ ) :\
		GAME->WndAlert( _T(F), ##__VA_ARGS__ );
#define	XWND_ALERT_OKCANCEL_T( IDS, F,...) \
	( GAME->GetpScene() )? \
		GAME->GetpScene()->WndAlertOkCancel( IDS, F, ##__VA_ARGS__ ) :\
		GAME->WndAlert( F, ##__VA_ARGS__ );

#define	XWND_ALERT_PARAM( TITLE, TYPE, COL, F,...) \
	( GAME->GetpScene() )? \
		GAME->GetpScene()->WndAlertParam( TITLE, TYPE, COL, _T(F), ##__VA_ARGS__ ) :\
		GAME->WndAlert( _T(F), ##__VA_ARGS__ );
#define	XWND_ALERT_PARAM_T(F,...) \
	( GAME->GetpScene() )? \
		GAME->GetpScene()->WndAlertParam( TITLE, TYPE, COL, F, ##__VA_ARGS__ ) :\
		GAME->WndAlert( F, ##__VA_ARGS__ );


class XWndCircleMenu;

////////////////////////////////////////////////////////////////
/**
 정찰 팝업창
*/
class XWndSpotRecon : public XWndPopup
{
private:
	XList4<XGAME::xRES_NUM> m_listLoot;
	XSpot *m_pBaseSpot = nullptr;
//	XLegion *m_pLegion = nullptr;	// 리젼이 파괴될수도 있어서 스팟에서 직접 얻도록 바꿈.
#ifdef _CHEAT
	bool m_bCheatShow = false;
#endif // _CHEAT
	void Init() {}
	void Destroy();
public:
	XWndSpotRecon( XLegion *pLegion, XSpot *pBaseSpot );
	virtual ~XWndSpotRecon() { Destroy(); }
	///< 
	XGAME::xRES_NUM* AddLoot( XGAME::xtResource typeRes, float num ) {
		XBREAK( num == 0 );
		XGAME::xRES_NUM res( typeRes, num );
		m_listLoot.Add( res );
		return m_listLoot.GetpLast();
	}
	XGAME::xRES_NUM* AddLoot( XGAME::xRES_NUM res ) {
		return AddLoot( res.type, res.num );
	}
	void Update() override;
	int OnClickSquad( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickShow( XWnd* pWnd, DWORD p1, DWORD p2 );
};

////////////////////////////////////////////////////////////////
/**
 유저스팟 정보
*/
class XWndCastleInfo : public XGameWndAlert
{
	void Init() {}
	void Destroy();
public:
	XWndCastleInfo();
	virtual ~XWndCastleInfo() { Destroy(); }
};


class XWndSquadInLegion : public XWnd
{
public:
	sGET_SET_ACCESSOR( int, idxSelectedSquad );
	static void sClearidxSelectedSquad() {
		s_idxSelectedSquad = -1;
	}
	static XWndSquadInLegion* sUpdateCtrl( XWnd *pRoot, int idxSquad, const XE::VEC2& v, LegionPtr spLegion );
	static std::string sGetIds( int idxSquad ) {
		return std::string( XE::Format("squad.idx.%d", idxSquad) );
	}
private:
	static int s_idxSelectedSquad;
	//
	XHero *m_pHero;
	XGAME::xtUnit m_Unit = XGAME::xUNIT_NONE;
	int m_idxSquad = -1;	// 이값은 한번 세팅되면 변경되지 않음.
	XLegion *m_pLegion;
	XSurface *m_psfcFace;
	XSurface *m_pStar[4];
//	XSurface *m_pNameCard;
	XSurface *m_pCrown;
	XBaseFontObj *m_pName;
	int m_nGrade;
	bool m_bDrawFrame = false;
	bool m_bQuestion = false;
//	XSurface *m_psfcSelected = nullptr;		// 선택되었을때 선택표시프레임
	bool m_bPlayer = false;
	int m_nSuperior = 0;		// 우세도마크, 2:매우우세 1:우세 0:동급 -1:열세 -2:매우열세
//	XSurface *m_psfcSuperior = nullptr;		// 우세도 마크
	void Init() {
		m_pHero = NULL;
		m_pLegion = nullptr;
		m_psfcFace = nullptr;
		for (int i = 0; i < 4; ++i)
			m_pStar[i] = NULL;
//		m_pNameCard = nullptr;
		m_nGrade = 0;
		m_pName = nullptr;
		m_pCrown = nullptr;
	}
	void Destroy();
public:
	XWndSquadInLegion( XHero *pHero, const XE::VEC2& vPos, XLegion *pLegion, bool bDrawFrame = true, bool bDragDrop = true, bool bQuestion = false );
	XWndSquadInLegion( int idxSquad, const XE::VEC2& vPos, XLegion *pLegion, bool bDrawFrame = true, bool bDragDrop = true );
	virtual ~XWndSquadInLegion() { Destroy(); }
	///< 
	GET_ACCESSOR( XHero*, pHero );
	GET_SET_ACCESSOR( bool, bPlayer );
	GET_SET_ACCESSOR( int, nSuperior);
	GET_ACCESSOR( int, idxSquad );
	//
	void Draw( void );
//	int OnDrop( XWnd* pWnd, DWORD p1, DWORD p2 );
	void SetFace( XHero *pHero, bool bQuestion = false );
	void DrawDrag( const XE::VEC2& vMouse );
	BOOL IsFree() {
		if (m_pHero)
			return FALSE;
		else
			return TRUE;
	}
	void Update() override;
private:
	XWndImage* CreateGlowSelected();
}; // class XWndSquadInLegion

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/01/13 12:52
*****************************************************************/
class XWndVisit : public XWndView
{
private:
	XSpotVisit *m_pSpot;
	void Init() {
		m_pSpot = nullptr;
	}
	void Destroy();
public:
	XWndVisit( XSpotVisit *pSpot );
	virtual ~XWndVisit() { Destroy(); }
	//
	BOOL OnCreate() override;
	int OnOk( XWnd* pWnd, DWORD p1, DWORD p2 );
}; // class XWndVisit

#ifdef _xIN_TOOL
/**
 @brief 스팟툴용 스팟 UI
*/
class XWndSpotForTool : public XWndSprObj
{
public:
	static XWndSpotForTool *s_pDrag;
	static XWndSpotForTool *s_pSelected;
	static XWndSpotForTool *s_pMouseOver;
	static XE::VEC2 s_vUndo;		// 드래그 하기 이전에 있던 스팟의 좌표(언두용)
	static ID s_idUndoSpot;
private:
	XPropWorld::xBASESPOT *m_pBaseProp = nullptr;
	XE::VEC2 m_vLDown;
	XE::VEC2 m_vPrev;
	XWndTextString *m_pTextName = nullptr;
	void Init() {
	}
	void Destroy() {}
public:
	XWndSpotForTool( XPropWorld::xBASESPOT *pBaseProp );
	virtual ~XWndSpotForTool() { Destroy(); }
	//
	GET_ACCESSOR( XPropWorld::xBASESPOT*, pBaseProp );
	void Draw();
	void OnLButtonDown( float lx, float ly ) override;
	void OnMouseMove( float lx, float ly ) override;
	void OnLButtonUp( float lx, float ly ) override;
	void Update() override;
}; // class XWndSpotForTool
#endif // _xIN_TOOL


/****************************************************************
* @brief 기습 팝업
* @author xuzhu
* @date	2015/02/16 13:49
*****************************************************************/
class XWndEncounter : public XWndPopup
{
	void Init() {}
	void Destroy() {}
public:
//	XWndEncounter( int numSulfur, ID idEnemy, int level, LPCTSTR szEnemy );
	XWndEncounter( XSpotSulfur *pSpot, const XGAME::xBattleStartInfo& info );
	virtual ~XWndEncounter() { Destroy(); }
	//
}; // class XWndEncounter

/****************************************************************
* @brief 계정버프 리스트
* @author xuzhu
* @date	2015/02/17 15:45
*****************************************************************/
class XWndBuffList : public XWnd
{
	void Init() {}
	void Destroy() {}
public:
	XWndBuffList( float x, float y );
	virtual ~XWndBuffList() { Destroy(); }
	//
	void Update() override;
}; // class XWndBuffList

/****************************************************************
* @brief 계정버프 아이콘
* @author xuzhu
* @date	2015/02/17 15:54
*****************************************************************/
class XWndBuffElem : public XWndImage
{
	XGAME::xBuff* m_pBuff = nullptr;
//	_tstring m_resImg;
// 	XTimer2 m_timerBuff;
	void Init() {}
	void Destroy();
public:
	XWndBuffElem( XGAME::xBuff *pBuff );
	virtual ~XWndBuffElem() { Destroy(); }
	//
	void Update() override;
	int OnClick( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickTooltip( XWnd* pWnd, DWORD p1, DWORD p2 );
}; // class XWndBuffElem

/****************************************************************
* @brief 전투통계
* @author xuzhu
* @date	2015/03/17 14:29
*****************************************************************/
class XWndStatistic : public XWndPopup
{
	enum xtStatistic {
		xST_NONE,
		xST_DEAL,
		xST_ATTACKED,
		xST_DEAL_BY_CRITICAL,
		xST_BY_EVADE,
//		xST_HEAL,		// 버프구조상 힐주는쪽을 알수가 없어 일단 뺌.
		xST_TREATED,
		xST_DEAL_BY_SKILL,
		xST_MAX,
	};
	XVector<XSPLegionObj> m_aryLegionObj;
	xtStatistic m_Type = xST_DEAL;
	XWndScrollView *m_pView = nullptr;
	void Init() {}
	void Destroy();
public:
	XWndStatistic( XSPLegionObj spLegionObj1, XSPLegionObj spLegionObj2 );
	~XWndStatistic() { Destroy(); }
	//
	void CreateHerosUI( XWnd *pRoot, XSPLegionObj spLegionObj, float max, xtStatistic type, XGAME::xtSide side );
	void Update() override;
	int OnClickPrev( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickNext( XWnd* pWnd, DWORD p1, DWORD p2 );
}; // class XWndStatistic

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/02/03 13:22
*****************************************************************/
class XWndTrainingCenter;
class XWndSlotByTrain : public XWnd
{
public:
	XWndSlotByTrain( XHero *pHero, XGAME::xtTrain type, ID snSlot, XLayout *pLayout, XWndTrainingCenter *pWndPopup );
	virtual ~XWndSlotByTrain() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	XLayout *m_pLayout = nullptr;
	XWndTrainingCenter *m_pPopup = nullptr;
	XGAME::xtTrain m_Type = XGAME::xTR_NONE;
	XHero *m_pHero = nullptr;
	ID m_snSlot = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update();
// 	XWnd* UpdateRootLayout( bool *pOutbCreated );
	void UpdateSlotBase( XWnd* pWndRoot );
	void UpdateSlotLock( XWnd* pWndRoot );
	void UpdateSlotEmpty( XWnd* pWndRoot );
	void UpdateSlotLvUp( XWnd* pWndRoot );
	void UpdateSlotSquadUp( XWnd* pWndRoot );
	void UpdateSlotSkillUp( XWnd* pWndRoot );
	void OnAutoUpdate() override;
}; // class XWndSlotByTrain

#include "XWndDelegator.h"
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/03/25 17:41
*****************************************************************/
class XWndTrainingCenter : public XWndPopup
{
	void Init() {}
	void Destroy();
public:
	XWndTrainingCenter();
	virtual ~XWndTrainingCenter() { Destroy(); }
	//
	void Update() override;
//	void OnAutoUpdate() override;
	int OnClickUnlockSlot( XWnd* pWnd, DWORD p1, DWORD p2 );
	void UpdateSlot( XHero *pHero, const XAccount::xTrainSlot& slot, ID idWnd );
	int OnClickComplete( XWnd* pWnd, DWORD p1, DWORD p2 );
private:
	BOOL OnCreate() override;
//	void DelegateTrainComplete( XGAME::xtTrain type, XHero* pHero ) override;
	void DelegateUnlockTrainSlot( const std::string& idsEvent );
	void DelegateTrainComplete( const std::string& idsEvent, XGAME::xtTrain type, XHero* pHero );
	void CreateSlots();
}; // class XWndTrainingCenter

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/03/31 13:45
*****************************************************************/
class XWndTrainCompleInWorld : public XWndImage
{
	ID m_snHero = 0;
	void Init() {}
	void Destroy();
public:
	XWndTrainCompleInWorld( XHero *pHero );
	virtual ~XWndTrainCompleInWorld() { Destroy(); }
	//
	void Update() override;
}; // class XWndTrainCompleInWorld
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/03/31 13:48
*****************************************************************/
// class XWndTrainCompleteInWorldMng : public XWnd
// {
// 	XArrayLinearN<ID, 64> m_aryHeroesPrev;
// 	void Init() {}
// 	void Destroy();
// public:
// 	XWndTrainCompleteInWorldMng( const XE::VEC2& vPos );
// 	virtual ~XWndTrainCompleteInWorldMng() { Destroy(); }
// 	//
// 	void Update() override;
// 	int OnClickHero( XWnd* pWnd, DWORD p1, DWORD p2 );
// }; // class XWndTrainCompleteInWorldMng

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/03/31 15:04
*****************************************************************/
// class XWndTrainComplete : public XGameWndAlert
// {
// 	void Init() {}
// 	void Destroy();
// public:
// 	XWndTrainComplete( XHero *pHero, XGAME::xtTrain type );
// 	virtual ~XWndTrainComplete() { Destroy(); }
// 	//
// 	void Update() override;
// }; // class XWndLevelUp

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/04/01 18:22
*****************************************************************/
class XWndLevelup : public XWndPopup
{
	void Init() {}
	void Destroy() {}
public:
	XWndLevelup();
	virtual ~XWndLevelup() { Destroy(); }
	//
// 	void Update() override;
}; // class XWndLevelup

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/04/09 12:11
*****************************************************************/
class XWndUnitinfo : public XWndPopup
{
	XHero *m_pHero = nullptr;
	XGAME::xtUnit m_Unit = XGAME::xUNIT_NONE;
	void Init() {}
	void Destroy() {}
public:
	XWndUnitinfo( XHero *pHero, LPCTSTR szTitle );
	XWndUnitinfo( XGAME::xtUnit unit, LPCTSTR szTitle );
	virtual ~XWndUnitinfo() { Destroy(); }
	//
	void Update() override;
}; // class XWndUnitinfo

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/05/07 16:18
*****************************************************************/
class XWndPaymentByCash : public XWndPopup
{
	void Init() {}
	void Destroy() {}
public:
	XWndPaymentByCash() : XWndPaymentByCash( nullptr, nullptr ) {}
	XWndPaymentByCash( LPCTSTR szMsg, LPCTSTR szTitle = nullptr );
	virtual ~XWndPaymentByCash() { Destroy(); }
	//
	XWnd* GetBuyButton() {
		return Find("butt.buy");
	}
	void Update() override;
	void SetTime( xSec secLack );
	void SetAP( int apLack );
	void SetGold( int goldLack );
	void SetResource( XGAME::xtResource resType, int numLack );
	void SetResource( const std::vector<XGAME::xRES_NUM>& aryLack );
	void SetFillTryByDailySpot();		// 요일스팟 도전횟수 리필
private:
	XGAME::xtPaymentRes m_typePayment = XGAME::xPR_NONE;
	int m_needCash = 0;		// 소모될 캐시개수
	_tstring m_strTitle;	// 없으면 디폴트 메시지가 출력된다.
	_tstring m_strMsg;		// 본문 메시지 없으면 출력하지 않는다.
private:
	int OnOk( XWnd *pWnd, DWORD p1, DWORD p2 ) override;
}; // class XWndBuyByCash

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/23 13:08
*****************************************************************/
class XWndOrderDialog : public XWndView
//						, public std::enable_shared_from_this < XWndOrderDialog >
{
public:
//	XWndOrderDialog( std::shared_ptr<xHelp::XOrderDialog> spOrder, float x, float y );
	XWndOrderDialog( xHelp::OrderPtr spOrder, float x, float y, ID idHero, XHero *pHero );
	XWndOrderDialog( xHelp::OrderPtr spOrder, const XE::VEC2& vPos ) 
		: XWndOrderDialog( spOrder, vPos.x, vPos.y, 0, nullptr ) {}
	XWndOrderDialog( xHelp::OrderPtr spOrder, const XE::VEC2& vPos, ID idHero, XHero *pHero )
		: XWndOrderDialog( spOrder, vPos.x, vPos.y, idHero, pHero ) {}
	virtual ~XWndOrderDialog() { Destroy(); }
	// get/setter
	// public member
	void Update() override;
//	GET_SET_ACCESSOR( const _tstring&, strFaceRes );
	GET_SET_ACCESSOR( const _tstring&, strText );
	GET_SET_ACCESSOR( bool, bTouch );
	GET_SET_ACCESSOR_CONST( XHero*, pHero );
	GET_SET_ACCESSOR_CONST( ID, idHero );
// private member
private:
// private method
	xHelp::OrderPtr m_spOrder;
//	_tstring m_strFaceRes;
	_tstring m_strText;
	bool m_bTouch = false;		// 손가락 touch ui
	XHero *m_pHero = nullptr;
	ID m_idHero = 0;		// m_pHero가 비어있으면 idHero라도 있다.
private:
	void Init() {}
	void Destroy() {}
	int OnClose( XWnd* pWnd, DWORD p1, DWORD p2 );
	int Process( float dt ) override {
		return XWndView::Process( dt );
	}
	void OnNCLButtonUp( float lx, float ly ) override;
}; // class XWndOrderDialog

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/23 20:54
*****************************************************************/
class XWndDialogMode : public XWnd
{
public:
	XWndDialogMode();
	virtual ~XWndDialogMode() { Destroy(); }
	// get/setter
	// public member
	// 걷히게 한다.
	void DoDecovering() {
		m_Mode = 3;
		m_Timer.Set( 1.f );
	}
	// 가려지게 한다.
	void DoCovering() {
		m_Mode = 1;
		m_Timer.Set( 1.f );
	}
// private member
private:
	CTimer m_Timer;
	float m_Height = 32.f;		// 장막 두께
	float m_HeightCurr = 0.f;	// 현재 장막 두께
	int m_Mode = 0;				// 0:완전히 걷힌상태 1:가려지고있는 상태 2:가려진상태 3:걷히고 있는상태
// private method
private:
	void Init() {}
	void Destroy() {}
	int Process( float dt ) override;
	void Draw() override;
}; // class XWndDialogMode

/****************************************************************
* @brief 캐쉬지불 팝업 공통
* @author xuzhu
* @date	2015/07/08 17:10
*****************************************************************/
class XWndPopupCashPay : public XWndPopup
{
public:
	XWndPopupCashPay();
	virtual ~XWndPopupCashPay() { Destroy(); }
	// get/setter
	// public member
	void SetTitle( LPCTSTR szTitle );
	void SetNeedText( LPCTSTR szDesc );
	void SetCost( int cash );
	XWnd* GetButtOk();
// private member
private:
// private method
private:
	void Init() {}
	void Destroy() {}
}; // class XWndPopupCashPay

/****************************************************************
* @brief 영웅 초상화를 그리는 최소한의 모듈
* @author xuzhu
* @date	2015/08/11 20:59
*****************************************************************/
class XWndHeroPortrait : public XWnd
{
public:
	XWndHeroPortrait( const XE::VEC2& vPos, XPropHero::xPROP *pProp );
	virtual ~XWndHeroPortrait() { Destroy(); }
	// get/setter
	// public member
// private member
private:
	XPropHero::xPROP *m_pProp = nullptr;
// private method
private:
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndHeroPortrait

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/10/11 22:00
*****************************************************************/
// class XWndAbilTreeDebug : public XWndPopup
// {
// public:
// 	XWndAbilTreeDebug( XGAME::xtUnit unit );
// 	virtual ~XWndAbilTreeDebug() { Destroy(); }
// 	// get/setter
// 	// public member
// 	void Update() override;
// private:
// 	// private member
// 	XGAME::xtUnit m_Unit = XGAME::xUNIT_NONE;
// private:
// 	// private method
// 	void Init() {}
// 	void Destroy() {}
// }; // class XWndAbilTreeDebug

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/11/07 18:22
*****************************************************************/
class XWndSelectHeroesInReady : public XWndPopup
{
public:
	XWndSelectHeroesInReady( XHero *pHeroFrom );
	virtual ~XWndSelectHeroesInReady() { Destroy(); }
	// get/setter
	GET_ACCESSOR( XHero*, pHeroFrom );
	void SetHeroFrom( XHero* pHero );
	// public member
private:
	// private member
	XHero *m_pHeroFrom = nullptr;
	XHero *m_pHeroSelected = nullptr;
private:
	// private method
	void Init() {}
	void Destroy();
	void Update() override;
	BOOL OnCreate() override;
	int OnClickHero( XWnd* pWnd, DWORD p1, DWORD p2 );
	void CreatePopupSelectUnit( XHero *pHero );
}; // class XWndSelectHeroesInReady

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/11/08 0:03
*****************************************************************/
class XWndSelectUnitsInReady : public XWndPopup
{
public:
public:
	XWndSelectUnitsInReady( XHero *pHero );
	virtual ~XWndSelectUnitsInReady() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	XHero *m_pHero = nullptr;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
	int OnClickUnit( XWnd* pWnd, DWORD p1, DWORD p2 );
}; // class XWndSelectUnitsInReady

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/08 12:13
*****************************************************************/
class XWndLoginFirst : public XWndPopup
{
public:
	XWndLoginFirst( XSceneTitle *pScene );
	virtual ~XWndLoginFirst() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	XSceneTitle *m_pScene = nullptr;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndLoginFirst

/****************************************************************
* @brief 실시간으로 소스측 타겟을 기준으로 좌표가 갱신되는 WndSpr버전
* @author xuzhu
* @date	2016/05/02 12:09
*****************************************************************/
class XWndSprTrace : public XWndSprObj
{
public:
	XWndSprTrace( const _tstring& strSpr, ID idAct, const std::string& idsSrc, float dAngPlace );
	XWndSprTrace( const _tstring& strSpr
							, ID idAct
							, const XE::VEC2& vPos
							, float dAngPlace
							, const XE::VEC2& sizeTarget );
	virtual ~XWndSprTrace() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	std::string m_idsSrc;
	float m_dAngPlace = 0.f;		// idsSrc 로부터 위치할 각도. >=0 이면 해당각도. -1은 center, -2는 none(원점)
	float m_radiusSrc = 0.f;		// src객체의 크기
	XE::VEC2 m_sizeTarget;			// 타겟이 좌표로만 지정되어있을때 타겟의 크기
	XE::VEC2 m_vPosOrig;				// 타겟이 좌표일때 그 좌표의 오리지날 값.
private:
	// private method
	void Init() {}
	void Destroy() {}
	int Process( float dt ) override;
}; // class XWndSprTrace

#include "JBWnd.h"		// 임재범 전용 Wnd구현
#include "JWWnd.h"		// 함정완 전용 Wnd구현

#pragma warning ( default : 4250 )


