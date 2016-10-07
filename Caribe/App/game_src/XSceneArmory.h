/********************************************************************
	@date:	2015/06/26 16:28
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XSceneArmory.h
	@author:	xuzhu
	
	@brief:	무기상
*********************************************************************/
#pragma once

#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XPropItem.h"

class XGame;
class XAppLayout;
class XWndStoragyItemElem;

// 무기상
class XSceneArmory : public XSceneBase
{
	XLayoutObj m_Layout;
	XLayoutObj m_LayoutTooltip;
	XWndStoragyItemElem *m_pSelectItem = nullptr;		// 선택된 아이템
	ID m_idSelected = 0;

private:
	void Init()  {
	}
	void Destroy();

protected:
public:
	XSceneArmory(XGame *pGame);
	virtual ~XSceneArmory(void) { Destroy(); }

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

//	int OnClickPopupCancel(XWnd *pWnd, DWORD p1, DWORD p2);					// 팝업 취소

	// 업데이트 함수
//	void RecvUpdate();				// 상단 골드 & 캐쉬 업데이트
	void NewListRecvUpdate(bool bSuccess);

	void UpdateBuyItemInfo();		// 아이템 클릭에 따른 정보 변경
	void UpdateCharacter();			// 현재 무기상이 있는지 확인

	void AlignStar(XE::VEC2 alignPos, XWnd star1, XWnd star2, XWnd star3, XWnd star4);		// 별 정렬
	void UpdateToolTipBySoul( XPropItem::xPROP *pProp );
	int OnEnterScene( XWnd*, DWORD, DWORD ) override;
};

extern XSceneArmory *SCENE_ARMORY;
namespace XGAME {
	void UpdateItemTooltip( const XPropItem::xPROP *pProp, XWnd *pRoot, int cost );
// 	void UpdateEquipItemTooltip( XPropItem::xPROP *pProp, XWnd *pRoot, int cost );
// 	void UpdateEtcItemTooltip( XPropItem::xPROP *pProp, XWnd *pRoot, int cost );
};
