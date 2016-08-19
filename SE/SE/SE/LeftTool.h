#pragma once


// CLeftTool 대화 상자입니다.

class CLeftTool : public CDialog
{
	DECLARE_DYNAMIC(CLeftTool)

public:
	CLeftTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLeftTool();

	void UpdateSlider();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LEFTTOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
};
