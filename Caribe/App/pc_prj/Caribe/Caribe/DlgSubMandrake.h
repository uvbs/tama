#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubMandrake 대화 상자입니다.

class CDlgSubMandrake : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubMandrake)

public:
	CDlgSubMandrake(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSubMandrake();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SUB_MANDRAKE };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_Idx;
	float m_Produce;
};
