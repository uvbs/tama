#pragma once
#include "LeftTool.h"
#include "DlgTest.h"
#include "ThumbView.h"
// CWndLeftTool

class CWndLeftTool : public CDockablePane
{
	DECLARE_DYNAMIC(CWndLeftTool)

public:
	CWndLeftTool();
	virtual ~CWndLeftTool();

	CLeftTool m_dlgLeftTool;
	CDlgTest		*m_pdlgTest;
	CThumbView *m_pviewThumb;

	void Update() { m_dlgLeftTool.UpdateSlider(); }
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


CWndLeftTool *GetWndLeftTool();
