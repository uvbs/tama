// XDlgPropCloud.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "XDlgPropCloud.h"
#include "afxdialogex.h"


// XDlgPropCloud 대화 상자입니다.

IMPLEMENT_DYNAMIC(XDlgPropCloud, CDialogEx)

XDlgPropCloud::XDlgPropCloud(CWnd* pParent /*=nullptr*/)
	: CDialogEx(XDlgPropCloud::IDD, pParent)
	, m_idCloud( 0 )
	, m_idxImg( 0 )
	, m_xPos( 0 )
	, m_yPos( 0 )
	, m_lvOpenable( 0 )
	, m_Cost( 0 )
	, m_xPosCost( 0 )
	, m_yPosCost( 0 )
	, m_idName( 0 )
	, m_idsPrecede( _T( "" ) )
	, m_idsItem( _T( "" ) )
	, m_idsArea( _T( "" ) )
{

}

XDlgPropCloud::~XDlgPropCloud()
{
}

void XDlgPropCloud::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT_ID, m_idCloud );
	DDX_Text( pDX, IDC_EDIT_IDXIMG, m_idxImg );
	DDX_Text( pDX, IDC_EDIT_POS_X, m_xPos );
	DDX_Text( pDX, IDC_EDIT_POS_Y, m_yPos );
	DDX_Text( pDX, IDC_EDIT_LV_OPEN, m_lvOpenable );
	DDX_Text( pDX, IDC_EDIT_COST, m_Cost );
	DDX_Text( pDX, IDC_EDIT_POS_X_COST, m_xPosCost );
	DDX_Text( pDX, IDC_EDIT_POS_Y_COST, m_yPosCost );
	DDX_Text( pDX, IDC_EDIT2, m_idName );
	DDX_Text( pDX, IDC_EDIT9, m_idsPrecede );
	DDX_Text( pDX, IDC_EDIT10, m_idsItem );
	DDX_Text( pDX, IDC_EDIT_IDS, m_idsArea );
}


BEGIN_MESSAGE_MAP(XDlgPropCloud, CDialogEx)
END_MESSAGE_MAP()


// XDlgPropCloud 메시지 처리기입니다.


BOOL XDlgPropCloud::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


