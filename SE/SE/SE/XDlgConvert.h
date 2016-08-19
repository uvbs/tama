#pragma once
#include "afxwin.h"


// XDlgConvert 대화 상자입니다.

class XDlgConvert : public CDialog
{
	DECLARE_DYNAMIC(XDlgConvert)

public:
	XDlgConvert(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~XDlgConvert();

	CString m_strConsole;
	CListCtrl m_ctrlList;
	int m_nNumList;
	std::list<CString> m_listFiles;

	void Message( LPCTSTR sz, ... );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONVERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedGo();
};
