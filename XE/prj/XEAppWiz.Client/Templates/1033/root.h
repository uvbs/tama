
// [!output PROJECT_NAME].h : [!output PROJECT_NAME] ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// [!output PROJECT_NAME]App:
// �� Ŭ������ ������ ���ؼ��� [!output PROJECT_NAME].cpp�� �����Ͻʽÿ�.
//

class [!output PROJECT_NAME]App : public CWinApp
{
public:
	[!output PROJECT_NAME]App();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern [!output PROJECT_NAME]App theApp;
