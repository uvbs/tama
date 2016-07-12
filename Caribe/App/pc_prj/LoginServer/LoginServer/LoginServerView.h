
// LoginServerView.h : CLoginServerView 클래스의 인터페이스
//

#pragma once

#include "resource.h"
#include "LoginServerDoc.h"
#include "XFramework/MFC/XEServerView.h"
#include "XMainDelegate.h"


class CLoginServerView : public CFormView, public XEServerView
{
	XMainDelegate m_delegateMain;
protected: // serialization에서만 만들어집니다.
	CLoginServerView();
	DECLARE_DYNCREATE(CLoginServerView)

public:
	enum{ IDD = IDD_LOGINSERVER_FORM };

// 특성입니다.
public:
	CLoginServerDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현입니다.
public:
	virtual ~CLoginServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
//	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);
};

#ifndef _DEBUG  // LoginServerView.cpp의 디버그 버전
inline CLoginServerDoc* CLoginServerView::GetDocument() const
   { return reinterpret_cast<CLoginServerDoc*>(m_pDocument); }
#endif

