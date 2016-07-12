#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubNpc 대화 상자입니다.

class CDlgSubNpc : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubNpc)

public:
	CDlgSubNpc(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSubNpc();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SUB_NPC };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float m_secRegen;
	virtual BOOL OnInitDialog();
	CString m_strClan;
//	int m_Elite;
};
