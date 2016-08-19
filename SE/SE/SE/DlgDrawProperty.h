#pragma once
#include "afxwin.h"
#include "Sprite.h"
#include "XDlgBaseKey.h"
// CDlgDrawProperty 대화 상자입니다.

class CDlgDrawProperty : public CDialog, public XDlgBaseKey
{
	DECLARE_DYNAMIC(CDlgDrawProperty)

public:
	CDlgDrawProperty(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDrawProperty();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DRAW_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bFlipHoriz;
	BOOL m_bFlipVert;
	CComboBox m_ctrlDrawMode;
	xDM_TYPE m_DrawMode;
	float m_fOpacity;
	BOOL m_bInterpolation;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	int m_nOpacitySlider;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedEditInterpolation();
	XE::VEC2 m_vRangeAlpha;
};
