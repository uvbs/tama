#pragma once
#include "resource.h"


// CDlgEditVal ��ȭ �����Դϴ�.

class CDlgEditVal : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEditVal)

public:
	CDlgEditVal(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgEditVal();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_EDIT_VAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	// ��������
	int m_nLevel;
	int m_dwEXP;
	int m_GMLevel = 0;
};
