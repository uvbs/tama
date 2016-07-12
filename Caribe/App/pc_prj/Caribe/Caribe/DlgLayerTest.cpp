// DlgLayerTest.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgLayerTest.h"
#include "afxdialogex.h"


// CDlgLayerTest 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgLayerTest, CDialogEx)

CDlgLayerTest::CDlgLayerTest(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLayerTest::IDD, pParent)
	, m_strXml( _T( "" ) )
	, m_strNode( _T( "" ) )
{

}

CDlgLayerTest::~CDlgLayerTest()
{
}

void CDlgLayerTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_strXml );
	DDX_Text( pDX, IDC_EDIT2, m_strNode );
}


BEGIN_MESSAGE_MAP(CDlgLayerTest, CDialogEx)
END_MESSAGE_MAP()


// CDlgLayerTest 메시지 처리기입니다.
