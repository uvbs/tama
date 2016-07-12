// DlgSubCash.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgSubCash.h"
#include "afxdialogex.h"


// CDlgSubCash ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgSubCash, CDialogEx)

CDlgSubCash::CDlgSubCash(CWnd* pParent /*=NULL*/)
	: m_secRegen( 3600 )
	, m_numMin( 1 )
	, m_numMax( 5 )
{

}

CDlgSubCash::~CDlgSubCash()
{
}

void CDlgSubCash::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_SEC_REGEN, m_secRegen );
	DDX_Text( pDX, IDC_PRODUCE, m_numMin );
	DDV_MinMaxInt( pDX, m_numMin, 1, 99 );
	DDX_Text( pDX, IDC_PRODUCE2, m_numMax );
	DDV_MinMaxInt(pDX, m_numMax, 1, 99);
}


BEGIN_MESSAGE_MAP(CDlgSubCash, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubCash �޽��� ó�����Դϴ�.
