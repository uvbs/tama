#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"

// CDlgSubJewel ��ȭ �����Դϴ�.

class CDlgSubJewel : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubJewel)

public:
	CDlgSubJewel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSubJewel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB_JEWEL };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_Idx;
	float m_Produce;
};
