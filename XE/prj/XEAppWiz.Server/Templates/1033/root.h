
// [!output PROJECT_NAME].h : [!output PROJECT_NAME] ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "MFC/XEServerApp.h"


// C[!output PROJECT_NAME]App:
// �� Ŭ������ ������ ���ؼ��� [!output PROJECT_NAME].cpp�� �����Ͻʽÿ�.
//

class C[!output PROJECT_NAME]App : public CWinApp, public XEServerApp
{
public:
	C[!output PROJECT_NAME]App();
	virtual ~C[!output PROJECT_NAME]App();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern C[!output PROJECT_NAME]App theApp;
