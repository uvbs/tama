#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubMandrake ��ȭ �����Դϴ�.

class CDlgSubMandrake : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubMandrake)

public:
	CDlgSubMandrake(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSubMandrake();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB_MANDRAKE };

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
