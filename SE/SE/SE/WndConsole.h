#pragma once
#include "ConsoleView.h"

// CWndConsole

class CWndConsole : public CDockablePane
{
	DECLARE_DYNAMIC(CWndConsole)

	CConsoleView *m_pView;
public:
	CWndConsole();
	virtual ~CWndConsole();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


