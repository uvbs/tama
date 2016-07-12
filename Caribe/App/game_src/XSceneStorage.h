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
class XWndStoragyItemElem;
class XBaseItem;

// 창고 씬
class XSceneStorage : public XSceneBase
{
public:
//	static void sUpdateItemOption( XPropItem::xPROP *pProp, XWnd *pRoot, const char *cToken, float addY );
private:
	enum eTab { ALL, EQUIP, TROPHY, ETC };
private:
	void Init() { }
	void Destroy();

	void Update();

protected:
	XLayoutObj m_Layout;
	XLayoutObj m_LayoutTooltip;
	XWndStoragyItemElem *m_pSelectItem = nullptr;		// 선택된 리스트 아이템

	eTab m_tab;
public:
	XSceneStorage(XGame *pGame);
	virtual ~XSceneStorage(void) { Destroy(); }
	
	// virtual
	// 안건드려도 될듯
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );

	// 버튼 클릭
	int OnBack(XWnd *pWnd, DWORD p1, DWORD p2);

	// 전체, 장비, 전리품, 기타 버튼
	int OnClickTabAll(XWnd *pWnd, DWORD p1, DWORD p2);				
	int OnClickTabEquipment(XWnd *pWnd, DWORD p1, DWORD p2);		
	int OnClickTabTrophy(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickTabEtc(XWnd *pWnd, DWORD p1, DWORD p2);

	int OnClickStorageListSlot(XWnd *pWnd, DWORD p1, DWORD p2);		// 리스트 아이템 클릭
	
	int OnClickEquipmentSell(XWnd *pWnd, DWORD p1, DWORD p2);		// 판매
	int OnClickSell(XWnd *pWnd, DWORD p1, DWORD p2);				// 판매 확정 버튼

	int OnClickEtcUse(XWnd *pWnd, DWORD p1, DWORD p2);				// 사용
	int OnClickUse(XWnd *pWnd, DWORD p1, DWORD p2);					// 사용 확정 버튼
	
	int OnClickLock(XWnd *pWnd, DWORD p1, DWORD p2);				// 인벤토리 잠금 버튼
	int OnClickLockOK(XWnd *pWnd, DWORD p1, DWORD p2);				// 인벤토리 잠금 버튼 수락

	int OnClickPopupCancel(XWnd *pWnd, DWORD p1, DWORD p2);			// 팝업 취소

	// ui
	void UpdateAll();		// 전부
	void UpdateEquipment();	// 장비
	void UpdateTrophy();	// 전리품
	void UpdateEtc();		// 기타

	void RecvUpdate();

	void UpdateSelItemInfo();

	// 헬퍼 함수
	int FilterList(eTab tabType);
	void UpdateEmptySlot( XWndList *pWndList );
};

extern XSceneStorage *SCENE_STORAGE;