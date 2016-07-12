// DlgCreateHero.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgCreateHero.h"
#include "afxdialogex.h"


// CDlgCreateHero 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCreateHero, CDialogEx)

CDlgCreateHero::CDlgCreateHero(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCreateHero::IDD, pParent)
	, m_strIdentifier( _T( "" ) )
	, m_Num( 1 )
{

}

CDlgCreateHero::~CDlgCreateHero()
{
}

void CDlgCreateHero::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_strIdentifier );
	DDX_Text( pDX, IDC_EDIT2, m_Num );
	DDV_MinMaxUInt(pDX, m_Num, 1, 10);
}


BEGIN_MESSAGE_MAP(CDlgCreateHero, CDialogEx)
END_MESSAGE_MAP()


// CDlgCreateHero 메시지 처리기입니다.
