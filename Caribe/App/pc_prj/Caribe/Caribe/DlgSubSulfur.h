#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubSulfur 대화 상자입니다.

class CDlgSubSulfur : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubSulfur)

public:
	CDlgSubSulfur(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSubSulfur();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SUB_SULFUR };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float m_Produce;
	float m_secRegen;
};
