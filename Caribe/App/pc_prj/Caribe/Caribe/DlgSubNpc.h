#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubNpc ��ȭ �����Դϴ�.

class CDlgSubNpc : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubNpc)

public:
	CDlgSubNpc(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSubNpc();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB_NPC };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	float m_secRegen;
	virtual BOOL OnInitDialog();
	CString m_strClan;
//	int m_Elite;
};
