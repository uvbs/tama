#pragma once
#include "resource.h"
#include "XPropBgObjH.h"

// CDlgNewBgObj 대화 상자입니다.

class CDlgNewBgObj : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNewBgObj)

public:
	CDlgNewBgObj(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNewBgObj();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_BGOBJ };

	BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	xnBgObj::xProp m_Prop;
// 	int m_posX;
// 	int m_posY;
// 	int m_type;
	CString m_strSpr;
	CString m_strSndFile;		// 확장자는 넣지 않음.
	CComboBox m_comboType;
};
