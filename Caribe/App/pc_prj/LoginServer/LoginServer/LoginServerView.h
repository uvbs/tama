
// LoginServerView.h : CLoginServerView Ŭ������ �������̽�
//

#pragma once

#include "resource.h"
#include "LoginServerDoc.h"
#include "XFramework/MFC/XEServerView.h"
#include "XMainDelegate.h"


class CLoginServerView : public CFormView, public XEServerView
{
	XMainDelegate m_delegateMain;
protected: // serialization������ ��������ϴ�.
	CLoginServerView();
	DECLARE_DYNCREATE(CLoginServerView)

public:
	enum{ IDD = IDD_LOGINSERVER_FORM };

// Ư���Դϴ�.
public:
	CLoginServerDoc* GetDocument() const;

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
	virtual ~CLoginServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
//	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);
};

#ifndef _DEBUG  // LoginServerView.cpp�� ����� ����
inline CLoginServerDoc* CLoginServerView::GetDocument() const
   { return reinterpret_cast<CLoginServerDoc*>(m_pDocument); }
#endif

