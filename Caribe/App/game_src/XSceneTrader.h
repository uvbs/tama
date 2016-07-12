/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"

class XGame;
class XWndSliderOption;

// 무역상 씬
class XSceneTrader : public XSceneBase
{
private:
	void Init() { }
	void Destroy();

protected:
	XLayoutObj m_Layout;

	XWndSliderOption *m_slider[XGAME::xRES_MAX];		// 슬라이더

	int m_resourceGold[XGAME::xRES_MAX];		// 각 자원별 골드량
	int m_totalGold;							// 총 골드량

public:
	XSceneTrader(XGame *pGame);
	virtual ~XSceneTrader(void) { Destroy(); }

	// virtual
	// 안건드려도 될듯
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );

	// 버튼 클릭
	int OnBack(XWnd *pWnd, DWORD p1, DWORD p2);					// 백버튼

	int OnClickTrade(XWnd *pWnd, DWORD p1, DWORD p2);			// 거래 버튼
	int OnClickTradeCancel(XWnd *pWnd, DWORD p1, DWORD p2);		// 취소 버튼

	int OnClickTradeOK(XWnd *pWnd, DWORD p1, DWORD p2);			// 거래 팝업 확인버튼

	int OnClickResource(XWnd *pWnd, DWORD p1, DWORD p2);		// 자원 Min, Max 버튼

	int OnClickRecall(XWnd *pWnd, DWORD p1, DWORD p2);			// 즉시호출 버튼

	int OnClickRecallOK(XWnd *pWnd, DWORD p1, DWORD p2);		// 즉시호출 수락
	//int OnClickRecallCancel(XWnd *pWnd, DWORD p1, DWORD p2);	// 즉시호출 취소

	int OnClickPopupCancel(XWnd *pWnd, DWORD p1, DWORD p2);		// 팝업 취소

	// 슬라이더
	int OnSlidingResource0(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnSlidingResource1(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnSlidingResource2(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnSlidingResource3(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnSlidingResource4(XWnd *pWnd, DWORD p1, DWORD p2);

	void UILoad();		// 초기 UI 로드

	// 업데이트
	void UpdateSlidingResource(const char *key, XGAME::xtResource resource);	// 슬라이드 하나 업데이트
	void UpdateTotalGold();														// 총 금액 업데이트
	void UpdateCharacter();			// 현재 무역상이 있는지 확인
	void UpdateRecallTimeCheck();
	int OnEnterScene( XWnd* pWnd, DWORD dw1, DWORD dw2 ) override;
	void Update() override;
};

extern XSceneTrader *SCENE_TRADER;