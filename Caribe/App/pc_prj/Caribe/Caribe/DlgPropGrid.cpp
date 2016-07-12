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

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if( !m_wndPropList.Create( WS_VISIBLE | WS_CHILD, rectDummy, this, 1231 ) )
	{
		TRACE0( "속성 표를 만들지 못했습니다. \n" );
		return -1;      // 만들지 못했습니다.
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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty( _T( "모양" ) );

	CMFCPropertyGridProperty *pProp1
		= new CMFCPropertyGridProperty( _T( "3D 모양" ),
		(_variant_t)false,
		_T( "창에 굵은 글꼴이 아닌 글꼴을 지정하고, 컨트롤에 3D 테두리를 지정합니다." ) );
	pGroup1->AddSubItem( pProp1 );
	pProp1->SetData( 123 );

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty( _T( "테두리" ), _T( "Dialog Frame" ), _T( "None, Thin, Resizable 또는 Dialog Frame 중 하나를 지정합니다." ) );
	pProp->AddOption( _T( "None" ) );
	pProp->AddOption( _T( "Thin" ) );
	pProp->AddOption( _T( "Resizable" ) );
	pProp->AddOption( _T( "Dialog Frame" ) );
	pProp->AllowEdit( FALSE );

	pGroup1->AddSubItem( pProp );
	pGroup1->AddSubItem( new CMFCPropertyGridProperty( _T( "캡션" ), (_variant_t)_T( "정보" ), _T( "창의 제목 표시줄에 표시되는 텍스트를 지정합니다." ) ) );

	m_wndPropList.AddProperty( pGroup1 );
	m_pCurrGrid = pGroup1;
}

void CDlgPropGrid::CreatePropType2()
{
	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty( _T( "글꼴" ) );

	LOGFONT lf;
	CFont* font = CFont::FromHandle( (HFONT)GetStockObject( DEFAULT_GUI_FONT ) );
	font->GetLogFont( &lf );

	lstrcpy( lf.lfFaceName, _T( "맑은 고딕" ) );

	pGroup2->AddSubItem( new CMFCPropertyGridFontProperty( _T( "글꼴" ), lf, CF_EFFECTS | CF_SCREENFONTS, _T( "창의 기본 글꼴을 지정합니다." ) ) );
	pGroup2->AddSubItem( new CMFCPropertyGridProperty( _T( "시스템 글꼴을 사용합니다." ), (_variant_t)true, _T( "창에서 MS Shell Dlg 글꼴을 사용하도록 지정합니다." ) ) );

	m_wndPropList.AddProperty( pGroup2 );
	m_pCurrGrid = pGroup2;
}

void CDlgPropGrid::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize( nType, cx, cy );

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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
		// 메시지 처리
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