#pragma once
#include "XPropCamp.h"
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWndListH.h"
#include "XSpotEtc.h"
#include "XWndCampaignH.h"

#ifndef _CLIENT
#error "only allow client build"
#endif // not CLIENT

class XWndList;
/****************************************************************
* @brief 영웅의 전장 새 버전
* @author xuzhu
* @date	
*****************************************************************/
class XWndCampaignHero2 : public XWndPopup
//												, public _XWndListDelegate
{
public:
	XWndCampaignHero2( XSpotCommon *pSpot, XSPCampObj spCampObj );
	virtual ~XWndCampaignHero2() { Destroy(); }
	// get/setter
	GET_ACCESSOR( const XPropCamp::xProp*, pProp );
	GET_ACCESSOR( XSpotCommon*, pSpot );
	GET_SET_ACCESSOR( int, idxLastUnlock );
	GET_SET_ACCESSOR( int, idxLastPlay );
	GET_SET_ACCESSOR( bool, bProgressLoading );
	XSPCampObjHero2 GetspCampObj();
private:
	static int s_idxFloorSelected;
	static XE::VEC2 s_vPosScroll;
	// public member
	XSpotCommon *m_pSpot = nullptr;
	XPropCamp::xPropHero *m_pProp = nullptr;
	int m_idxLastUnlock = 0;      // 잠금해제된 제일 마지막 스테이지의 인덱스.
	int m_idxLastPlay = 0;        // 마지막으로 플레이했던 스테이지의 인덱스.
	bool m_bProgressLoading = false;	// 점진적 로딩
	int m_idxProgressLoading = 0;
	int m_idFilter = 0;
	XVector<XSPStageObjHero2> m_aryFiltered;
private:
	// private method
	void Init() {}
	void Destroy() {}
	xCampaign::xtStageState
		GetStageState( XSPStageObj spStage, int idxFloor );
//	bool IsShowStar() override { return false; }
	XWnd* UpdateListElem( XWnd *pElem, XSPStageObj spStageObj );
//	void ShowStar( XWnd *pRoot, bool bShow, int numStar );
	void ShowRetryMark( XWnd *pRoot, bool bShow, int num );
	void SetHeroFace( XWnd *pElem, XSPStageObjHero2 spStageObj );
//	void UpdateElemByStopScroll( XWnd *pElem, XSPStageObj spStageObj );
	void Update() override;
	void OnAutoUpdate() override;
	int OnClickStage( XWnd* pWnd, DWORD p1, DWORD p2 );
//	void DelegateStopScroll( XWndList* pList, const XE::VEC2& vCurrLT, const XE::VEC2& sizeScrollWindow ) override;
//	void OnRecvClickStage( XGAME::xtError errCode, int idxStage ) const;
	int Process( float dt ) override;
	void ProgressLoading();
	void UpdateFloorButts( XSPCampObjHero2 spCampObj );
	BOOL OnCreate() override;
	int OnClickFloor( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnDestroy() override;
	void UpdateFiltered();
}; // class XWndCampaignHero2

