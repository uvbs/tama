#pragma once


// CDlgRenameAction 대화 상자입니다.

class CDlgRenameAction : public CDialog
{
	DWORD *indexToID;
	DECLARE_DYNAMIC(CDlgRenameAction)

public:
	CDlgRenameAction(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgRenameAction();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_RENAME_ACTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
	int m_index;
	DWORD m_dwID;
	BOOL m_bDefaultAutoType;		// 다이얼로그 창이 뜰때 m_dwID초기값이 0:고정아이디 1:자동아이디
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnCbnSelchangeCombo1();
};
