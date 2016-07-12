// DlgSubVisit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSubVisit.h"
#include "afxdialogex.h"


// CDlgSubVisit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSubVisit, CDialogEx)

CDlgSubVisit::CDlgSubVisit(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CDlgSubVisit::IDD, pParent)
: m_strDialog( _T( "" ) )
{

}

CDlgSubVisit::~CDlgSubVisit()
{
}

void CDlgSubVisit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_DIALOG, m_strDialog );
}


BEGIN_MESSAGE_MAP(CDlgSubVisit, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubVisit 메시지 처리기입니다.
