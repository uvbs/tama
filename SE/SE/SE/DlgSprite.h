#pragma once
#include "Resource.h"

// CDlgSprite 대화 상자입니다.

class CDlgSprite : public CDialog
{
	DECLARE_DYNAMIC(CDlgSprite)

public:
	CDlgSprite(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSprite();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SPRITE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
