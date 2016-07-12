#pragma once
#include "resource.h"


// XDlgPropCloud 대화 상자입니다.

class XDlgPropCloud : public CDialogEx
{
	DECLARE_DYNAMIC(XDlgPropCloud)

public:
	XDlgPropCloud(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~XDlgPropCloud();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROP_CLOUD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	DWORD m_idCloud;
	int m_idxImg;
	int m_xPos;
	int m_yPos;
	int m_lvOpenable;
	int m_Cost;
	int m_xPosCost;
	int m_yPosCost;
	DWORD m_idName;
	CString m_idsPrecede;
	CString m_idsItem;
	CString m_idsArea;
};
