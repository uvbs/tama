#pragma once


// XDlgLuaEditor 대화 상자입니다.

class XDlgLuaEditor : public CDialog
{
	DECLARE_DYNAMIC(XDlgLuaEditor)

public:
	XDlgLuaEditor(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~XDlgLuaEditor();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_LUA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strLua;
	CString m_strTestLua;
	afx_msg void OnBnClickedOk();
};
