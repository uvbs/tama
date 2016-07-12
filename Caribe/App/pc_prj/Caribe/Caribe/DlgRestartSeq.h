#pragma once
#include "resource.h"


// CDlgRestartSeq 대화 상자입니다.

class CDlgRestartSeq : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRestartSeq)

public:
	CDlgRestartSeq(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgRestartSeq();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_RESTART_SEQ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIds;
};
