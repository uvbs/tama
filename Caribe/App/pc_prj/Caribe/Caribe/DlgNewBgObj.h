#pragma once
#include "resource.h"
#include "XPropBgObjH.h"

// CDlgNewBgObj ��ȭ �����Դϴ�.

class CDlgNewBgObj : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNewBgObj)

public:
	CDlgNewBgObj(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgNewBgObj();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATE_BGOBJ };

	BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	xnBgObj::xProp m_Prop;
// 	int m_posX;
// 	int m_posY;
// 	int m_type;
	CString m_strSpr;
	CString m_strSndFile;		// Ȯ���ڴ� ���� ����.
	CComboBox m_comboType;
};
