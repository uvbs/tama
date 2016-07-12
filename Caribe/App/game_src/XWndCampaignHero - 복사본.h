#pragma once
#include "XPropCamp.h"
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWndList.h"
#include "XSpotEtc.h"

#ifndef _CLIENT
#error "only allow client build"
#endif // not CLIENT


/****************************************************************
* @brief 영웅의 전장 새 버전
* @author xuzhu
* @date	
*****************************************************************/
class XWndCampaignHero2 : public XWndPopup
{
	enum xtStageState {
		xSS_NONE = 0,
		xSS_ENTERABLE,	// 스테이지에 진입가능한 상태
		xSS_CLEARED,	// 클리어함. 진입불가
		xSS_LOCK,		// 잠긴상태
		xSS_BOSS,		// 보스상태
		xSS_RETRY,		// 재도전할수 있는곳.
		xSS_NEW_OPEN,	// 새로오픈된곳.
	};
public:
	XWndCampaignHero2( XSpotCommon *pSpot, xCampaign::CampObjPtr& spCampObj );
	virtual ~XWndCampaignHero2() { Destroy(); }
	// get/setter
	GET_ACCESSOR( XPropCamp::xProp*, pProp );
private:
	// public member
	XSpotCommon *m_pSpot = nullptr;
	XPropCamp::xProp *m_pProp = nullptr;
	int m_idxLastUnlock = 0;      // 잠금해제된 제일 마지막 스테이지의 인덱스.
	int m_idxLastPlay = 0;        // 마지막으로 플레이했던 스테이지의 인덱스.
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
}; // class XWndCampaignHero2

