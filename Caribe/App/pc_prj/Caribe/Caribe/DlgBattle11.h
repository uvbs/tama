#pragma once
#include "resource.h"


// CDlgBattle11 ��ȭ �����Դϴ�.

class CDlgBattle11 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBattle11)

public:
	CDlgBattle11(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgBattle11();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_BATTLE1_1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	int m_radioPlayer;
	int m_radioAI;
	afx_msg void OnBnClickedRadio11();
	afx_msg void OnBnClickedRadio12();
	afx_msg void OnBnClickedRadio13();
	afx_msg void OnBnClickedRadio14();
	afx_msg void OnBnClickedRadio15();
	afx_msg void OnBnClickedRadio16();
	afx_msg void OnBnClickedRadio17();
	afx_msg void OnBnClickedRadio18();
	afx_msg void OnBnClickedRadio19();
	afx_msg void OnBnClickedRadio20();
	//
	afx_msg void OnBnClickedRadio21();
	afx_msg void OnBnClickedRadio22();
	afx_msg void OnBnClickedRadio23();
	afx_msg void OnBnClickedRadio24();
	afx_msg void OnBnClickedRadio25();
	afx_msg void OnBnClickedRadio26();
	afx_msg void OnBnClickedRadio27();
	afx_msg void OnBnClickedRadio28();
	afx_msg void OnBnClickedRadio29();
	afx_msg void OnBnClickedRadio30();
	int m_lvSquadLeft;
	int m_lvSquadRight;
	CString m_strHero1;
	CString m_strHero2;
	int m_lvHero1;
	int m_lvHero2;
};
