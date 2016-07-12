
// DBAgentServerView.h : CDBAgentServerView Ŭ������ �������̽�
//

#pragma once

#include "resource.h"
#include "DBAgentServerDoc.h"
#include "XFramework/MFC/XEServerView.h"
#include "XMainDelegate.h"


class CDBAgentServerView : public CFormView, public XEServerView
{
	XMainDelegate m_delegateMain;
protected: // serialization������ ��������ϴ�.
	CDBAgentServerView();
	DECLARE_DYNCREATE(CDBAgentServerView)

public:
	enum{ IDD = IDD_DBAGENTSERVER_FORM };

// Ư���Դϴ�.
public:
	CDBAgentServerDoc* GetDocument() const;

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
	virtual ~CDBAgentServerView();
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

#ifndef _DEBUG  // DBAgentServerView.cpp�� ����� ����
inline CDBAgentServerDoc* CDBAgentServerView::GetDocument() const
   { return reinterpret_cast<CDBAgentServerDoc*>(m_pDocument); }
#endif

