#pragma once
#include "XDlgBaseKey.h"

// CDlgScaleProp 대화 상자입니다.

class CDlgScaleProp : public CDialog, public XDlgBaseKey
{
	DECLARE_DYNAMIC(CDlgScaleProp)

public:
	CDlgScaleProp(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgScaleProp();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROP_SCALE };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEditInterpolation();
	XE::VEC2 m_rangeXY;
	XE::VEC2 m_rangeX, m_rangeY;
	//	float m_rangeXY1, m_rangeXY2;
	// 	float m_rangeX1, m_rangeX2;
// 	float m_rangeY1, m_rangeY2;
	float m_fScaleX;
	float m_fScaleY;
	BOOL m_bInterpolation;
};
