#pragma once
//#include "XWindow.h"
#include "etc/Timer.h"
#include "../XDrawGraph.h"
#include "XFramework/client/XTimeoutMng.h"
#include "XFramework/Game/XFLevel.h"
//#include "XWorld.h"
#include "XPropUnit.h"
#include "XPropTech.h"
#include "XBaseItem.h"
#include "XAccount.h"
#include "XFramework/client/XEContent.h"
#include "XPropCamp.h"
#include "XPropHero.h"
#include "XPropBgObjH.h"
#include "XStruct.h"
#include "xscenebase.h"
#include "_Wnd2/XWndView.h"
#include "_Wnd2/XWndCtrls.h"
#include "_Wnd2/XWndButton.h"
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
class XUnitHero;
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
		return m_listLoot.GetLast();
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


/****************************************************************
* @brief 스킬사용버튼.
* @author xuzhu
* @date	2014/10/28 9:29
*****************************************************************/
class XWndSkillButton : public XWndButton
{
	CTimer m_timerCool;
	ID m_snHero = 0;
	void Init() {}
	void Destroy();
public:
	XWndSkillButton( float x, float y, XSKILL::XSkillDat *pSkillDat, XUnitHero *pUnitHero );
	virtual ~XWndSkillButton() { Destroy(); }
	///< 
	GET_ACCESSOR_CONST( ID, snHero );
	void SetCoolTimer( float sec );
	bool IsCoolTime() {
		return m_timerCool.IsOn() && !m_timerCool.IsOver();
	}
	int Process( float dt ) override;
	void Draw( void ) override;

}; // class XWndSkillButton

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
* @brief 캠페인 UI기본형
* @author xuzhu
* @date	2015/01/13 12:52
*****************************************************************/
class XWndCampaignBase : public XWndPopup, public _XWndListDelegate
{
protected:
	enum xtStageState {
		xSS_NONE = 0,
		xSS_ENTERABLE,	// 스테이지에 진입가능한 상태
		xSS_CLEARED,	// 클리어함. 진입불가
		xSS_LOCK,		// 잠긴상태
		xSS_BOSS,		// 보스상태
		xSS_RETRY,		// 재도전할수 있는곳.
		xSS_NEW_OPEN,	// 새로오픈된곳.
	};
	XSpot *m_pBaseSpot = nullptr;
	XPropCamp::xProp *m_pProp = nullptr;
//	xCampaign::CampObjPtr m_spCampObj;
private:
	int m_idxLastUnlock = 0;      // 잠금해제된 제일 마지막 스테이지의 인덱스.
	int m_idxLastPlay = 0;        // 마지막으로 플레이했던 스테이지의 인덱스.
//	int m_idxLoading = 0;				// 점진적 로딩시 현재 로딩해야할 스테이지인덱스.
	bool m_bProgressLoading = false;	// 점진적 로딩
	void Init() {}
	void Destroy() {}
public:
//	XWndCampaignBase( XSpot *pBaseSpot, XPropCamp::xProp *pProp );
	XWndCampaignBase( XSpot *pBaseSpot, xCampaign::CampObjPtr& spCampObj );
	virtual ~XWndCampaignBase() { Destroy(); }
	//
	GET_ACCESSOR( XPropCamp::xProp*, pProp );
	GET_ACCESSOR( XSpot*, pBaseSpot );
//	GET_ACCESSOR( xCampaign::CampObjPtr&, spCampObj );
	GET_SET_ACCESSOR( int, idxLastUnlock );
	GET_SET_ACCESSOR( int, idxLastPlay );
	GET_SET_ACCESSOR( bool, bProgressLoading );
	xCampaign::CampObjPtr GetspCampObj();
	//
	int OnOk( XWnd* pWnd, DWORD p1, DWORD p2 );
	void Update() override;
	int Process( float dt ) override;
	
	virtual XWnd* UpdateListElem( XWnd *pElem, xCampaign::StageObjPtr spStageObj ) = 0;
// 	virtual XWnd* UpdateListElem( std::shared_ptr<XPropCamp::xStage> spStage ) = 0;
	virtual void SetEventByList( XWndList *pList ) {}
	void OnAutoUpdate() override;
	int OnClickStage( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnRecvClickStage( XGAME::xtError errCode, int idxStage );
protected:
	virtual xtStageState GetStageState( xCampaign::StageObjPtr spStage ) { return xSS_NONE; }
	virtual bool IsShowStar() { return true; }
private:
	void DelegateStopScroll( XWndList* pList, const XE::VEC2& vCurrLT, const XE::VEC2& sizeScrollWindow ) override;
	virtual void UpdateElemByStopScroll( XWnd *pElem, xCampaign::StageObjPtr spStageObj ) {}
}; // class XWndCampaignBase

/****************************************************************
* @brief 길드레이드 팝업
* @author xuzhu
* @date	2015/05/08 16:43
*****************************************************************/
class XWndGuildRaid : public XWndCampaignBase
{
	void Init() {}
	void Destroy() {}
public:
	XWndGuildRaid( XSpotCommon *pSpot, xCampaign::CampObjPtr& spCampObj );
	virtual ~XWndGuildRaid() { Destroy(); }
	//
	void Update() override;
	
	XWnd* UpdateListElem( XWnd *pElem, xCampaign::StageObjPtr spStageObj ) override;
// 	XWnd* UpdateListElem( std::shared_ptr<XPropCamp::xStage> spPropStage ) override;
	void SetEventByList( XWndList *pList ) override;
	int OnClickStage( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickStart( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnAutoUpdate() override;
	template<typename T>
	void ArrangeDebugButton( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szLabel, const char *cIdentifier, T func );
// 	template<typename T>
// 	void ArrangeDebugButton( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szLabel, const char *cIdentifier, T func ) {
// 		auto pButt = Find( cIdentifier );
// 		if( pButt == nullptr ) {
// 			pButt = new XWndButtonDebug( vPos, vSize, szLabel );
// 			pButt->SetstrIdentifier( cIdentifier );
// 			pButt->SetEvent( XWM_CLICKED, this, func );
// 			Add( pButt );
// 		}
// 	}
	int OnClickClear( XWnd* pWnd, DWORD p1, DWORD p2 );
private:
	xCampaign::CampObjPtr m_spCampObj;
}; // class XWndGuildRaid

/****************************************************************
* @brief 범용 캠페인 팝업
* @author xuzhu
* @date	2015/05/28 16:06
*****************************************************************/
class XWndPopupCampaign : public XWndCampaignBase
{
	void Init() {}
	void Destroy() {}
public:
	XWndPopupCampaign( XSpot *pBaseSpot, xCampaign::CampObjPtr spCampObj );
	virtual ~XWndPopupCampaign() { Destroy(); }
	//
	void Update() override;
	XWnd* UpdateListElem( XWnd *pElem, xCampaign::StageObjPtr spStageObj ) override;
//	XWnd* UpdateListElem( std::shared_ptr<XPropCamp::xStage> spPropStage ) override;
	void SetEventByList( XWndList *pList ) override;
	int OnClickStage( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickReward( XWnd* pWnd, DWORD p1, DWORD p2 );
	virtual LPCTSTR GetidsDropItem( std::shared_ptr<XPropCamp::xStage> spPropStage );
protected:
	virtual XWndCampaignBase::xtStageState 
		GetStageState( xCampaign::StageObjPtr spStage ) override;
}; // class XWndPopupCampaign

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/05/29 14:56
*****************************************************************/
class XWndPopupCampaignMedal : public XWndPopupCampaign
{
	void Init() {}
	void Destroy() {}
public:
	XWndPopupCampaignMedal( XSpot *pBaseSpot, xCampaign::CampObjPtr& spCampObj );
	virtual ~XWndPopupCampaignMedal() { Destroy(); }
	//
	LPCTSTR GetidsDropItem( std::shared_ptr<XPropCamp::xStage> spPropStage ) override;
protected:
	XWndCampaignBase::xtStageState 
		GetStageState( xCampaign::StageObjPtr spStage ) override;
	bool IsShowStar() override { return false; }
	XWnd* UpdateListElem( XWnd *pElem, xCampaign::StageObjPtr spStageObj ) override;
// 	XWnd* UpdateListElem( std::shared_ptr<XPropCamp::xStage> spPropStage ) override;
}; // class XWndPopupCampaignMedal

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
* @brief 
* @author xuzhu
* @date	2015/03/05 17:37
*****************************************************************/
class XWndSpotMsg : public XWnd
{
protected:
	_tstring m_strText;
	int m_State = 0;
	float m_Size = 20.f;
	XCOLOR m_Color = XCOLOR_WHITE;
	void Init() {}
	void Destroy();
	CTimer m_timerLife;
public:
	XWndSpotMsg( ID idSpot, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
	XWndSpotMsg( const XE::VEC2& vPos, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
	virtual ~XWndSpotMsg() { Destroy(); }
	//
	int Process( float dt ) override;
}; // class XWndSpotMsg

/****************************************************************
* @brief 자원지에서 주기적으로 뜨는 생산량 메시지
* @author xuzhu
* @date	2015/03/18 13:49
*****************************************************************/
class XWndProduceMsg : public XWndSpotMsg
{
	void Init() {}
	void Destroy() {}
public:
	XWndProduceMsg( ID idSpot, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
	XWndProduceMsg( const XE::VEC2& vPos, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
	virtual ~XWndProduceMsg() { Destroy(); }
	//
	int Process( float dt ) override;
}; // class XWndProduceMsg
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
	XArrayLinearN<XLegionObj*, 2> m_aryLegionObj;
	xtStatistic m_Type = xST_DEAL;
	XWndScrollView *m_pView = nullptr;
	void Init() {}
	void Destroy();
public:
	XWndStatistic( XLegionObj *pLegionObj1, XLegionObj *pLegionObj2 );
	virtual ~XWndStatistic() { Destroy(); }
	//
	void CreateHerosUI( XWnd *pRoot, XLegionObj *pLegionObj, float max, xtStatistic type, XGAME::xtSide side );
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
* @date	2015/04/16 14:16
*****************************************************************/
// class XWndSquadLevelup : public XWndPopup
// {
// 	XHero *m_pHero = nullptr;
// 	bool m_bNotEnoughRes = false;
// 	const XGAME::xtTrain m_Type = XGAME::xTR_SQUAD_UP;
// 	void Init() {}
// 	void Destroy() {}
// public:
// 	XWndSquadLevelup( XHero *pHero );
// 	virtual ~XWndSquadLevelup() { Destroy(); }
// 	//
// 	void Update() override;
// 	int OnClickProvideMedal( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickTrain( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickComplete( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	void OnAutoUpdate() override;
// }; // class XWndSquadLevelup

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/04/16 14:16
*****************************************************************/
// class XWndSkillLevelup : public XWndPopup
// {
// 	XHero *m_pHero = nullptr;
// 	bool m_bNotEnoughRes = false;
// 	XGAME::xtTrain m_Type = XGAME::xTR_NONE;
// 	void Init() {}
// 	void Destroy() {}
// public:
// 	XWndSkillLevelup( XHero *pHero, XGAME::xtTrain type );
// 	virtual ~XWndSkillLevelup() { Destroy(); }
// 	//
// 	void Update() override;
// 	int OnClickProvideScroll( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickTrain( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickComplete( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	void OnAutoUpdate() override;
// }; // class XWndSkillLevelup

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
	XWndPaymentByCash( XGAME::xtResource resType, int goldLack, int cash, LPCTSTR szMsg );
	virtual ~XWndPaymentByCash() { Destroy(); }
	//
  XWnd* GetBuyButton() {
    return Find("butt.buy");
  }
}; // class XWndBuyByCash

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/05/19 18:03
*****************************************************************/
// class XWndPopupDailyCamp : public XWndPopup
// {
// 	XSpotDaily *m_pSpot = nullptr;
// 	int m_dowSelected = 0;
// 	void Init() {}
// 	void Destroy() {}
// public:
// 	XWndPopupDailyCamp( XSpotDaily *pSpot );
// 	virtual ~XWndPopupDailyCamp() { Destroy(); }
// 	//
// 	void Update() override;
// 	void OnAutoUpdate() override;
// 	int OnClickDow( XWnd* pWnd, DWORD p1, DWORD p2 );
// }; // class XWndPopupDailyCamp

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
* @brief 영웅의 전장
* @author xuzhu
* @date	2015/09/18 12:00
*****************************************************************/
class XWndCampaignHero : public XWndCampaignBase
{
public:
	XWndCampaignHero( XSpotCommon *pSpot, xCampaign::CampObjPtr& spCampObj );
	virtual ~XWndCampaignHero() { Destroy(); }
	// get/setter
private:
	// public member
	XSpotCommon *m_pSpot = nullptr;
private:
	// private method
	void Init() {}
	void Destroy() {}
	XWndCampaignBase::xtStageState
		GetStageState( xCampaign::StageObjPtr spStage ) override;
	bool IsShowStar() override { return false; }
	XWnd* UpdateListElem( XWnd *pElem, xCampaign::StageObjPtr spStageObj ) override;
	void ShowStar( XWnd *pRoot, bool bShow, int numStar );
	void ShowRetryMark( XWnd *pRoot, bool bShow, int num );
	void SetHeroFace( XWnd *pElem, xCampaign::StageObjHeroPtr spStageObj );
	void UpdateElemByStopScroll( XWnd *pElem, xCampaign::StageObjPtr spStageObj ) override;
}; // class XWndCampaignHero

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/10/11 22:00
*****************************************************************/
class XWndAbilTreeDebug : public XWndPopup
{
public:
	XWndAbilTreeDebug( XGAME::xtUnit unit );
	virtual ~XWndAbilTreeDebug() { Destroy(); }
	// get/setter
	// public member
	void Update() override;
private:
	// private member
	XGAME::xtUnit m_Unit = XGAME::xUNIT_NONE;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XWndAbilTreeDebug

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
* @brief 
* @author xuzhu
* @date	2016/01/20 17:25
*****************************************************************/
class XWndPlayerInfo : public XWndPopup
{
public:
	XWndPlayerInfo();
	virtual ~XWndPlayerInfo() { Destroy(); }
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
}; // class XWndPlayerInfo

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/25 17:03
*****************************************************************/
class XWndBgObj : public XWndSprObj
{
public:
//	XWndBgObj( const _tstring& strSpr, ID idAct, const XE::VEC2& vwPos );
	XWndBgObj( const xnBgObj::xProp& prop );
	virtual ~XWndBgObj() { Destroy(); }
	// get/setter
	GET_ACCESSOR_CONST( const xnBgObj::xProp&, Prop );
	// public member
	void SetTimer( float sec ) {
		if( m_timerOut.IsOff() )
			m_timerOut.Set( sec );
	}
	// 화면밖을 벗어난지 
	bool IsOverTime() const {
		return m_timerOut.IsOver() != FALSE;
	}
	void OffTimer() {
		m_timerOut.Off();
	}
	void UpdateProp( const xnBgObj::xProp* pProp ) {
		m_Prop = *pProp;
		SetbUpdate( true );
	}
private:
	// private member
	xnBgObj::xProp m_Prop;
	CTimer m_timerOut;			// 화면을 벗어남 타이머
	ID m_idStream = 0;			// 사운드 스트림(아이디는 0부터 나옴. -1이 실패한 아이디)
private:
	// private method
	void Init() {}
	void Destroy();
	void ProcessMsg( const std::string& strMsg ) override;
	int Process( float dt ) override;
	void Update() override;
}; // class XWndBgObj

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/25 18:01
*****************************************************************/
// class XWndBgObjSound : public XWnd
// {
// public:
// 	XWndBgObjSound() { Init(); }
// 	virtual ~XWndBgObjSound() { Destroy(); }
// 	// get/setter
// 	// public member
// private:
// 	// private member
// 	int m_Test = 0;
// private:
// 	// private method
// 	void Init() {}
// 	void Destroy() {}
// }; // class XWndBgObjSound
// ////////////////////////////////////////////////////////////////
// XWndBgObjSound::XWndBgObjSound( )
// {
// 	Init();
// }
// 
// void XWndBgObjSound::Destroy()
// {
// }

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/25 11:01
*****************************************************************/
class XWndBgObjLayer : public XWnd
{
public:
	XWndBgObjLayer();
	virtual ~XWndBgObjLayer() { Destroy(); }
	// get/setter
	// public member
#ifdef _xIN_TOOL
	ID GetidSelected() {
		return ( m_pPropSelected ) ? m_pPropSelected->m_idObj : 0;
	}
	void OnClickWorld() {
		m_pSelected = nullptr;
		m_pPropSelected = nullptr;
	}
	bool IsObjDragging() {
		return m_bDragging;
	}
	void UpdateSelected() {
		if( m_pSelected ) {
			m_pSelected->UpdateProp( m_pPropSelected );
			m_pSelected->SetbUpdate( true );
		}
	}
#endif // _xIN_TOOL
	void UpdateCurrFocus( const XE::VEC2& vFocus, const XE::VEC2& sizeView );
private:
	// private member
//	XVector<xnBgObj::xProp*> m_aryAllProps;
#ifdef _xIN_TOOL
	XWndBgObj* m_pSelected = nullptr;
	xnBgObj::xProp* m_pPropSelected = nullptr;
	bool m_bRButtDown = false;
	bool m_bCopyed = false;
	bool m_bMoved = false;
	bool m_bDragging = false;
	XE::VEC2 m_vTouchPrev;
	xnBgObj::xProp m_PropPrev;
#endif // _xIN_TOOL
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
	bool UpdateSprObj( const xnBgObj::xProp* pProp, bool bIsIn );
	bool UpdateSoundObj( const xnBgObj::xProp* pProp, bool bIsIn );
#ifdef _xIN_TOOL
	void Draw() override;
	void OnRButtonDown( float lx, float ly ) override;
	void OnRButtonUp( float lx, float ly ) override;
	void OnMouseMove( float lx, float ly ) override;
	int OnClickBgObj( XWnd* pWnd, DWORD p1, DWORD p2 );
	BOOL OnKeyDown( int keyCode ) override;
	XWndBgObj* CreateWndBgObj( xnBgObj::xProp *pProp );
#endif // _xIN_TOOL
}; // class XWndBgObjLayer

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


