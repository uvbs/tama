#pragma once
#include "XFramework/MFC/GenericChildDialog.h"
#include "resource.h"


// CDlgSubCommon ��ȭ �����Դϴ�.

class CDlgSubCommon : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubCommon)

public:
	CDlgSubCommon(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSubCommon();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB_COMMON };
  virtual int CRHGetDialogID() {
    return IDD;
  }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
