#pragma once
#include "resource.h"


// CDlgLayerTest 대화 상자입니다.

class CDlgLayerTest : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLayerTest)

public:
	CDlgLayerTest(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgLayerTest();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LAYER_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strXml;
	CString m_strNode;
};
