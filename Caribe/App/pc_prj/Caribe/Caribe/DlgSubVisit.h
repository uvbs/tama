#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubVisit 대화 상자입니다.

class CDlgSubVisit : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubVisit)

public:
	CDlgSubVisit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSubVisit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SUB_VISIT };
	virtual int CRHGetDialogID() override {
		return IDD;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strDialog;
};
