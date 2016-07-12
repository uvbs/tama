// DlgEditAbility.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgEditAbility.h"
#include "afxdialogex.h"


// CDlgEditAbility ��ȭ �����Դϴ�.

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


// CDlgEditAbility �޽��� ó�����Դϴ�.


BOOL CDlgEditAbility::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	if( m_idName > 0 )
		m_strName = XTEXT( m_idName );
	if( m_idDesc > 0 )
		m_strDesc = XTEXT( m_idDesc );
	UPDATE_D2C();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
