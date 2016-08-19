
// SE.h : SE 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "XGraphicsD3DTool.h"

// CSEApp:
// 이 클래스의 구현에 대해서는 SE.cpp을 참조하십시오.
//

class CSEApp : public CWinAppEx
{
public:
	CSEApp();
	~CSEApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
	virtual int Run();
};

extern CSEApp theApp;
#include "ActionListView.h"
#include "AdjustView.h"
#include "AnimationView.h"
#include "ConsoleView.h"
#include "ModifyView.h"
#include "FrameView.h"
#include "ThumbView.h"
#include "XDlgEditInterpolation.h"

#define ALL_VIEW_UPDATE()				\
	if( GetThumbView() )	GetThumbView()->Update();			\
	if( GetAdjustView() )		GetAdjustView()->Update();			\
	if( GetActionListView() )	GetActionListView()->Update();		\
	if( GetAnimationView() ) GetAnimationView()->Update();		\
	if( GetFrameView() )		GetFrameView()->Update();
