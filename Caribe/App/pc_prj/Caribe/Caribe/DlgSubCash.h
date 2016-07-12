#pragma once
#include "XFramework/MFC/GenericChildDialog.h"
#include "resource.h"


// CDlgSubCash ��ȭ �����Դϴ�.

class CDlgSubCash : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubCash)

public:
	CDlgSubCash(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSubCash();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB_CASH };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_secRegen;
	int m_numMin;
	int m_numMax;
};
