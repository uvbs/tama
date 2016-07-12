#pragma once
#include "resource.h"


// CDlgCreateItem 대화 상자입니다.

class CDlgCreateItem : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCreateItem)

public:
	CDlgCreateItem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCreateItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIdentifier;
	int m_numItem;
	virtual void OnOK();
};
