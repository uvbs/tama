
// GameServerView.h : CGameServerView 클래스의 인터페이스
//

#pragma once

#include "resource.h"
#include "GameServerDoc.h"
#include "XFramework/MFC/XEServerView.h"
#include "XMainDelegate.h"


class CGameServerView : public CFormView, public XEServerView
{
	XMainDelegate m_delegateMain;
protected: // serialization에서만 만들어집니다.
	CGameServerView();
	DECLARE_DYNCREATE(CGameServerView)

public:
	enum{ IDD = IDD_GAMESERVER_FORM };

// 특성입니다.
public:
	CGameServerDoc* GetDocument() const;

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
	virtual ~CGameServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);
	virtual void OnDrawBefore( _tstring& str );
};

#ifndef _DEBUG  // GameServerView.cpp의 디버그 버전
inline CGameServerDoc* CGameServerView::GetDocument() const
   { return reinterpret_cast<CGameServerDoc*>(m_pDocument); }
#endif

