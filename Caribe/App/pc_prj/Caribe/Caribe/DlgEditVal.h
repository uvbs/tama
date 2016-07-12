#pragma once
#include "resource.h"


// CDlgEditVal 대화 상자입니다.

class CDlgEditVal : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEditVal)

public:
	CDlgEditVal(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgEditVal();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_VAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	// 계정레벨
	int m_nLevel;
	int m_dwEXP;
	int m_GMLevel = 0;
};
