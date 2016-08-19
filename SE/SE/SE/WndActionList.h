#pragma once
#include "ActionListView.h"

// CWndActionList

class CWndActionList : public CDockablePane
{
	DECLARE_DYNAMIC(CWndActionList)

public:
	CWndActionList();
	virtual ~CWndActionList();

	CActionListView *m_pView;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


