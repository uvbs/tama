#pragma once
#include "resource.h"


// CDlgLayerTest ��ȭ �����Դϴ�.

class CDlgLayerTest : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLayerTest)

public:
	CDlgLayerTest(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgLayerTest();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LAYER_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strXml;
	CString m_strNode;
};
