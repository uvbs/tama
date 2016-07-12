#include "stdafx.h"
#include "DlgPropGrid.h"


BEGIN_MESSAGE_MAP( CDlgPropGrid, CDialog )
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()


CDlgPropGrid* CDlgPropGrid::s_pInstance = nullptr;

CDlgPropGrid::CDlgPropGrid()
{
	s_pInstance = this;
}


CDlgPropGrid::~CDlgPropGrid()
{
	s_pInstance = nullptr;
}

int CDlgPropGrid::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CDialog::OnCreate( lpCreateStruct ) == -1 )
		return -1;
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	if( !m_wndPropList.Create( WS_VISIBLE | WS_CHILD, rectDummy, this, 1231 ) )
	{
		TRACE0( "�Ӽ� ǥ�� ������ ���߽��ϴ�. \n" );
		return -1;      // ������ ���߽��ϴ�.
	}

	InitPropList();

	CRect rectClient;
	GetClientRect( rectClient );
	m_wndPropList.SetWindowPos( NULL, rectClient.left, rectClient.top/* + m_nComboHeight + cyTlb*/, rectClient.Width(), rectClient.Height() /*- ( m_nComboHeight + cyTlb )*/, SWP_NOACTIVATE | SWP_NOZORDER );
// 	m_wndPropList.ShowWindow( SW_SHOW );
// 	m_wndPropList.UpdateWindow();
// 	m_wndPropList.SetFocus();
	return 0;
}

BOOL CDlgPropGrid::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgPropGrid::InitPropList()
{
	//
	m_wndPropList.EnableHeaderCtrl( FALSE );
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
}

void CDlgPropGrid::UpdateData( int type )
{
	if( m_pCurrGrid )
		m_wndPropList.DeleteProperty( m_pCurrGrid );
	switch( type )
	{
	case 0:
		CreatePropType1();
		break;
	case 1:
		CreatePropType2();
		break;
	default:
		break;
	}
}

void CDlgPropGrid::CreatePropType1()
{
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty( _T( "���" ) );

	CMFCPropertyGridProperty *pProp1
		= new CMFCPropertyGridProperty( _T( "3D ���" ),
		(_variant_t)false,
		_T( "â�� ���� �۲��� �ƴ� �۲��� �����ϰ�, ��Ʈ�ѿ� 3D �׵θ��� �����մϴ�." ) );
	pGroup1->AddSubItem( pProp1 );
	pProp1->SetData( 123 );

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty( _T( "�׵θ�" ), _T( "Dialog Frame" ), _T( "None, Thin, Resizable �Ǵ� Dialog Frame �� �ϳ��� �����մϴ�." ) );
	pProp->AddOption( _T( "None" ) );
	pProp->AddOption( _T( "Thin" ) );
	pProp->AddOption( _T( "Resizable" ) );
	pProp->AddOption( _T( "Dialog Frame" ) );
	pProp->AllowEdit( FALSE );

	pGroup1->AddSubItem( pProp );
	pGroup1->AddSubItem( new CMFCPropertyGridProperty( _T( "ĸ��" ), (_variant_t)_T( "����" ), _T( "â�� ���� ǥ���ٿ� ǥ�õǴ� �ؽ�Ʈ�� �����մϴ�." ) ) );

	m_wndPropList.AddProperty( pGroup1 );
	m_pCurrGrid = pGroup1;
}

void CDlgPropGrid::CreatePropType2()
{
	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty( _T( "�۲�" ) );

	LOGFONT lf;
	CFont* font = CFont::FromHandle( (HFONT)GetStockObject( DEFAULT_GUI_FONT ) );
	font->GetLogFont( &lf );

	lstrcpy( lf.lfFaceName, _T( "���� ���" ) );

	pGroup2->AddSubItem( new CMFCPropertyGridFontProperty( _T( "�۲�" ), lf, CF_EFFECTS | CF_SCREENFONTS, _T( "â�� �⺻ �۲��� �����մϴ�." ) ) );
	pGroup2->AddSubItem( new CMFCPropertyGridProperty( _T( "�ý��� �۲��� ����մϴ�." ), (_variant_t)true, _T( "â���� MS Shell Dlg �۲��� ����ϵ��� �����մϴ�." ) ) );

	m_wndPropList.AddProperty( pGroup2 );
	m_pCurrGrid = pGroup2;
}

void CDlgPropGrid::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize( nType, cx, cy );

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
// 	CRect rectClient;
// 	GetClientRect( rectClient );
// 	m_wndPropList.SetWindowPos( NULL, rectClient.left, rectClient.top/* + m_nComboHeight + cyTlb*/, rectClient.Width(), rectClient.Height() /*- ( m_nComboHeight + cyTlb )*/, SWP_NOACTIVATE | SWP_NOZORDER );
}

LRESULT CDlgPropGrid::OnPropertyChanged( WPARAM wparam, LPARAM lparam )
{
#ifdef _XPROP_GRID
	CMFCPropertyGridProperty *pProp = (CMFCPropertyGridProperty*)lparam;
	if( !pProp ) return 0;
	static int cnt = 0; 

	switch( pProp->GetData() )
	{
	case 123: {
		// �޽��� ó��
//		CMFCPropertyGridProperty *pPropCtrl = m_wndPropList.GetProperty(0);
		COleVariant rVariant = pProp->GetValue();
		VARIANT v = rVariant.Detach();
		VARTYPE vt = v.vt;
		assert( vt == VT_BOOL );
		if( (bool)v.boolVal )
			TRACE("%d:%d\n", cnt++, 1);
		else
			TRACE("%d:%d\n", cnt++, 0);
	} break;
	}
#endif // _XPROP_GRID
	return 1;
}