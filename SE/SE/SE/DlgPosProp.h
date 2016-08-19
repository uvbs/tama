#pragma once
#include "XDlgBaseKey.h"

// CDlgPosProp 대화 상자입니다.

class CDlgPosProp : public CDialog, public XDlgBaseKey
{
	DECLARE_DYNAMIC(CDlgPosProp)

public:
	CDlgPosProp(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPosProp();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROP_POS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float m_x;
	float m_y;
protected:
	virtual void OnOK();
public:
	BOOL m_bInterpolation;
	afx_msg void OnBnClickedButton1();
	float m_minX;
	float m_maxX;
	float m_minY;
	float m_maxY;
	float m_radius;
};
