
// [!output PROJECT_NAME]View.h : C[!output PROJECT_NAME]View 클래스의 인터페이스
//

#pragma once

#include "resource.h"
#include "[!output PROJECT_NAME]Doc.h"
#include "MFC/XEServerView.h"
#include "XMainDelegate.h"


class C[!output PROJECT_NAME]View : public CFormView, public XEServerView
{
	XMainDelegate m_delegateMain;
protected: // serialization에서만 만들어집니다.
	C[!output PROJECT_NAME]View();
	DECLARE_DYNCREATE(C[!output PROJECT_NAME]View)

public:
	enum{ IDD = IDD_[!output UPPERCASE_SAFE_PROJECT_NAME]_FORM };

// 특성입니다.
public:
	C[!output PROJECT_NAME]Doc* GetDocument() const;

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
	virtual ~C[!output PROJECT_NAME]View();
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

#ifndef _DEBUG  // [!output PROJECT_NAME]View.cpp의 디버그 버전
inline C[!output PROJECT_NAME]Doc* C[!output PROJECT_NAME]View::GetDocument() const
   { return reinterpret_cast<C[!output PROJECT_NAME]Doc*>(m_pDocument); }
#endif

