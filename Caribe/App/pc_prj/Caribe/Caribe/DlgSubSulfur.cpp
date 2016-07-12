// DlgSubSulfur.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSubSulfur.h"
#include "afxdialogex.h"


// CDlgSubSulfur 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSubSulfur, CDialogEx)

CDlgSubSulfur::CDlgSubSulfur(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CDlgSubSulfur::IDD, pParent)
: m_Produce( 0 )
, m_secRegen( 0 )
{

}

CDlgSubSulfur::~CDlgSubSulfur()
{
}

void CDlgSubSulfur::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_PRODUCE, m_Produce );
	DDX_Text( pDX, IDC_SEC_REGEN, m_secRegen );
}


BEGIN_MESSAGE_MAP(CDlgSubSulfur, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubSulfur 메시지 처리기입니다.
