// DlgLayerTest.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgLayerTest.h"
#include "afxdialogex.h"


// CDlgLayerTest ��ȭ �����Դϴ�.

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


// CDlgLayerTest �޽��� ó�����Դϴ�.
