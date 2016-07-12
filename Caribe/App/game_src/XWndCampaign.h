#pragma once
#include "XPropCamp.h"
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWndListH.h"
#include "XSpotEtc.h"

#ifndef _CLIENT
#error "only allow client build"
#endif // not CLIENT

#define STATIC_DESTROY_WND(IDS)		static void sDestroyWnd() { XE::GetGame()->DestroyWndByIdentifier(IDS); }

class XWndList;


XE_NAMESPACE_START( xCampaign )
bool sDoAlertCampaignByError( XGAME::xtError errCode );
void sDoPopupSpotWithStageObj( XSpot* pBaseSpot, XSPCampObj spCampObj, XSPStageObj spStageObj, int idxFloor );

XE_NAMESPACE_END;

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
//	void OnRecvClickStage( XGAME::xtError errCode, int idxStage );
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

