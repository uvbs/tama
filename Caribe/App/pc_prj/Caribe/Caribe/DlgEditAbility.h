#pragma once
#include "afxcmn.h"
#include "resource.h"


// CDlgEditAbility ��ȭ �����Դϴ�.

class CDlgEditAbility : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEditAbility)

public:
	CDlgEditAbility(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgEditAbility();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_EDIT_ABIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIdentifier;
	CString m_idsSkill;
	CString m_strIcon;
	CString m_strName;
	CString m_strDesc;
	DWORD m_idName;
	DWORD m_idDesc;
	int m_lvOpen;
	int m_maxPoint;
	int m_x;
	int m_y;
	CListCtrl m_ctrlListParents;
	CListCtrl m_ctrlListChilds;
	DWORD m_idNode;
	virtual BOOL OnInitDialog();
};
