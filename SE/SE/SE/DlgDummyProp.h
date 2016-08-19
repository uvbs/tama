#pragma once


// CDlgDummyProp 대화 상자입니다.

class CDlgDummyProp : public CDialog
{
	DECLARE_DYNAMIC(CDlgDummyProp)

public:
	CDlgDummyProp(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDummyProp();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROP_DUMMY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_idDummy;
	BOOL m_bActive;
};
