
// LoginServer.h : LoginServer ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "XFramework/MFC/XEServerApp.h"


// CLoginServerApp:
// �� Ŭ������ ������ ���ؼ��� LoginServer.cpp�� �����Ͻʽÿ�.
//

class CLoginServerApp : public CWinApp, public XEServerApp
{
public:
	CLoginServerApp();
	virtual ~CLoginServerApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CLoginServerApp theApp;
