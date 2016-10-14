
// CaribeView.h : CaribeView Ŭ������ �������̽�
//

#pragma once
#if defined(_VER_DX) 
#include "_DirectX/d3dView.h"
#elif defined(_VER_OPENGL)
#include "OpenGL2/WinGLES2/glesView.h"
#else
#error "??"
#endif // defined(_VER_DX) 
#include "client/XAppDelegate.h"
#include "XPropTech.h"
#include "XPropWorld.h"

class CDlgSpot;
class CDlgEditAbility;
class CaribeDoc;
class CaribeView  
#if defined(_VER_DX)
	: public CView, public XD3DView
#elif defined(_VER_OPENGL)
	: public XGLESView
#endif // _VER_DX
{
public:
	XE::VEC2 m_vRClick;
private:
	XAppDelegate appDelegate;
	XE::VEC2 m_vMouse;
	float m_Scale;
protected: // serialization������ ��������ϴ�.
	CaribeView();
	DECLARE_DYNCREATE(CaribeView)

// Ư���Դϴ�.
public:
	CaribeDoc* GetDocument() const;

// �۾��Դϴ�.
public:
// 	bool m_bCtrl = false;			// ctrlŰ�� ������ �ִ°�.
// 	bool m_bAlt = false;			// altŰ�� ������ �ִ°�.
	static int s_idxLogin;
#ifdef _VER_DX
	void Draw( void );
#endif // _VER_DX
	void Update( void ) { Invalidate( FALSE ); }
	void UpdateNodeFromDlg( XPropTech::xNodeAbil *pNodeAbil, CDlgEditAbility& dlg );
	bool DoNewSpotDlg( XGAME::xtSpot type );
	bool DoEditSpotDlg( XPropWorld::xBASESPOT *pBaseSpot );
	void DoPreInputSpot( XGAME::xtSpot type, XPropWorld::xBASESPOT *pBaseProp, CDlgSpot& dlg );
	void DoAfterInputSpot( CDlgSpot& dlg, XPropWorld::xBASESPOT *pOutBaseProp );
	void DoSpawnSquad( XGAME::xtUnit unit, XSPAcc spAcc );

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	bool DoDlgNewBgObj();
protected:
	int DoDlgSelectConnect();
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// �����Դϴ�.
public:
	virtual ~CaribeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate( LPCREATESTRUCT lpcs );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
	afx_msg void OnNewCloudGroup();
	afx_msg void OnCreateAbil();
	afx_msg void OnEditAbil();
	afx_msg void OnUpdateEditAbil( CCmdUI *pCmdUI );
// 	afx_msg void OnDelNode();
//	afx_msg void OnUpdateDelNode( CCmdUI *pCmdUI );
	afx_msg void OnToolUndo();
	afx_msg void OnCreateSpot();
	afx_msg void OnUpdateEditSpot( CCmdUI *pCmdUI );
	afx_msg void OnCreateSpotCastle();
	afx_msg void OnCreateSpotJewel();
	afx_msg void OnCreateSpotSulfur();
	afx_msg void OnCreateSpotMandrake();
	afx_msg void OnCreateSpotNpc();
	afx_msg void OnCreateSpotCampaign();
	afx_msg void OnCreateSpotVisit();
	afx_msg void OnEditSpot();
	afx_msg void OnCreateSpearman();
	afx_msg void OnCreateArcher();
	afx_msg void OnCreatePaladin();
	afx_msg void OnCreateMinotaur();
	afx_msg void OnCreateCyclops();
	afx_msg void OnCreateLycan();
	afx_msg void OnCreateGolem();
	afx_msg void OnCreateTreant();
	afx_msg void OnCreateFallenAngel();
	afx_msg void OnCreateSpotCash();
	afx_msg void OnCreateSpotCommon();
	afx_msg void OnCopySpot();
	afx_msg void OnPasteSpot();
//	afx_msg void OnDestroy();
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnDelOutLink();
	afx_msg void OnCreateBgObj();
	afx_msg void OnEditBgObj();
	afx_msg void OnDelBgObj();
	afx_msg void OnViewBgObjBoundBox();
	afx_msg void OnUpdateViewBgObjBoundBox( CCmdUI *pCmdUI );
};
#ifndef _DEBUG  // CaribeView.cpp�� ����� ����
inline CaribeDoc* CaribeView::GetDocument() const
   { return reinterpret_cast<CaribeDoc*>(m_pDocument); }
#endif

CaribeView* GetView( void );