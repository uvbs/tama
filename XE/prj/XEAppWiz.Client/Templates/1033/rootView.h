
// [!output PROJECT_NAME]View.h : [!output PROJECT_NAME]View Ŭ������ �������̽�
//

#pragma once
#include "_DirectX/d3dView.h"
#include "client/XAppDelegate.h"


class [!output PROJECT_NAME]View : public CScrollView, public XD3DView
{
	XAppDelegate appDelegate;
protected: // serialization������ ��������ϴ�.
	[!output PROJECT_NAME]View();
	DECLARE_DYNCREATE([!output PROJECT_NAME]View)

// Ư���Դϴ�.
public:
	[!output PROJECT_NAME]Doc* GetDocument() const;

// �۾��Դϴ�.
public:
	void Draw( void );
	void Update( void ) { Invalidate( FALSE ); }

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// �����Դϴ�.
public:
	virtual ~[!output PROJECT_NAME]View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // [!output PROJECT_NAME]View.cpp�� ����� ����
inline [!output PROJECT_NAME]Doc* [!output PROJECT_NAME]View::GetDocument() const
   { return reinterpret_cast<[!output PROJECT_NAME]Doc*>(m_pDocument); }
#endif

[!output PROJECT_NAME]View* GetView( void );