
// DBAgentServer.h : DBAgentServer ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "XFramework/MFC/XEServerApp.h"


// CDBAgentServerApp:
// �� Ŭ������ ������ ���ؼ��� DBAgentServer.cpp�� �����Ͻʽÿ�.
//

class CDBAgentServerApp : public CWinApp, public XEServerApp
{
public:
	CDBAgentServerApp();
	virtual ~CDBAgentServerApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CDBAgentServerApp theApp;
