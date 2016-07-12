#pragma once
#include "resource.h"


// CDlgCreateHero 대화 상자입니다.

class CDlgCreateHero : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCreateHero)

public:
	CDlgCreateHero(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCreateHero();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_HERO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIdentifier;
	DWORD m_Num;
};
