#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubSulfur ��ȭ �����Դϴ�.

class CDlgSubSulfur : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubSulfur)

public:
	CDlgSubSulfur(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSubSulfur();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB_SULFUR };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	float m_Produce;
	float m_secRegen;
};
