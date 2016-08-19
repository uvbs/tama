#pragma once
#include "afxwin.h"
#include "XDlgBaseKey.h"

// CDlgObjProperty 대화 상자입니다.

class CDlgObjProperty : public CDialog, public XDlgBaseKey
{
	DECLARE_DYNAMIC(CDlgObjProperty)

public:
	CDlgObjProperty(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgObjProperty();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROP_OBJ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	// 액션 ID
	DWORD m_id;
	CComboBox m_ctrlPlayType;
	xRPT_TYPE m_PlayType;
//	CString m_strLua;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedLuaDialog();
};
