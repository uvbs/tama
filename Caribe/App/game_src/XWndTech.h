/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "_Wnd2/XWnd.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndPopup.h"
//#include "etc/XSurface.h"
#include "XPropTech.h"

class XHero;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2014/12/15 12:02
*****************************************************************/
class XWndArrow4Abil : public XWnd
{
	XSurface *m_psfcArrow;			///< 화살표 선
	XE::VEC2 m_vStart;
	XE::VEC2 m_vEnd;
	XE::VEC2 m_vAdjust;
	void Init() {
		m_psfcArrow = nullptr;
	}
	void Destroy();
public:
	XWndArrow4Abil( const XE::VEC2& vStart, const XE::VEC2& vEnd );
	XWndArrow4Abil( LPCTSTR szImg );
	virtual ~XWndArrow4Abil() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vAdjust );
	void SetDirection( const XE::VEC2& vStart, const XE::VEC2& vEnd, LPCTSTR szImg );
	void Draw();
}; // class XWndArrow4Abil

/****************************************************************
* @brief 
* @author xuzhu
* @date	2014/12/17 11:49
*****************************************************************/
class XWndAbilButton : public XWndButton
{
public:
	static ID s_idSelected;
	static ID s_idResearching;
	static ID s_snHeroResearching;
private:
	XGAME::xtUnit m_Unit;
	ID m_idNode;
	XWndTextString *m_pText;
	XSurface *m_pCircle;
	XPropTech::xNodeAbil *m_pProp = nullptr;
	ID m_snHero = 0;
	bool m_bInRightPanel = false;
	void Init() {
		m_idNode = 0;
		m_pText = nullptr;
		m_Unit = XGAME::xUNIT_NONE;
		m_pCircle = nullptr;
	}
	void Destroy();
public:
	XWndAbilButton( XSPHero pHero, XGAME::xtUnit unit, XPropTech::xNodeAbil *pNodeAbil, const XE::VEC2& vPos );
	virtual ~XWndAbilButton() { Destroy(); }
	//
	GET_SET_ACCESSOR( bool, bInRightPanel );
	GET_ACCESSOR_CONST( ID, idNode );
	int Process( float dt ) override;
	void Draw() override;
	void Update() override;
	void SetNode( XGAME::xtUnit unit, XPropTech::xNodeAbil *pProp );
//	void OnMouseMove( float lx, float ly ) override;
}; // class XWndAbilButton

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/05/24 17:17
*****************************************************************/
class XWndResearchConfirm : public XWndPopup
{
public:
	XWndResearchConfirm( XGAME::xtUnit unit, ID idAbil, int point, const XPropTech::xtResearch& costAbil );
	~XWndResearchConfirm() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	int m_Test = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	BOOL OnCreate() override;
	void Update() override;
}; // class XWndResearchConfirm

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/10/02 15:44
*****************************************************************/
class XWndResearchComplete : public XWndPopup
{
public:
	XWndResearchComplete( XSPHero pHero, ID idAbil, int point );
	~XWndResearchComplete() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	int m_Test = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	BOOL OnCreate() override;
	void Update() override;
}; // class XWndResearchComplete
