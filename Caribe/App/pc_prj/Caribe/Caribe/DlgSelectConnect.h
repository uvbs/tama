#pragma once
#include "resource.h"

// CDlgSelectConnect 대화 상자입니다.

class CDlgSelectConnect : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSelectConnect)

public:
	CDlgSelectConnect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSelectConnect();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SELECT_CONNECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButton1();
  int m_idxSelect = 0;
  afx_msg void OnBnClickedButton2();
  afx_msg void OnBnClickedButton3();
};
