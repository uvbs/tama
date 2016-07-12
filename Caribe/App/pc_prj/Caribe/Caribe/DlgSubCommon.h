#pragma once
#include "XFramework/MFC/GenericChildDialog.h"
#include "resource.h"


// CDlgSubCommon 대화 상자입니다.

class CDlgSubCommon : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubCommon)

public:
	CDlgSubCommon(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSubCommon();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SUB_COMMON };
  virtual int CRHGetDialogID() {
    return IDD;
  }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
  CString m_strParam;
  int m_nParam1;
  int m_nParam2;
  int m_nParam3;
  int m_nParam4;
  CString m_strType;
  CString m_strSpr;
};
