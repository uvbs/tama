#pragma once
#include "resource.h"


// CDlgCreateHero ��ȭ �����Դϴ�.

class CDlgCreateHero : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCreateHero)

public:
	CDlgCreateHero(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgCreateHero();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATE_HERO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIdentifier;
	DWORD m_Num;
};
