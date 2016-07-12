// DlgSubCash.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSubCash.h"
#include "afxdialogex.h"


// CDlgSubCash 대화 상자입니다.

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


// CDlgSubCash 메시지 처리기입니다.
