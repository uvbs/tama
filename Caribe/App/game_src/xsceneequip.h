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
class XHero;
class XWndStoragyItemElem;
class XSceneEquip : public XSceneBase
{
public:
	struct xSPM_EQUIP : public XGAME::xSceneParamBase
	{
		ID snHero;
		XGAME::xtParts idxParts;
		void Init() {
			snHero = 0;
			idxParts = XGAME::xPARTS_NONE;
		}
		xSPM_EQUIP()
		{
			Init();
		}
		virtual ~xSPM_EQUIP(){}
		bool IsEmpty() {
			return snHero == 0;
		}
	};
private:
	XLayoutObj m_Layout;
	XLayoutObj m_LayoutTooltip;
	XWndStoragyItemElem *m_pSelectItem;		// 선택된 리스트 아이템
	XHero *m_pHero;
	xSPM_EQUIP m_BaseParam;
	//
	void Init()  {
		m_pSelectItem = nullptr;
		m_pHero = nullptr;
	}
	void Destroy();
protected:
public:
	XSceneEquip(XGame *pGame, XSPSceneParam& spBaseParam);
	virtual ~XSceneEquip(void) { Destroy(); }
	//
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	void Update() override;
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );

	int OnEquip(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnBack(XWnd *pWnd, DWORD p1, DWORD p2);

	void RecvReqHeroEquip(ID snHero, ID snItem, ID typeAction);
	void UpdateItemInfo();
	int OnSelectItem(XWnd *pWnd, DWORD p1, DWORD p2);
};

extern XSceneEquip *SCENE_EQUIP;

