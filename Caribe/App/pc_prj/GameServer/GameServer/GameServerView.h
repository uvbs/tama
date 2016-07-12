
// GameServerView.h : CGameServerView Ŭ������ �������̽�
//

#pragma once

#include "resource.h"
#include "GameServerDoc.h"
#include "XFramework/MFC/XEServerView.h"
#include "XMainDelegate.h"


class CGameServerView : public CFormView, public XEServerView
{
	XMainDelegate m_delegateMain;
protected: // serialization������ ��������ϴ�.
	CGameServerView();
	DECLARE_DYNCREATE(CGameServerView)

public:
	enum{ IDD = IDD_GAMESERVER_FORM };

// Ư���Դϴ�.
public:
	CGameServerDoc* GetDocument() const;

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
	virtual ~CGameServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);
	virtual void OnDrawBefore( _tstring& str );
};

#ifndef _DEBUG  // GameServerView.cpp�� ����� ����
inline CGameServerDoc* CGameServerView::GetDocument() const
   { return reinterpret_cast<CGameServerDoc*>(m_pDocument); }
#endif

