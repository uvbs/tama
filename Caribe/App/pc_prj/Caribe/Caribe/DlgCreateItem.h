#pragma once
#include "resource.h"


// CDlgCreateItem ��ȭ �����Դϴ�.

class CDlgCreateItem : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCreateItem)

public:
	CDlgCreateItem(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgCreateItem();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATE_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIdentifier;
	int m_numItem;
	virtual void OnOK();
};
