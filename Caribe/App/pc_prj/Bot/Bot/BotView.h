// BotView.h : CBotView Ŭ������ �������̽�
//

#pragma once
#include "resource.h"

class XMain;
class CBotDoc;
class CBotView : public CFormView
{
public:
	static CBotView* sGet() {
		return s_pInstance;
	}
private:
	static CBotView* s_pInstance;
	CTimer m_timerSec;
//	XMain* m_pMain = nullptr;
protected: // serialization������ ��������ϴ�.
	CBotView();
	DECLARE_DYNCREATE(CBotView)

// Ư���Դϴ�.
public:
	enum{ IDD = IDD_VIEW_FORM };
	CBotDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void Update() {
		Invalidate( FALSE );
	}
protected:

// �����Դϴ�.
public:
	virtual ~CBotView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void OnInitialUpdate() override;

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // BotView.cpp�� ����� ����
inline CBotDoc* CBotView::GetDocument() const
   { return reinterpret_cast<CBotDoc*>(m_pDocument); }
#endif

