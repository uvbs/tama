#pragma once
#include "resource.h"


// CDlgRestartSeq ��ȭ �����Դϴ�.

class CDlgRestartSeq : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRestartSeq)

public:
	CDlgRestartSeq(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgRestartSeq();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_RESTART_SEQ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIds;
};
