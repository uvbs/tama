#pragma once
#include "resource.h"


// CDlgCloseArea 대화 상자입니다.

class CDlgCloseArea : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCloseArea)

public:
	CDlgCloseArea(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCloseArea();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLOSE_AREA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strArea;
};
