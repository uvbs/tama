// DlgSubJewel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSubJewel.h"
#include "afxdialogex.h"


// CDlgSubJewel 대화 상자입니다.

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


// CDlgSubJewel 메시지 처리기입니다.
