#pragma once

#include "d3dView.h"
#include "XSurfaceD3D.h"
#include "XFont.h"
// CThumbView 뷰입니다.

class CThumbView : public CScrollView, public XD3DView
{
	DECLARE_DYNCREATE(CThumbView)

    
	float m_fWPI;				// 섬네일 하나의 가로크기
	CPoint m_pointMouse;
	XBaseFontObj *m_pFont;
//	CToolBar m_ToolBar;
protected:
	CThumbView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThumbView();

public:

	int m_w, m_h;
	LPDIRECT3DTEXTURE9 m_pd3dTexture;		// 렌더 타겟 텍스쳐
	LPD3DXRENDERTOSURFACE m_pd3dRTS;	
	XSurfaceD3D *m_pSurface;

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	static CThumbView *CreateOne( CWnd *pParent );

	void UpdateBitmap();
	int GetSprIdx( CPoint point );
	void Update() { Invalidate( FALSE ); }
	void DrawThumbNailView( int nStartSpr, float nWPI );

protected:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDelSelSpr();
	afx_msg void OnAddSelSpr();
	afx_msg void OnInitAdj();
	afx_msg void OnSetBaseSpr();
	afx_msg void OnClearBaseSpr();
	afx_msg void OnInsKeyFrame();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnAdjCenter();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnEditSpriteAttr();
	afx_msg void OnSavePng();
};

CThumbView *GetThumbView();
