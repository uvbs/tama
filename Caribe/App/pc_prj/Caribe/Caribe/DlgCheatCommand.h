#pragma once
#include "resource.h"

// CDlgCheatCommand ��ȭ �����Դϴ�.

class CDlgCheatCommand : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCheatCommand)
	
public:
	CDlgCheatCommand(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgCheatCommand();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CHEAT_COMMAND };
	CString m_strCmd;
	CString m_strDesc;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
