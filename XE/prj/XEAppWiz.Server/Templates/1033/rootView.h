
// [!output PROJECT_NAME]View.h : C[!output PROJECT_NAME]View Ŭ������ �������̽�
//

#pragma once

#include "resource.h"
#include "[!output PROJECT_NAME]Doc.h"
#include "MFC/XEServerView.h"
#include "XMainDelegate.h"


class C[!output PROJECT_NAME]View : public CFormView, public XEServerView
{
	XMainDelegate m_delegateMain;
protected: // serialization������ ��������ϴ�.
	C[!output PROJECT_NAME]View();
	DECLARE_DYNCREATE(C[!output PROJECT_NAME]View)

public:
	enum{ IDD = IDD_[!output UPPERCASE_SAFE_PROJECT_NAME]_FORM };

// Ư���Դϴ�.
public:
	C[!output PROJECT_NAME]Doc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// �����Դϴ�.
public:
	virtual ~C[!output PROJECT_NAME]View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);
};

#ifndef _DEBUG  // [!output PROJECT_NAME]View.cpp�� ����� ����
inline C[!output PROJECT_NAME]Doc* C[!output PROJECT_NAME]View::GetDocument() const
   { return reinterpret_cast<C[!output PROJECT_NAME]Doc*>(m_pDocument); }
#endif

