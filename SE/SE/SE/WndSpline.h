#pragma once
#include "XViewSpline.h"

// CWndSpline

class CWndSpline : public CDockablePane
{
	DECLARE_DYNAMIC(CWndSpline)

public:
	CWndSpline();
	virtual ~CWndSpline();

	XViewSpline *m_pviewSpline;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


