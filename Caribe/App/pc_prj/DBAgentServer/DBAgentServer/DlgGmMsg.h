#pragma once

// DlgGmMsg ��ȭ �����Դϴ�.

class DlgGmMsg : public CDialogEx
{
	DECLARE_DYNAMIC(DlgGmMsg)

public:
	DlgGmMsg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgGmMsg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_GM_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMsg;
	static CString s_strMsg;		// ���� �޽���
	virtual void OnOK();
};
