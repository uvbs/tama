/********************************************************************
	@date:	2014/12/10 16:45
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XSceneTech.h
	@author:	xuzhu
	
	@brief:	기술연구 유닛특성트리 씬.
*********************************************************************/
#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XPropTech.h"
//#include "XAccount.h"
#include "XStruct.h"

class XGame;
class XAccount;
class XWndAbilButton;
class XSceneTech : public XSceneBase
{
public:
	enum xtMode {
		xMODE_MOVE,
		xMODE_LINK,
	};
	XPropTech::xNodeAbil *m_pLDownNodeT;			///< LButtonDown한 노드
//	XPropTech::xNodeAbil *m_pMouseOverNodeT;		///< 현재 마우스 오버된 노드
	ID m_idMouseOverNodeT = 0;								// 현재 마우스 오버된 노드의 아이디
	XPropTech::xNodeAbil *m_pSelNodeT;				///< 선택된 노드
	XPropTech::xNodeAbil *m_pDeletedNodeT;			///< 삭제될 노드. 노드를 삭제시키면 언두를 위해서 잠시 받아둠.
private:
	XLayoutObj m_Layout;
	XE::VEC2 m_vMouse, m_vMousePrev;
	XGAME::xtUnit m_unitSelected = XGAME::xUNIT_NONE;	///< 현재 보거나 편집중인 유닛
	XSurface *m_psfcArrow;			///< 화살표 선
	BOOL m_bDrag;
	CTimer m_timerAutoSave;	///< 수정한지 3초가 지나면 자동세이브 한다.
	ID m_idSelectedNode = 0;
	ID m_snSelectedHero = 0;
	XHero *m_pHeroSected = nullptr;
	CTimer m_timerGlow;
	XList4<XHero*> m_listHeroes;		// 영웅들 리스트(현재 소트된 상태의)
private:
	enum { SIZE_GRID=7 };
	XE::VEC2 m_vLocalLDownT;						///< LDown을 했을때 아이콘내 마우스 로컬좌표
	xtMode m_modeTool;
	int m_stateMouseT;	// 0:up 1:ldown 2:move
	bool m_bUpdateSort = false;		// 왼쪽 영웅리스트 소트
private:
	//
	void Init()  {
		m_psfcArrow = nullptr;
		m_bDrag = FALSE;
#ifdef _xIN_TOOL
		m_pLDownNodeT = nullptr;
//		m_pMouseOverNodeT = nullptr;
		m_pSelNodeT = nullptr;
		m_pDeletedNodeT = nullptr;
		m_modeTool = xMODE_MOVE;
		m_stateMouseT = 0;
#endif // _xIN_TOOL
	}
	void Destroy();
protected:
public:
	XSceneTech( XGame *pGame );
	virtual ~XSceneTech(void) { Destroy(); }
	//
	GET_ACCESSOR( XGAME::xtUnit, unitSelected );
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	int OnBack( XWnd *pWnd, DWORD p1, DWORD p2 );
#ifdef _xIN_TOOL
	GET_SET_ACCESSOR( xtMode, modeTool );
	void OnModeTool( xtToolMode toolMode );
	void DrawTreeT();
	/**
	 @brief 현재 마우스 좌표를 그리드 단위좌표로 변환해 돌려준다.
	*/
	XE::VEC2 GetPosMouseByGrid( const XE::VEC2& vAdj = XE::VEC2(0)) {
		XE::VEC2 vMouse = m_vMouse + vAdj;
		int xofs = (int)( vMouse.x / SIZE_GRID );
		int yofs = (int)( vMouse.y / SIZE_GRID );
		return XE::VEC2(xofs * SIZE_GRID, yofs * SIZE_GRID);
	}
	XE::VEC2 GetPosByGrid( const XE::VEC2& vPos, const XE::VEC2& vAdj = XE::VEC2(0)) {
		XE::VEC2 vMouse = vPos + vAdj;
		int xofs = (int)( vMouse.x / SIZE_GRID );
		int yofs = (int)( vMouse.y / SIZE_GRID );
		return XE::VEC2(xofs * SIZE_GRID, yofs * SIZE_GRID);
	}
	void UpdateAutoSave( void ) {
		if( m_timerAutoSave.IsOff() )
			m_timerAutoSave.Set( 3.f );
		else
			m_timerAutoSave.Reset();
	}
	void DelNodeSelected();
	void DelOutLinkBySelectedNode();
	void Undo( void );
	int OnSelectUnitTool( XWnd* pWnd, DWORD p1, DWORD p2 );
	void CreateToolCtrl( void );
	void DoCreateNewNode( XPropTech::xNodeAbil *pNewNode );
	BOOL OnKeyDown( int keyCode );
#endif // xIN_TOOL

	int OnClickNode( XWnd* pWnd, DWORD p1, DWORD p2 );
	void UpdateUnitButtons( void );
	int OnClickUnit( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickPlus( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickMinus( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnOkResearch( XWnd* pWnd, DWORD p1, DWORD p2 );
#ifdef _CHEAT
	int OnDebug( XWnd* pWnd, DWORD p1, DWORD p2 );
#endif
	void Update();
	//	void UpdateTreeNode( XWnd *pRoot );
	void UpdateHeroList( XWnd *pRoot );
	void UpdateRightPanel( XWnd *_pRoot, XHero *pHero, XPropTech::xNodeAbil* pProp, const XGAME::xAbil& abil );
	void UpdateInitAbil( XWnd *pRoot, XHero *pHero );
	void UpdateUnlockButton( XWnd *pRoot, XHero *pHero, XPropTech::xNodeAbil* pProp );
	void UpdateSelectedAbil( XWnd *pRoot, XHero *pHero, XPropTech::xNodeAbil* pProp, const XGAME::xAbil& abil );
	void UpdateNeedRes( const XPropTech::xtResearch& costAbil );
	void UpdateTopUnitButtons( XWnd *pRoot, XHero *pHero );
	void UpdateAbilResearch( XWnd *pRoot, XHero *pHero, XPropTech::xNodeAbil* pPropNode, const XGAME::xAbil& abil );
	void UpdateTree( XWnd *pRoot, XHero *pHero );
	int OnClickLockFree( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnCheat( XWnd* pWnd, DWORD p1, DWORD p2 );
	static _tstring _sGetResearchTime( int sec );
	static void sGetRemainResearchTime( _tstring *pOutStr, 
										xSec secStart, 
										int secTotal,
										DWORD *pOutsecPass =nullptr, 
										DWORD *pOutsecRemain = nullptr);
// 	static _tstring sGetResearchTime( ID idAbil );
	void GetRemainResearchTime( _tstring *pOutStr/*, XPropTech::xNodeAbil *pProp*/, int secTotal, DWORD *pOutsecPass=nullptr, DWORD *pOutsecRemain=nullptr );
	void RecvResearchComplete( XPropTech::xNodeAbil *pProp, const XGAME::xAbil& abil );
	int OnClickResearchComplete( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickUnlockUnit( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickHero( XWnd* pWnd, DWORD p1, DWORD p2 );
#ifdef _xIN_TOOL
	void DelegateChangeToolMode( xtToolMode modeOld, xtToolMode modeCurr ) override;
#endif // _xIN_TOOL
private:
	int OnClickInitAbil( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnOkInitAbil( XWnd* pWnd, DWORD p1, DWORD p2 );
//	XWndAbilButton* GetpWndNodeMouseOver( const XE::VEC2& vPos );
};

extern XSceneTech *SCENE_TECH;

