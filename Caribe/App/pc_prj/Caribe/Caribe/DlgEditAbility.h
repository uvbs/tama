#pragma once
#include "afxcmn.h"
#include "resource.h"


// CDlgEditAbility 대화 상자입니다.

class CDlgEditAbility : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEditAbility)

public:
	CDlgEditAbility(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgEditAbility();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_ABIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
