/********************************************************************
	@date:	2016/09/30 13:28
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XWndPrivateRaid.h
	@author:	xuzhu
	
	@brief:	개인레이드 군단방어전
*********************************************************************/
#pragma once
#include "_Wnd2/XWndPopup.h"


class XSpotPrivateRaid;
class XWndList;
class XWndStoragyItemElem;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/09/30 13:28
*****************************************************************/
class XWndPrivateRaid : public XWndPopup
{
public:
	XWndPrivateRaid( XSpotPrivateRaid* pSpot );
	virtual ~XWndPrivateRaid() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	XSpotPrivateRaid* m_pSpot = nullptr;
//	XVector<XSPHero> m_arySelectedEnter;
private:
	// private method
	void Init() {}
	void Destroy() {}
	BOOL OnCreate() override;
	void Update() override;
	void UpdateEnterHeroes( XWndList* pList, int idxSide );
	int OnSelectHeroAtHave( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickedEnterHeroLeft( XWnd* pWnd, DWORD p1, DWORD p2 );
	void ClearEnterHeroes( const std::string& strKey );
	XWndStoragyItemElem* FindpHeroCtrl( XWndList* pWndList, ID snHero );
}; // class XWndPrivateRaid
