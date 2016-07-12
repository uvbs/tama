// DlgSubMandrake.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSubMandrake.h"
#include "afxdialogex.h"


// CDlgSubMandrake 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSubMandrake, CDialogEx)

CDlgSubMandrake::CDlgSubMandrake(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CDlgSubMandrake::IDD, pParent)
: m_Idx( 0 )
, m_Produce( 0 )
{

}

CDlgSubMandrake::~CDlgSubMandrake()
{
}

void CDlgSubMandrake::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_IDX, m_Idx );
	DDX_Text( pDX, IDC_PRODUCE, m_Produce );
}


BEGIN_MESSAGE_MAP(CDlgSubMandrake, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubMandrake 메시지 처리기입니다.
