
// DBAgentServerView.h : CDBAgentServerView 클래스의 인터페이스
//

#pragma once

#include "resource.h"
#include "DBAgentServerDoc.h"
#include "XFramework/MFC/XEServerView.h"
#include "XMainDelegate.h"


class CDBAgentServerView : public CFormView, public XEServerView
{
	XMainDelegate m_delegateMain;
protected: // serialization에서만 만들어집니다.
	CDBAgentServerView();
	DECLARE_DYNCREATE(CDBAgentServerView)

public:
	enum{ IDD = IDD_DBAGENTSERVER_FORM };

// 특성입니다.
public:
	CDBAgentServerDoc* GetDocument() const;

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
	virtual ~CDBAgentServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);
};

#ifndef _DEBUG  // DBAgentServerView.cpp의 디버그 버전
inline CDBAgentServerDoc* CDBAgentServerView::GetDocument() const
   { return reinterpret_cast<CDBAgentServerDoc*>(m_pDocument); }
#endif

