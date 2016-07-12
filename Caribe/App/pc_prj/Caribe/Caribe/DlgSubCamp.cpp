// DlgSubCamp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSubCamp.h"
#include "afxdialogex.h"


// CDlgSubCamp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSubCamp, CDialogEx)

CDlgSubCamp::CDlgSubCamp(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CDlgSubCamp::IDD, pParent)
: m_strCamp( _T( "" ) )
{

}

CDlgSubCamp::~CDlgSubCamp()
{
}

void CDlgSubCamp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_IDS_CAMP, m_strCamp );
}


BEGIN_MESSAGE_MAP(CDlgSubCamp, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubCamp 메시지 처리기입니다.
