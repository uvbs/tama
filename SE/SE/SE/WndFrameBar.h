#pragma once


// CWndFrameBar

class CWndFrameBar : public CDockablePane
{
public:
// 	static CWndFrameBar* sGet() {
// 		return s_pInstance;
// 	}
// 	static CWndFrameBar* s_pInstance;
private:
	DECLARE_DYNAMIC(CWndFrameBar)

	CDialogBar m_dlgBar;
public:
	CWndFrameBar();
	virtual ~CWndFrameBar();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


