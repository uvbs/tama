#pragma once
#include "AdjustView.h"
//#include "DlgTest.h"
class CWndAdjustView : public CDockablePane
{
	DECLARE_DYNAMIC(CWndAdjustView)

public:
	CWndAdjustView();
	virtual ~CWndAdjustView();

	CAdjustView *m_pviewAdjust;
//	CDlgTest *m_pviewAdjust;

	void Update() { }
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


CWndAdjustView *GetWndAdjustView();
