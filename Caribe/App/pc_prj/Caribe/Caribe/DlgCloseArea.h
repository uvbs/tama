#pragma once
#include "resource.h"


// CDlgCloseArea ��ȭ �����Դϴ�.

class CDlgCloseArea : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCloseArea)

public:
	CDlgCloseArea(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgCloseArea();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CLOSE_AREA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strArea;
};
