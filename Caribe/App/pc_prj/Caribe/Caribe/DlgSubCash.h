#pragma once
#include "XFramework/MFC/GenericChildDialog.h"
#include "resource.h"


// CDlgSubCash 대화 상자입니다.

class CDlgSubCash : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubCash)

public:
	CDlgSubCash(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSubCash();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SUB_CASH };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_secRegen;
	int m_numMin;
	int m_numMax;
};
