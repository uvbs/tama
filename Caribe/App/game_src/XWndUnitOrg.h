/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWndProgressBar.h"
#include "_Wnd2/XWndSprObj.h"
#include "Sprite/SprObj.h"
/**
 @brief 영웅 업글창
*/
class XWndLevelupHero : public XWndPopup
											, public XEDelegateProgressBar2
											, public XEDelegateSprObj
{
//	const int c_minGold = 1000;
public:
	XWndLevelupHero( XHero *pHero, XGAME::xtTrain typeTrain );
	virtual ~XWndLevelupHero() { Destroy(); }
	// get/setter
	// public member
	void UpdateTrain( ID snHero, XGAME::xtTrain typeTrain, ID snSlot );
// private member
private:
// private method
	XHero *m_pHero = nullptr;
	int m_GoldUse = 0;
	int m_ExpByGold = 0;	// m_Gold에 의해 받게될 경험치총량
	int m_Level = 0;	// m_Gold에 의해서 exp를 받았을때 업그레이드될 레벨
	int m_ExpRest = 0;		// m_Gold에 의해서 렙업하고 나머지 exp
	int m_secTrainByGold = 0;	// m_Gold에 의한 훈련시간
	XGAME::xtTrain m_typeTrain = XGAME::xTR_NONE;
	int m_lvBar = 0;				// 바에 표시되는 현재 금화에 의한 레벨
private:
	void Init() {}
	void Destroy();
	int OnClickMinus( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickPlus( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickMax( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickTrain( XWnd* pWnd, DWORD p1, DWORD p2 );
	void Update() override;
	void OnAutoUpdate() override;
	void UpdateGold();
	int OnClickCompleteNow( XWnd* pWnd, DWORD p1, DWORD p2 );
	int GetGoldMinForTrain( XGAME::xtTrain typeTrain, int lvHero ) const;
	void OnDelegateChangeLevel( XWndProgressBar2 *pBar, int lvCurrByAni ) override;
	bool DelegateSprObjFinishAni( XWndSprObj *pWndSpr ) override;
	void UpdateLevel( XWndProgressBar2 *pBar, int lvCurrByAni );
}; // class XWndLevelupHero
