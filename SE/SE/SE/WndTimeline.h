#pragma once
#include "XMyD3DView.h"
// CWndTimeline

class XDlgBarTimeline : public CDialogBar
{
public:
	static XDlgBarTimeline* sGet() {
		return s_pInstance;
	}
	static XDlgBarTimeline* s_pInstance;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
public:
	XDlgBarTimeline();

	int GetPlayMode() { return m_comboPlayMode.GetCurSel(); }
	void UpdateSlider();
	void Update();

public:
	CComboBox m_comboPlayMode;
	int m_nKeyEditMode;

	virtual void OnInitialUpdate();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtAddLayer();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedRadioKeyMove();
	afx_msg void OnBnClickedRadioKeyScale();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
class CWndTimeline : public CDockablePane
{
public:
	static CWndTimeline* sGet() {
		return s_pInstance;
	}
	static CWndTimeline* s_pInstance;
private:
	DECLARE_DYNAMIC(CWndTimeline)

	XDlgBarTimeline m_dlgBar;
public:
	CWndTimeline();
	virtual ~CWndTimeline();

	XMyD3DView *m_pView;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
};

XDlgBarTimeline *GetDlgBarTimeline();
CWndTimeline *GetWndTimeline();

