#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubVisit ��ȭ �����Դϴ�.

class CDlgSubVisit : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubVisit)

public:
	CDlgSubVisit(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSubVisit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB_VISIT };
	virtual int CRHGetDialogID() override {
		return IDD;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strDialog;
};
