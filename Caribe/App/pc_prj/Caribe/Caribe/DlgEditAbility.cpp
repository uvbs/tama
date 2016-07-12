// DlgEditAbility.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgEditAbility.h"
#include "afxdialogex.h"


// CDlgEditAbility 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgEditAbility, CDialogEx)

CDlgEditAbility::CDlgEditAbility(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgEditAbility::IDD, pParent)
	, m_strIdentifier( _T( "" ) )
	, m_idsSkill( _T( "" ) )
	, m_strIcon( _T( "" ) )
	, m_strName( _T( "" ) )
	, m_strDesc( _T( "" ) )
	, m_idName( 0 )
	, m_idDesc( 0 )
	, m_lvOpen( 0 )
	, m_maxPoint( 0 )
	, m_x( 0 )
	, m_y( 0 )
	, m_idNode( 0 )
{

}

CDlgEditAbility::~CDlgEditAbility()
{
}

void CDlgEditAbility::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT_IDS, m_strIdentifier );
	DDX_Text( pDX, IDC_EDIT_SKILL, m_idsSkill );
	DDX_Text( pDX, IDC_EDIT_ICON, m_strIcon );
	DDX_Text( pDX, IDC_EDIT_NAME, m_strName );
	DDX_Text( pDX, IDC_EDIT_DESC, m_strDesc );
	DDX_Text( pDX, IDC_EDIT_IDNAME, m_idName );
	DDX_Text( pDX, IDC_EDIT_IDDESC, m_idDesc );
	DDX_Text( pDX, IDC_EDIT_LEVEL, m_lvOpen );
	DDV_MinMaxInt( pDX, m_lvOpen, 1, 100 );
	DDX_Text( pDX, IDC_EDIT_MAX_POINT, m_maxPoint );
	DDV_MinMaxInt( pDX, m_maxPoint, 1, 5 );
	DDX_Text( pDX, IDC_EDIT_X, m_x );
	DDX_Text( pDX, IDC_EDIT_Y, m_y );
	DDX_Control( pDX, IDC_LIST_PARENTS, m_ctrlListParents );
	DDX_Control( pDX, IDC_LIST_CHILDS, m_ctrlListChilds );
	DDX_Text( pDX, IDC_EDIT1, m_idNode );
}


BEGIN_MESSAGE_MAP(CDlgEditAbility, CDialogEx)
END_MESSAGE_MAP()


// CDlgEditAbility 메시지 처리기입니다.


BOOL CDlgEditAbility::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if( m_idName > 0 )
		m_strName = XTEXT( m_idName );
	if( m_idDesc > 0 )
		m_strDesc = XTEXT( m_idDesc );
	UPDATE_D2C();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
