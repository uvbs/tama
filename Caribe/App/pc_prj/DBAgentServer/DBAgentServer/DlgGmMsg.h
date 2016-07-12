#pragma once

// DlgGmMsg 대화 상자입니다.

class DlgGmMsg : public CDialogEx
{
	DECLARE_DYNAMIC(DlgGmMsg)

public:
	DlgGmMsg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgGmMsg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GM_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMsg;
	static CString s_strMsg;		// 이전 메시지
	virtual void OnOK();
};
