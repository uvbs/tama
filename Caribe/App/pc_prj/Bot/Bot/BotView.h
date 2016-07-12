// BotView.h : CBotView 클래스의 인터페이스
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
protected: // serialization에서만 만들어집니다.
	CBotView();
	DECLARE_DYNCREATE(CBotView)

// 특성입니다.
public:
	enum{ IDD = IDD_VIEW_FORM };
	CBotDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void Update() {
		Invalidate( FALSE );
	}
protected:

// 구현입니다.
public:
	virtual ~CBotView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void OnInitialUpdate() override;

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // BotView.cpp의 디버그 버전
inline CBotDoc* CBotView::GetDocument() const
   { return reinterpret_cast<CBotDoc*>(m_pDocument); }
#endif

