
// Bot.h : Bot ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CBotApp:
// �� Ŭ������ ������ ���ؼ��� Bot.cpp�� �����Ͻʽÿ�.
//

class CBotApp : public CWinApp
{
public:
	CBotApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	BOOL OnIdle( LONG lCount ) override;

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CBotApp theApp;
