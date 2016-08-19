#pragma once
#include "ThumbView.h"

class CWndThumbView : public CDockablePane
{
	DECLARE_DYNAMIC(CWndThumbView)

public:
	CWndThumbView();
	virtual ~CWndThumbView();

	CThumbView *m_pviewThumb;

	void Update() { }
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


CWndThumbView *GetWndThumbView();
