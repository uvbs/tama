#pragma once
#include "d3dView.h"
#include "xe.h"

// XViewSpline 뷰입니다.
#define MAX_JOINT	10
class XViewSpline : public CView, public XD3DView
{
public:
	static XViewSpline* CreateOne( CWnd *pParent );
private:
	DECLARE_DYNCREATE(XViewSpline)
	XE::VEC2 m_vMouse;
	struct JOINT
	{
		D3DXVECTOR2 m_vLocal;		// 부모관절로부터의 회전축 위치
		D3DXMATRIX m_mWorld;		// 실시간 갱신용 관절의 현재 행렬
		float m_rAngle;			// 관절의 각도
		D3DXVECTOR2 m_vNormal;	// 관절의 수직벡터
		float dot;
		D3DXVECTOR2 m_vForce;		// 힘벡터
		D3DXVECTOR2 m_vStart;		// 관절 시작 좌표(월드)
		D3DXVECTOR2 m_vEnd;		// 관절끝 좌표(월드)
	};
	JOINT m_listJoint[ MAX_JOINT ];
	int m_idxSelected;		// lbuttondown한 관절 인덱스
	BOOL m_bIK;
	enum xtIK { IK_DOT, IK_CCD, IK_JT };		// IK 알고리즘 선택
	xtIK m_typeIK;
	D3DXVECTOR2 m_vTarget;		// IK가 찾아가야할 목표지점(마우스 좌표)
	BOOL m_bDrawDebug;
protected:
	XViewSpline() {
		m_idxSelected = -1;
		m_bIK = TRUE;
		m_typeIK = IK_CCD;
		m_vTarget = D3DXVECTOR2( 0, 0 );
		m_bDrawDebug = FALSE;
	}           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~XViewSpline() {}

public:
//	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	void UpdateMatrix( int idx=0 );
	void Draw();
	void FrameMove();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

XViewSpline *GetViewSpline();

