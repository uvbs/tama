#pragma once
#include "resource.h"

// CDlgSelectConnect ��ȭ �����Դϴ�.

class CDlgSelectConnect : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSelectConnect)

public:
	CDlgSelectConnect(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSelectConnect();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SELECT_CONNECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButton1();
  int m_idxSelect = 0;
  afx_msg void OnBnClickedButton2();
  afx_msg void OnBnClickedButton3();
};
