#pragma once
#include "Sprdef.h"
#include "afxwin.h"

// XDlgEditInterpolation 대화 상자입니다.

class XDlgEditInterpolation : public CDialog
{
	DECLARE_DYNAMIC(XDlgEditInterpolation)

public:
	XDlgEditInterpolation(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~XDlgEditInterpolation();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_INTERPOLATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nSelectFunc;
	xSpr::xtInterpolation m_Interpolation;
protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBoganNone();
	afx_msg void OnBnClickedBoganLinear();
	afx_msg void OnBnClickedBoganAccel();
	afx_msg void OnBnClickedBoganDeaccel();
	afx_msg void OnBnClickedBoganSmooth();
	afx_msg void OnBnClickedBoganSpline();
};
