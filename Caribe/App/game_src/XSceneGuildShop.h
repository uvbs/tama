/********************************************************************
	@date:	2015/07/07 19:12
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XSceneGuildShop.h
	@author:	xuzhu
	
	@brief:	연맹상점
*********************************************************************/
#pragma once

#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"

class XGame;
class XAppLayout;
class XWndStoragyItemElem;

// 연맹상점
class XSceneGuildShop : public XSceneBase
{
	XLayoutObj m_Layout;
	XLayoutObj m_LayoutTooltip;
//	XWndStoragyItemElem *m_pSelectItem = nullptr;		// 선택된 아이템
//	ID m_idSelected = 0;
	int m_idxSlot = 0;

private:
	void Init()  {
	}
	void Destroy();

protected:
public:
	XSceneGuildShop(XGame *pGame);
	virtual ~XSceneGuildShop(void) { Destroy(); }

	// virtual
	void Create( void ) override;
	void Update() override;
	int Process( float dt ) override;
	void Draw( void ) override;
	void OnLButtonDown( float lx, float ly ) override;
	void OnLButtonUp( float lx, float ly ) override;
	void OnMouseMove( float lx, float ly ) override;

	// 버튼 함수
	int OnBack(XWnd *pWnd, DWORD p1, DWORD p2);								// 월드 씬으로

	int OnClickSlot(XWnd *pWnd, DWORD p1, DWORD p2);						// 아이템 클릭

	int OnClickNewItemChange(XWnd *pWnd, DWORD p1, DWORD p2);				// 새로배치 버튼
	int OnClickNewItemChangeOK(XWnd *pWnd, DWORD p1, DWORD p2);				// 새로배치 확인 버튼

	int OnClickNewItemChangeShortage(XWnd *pWnd, DWORD p1, DWORD p2);		// 새로배치 부족 버튼

	int OnClickBuy(XWnd *pWnd, DWORD p1, DWORD p2);							// 구입 버튼
	int OnClickBuyOK(XWnd *pWnd, DWORD p1, DWORD p2);						// 구입 확인 버튼

	int OnClickPopupCancel(XWnd *pWnd, DWORD p1, DWORD p2);					// 팝업 취소

	// 업데이트 함수
//	void RecvUpdate();				// 상단 골드 & 캐쉬 업데이트
	void NewListRecvUpdate(bool bSuccess);

	void UpdateBuyItemInfo();		// 아이템 클릭에 따른 정보 변경
	void UpdateCharacter();			// 현재 연맹상점이 있는지 확인

	void AlignStar(XE::VEC2 alignPos, XWnd star1, XWnd star2, XWnd star3, XWnd star4);		// 별 정렬
	void UpdateToolTip();
	void ShowRootItemTooltip( bool bFlag );
	void ShowRootHeroTooltip( bool bFlag );
	int OnClickGirl( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickCashPayForGuildPoint( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnEnterScene( XWnd* pWnd, DWORD dw1, DWORD dw2 ) override;
};

extern XSceneGuildShop *SCENE_GUILD_SHOP;
namespace XGAME {
//  	void UpdateHeroTooltip( const _tstring& idsHero, XWnd *pRoot, int cost );
// 	void UpdateHeroTooltip( ID idHero, XWnd *pRoot, int cost );
};
