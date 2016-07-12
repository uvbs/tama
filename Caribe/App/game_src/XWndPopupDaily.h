/********************************************************************
	@date:	2016/07/06 15:55
	@file: 	C:\xuzhu_work\Project\iPhone_day\Caribe\App\game_src\XWndPopupDaily.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

#include "_Wnd2/XWndPopup.h"

class XSpotDaily;
namespace XGAME {
	struct xReward;
}

class XWndPopupDaily : public XWndPopup
{
	XSpotDaily *m_pSpot = nullptr;
	int m_dowToday = 0;
	CTimer m_timerAlpha;
	void Init() {}
	void Destroy() {}
public:
	XWndPopupDaily( XSpotDaily *pSpot );
	~XWndPopupDaily() {		Destroy();	}
	//
	static bool s_bTestMode;
private:
	void Update() override;
	void OnAutoUpdate() override;
	int OnClickDow( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickEnter( XWnd* pWnd, DWORD p1, DWORD p2 );
	int Process( float dt ) override;
	int OnClickCheat( XWnd* pWnd, DWORD p1, DWORD p2 );
	void CreateRewardCtrl( XWnd* pWndElem, const XE::xtDOW dow );
	void CreateRewardCtrlByItem( XWnd* pWndElem, const XVector<XGAME::xReward>& aryReward, const XE::xtDOW dow );
	void UpdateElem( XWnd* pWndElem, XE::xtDOW dow, int idx );
}; // class XWndPopupDaily
