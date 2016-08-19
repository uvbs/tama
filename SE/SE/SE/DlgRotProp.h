#pragma once
#include "XDlgBaseKey.h"


// CDlgRotProp 대화 상자입니다.

class CDlgRotProp : public CDialog, public XDlgBaseKey
{
	DECLARE_DYNAMIC(CDlgRotProp)

public:
	CDlgRotProp(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgRotProp();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROP_ROT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float m_fRotZ;
	BOOL m_bInterpolation;
	afx_msg void OnBnClickedEditInterpolation();
	XE::VEC2 m_vRange;
};
