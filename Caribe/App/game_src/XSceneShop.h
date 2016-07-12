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
class XAppLayout;

// 잡화상 씬
class XSceneShop : public XSceneBase
{
	enum SHOPTYPE { SHOPTYPE_CASH, SHOPTYPE_ETC };	// 캐쉬, 기타

	XLayoutObj m_Layout;
	XWndList *m_pList;

	SHOPTYPE m_shopType;		// 샵 타입

private:
	//
	void Init()  {
//		m_pLayout = nullptr;
		m_pList = nullptr;
	}
	void Destroy();

protected:
public:
	XSceneShop(XGame *pGame);
	virtual ~XSceneShop(void) { Destroy(); }
	//
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );

	// 버튼
	int OnBack(XWnd *pWnd, DWORD p1, DWORD p2);

	int OnClickTabCash(XWnd *pWnd, DWORD p1, DWORD p2);				// 캐쉬 탭
	int OnClickTabEtc(XWnd *pWnd, DWORD p1, DWORD p2);				// 기타 탭

	int OnClickList(XWnd *pWnd, DWORD p1, DWORD p2);				// 리스트 아이템 클릭

	int OnClickBuyCash(XWnd *pWnd, DWORD p1, DWORD p2);				// 캐쉬 구매
	int OnClickBuyEtc(XWnd *pWnd, DWORD p1, DWORD p2);				// 기타 아이템 구매

	int OnClickPopupCancel(XWnd *pWnd, DWORD p1, DWORD p2);			// 팝업 취소

	void UpdateList();		// 리스트 업데이트

	void RevUpdate(bool bBuy = false);		// 구매 후 업데이트
	int OnEnterScene( XWnd*, DWORD, DWORD ) override;
	void Update() override;
};

extern XSceneShop *SCENE_SHOP;