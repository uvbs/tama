#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubCamp ��ȭ �����Դϴ�.

class CDlgSubCamp : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubCamp)

public:
	CDlgSubCamp(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSubCamp();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB_CAMPAIGN };

	virtual int CRHGetDialogID() {
		return IDD;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCamp;
};
