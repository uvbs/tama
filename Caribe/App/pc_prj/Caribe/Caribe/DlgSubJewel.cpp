// DlgSubJewel.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgSubJewel.h"
#include "afxdialogex.h"


// CDlgSubJewel ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgSubJewel, CDialogEx)

CDlgSubJewel::CDlgSubJewel(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CDlgSubJewel::IDD, pParent)
: m_Idx( 0 )
, m_Produce( 0 )
{

}

CDlgSubJewel::~CDlgSubJewel()
{
}

void CDlgSubJewel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_IDX, m_Idx );
	DDX_Text( pDX, IDC_PRODUCE, m_Produce );
}


BEGIN_MESSAGE_MAP(CDlgSubJewel, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubJewel �޽��� ó�����Դϴ�.
