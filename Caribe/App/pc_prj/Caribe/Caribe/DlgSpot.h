#pragma once
#include "resource.h"

class CRHGenericChildDialog;
// CDlgSpot 대화 상자입니다.

class CDlgSpot : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSpot)

public:
	CRHGenericChildDialog *m_pDlgSub = nullptr;

	CDlgSpot(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSpot();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SPOT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	XGAME::xtSpot m_Type = XGAME::xSPOT_NONE;
	CString m_strIds;
	DWORD m_idSpot;
	CString m_strName;
	int m_x;
	int m_y;
	CString m_strArea;
	CString m_strWhen;
	DWORD m_idWhen;
	CString m_strWhenDel;
	DWORD m_idWhenDel;
	CString m_strCode;
	DWORD m_idCode;
	CString m_strSpr;
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	virtual void OnOK();
	DWORD m_idName;
	float m_numProduce[2];
	float m_secRegen = 0;
	int m_Idx = 0;
	_tstring m_str1;
	ID m_idCommon = 0;
	DWORD m_idArea;
	XGAME::xtDropCastle m_typeDrop = XGAME::xDC_NO_DROP;
	CString m_strType;
	int m_nParam[ 4 ];
};
