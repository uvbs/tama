// DlgEditVal.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgEditVal.h"
#include "afxdialogex.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// CDlgEditVal 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgEditVal, CDialogEx)

CDlgEditVal::CDlgEditVal(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgEditVal::IDD, pParent)
	, m_nLevel( 0 )
	, m_dwEXP( 0 )
{

}

CDlgEditVal::~CDlgEditVal()
{
}

void CDlgEditVal::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_nLevel );
	DDV_MinMaxInt( pDX, m_nLevel, 1, 60 );
	DDX_Text( pDX, IDC_EDIT2, m_dwEXP );
	DDV_MinMaxInt(pDX, m_dwEXP, 0, 5000000);
	DDX_Text( pDX, IDC_EDIT_GM_LEVEL, m_GMLevel );
	DDV_MinMaxInt( pDX, m_GMLevel, 1, 5 );
}


BEGIN_MESSAGE_MAP(CDlgEditVal, CDialogEx)
END_MESSAGE_MAP()


// CDlgEditVal 메시지 처리기입니다.
