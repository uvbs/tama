// DlgSubCommon.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSubCommon.h"
#include "afxdialogex.h"


// CDlgSubCommon 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSubCommon, CDialogEx)

CDlgSubCommon::CDlgSubCommon(CWnd* pParent /*=NULL*/)
: //m_TypeCommon( 0 )
  m_strParam( _T( "" ) )
, m_nParam1( 0 )
, m_nParam2( 0 )
, m_nParam3( 0 )
, m_nParam4( 0 )
, m_strType( _T( "" ) )
, m_strSpr( _T( "" ) )
{
}

CDlgSubCommon::~CDlgSubCommon()
{
}

void CDlgSubCommon::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT2, m_strParam );
	DDX_Text( pDX, IDC_EDIT5, m_nParam1 );
	DDX_Text( pDX, IDC_EDIT6, m_nParam2 );
	DDX_Text( pDX, IDC_EDIT7, m_nParam3 );
	DDX_Text( pDX, IDC_EDIT8, m_nParam4 );
	DDX_Text( pDX, IDC_EDIT1, m_strType );
	DDX_Text( pDX, IDC_EDIT11, m_strSpr );
}


BEGIN_MESSAGE_MAP(CDlgSubCommon, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubCommon 메시지 처리기입니다.
