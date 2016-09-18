#include "stdafx.h"
#ifndef SHARED_HANDLERS
#include "Caribe.h"
#endif

#include "CaribeDoc.h"
#include "CaribeView.h"
#include "MainFrm.h"
#include "XFramework/client/XClientMain.h"
#include "client/XAppMain.h"
#include "XSceneWorld.h"
#include "XSceneTech.h"
#include "XSceneBattle.h"
#include "XDlgPropCloud.h"
#include "DlgEditAbility.h"
#include "DlgSpot.h"
#include "XGameWnd.h"
#include "XQuestProp.h"
#include "XTool.h"
#include "XBattleField.h"
#include "XWndBattleField.h"
#include "XGame.h"
#include "DlgSubCastle.h"
#include "DlgSubJewel.h"
#include "DlgSubSulfur.h"
#include "DlgSubMandrake.h"
#include "DlgSubNpc.h"
#include "DlgSubCamp.h"
#include "DlgSubVisit.h"
#include "DlgSubCash.h"
#include "DlgSubCommon.h"
#include "DlgNewBgObj.h"
#include "XPropBgObj.h"
#include "DlgSelectConnect.h"
#include "XWndObj.h"
#include "XImageMng.h"

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

CaribeView* s_pView = nullptr;
CaribeView* GetView() { return s_pView; }
int CaribeView::s_idxLogin = -1;
// CaribeView

// ON_COMMAND( IDM_DEL_NODE, &CaribeView::OnDelNode )
// ON_UPDATE_COMMAND_UI( IDM_DEL_NODE, &CaribeView::OnUpdateDelNode )

IMPLEMENT_DYNCREATE(CaribeView, CView)

BEGIN_MESSAGE_MAP(CaribeView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND( ID_NEW_CLOUD_GROUP, &CaribeView::OnNewCloudGroup )
	ON_COMMAND( IDM_CREATE_ABIL, &CaribeView::OnCreateAbil )
	ON_COMMAND( IDM_EDIT_ABIL, &CaribeView::OnEditAbil )
	ON_UPDATE_COMMAND_UI( IDM_EDIT_ABIL, &CaribeView::OnUpdateEditAbil )
	ON_COMMAND( IDM_TOOL_UNDO, &CaribeView::OnToolUndo )
	ON_COMMAND( IDM_CREATE_SPOT, &CaribeView::OnCreateSpot )
	ON_UPDATE_COMMAND_UI( IDM_EDIT_SPOT, &CaribeView::OnUpdateEditSpot )
	ON_COMMAND( IDM_CREATE_SPOT_CASTLE, &CaribeView::OnCreateSpotCastle )
	ON_COMMAND( IDM_CREATE_SPOT_JEWEL, &CaribeView::OnCreateSpotJewel )
	ON_COMMAND( IDM_CREATE_SPOT_SULFUR, &CaribeView::OnCreateSpotSulfur )
	ON_COMMAND( IDM_CREATE_SPOT_MANDRAKE, &CaribeView::OnCreateSpotMandrake )
	ON_COMMAND( IDM_CREATE_SPOT_NPC, &CaribeView::OnCreateSpotNpc )
	ON_COMMAND( IDM_CREATE_SPOT_CAMPAIGN, &CaribeView::OnCreateSpotCampaign )
	ON_COMMAND( IDM_CREATE_SPOT_VISIT, &CaribeView::OnCreateSpotVisit )
	ON_COMMAND( IDM_EDIT_SPOT, &CaribeView::OnEditSpot )
	ON_COMMAND( IDM_CREATE_SPEARMAN, &CaribeView::OnCreateSpearman )
	ON_COMMAND( IDM_CREATE_ARCHER, &CaribeView::OnCreateArcher )
	ON_COMMAND( IDM_CREATE_PALADIN, &CaribeView::OnCreatePaladin )
	ON_COMMAND( IDM_CREATE_MINOTAUR, &CaribeView::OnCreateMinotaur )
	ON_COMMAND( IDM_CREATE_CYCLOPS, &CaribeView::OnCreateCyclops )
	ON_COMMAND( IDM_CREATE_LYCAN, &CaribeView::OnCreateLycan )
	ON_COMMAND( IDM_CREATE_GOLEM, &CaribeView::OnCreateGolem )
	ON_COMMAND( IDM_CREATE_TREANT, &CaribeView::OnCreateTreant )
	ON_COMMAND( IDM_CREATE_FALLEN_ANGEL, &CaribeView::OnCreateFallenAngel )
	ON_COMMAND( IDM_CREATE_SPOT_CASH, &CaribeView::OnCreateSpotCash )
	ON_COMMAND( IDM_CREATE_SPOT_COMMON, &CaribeView::OnCreateSpotCommon )
	ON_COMMAND( IDM_COPY_SPOT, &CaribeView::OnCopySpot )
	ON_COMMAND( IDM_PASTE_SPOT, &CaribeView::OnPasteSpot )
	ON_COMMAND( IDM_NEW_CREATE_BGOBJ, &CaribeView::OnCreateBgObj )
	ON_COMMAND( IDM_EDIT_BGOBJ, &CaribeView::OnEditBgObj )
	ON_COMMAND( IDM_DEL_BGOBJ, &CaribeView::OnDelBgObj )
	ON_COMMAND( IDM_VIEW_BGOBJ_BOUNDBOX, &CaribeView::OnViewBgObjBoundBox )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_BGOBJ_BOUNDBOX, &CaribeView::OnUpdateViewBgObjBoundBox )
	ON_COMMAND( IDM_DEL_OUT_LINK, &CaribeView::OnDelOutLink )
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()
//	ON_WM_DESTROY()

// CaribeView ����/�Ҹ�

CaribeView::CaribeView()
//	: XE::GetMain()->m_bCtrl(false)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	s_pView = this;
	m_Scale = 1.0f;
}

CaribeView::~CaribeView()
{
	s_pView = nullptr;
	TRACE("~CaribeView\n");
}

BOOL CaribeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}



void CaribeView::OnInitialUpdate()
{
#ifdef _VER_DX
	CView::OnInitialUpdate();

	CRect rect;
	GetClientRect( &rect );
	//	MAP->SetViewSize( rect.Width(), rect.Height() );
//	DragAcceptFiles( TRUE );
	XD3DView::CreateView( GetSafeHwnd(), MAINFRAME->GetDevice(), MAINFRAME->m_d3dpp.BackBufferFormat );
	CSize size(PHY_WIDTH, PHY_HEIGHT);
	//	CSize size(0, 0);
//	SetScrollSizes( MM_TEXT, size );	
	GetClientRect( &rect );
	SetWindowPos( nullptr, 0, 0, PHY_WIDTH, PHY_HEIGHT, SWP_NOZORDER | SWP_NOMOVE );
	GetClientRect( &rect );
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetMainWnd();
//	pMainFrm->RecalcLayout();
//	ResizeParentToFit(FALSE);
//	ResizeParentToFit();
	GetParentFrame()->RecalcLayout();
	GetClientRect( &rect );
//	SetScaleToFitSize(rect.Size());
	CFrameWnd *pFrm = GetParentFrame();

	appDelegate.Create( XE::DEVICE_WINDOWS, PHY_WIDTH, PHY_HEIGHT );
#endif // _VER_DX
#ifndef _XSINGLE
	s_idxLogin = DoDlgSelectConnect();
	XBREAK( s_idxLogin == -1 );
#endif // not _XSINGLE
#ifdef _VER_OPENGL
  __super::OnInitialUpdate();
	auto sizePhy = GRAPHICS_GL->GetPhyScreenSize();
	appDelegate.Create( XE::DEVICE_WINDOWS, sizePhy.w, sizePhy.h );
#endif // _VER_OPENGL
}

int CaribeView::DoDlgSelectConnect()
{
	int idxLogin = -1;
	CDlgSelectConnect dlg;
	if( dlg.DoModal() == IDOK ) {
		if( dlg.m_idxSelect >= 0 )
			idxLogin = dlg.m_idxSelect;
	}
	return idxLogin;
}
// CaribeView �޽��� ó����
void CaribeView::OnSize(UINT nType, int cx, int cy)	
{
	__super::OnSize( nType, cx, cy );
#ifdef _VER_DX
//	CView::OnSize(nType, cx, cy);
	// TODO: ???? ????? o???? ??? ???????.
	if( D3DDEVICE ) {
		GRAPHICS->SetPhyScreenSize( cx, cy );
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	}
#endif // _VER_DX

}


// CaribeView �׸���
#ifdef _VER_DX
void CaribeView::Draw( void )
{
	XPROF_OBJ_AUTO();
	XD3DView::Draw( D3DDEVICE, XApp::sGetMain(), this );
	if( XApp::sGetMain()->GetbDestroy() )
	{
		ASSERT(AfxGetMainWnd() != nullptr );
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
	}
}
#endif // _VER_DX

void CaribeView::OnDraw(CDC* /*pDC*/)
{
#ifdef _VER_DX
	CaribeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	Draw();
#endif // _VER_DX
#ifdef _VER_OPENGL
	__super::OnDraw( nullptr );
#endif // _VER_OPENGL
}
// CaribeView ����

#ifdef _DEBUG
void CaribeView::AssertValid() const
{
	CView::AssertValid();
}

void CaribeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CaribeDoc* CaribeView::GetDocument() const // �����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CaribeDoc)));
	return (CaribeDoc*)m_pDocument;
}
#endif //_DEBUG


void CaribeView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if( nFlags & MK_CONTROL )
		XE::GetMain()->m_bCtrl = true;
	else
		XE::GetMain()->m_bCtrl = false;
	XE::GetMain()->m_bAlt = ( nFlags & MK_SHIFT) != 0;
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	m_vMouse = point;
	if( XApp::sGetMain() )
		XApp::sGetMain()->OnTouchEvent( XClientMain::xTE_MOVE, 1, (float)point.x, (float)point.y );
	__super::OnMouseMove(nFlags, point);
}

void CaribeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	if( nFlags & MK_CONTROL )
		XE::GetMain()->m_bCtrl = true;
	else
		XE::GetMain()->m_bCtrl = false;
	XE::GetMain()->m_bAlt = (nFlags & MK_SHIFT) != 0;
	
	if( XApp::sGetMain() )
		XApp::sGetMain()->OnTouchEvent( XClientMain::xTE_DOWN, 1, (float)point.x, (float)point.y );

	__super::OnLButtonDown(nFlags, point);
}

void CaribeView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if( nFlags & MK_CONTROL )
		XE::GetMain()->m_bCtrl = true;
	else
		XE::GetMain()->m_bCtrl = false;
	XE::GetMain()->m_bAlt = ( nFlags & MK_SHIFT) != 0;
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	if( XApp::sGetMain() )
		XApp::sGetMain()->OnTouchEvent( XClientMain::xTE_UP, 1, (float)point.x, (float)point.y );
	__super::OnLButtonUp(nFlags, point);
}


BOOL CaribeView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( nFlags & MK_CONTROL )
		XE::GetMain()->m_bCtrl = true;
	else
		XE::GetMain()->m_bCtrl = false;
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if( zDelta < 0 )	// �Ʒ��� ����(����)
	{
	} else
	if( zDelta > 0 )
	{
	}
	if( XAPP )
		XAPP->OnTouchEvent( XClientMain::xTE_ZOOM, 1, m_vMouse.x, m_vMouse.y, 1.0f + zDelta / 1000.f);
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}


void CaribeView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if( nFlags & MK_CONTROL )
		XE::GetMain()->m_bCtrl = true;
	else
		XE::GetMain()->m_bCtrl = false;
	m_vRClick = INPUTMNG->GetMousePos();
	CONSOLE("m_vRClick down:%d,%d-%d,%d", (int)m_vRClick.x, (int)m_vRClick.y, point.x, point.y );
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	if( XAPP )
		XAPP->OnTouchEvent( XClientMain::xTE_RDOWN, 1, (float)point.x, (float)point.y );

	__super::OnLButtonDown(nFlags, point);
}
void CaribeView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CONSOLE( "m_vRClick up:%d,%d-%d,%d", (int)m_vRClick.x, (int)m_vRClick.y, point.x, point.y );
	ReleaseCapture();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if( nFlags & MK_CONTROL )
		XE::GetMain()->m_bCtrl = true;
	else
		XE::GetMain()->m_bCtrl = false;
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	// �׳� ��Ʈ�Ѵ����� ��Ŭ���� ������ ���ؽ�Ʈ �޴���.
// 	m_vRClick = INPUTMNG->GetMousePos();
// 	CONSOLE( "m_vRClick up:%d,%d-%d,%d", (int)m_vRClick.x, (int)m_vRClick.y, point.x, point.y );
#ifdef _xIN_TOOL
	DWORD idMenu = 0;
	if( SCENE_WORLD && XBaseTool::sIsToolCloudMode() && XE::GetMain()->m_bCtrl ) {
		idMenu = IDR_MENU_CLOUD;
	} else
	if( SCENE_WORLD && XBaseTool::sIsToolSpotMode() ) {
		idMenu = IDR_MENU_SPOT;
	} else
	if( SCENE_WORLD && XBaseTool::sIsToolBgObjMode() && XE::GetMain()->m_bCtrl ) {
		if( SCENE_WORLD ) {
			auto pWnd = SCENE_WORLD->GetpLayerBgObjUnder();
			if( pWnd && !pWnd->IsObjDragging() )
				idMenu = IDR_MENU_BGOBJ;
		}
	} else
	if( SCENE_TECH && XBaseTool::sIsToolTechMode() ) {
		idMenu = IDR_MENU_ABIL;
	} else
	if( SCENE_BATTLE ) {
		idMenu = IDR_MENU_BATTLE;
	}
	if( idMenu ) {
		CMenu		menu, *popup;
		::POINT	p;
		menu.LoadMenu( idMenu );
		popup = menu.GetSubMenu( 0 );
		GetCursorPos( &p );
		popup->TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, p.x, p.y, this );
		menu.DestroyMenu();
	}
#endif // _xIN_TOOL
	if( XAPP )
		XAPP->OnTouchEvent( XClientMain::xTE_RUP, 1, (float)point.x, (float)point.y );
	__super::OnLButtonUp(nFlags, point);
}

/**
 @brief �� �����׷� ����.
*/
void CaribeView::OnNewCloudGroup()
{
#ifdef _xIN_TOOL
	static _tstring s_strIdsLast;
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
//	if( SCENE_WORLD == nullptr || XAPP->IsToolCloudMode() == FALSE )
	// 월드씬이 없거나 구름툴모드가 아니면 취소시킴.
	if( !SCENE_WORLD || XBaseTool::sIsToolCloudMode() == false )
		return;
	auto pProp = new XPropCloud::xCloud;
	pProp->idCloud = XPropCloud::sGenerateID();
	if( pProp->strIdentifier.empty() ) {
		if( s_strIdsLast.empty() )
			pProp->strIdentifier = XFORMAT( "area.%d", pProp->idCloud );
		else
			pProp->strIdentifier = XFORMAT( "%s.", s_strIdsLast.c_str() );
	}
	pProp->lvArea = 1;
	pProp->cost = 1000;
	pProp->vAdjust.Set(0);

	XDlgPropCloud dlg;
	dlg.m_idCloud = pProp->idCloud;
	dlg.m_idsArea = pProp->strIdentifier.c_str();
	dlg.m_idName = pProp->idName;
// 	dlg.m_xPos = (int)pProp->vwPos.x;
// 	dlg.m_yPos = (int)pProp->vwPos.y;
	dlg.m_lvOpenable = pProp->lvArea;
	dlg.m_Cost = pProp->cost;
	dlg.m_xPosCost = (int)pProp->vAdjust.x;
	dlg.m_yPosCost = (int)pProp->vAdjust.y;
	if( s_strIdsLast.empty() )
		dlg.m_idsPrecede = pProp->idsPrecedeArea.c_str();
	else
		dlg.m_idsPrecede = s_strIdsLast.c_str();
	dlg.m_idsItem = pProp->idsItem.c_str();
	if( dlg.DoModal() == IDOK )
	{
		pProp->idCloud = dlg.m_idCloud;
		pProp->strIdentifier = dlg.m_idsArea;
		if( pProp->strIdentifier.empty() )
			pProp->strIdentifier = XFORMAT( "area.%d", pProp->idCloud );
		s_strIdsLast = pProp->strIdentifier;
		pProp->idName = dlg.m_idName;
// 		pProp->vwPos.x = (float)dlg.m_xPos;
// 		pProp->vwPos.y = (float)dlg.m_yPos;
		pProp->lvArea = dlg.m_lvOpenable;
		pProp->cost = dlg.m_Cost;
		pProp->vAdjust.x = (float)dlg.m_xPosCost;
		pProp->vAdjust.y = (float)dlg.m_yPosCost;
		pProp->idsPrecedeArea = dlg.m_idsPrecede;
		// 선행지역이 없는곳이면 경고.
		if( !pProp->idsPrecedeArea.empty() ) {
			auto pPrecede = PROP_CLOUD->GetpProp( pProp->idsPrecedeArea );
			if( pPrecede == nullptr ) {
				AfxMessageBox( XFORMAT("%s라는 지역은 없습니다.", pProp->idsPrecedeArea.c_str()) );
				pProp->idsPrecedeArea.clear();
			}
		}
		pProp->idsItem = dlg.m_idsItem;
		if( !pProp->idsItem.empty() ) {
			auto pPropItem = PROP_ITEM->GetpProp( pProp->idsItem );
			if( pProp == nullptr ) {
				AfxMessageBox( XFORMAT( "%s라는 아이템은 없습니다.", pProp->idsItem.c_str() ) );
				pProp->idsItem.clear();
			}
		}
		pProp->AddIdx( SCENE_WORLD->GetidxClickedHexa() );
		PROP_CLOUD->AddCloud( pProp );
		PROP_CLOUD->UpdateAreaIds( pProp );
		SCENE_WORLD->EditedCloud();
		SCENE_WORLD->UpdateCloudSpotList( true );
	} else
	{
		SAFE_DELETE( pProp );
	}
#endif
}

/**
 @brief �� Ư�� ����
*/
void CaribeView::OnCreateAbil()
{
#ifdef _xIN_TOOL
	if( !SCENE_TECH || XBaseTool::sIsToolTechMode() == false )
		return;
// 	if( SCENE_TECH == nullptr || XAPP->m_ToolMode != xTM_TECH )
// 		return;
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	CDlgEditAbility dlg;
	dlg.m_lvOpen = 1;
	dlg.m_maxPoint = 5;
	XE::VEC2 vPos = SCENE_TECH->GetPosMouseByGrid();
	dlg.m_x = (int)vPos.x;
	dlg.m_y = (int)vPos.y;
	if( dlg.DoModal() == IDOK )
	{
		XPropTech::xNodeAbil *pNodeAbil = new XPropTech::xNodeAbil;
		pNodeAbil->idNode = XPropTech::sGenerateID();
		UpdateNodeFromDlg( pNodeAbil, dlg );
		SCENE_TECH->DoCreateNewNode( pNodeAbil );
		// text_ko ������.
		//
		SCENE_TECH->UpdateAutoSave();

	}
#endif // _xIN_TOOL
}

void CaribeView::UpdateNodeFromDlg( XPropTech::xNodeAbil *pNodeAbil, CDlgEditAbility& dlg )
{
	if( dlg.m_idNode )
		pNodeAbil->idNode = dlg.m_idNode;
	pNodeAbil->strSkill = dlg.m_idsSkill;
	pNodeAbil->strIcon = dlg.m_strIcon;
	LPCTSTR szDummyIcon = _T( "icon_dummy.png" );
	if( pNodeAbil->strIcon.empty() )
		pNodeAbil->strIcon = szDummyIcon;
	_tstring strIconExt = XE::GetFileExt( pNodeAbil->strIcon.c_str() );
	if( strIconExt.empty() )
		pNodeAbil->strIcon += _T( ".png" );
	BOOL bReload = TRUE;
	if( pNodeAbil->psfcIcon )
	{
		_tstring strFile = XE::GetFileName( pNodeAbil->psfcIcon->GetstrRes().c_str() );
		if( pNodeAbil->strIcon == strFile )
			bReload = FALSE;
	}
	if( bReload )
	{
		SAFE_RELEASE2( IMAGE_MNG, pNodeAbil->psfcIcon );
		pNodeAbil->psfcIcon = IMAGE_MNG->Load( XE::MakePath( DIR_IMG, pNodeAbil->strIcon ) );
		if( pNodeAbil->psfcIcon == nullptr )
			pNodeAbil->psfcIcon = IMAGE_MNG->Load( XE::MakePath( DIR_IMG, szDummyIcon ) );

	}
	// Ư���̸��� �Է��ߴٸ� �ڵ����� text_ko�� �߰���Ŵ
	if( dlg.m_strName.IsEmpty() == FALSE )
	{
		ID idText = TEXT_TBL->AddTextAutoID( dlg.m_strName, 60000, 10000 );
		pNodeAbil->idName = idText;
	}
	if( dlg.m_strDesc.IsEmpty() == FALSE )
	{
		ID idText = TEXT_TBL->AddTextAutoID( dlg.m_strDesc, 60000, 10000 );
		pNodeAbil->idDesc = idText;
	}
	pNodeAbil->tier = dlg.m_lvOpen;
	pNodeAbil->point = 0;
	pNodeAbil->maxPoint = dlg.m_maxPoint;
	pNodeAbil->vPos.Set( dlg.m_x, dlg.m_y );
}

void CaribeView::OnEditAbil()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( !SCENE_TECH || XBaseTool::sIsToolTechMode() == false )
		return;
// 	if( SCENE_TECH == nullptr || XAPP->m_ToolMode != xTM_TECH )
// 		return;
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	XPropTech::xNodeAbil *pNodeAbil = SCENE_TECH->m_pSelNodeT;
	if( XBREAK( pNodeAbil == nullptr ) )
		return;
	CDlgEditAbility dlg;
	dlg.m_idNode = pNodeAbil->idNode;
//	dlg.m_strIdentifier = C2SZ(pNodeAbil->strIdentifier.c_str());
	dlg.m_idsSkill = pNodeAbil->strSkill.c_str();
	dlg.m_strIcon = pNodeAbil->strIcon.c_str();
	dlg.m_idName = pNodeAbil->idName;
	dlg.m_idDesc = pNodeAbil->idDesc;
	dlg.m_lvOpen = pNodeAbil->tier;
	dlg.m_maxPoint = pNodeAbil->maxPoint;
	dlg.m_x = (int)pNodeAbil->vPos.x;
	dlg.m_y = (int)pNodeAbil->vPos.y;
	if( dlg.DoModal() == IDOK )
	{
		UpdateNodeFromDlg( pNodeAbil, dlg );
		//
		SCENE_TECH->UpdateAutoSave();
	}
#endif // _xIN_TOOL
}


void CaribeView::OnUpdateEditAbil( CCmdUI *pCmdUI )
{
	// TODO: ���⿡ ���� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( !SCENE_TECH || XBaseTool::sIsToolTechMode() == false ||
		(SCENE_TECH && SCENE_TECH->m_pSelNodeT == nullptr) )	{
		pCmdUI->Enable( FALSE );
		return;
	}
#endif // _xIN_TOOL
}


// void CaribeView::OnDelNode()
// {
// 	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
// #ifdef _xIN_TOOL
// // 	if( SCENE_TECH == nullptr || XAPP->m_ToolMode != xTM_TECH ||
// // 		( SCENE_TECH && SCENE_TECH->m_pSelNodeT == nullptr ) )
// // 	{
// // 		return;
// // 	}
// 	if( XBaseTool::sIsToolTechMode() && //XAPP->m_ToolMode == xTM_TECH &&
// 		(SCENE_TECH && SCENE_TECH->m_pSelNodeT ) )
// 		SCENE_TECH->DelNodeSelected();
// 	else
// 	if( XBaseTool::sIsToolSpotMode() && // XAPP->m_ToolMode == xTM_SPOT &&
// 		SCENE_WORLD && XWndSpotForTool::s_pSelected )
// 		SCENE_WORLD->DelSpotT();
// #endif // _xIN_TOOL
// }


// void CaribeView::OnUpdateDelNode( CCmdUI *pCmdUI )
// {
// 	// TODO: ���⿡ ���� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
// #ifdef _xIN_TOOL
// // 	if( SCENE_TECH == nullptr || XAPP->m_ToolMode != xTM_TECH ||
// // 		( SCENE_TECH && SCENE_TECH->m_pSelNodeT == nullptr ) )
// // 	{
// // 		pCmdUI->Enable( FALSE );
// // 		return;
// // 	}
// 	pCmdUI->Enable( TRUE );
// #endif // _xIN_TOOL
// }
/**
 @brief 노드에서 나가는 링크를 삭제.
*/
void CaribeView::OnDelOutLink()
{
#ifdef _xIN_TOOL
//	if( XBaseTool::sIsToolTechMode() && (SCENE_TECH && SCENE_TECH->m_pSelNodeT ) )
//		SCENE_TECH->DelNodeSelected();
	SCENE_TECH->DelOutLinkBySelectedNode();

#endif // _xIN_TOOL
}

void CaribeView::OnToolUndo()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( SCENE_TECH && XBaseTool::sIsToolTechMode() )
		SCENE_TECH->Undo();
	else
	if( SCENE_WORLD && XBaseTool::sIsToolSpotMode() )	{
		SCENE_WORLD->Undo();
	}
#endif // _xIN_TOOL
}


void CaribeView::OnCreateSpot()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
}


bool CaribeView::DoNewSpotDlg( XGAME::xtSpot type )
{
#ifdef _xIN_TOOL
	if( SCENE_WORLD == nullptr || !XBaseTool::sIsToolSpotMode() )
		return false;

	// �� ���̸��鶧�� ���콺 ������ ������ ������ �Ѵ�.
	if( XWndSpotForTool::s_pMouseOver )
		return false;

	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	auto pNewSpot = PROP_WORLD->CreateSpot( type );
 	CDlgSpot dlg;
	DoPreInputSpot( type, nullptr, dlg );
 	dlg.m_Type = type;
	dlg.m_strIds = pNewSpot->strIdentifier.c_str();
	dlg.m_idSpot = pNewSpot->idSpot;
	XE::VEC2 vw = SCENE_WORLD->GetvMouseWorld();
	dlg.m_x = (int)vw.x;
	dlg.m_y = (int)vw.y;
	if( dlg.DoModal() == IDOK )
	{
		pNewSpot->strIdentifier = dlg.m_strIds;
		pNewSpot->strWhen = dlg.m_strWhen;
		pNewSpot->strWhenDel = dlg.m_strWhenDel;
#ifdef _DEV
		pNewSpot->strCode = dlg.m_strCode;
#endif // _DEV
		pNewSpot->idWhen = dlg.m_idWhen;
		pNewSpot->idWhenDel = dlg.m_idWhenDel;
		pNewSpot->vWorld.Set( dlg.m_x, dlg.m_y );
		pNewSpot->idCode = dlg.m_idCode;
		if( dlg.m_strName.IsEmpty() == false )
		{
			ID idText = TEXT_TBL->AddTextAutoID( dlg.m_strName, 15000, 10000 );
			pNewSpot->idName = idText;
		}
		pNewSpot->strSpr = dlg.m_strSpr;
		PROP_WORLD->AddSpot( pNewSpot );
		SCENE_WORLD->UpdateSpotForTool();
		SCENE_WORLD->UpdateCloudSpotList( true );
		pNewSpot->idArea = PROP_CLOUD->GetidAreaHaveSpot( pNewSpot->idSpot );
		DoAfterInputSpot( dlg, pNewSpot );
		// text_ko ������.
		if( pNewSpot->idName )
			TEXT_TBL->Save( _T( "text_ko.txt" ) );
		//
		SCENE_WORLD->UpdateAutoSave();

	} else
	{
		SAFE_DELETE( pNewSpot );
	}
#endif // _xIN_TOOL
	return true;
}

bool CaribeView::DoEditSpotDlg( XPropWorld::xBASESPOT *pBaseSpot )
{
#ifdef _xIN_TOOL
	if( SCENE_WORLD == nullptr || !XBaseTool::sIsToolSpotMode() )
		return false;

// 	// �� ���̸��鶧�� ���콺 ������ ������ ������ �Ѵ�.
// 	if( XWndSpotForTool::s_pMouseOver == nullptr )
// 		return false;

	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	_tstring strOldName;
	if( pBaseSpot->idName )
		strOldName = XTEXT( pBaseSpot->idName );
	CDlgSpot dlg;
	dlg.m_Type = pBaseSpot->type;
	dlg.m_strIds = pBaseSpot->strIdentifier.c_str();
	dlg.m_idSpot = pBaseSpot->idSpot;
	dlg.m_x = (int)pBaseSpot->vWorld.x;
	dlg.m_y = (int)pBaseSpot->vWorld.y;
	dlg.m_strWhen = pBaseSpot->strWhen.c_str();
	dlg.m_strWhenDel = pBaseSpot->strWhenDel.c_str();
#ifdef _DEV
	dlg.m_strCode = pBaseSpot->strCode.c_str();
#endif // _DEV
	dlg.m_idWhen = pBaseSpot->idWhen;
	dlg.m_idWhenDel = pBaseSpot->idWhenDel;
	dlg.m_idCode = pBaseSpot->idCode;
	if( pBaseSpot->idName )
		dlg.m_strName = XTEXT(pBaseSpot->idName);
	dlg.m_idName = pBaseSpot->idName;
	dlg.m_idArea = pBaseSpot->idArea;
	dlg.m_strSpr = pBaseSpot->strSpr.c_str();
	DoPreInputSpot( pBaseSpot->GetType(), pBaseSpot, dlg );
	if( dlg.DoModal() == IDOK )
	{
		pBaseSpot->strIdentifier = dlg.m_strIds;
		pBaseSpot->strWhen = dlg.m_strWhen;
		pBaseSpot->strWhenDel = dlg.m_strWhenDel;
#ifdef _DEV
		pBaseSpot->strCode = dlg.m_strCode;
#endif // _DEV
		if( dlg.m_strWhen.IsEmpty() == false )
		{
			std::string strTemp = SZ2C( (LPCTSTR)dlg.m_strWhen );
			pBaseSpot->idWhen = XQuestProp::sGetIdsToId( strTemp );
		}
		if( dlg.m_strWhenDel.IsEmpty() == false )
		{
			std::string strTemp = SZ2C( (LPCTSTR)dlg.m_strWhenDel );
			pBaseSpot->idWhenDel = XQuestProp::sGetIdsToId( strTemp );
		}
		pBaseSpot->vWorld.Set( dlg.m_x, dlg.m_y );
		if( dlg.m_strCode.IsEmpty() == false )
		{
			std::string strTemp = SZ2C( (LPCTSTR)dlg.m_strCode );
			pBaseSpot->idCode = XQuestProp::sGetIdsToId( strTemp );
		}
		if( FALSE == XE::IsSame( strOldName.c_str(), (LPCTSTR)dlg.m_strName ) )
		{
			ID idText = TEXT_TBL->AddTextAutoID( dlg.m_strName, 15000, 10000 );
			pBaseSpot->idName = idText;
			TEXT_TBL->Save( _T( "text_ko.txt" ) );
		}
		pBaseSpot->idArea = dlg.m_idArea;
		pBaseSpot->strSpr = dlg.m_strSpr;
		DoAfterInputSpot( dlg, pBaseSpot );
		//
		SCENE_WORLD->UpdateAutoSave();

	}
#endif // _xIN_TOOL
	return true;
} // DoEditSpotDlg

void CaribeView::OnCreateSpotCastle()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( XWndSpotForTool::s_pMouseOver == nullptr )
		DoNewSpotDlg( XGAME::xSPOT_CASTLE );
#endif // _xIN_TOOL
}


void CaribeView::OnCreateSpotJewel()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( XWndSpotForTool::s_pMouseOver == nullptr )
		DoNewSpotDlg( XGAME::xSPOT_JEWEL );
#endif // _xIN_TOOL
}


void CaribeView::OnCreateSpotSulfur()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( XWndSpotForTool::s_pMouseOver == nullptr )
		DoNewSpotDlg( XGAME::xSPOT_SULFUR );
#endif // _xIN_TOOL
}


void CaribeView::OnCreateSpotMandrake()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( XWndSpotForTool::s_pMouseOver == nullptr )
		DoNewSpotDlg( XGAME::xSPOT_MANDRAKE );
#endif // _xIN_TOOL
}


void CaribeView::OnCreateSpotNpc()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( XWndSpotForTool::s_pMouseOver == nullptr )
		DoNewSpotDlg( XGAME::xSPOT_NPC );
#endif // _xIN_TOOL
}


void CaribeView::OnCreateSpotCampaign()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( XWndSpotForTool::s_pMouseOver == nullptr )
		DoNewSpotDlg( XGAME::xSPOT_CAMPAIGN );
#endif // _xIN_TOOL
}


void CaribeView::OnCreateSpotVisit()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( XWndSpotForTool::s_pMouseOver == nullptr )
		DoNewSpotDlg( XGAME::xSPOT_VISIT );
#endif // _xIN_TOOL
}

void CaribeView::OnCreateSpotCash()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( XWndSpotForTool::s_pMouseOver == nullptr )
		DoNewSpotDlg( XGAME::xSPOT_CASH );
#endif // _xIN_TOOL
}

void CaribeView::OnCreateBgObj()
{
#ifdef _xIN_TOOL
	CONSOLE( "%s", __TFUNC__ );
	if( SCENE_WORLD == nullptr || !XBaseTool::sIsToolBgObjMode() )
		return;
// 	if( SCENE_WORLD == nullptr ||  XAPP->m_ToolMode != xTM_OBJ )
// 		return;
	DoDlgNewBgObj();
#endif // _xIN_TOOL
}

bool CaribeView::DoDlgNewBgObj()
{
#ifdef _xIN_TOOL
	if( SCENE_WORLD == nullptr || !XBaseTool::sIsToolBgObjMode() )
		return false;
// 	if( SCENE_WORLD == nullptr || XAPP->m_ToolMode != xTM_OBJ )
// 		return false;
	CDlgNewBgObj dlg;
	dlg.m_Prop.m_vwPos = SCENE_WORLD->GetvMouseWorld();
	dlg.m_Prop.m_Type = (XGAME::xtBgObj)1;
	dlg.m_Prop.m_strSpr = _T("obj_bg.spr");
	dlg.m_Prop.m_idAct = 1;
	if( dlg.DoModal() == IDOK ) {
		auto pProp = new xnBgObj::xProp( dlg.m_Prop );
		XPropBgObj::sGet()->AddBgObj( pProp );
		SCENE_WORLD->SetbUpdate( true );
		if( XBaseTool::s_pCurr )
			XBaseTool::s_pCurr->UpdateAutoSave();
	}
#endif // _xIN_TOOL
	return true;
}

void CaribeView::OnEditBgObj()
{
#ifdef _xIN_TOOL
	CONSOLE("%s", __TFUNC__);
	if( SCENE_WORLD == nullptr )
		return;
	auto pWndBgObjLayer = SCENE_WORLD->GetpLayerBgObjUnder();
	if( pWndBgObjLayer ) {
		const ID idBgObj = pWndBgObjLayer->GetidSelected();
		auto pProp = XPropBgObj::sGet()->GetpProp( idBgObj );
		if( pProp ) {
			CDlgNewBgObj dlg;
			dlg.m_Prop = *pProp;
			dlg.m_strSpr = pProp->m_strSpr.c_str();
			if( dlg.DoModal() == IDOK ) {
				*pProp = dlg.m_Prop;
				SCENE_WORLD->SetbUpdate( true );
				if( XBaseTool::s_pCurr )
					XBaseTool::s_pCurr->UpdateAutoSave();
				SCENE_WORLD->UpdateBgObjSelected();
			}
		} else {
			CONSOLE("OnEditBgObj: not found obj: idObj = %d", idBgObj );
			return;
		}
	}
#endif // _xIN_TOOL
}

void CaribeView::OnDelBgObj()
{
#ifdef _xIN_TOOL
	CONSOLE( "%s", __TFUNC__ );
#endif // _xIN_TOOL
}

void CaribeView::OnCreateSpotCommon()
{
  // TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
  if( XWndSpotForTool::s_pMouseOver == nullptr )
    DoNewSpotDlg( XGAME::xSPOT_COMMON );
#endif // _xIN_TOOL
}

void CaribeView::OnEditSpot()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	if( XWndSpotForTool::s_pMouseOver )
	{
		XPropWorld::xBASESPOT *pBaseProp = XWndSpotForTool::s_pMouseOver->GetpBaseProp();
		DoEditSpotDlg( pBaseProp );
		XWndSpotForTool::s_pMouseOver->SetbUpdate( true );
	}
#endif // _xIN_TOOL
}

void CaribeView::OnUpdateEditSpot( CCmdUI *pCmdUI )
{
	// TODO: ���⿡ ���� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
}


/**
 @brief �����Է�â�� ���������� �̹� �������� �����͸� �ִ´�.
*/
void CaribeView::DoPreInputSpot( XGAME::xtSpot type, XPropWorld::xBASESPOT *pBaseProp, CDlgSpot& dlg )
{
	switch( type )
	{
	case XGAME::xSPOT_CASTLE: {
		auto pDlg = new CDlgSubCastle;
		dlg.m_pDlgSub = pDlg;
		if( pBaseProp ) {
			auto pProp = static_cast<XPropWorld::xCASTLE*>( pBaseProp );
#if _DEV_LEVEL <= DLV_DEV_CREW
			for( auto& res : pProp->m_aryProduceOrig )
				pDlg->m_aryResource[ res.type ] = res.num;
#endif
			auto& numWood = pDlg->m_aryResource[ XGAME::xRES_WOOD ];
			if( numWood == 0 )
				numWood = 60;
			auto& numIron = pDlg->m_aryResource[ XGAME::xRES_IRON ];
			if( numIron == 0 )
				numIron = 60;
//			pDlg->SettypeDrop( pProp->typeDrop );
		}
	} break;
	case XGAME::xSPOT_JEWEL: {
		auto pDlg = new CDlgSubJewel;
		dlg.m_pDlgSub = pDlg;
		if( pBaseProp ) {
			auto pProp = static_cast<XPropWorld::xJEWEL*>( pBaseProp );
			pDlg->m_Idx = pProp->idx;
			pDlg->m_Produce = pProp->produce;
		}
	} break;
	case XGAME::xSPOT_SULFUR: {
		auto pDlg = new CDlgSubSulfur;
		dlg.m_pDlgSub = pDlg;
		if( pBaseProp ) {
			auto pProp = static_cast<XPropWorld::xSULFUR*>( pBaseProp );
			pDlg->m_Produce = pProp->produce;
			pDlg->m_secRegen = pProp->secRegenNpc;
			if( pDlg->m_secRegen == 0 )
				pDlg->m_secRegen = 1.f;
		}
	} break;
	case XGAME::xSPOT_MANDRAKE: {
		auto pDlg = new CDlgSubMandrake;
		dlg.m_pDlgSub = pDlg;
		if( pBaseProp ) {
			auto pProp = static_cast<XPropWorld::xMANDRAKE*>( pBaseProp );
			pDlg->m_Idx = pProp->idx;
			pDlg->m_Produce = pProp->produce;
		}
	} break;
	case XGAME::xSPOT_NPC: {
		auto pDlg = new CDlgSubNpc;
		dlg.m_pDlgSub = pDlg;
		dlg.m_secRegen = 180.f;
		if( pBaseProp ) {
			auto pProp = static_cast<XPropWorld::xNPC*>( pBaseProp );
			pDlg->m_secRegen = pProp->GetsecRegen();
			if( pDlg->m_secRegen == 0 )
				pDlg->m_secRegen = 1.f;

		}
	} break;
	case XGAME::xSPOT_DAILY: {
		if( pBaseProp )
			auto pProp = static_cast<XPropWorld::xDaily*>( pBaseProp );
	} break;
// 	case XGAME::xSPOT_SPECIAL: {
// 		if( pBaseProp )
// 			auto pProp = static_cast<XPropWorld::xSpecial*>( pBaseProp );
// 	} break;
	case XGAME::xSPOT_CAMPAIGN: {
		auto pDlg = new CDlgSubCamp;
		dlg.m_pDlgSub = pDlg;
		if( pBaseProp ) {
			auto pProp = static_cast<XPropWorld::xCampaign*>( pBaseProp );
			pDlg->m_strCamp = pProp->strCamp.c_str();
		}

	} break;
	case XGAME::xSPOT_VISIT: {
		auto pDlg = new CDlgSubVisit;
		dlg.m_pDlgSub = pDlg;
		if( pBaseProp ) {
			auto pProp = static_cast<XPropWorld::xVisit*>( pBaseProp );
			pDlg->m_strDialog = XTEXT(pProp->idDialog);
		}

	} break;
	case XGAME::xSPOT_CASH: {
		auto pDlg = new CDlgSubCash;
		dlg.m_pDlgSub = pDlg;
		if( pBaseProp ) {
			auto pProp = static_cast<XPropWorld::xCash*>( pBaseProp );
	// 		dlg.m_secRegen = pProp->secRegen;
	// 		dlg.m_numProduce[ 0 ] = pProp->produceMin;
	// 		dlg.m_numProduce[ 1 ] = pProp->produceMax;
			pDlg->m_numMin = (int)pProp->produceMin;
			pDlg->m_numMax = (int)pProp->produceMax;
		}
	} break;
	case XGAME::xSPOT_COMMON: {
		auto pDlg = new CDlgSubCommon;
		dlg.m_pDlgSub = pDlg;
		if( pBaseProp ) {
			auto pProp = static_cast<XPropWorld::xCommon*>( pBaseProp );
	// 		dlg.m_strType = pProp->strType.c_str();
	// 		dlg.m_str1 = pProp->strParam;
	// 		int num = XNUM_ARRAY( pProp->nParam );
	// 		for( int i = 0; i < num; ++i )
	// 			dlg.m_nParam[ i ] = pProp->nParam[ i ];
			pDlg->m_strType = pProp->strType.c_str();
			pDlg->m_strParam = pProp->strParam.c_str();
			pDlg->m_nParam1 = pProp->nParam[ 0 ];
			pDlg->m_nParam2 = pProp->nParam[ 1 ];
			pDlg->m_nParam3 = pProp->nParam[ 2 ];
			pDlg->m_nParam4 = pProp->nParam[ 3 ];
		}
	} break;
	default:
		XBREAK(1);
		break;
	}
	
}

/**
 @brief ��������â�� ������ �� ���̺��� ���� �����͸� ������Ƽ�� �ִ´�
*/
void CaribeView::DoAfterInputSpot( CDlgSpot& dlg, XPropWorld::xBASESPOT *pOutBaseProp )
{
	if( XBREAK( dlg.m_pDlgSub == nullptr ) )
		return;
	switch( pOutBaseProp->type )
	{
	case XGAME::xSPOT_CASTLE: {
		auto pDlg = dynamic_cast<CDlgSubCastle*>( dlg.m_pDlgSub );
		auto pProp = static_cast<XPropWorld::xCASTLE*>( pOutBaseProp );
// 		if( pDlg->m_numWood == 0 && pDlg->m_numIron == 0 ) {
// 			XALERT( "생산량이 0이어선 안됩니다." );
// 			return;
// 		}
#if _DEV_LEVEL <= DLV_DEV_CREW
		pProp->m_aryProduceOrig.clear();
#endif
		std::vector<XGAME::xRES_NUM> aryRes;
		for( int i = 0; i < XGAME::xRES_MAX; ++i ) {
			float num = pDlg->m_aryResource[i];
			if( num > 0 ) {
				XGAME::xRES_NUM res( (XGAME::xtResource)i, num );
				aryRes.push_back( res );
//				pProp->m_aryProduceOrig.push_back( res );
			}
		}
		pProp->SetAryProduce( aryRes );
// 		pProp->numWood = pDlg->m_numWood;
// 		pProp->numIron = pDlg->m_numIron;
	} break;
	case XGAME::xSPOT_JEWEL: {
		auto pDlg = dynamic_cast<CDlgSubJewel*>( dlg.m_pDlgSub );
		auto pProp = static_cast<XPropWorld::xJEWEL*>( pOutBaseProp );
// 		if( pDlg->m_Produce == 0 )	{
// 			XALERT( "생산량이 0이어선 안됩니다." );
// 			return;
// 		}
		pProp->idx = pDlg->m_Idx;
		pProp->produce = pDlg->m_Produce;
// 		pProp->idx = dlg.m_Idx;
// 		pProp->produce = dlg.m_numProduce[ 0 ];
	} break;
	case XGAME::xSPOT_SULFUR: {
		auto pDlg = dynamic_cast<CDlgSubSulfur*>( dlg.m_pDlgSub );
		auto pProp = static_cast<XPropWorld::xSULFUR*>( pOutBaseProp );
// 		if( pDlg->m_Produce == 0 ) {
// 			XALERT( "생산량이 0이어선 안됩니다." );
// 			return;
// 		}
// 		if( pDlg->m_secRegen < 3.f ) {
// 			XALERT( "리스폰 주기가 너무 짧습니다." );
// 			return;
// 		}
		pProp->produce = pDlg->m_Produce;
		pProp->secRegenNpc = pDlg->m_secRegen;
// 		pProp->secRegenNpc = dlg.m_secRegen;
// 		pProp->produce = dlg.m_numProduce[ 0 ];
	} break;
	case XGAME::xSPOT_MANDRAKE: {
		auto pDlg = dynamic_cast<CDlgSubMandrake*>( dlg.m_pDlgSub );
		auto pProp = static_cast<XPropWorld::xMANDRAKE*>( pOutBaseProp );
// 		if( pDlg->m_Produce == 0 ) {
// 			XALERT( "생산량이 0이어선 안됩니다." );
// 			return;
// 		}
		pProp->idx = pDlg->m_Idx;
		pProp->produce = pDlg->m_Produce;
// 		pProp->idx = dlg.m_Idx;
// 		pProp->produce = dlg.m_numProduce[ 0 ];
	} break;
	case XGAME::xSPOT_NPC: {
		auto pDlg = dynamic_cast<CDlgSubNpc*>( dlg.m_pDlgSub );
		auto pProp = static_cast<XPropWorld::xNPC*>( pOutBaseProp );
// 		if( pDlg->m_secRegen < 3.f ) {
// 			XALERT( "리스폰 주기가 너무 짧습니다." );
// 			return;
// 		}
		pProp->secRegen = pDlg->m_secRegen;
		if( pProp->secRegen == 0 )
			pProp->secRegen = 1.f;
// 		pProp->legion.gradeLegion = ( pDlg->m_Elite ) ? XGAME::xGL_ELITE : XGAME::xGL_NORMAL;
// 		pProp->gradeLegion = (pDlg->m_Elite) ? XGAME::xGL_ELITE : XGAME::xGL_NORMAL;
// 		if( !pDlg->m_strClan.IsEmpty() ) {
// 			pProp->clan = (XGAME::xtClan)CONSTANT->GetValue( (LPCTSTR)pDlg->m_strClan );
// 		}
//		pProp->secRegen = dlg.m_secRegen;
	} break;
	case XGAME::xSPOT_DAILY: {
		auto pProp = static_cast<XPropWorld::xDaily*>( pOutBaseProp );
	} break;
// 	case XGAME::xSPOT_SPECIAL: {
// 		auto pProp = static_cast<XPropWorld::xSpecial*>( pOutBaseProp );
// 	} break;
	case XGAME::xSPOT_CAMPAIGN: {
		auto pDlg = dynamic_cast<CDlgSubCamp*>( dlg.m_pDlgSub );
		auto pProp = static_cast<XPropWorld::xCampaign*>( pOutBaseProp );
		pProp->strCamp = pDlg->m_strCamp;
// 		pProp->strCamp = dlg.m_str1;
 		pProp->idCamp = XQuestProp::sGetIdsToId( pProp->strCamp );
	} break;
	case XGAME::xSPOT_VISIT: {
		auto pDlg = dynamic_cast<CDlgSubVisit*>( dlg.m_pDlgSub );
		auto pProp = static_cast<XPropWorld::xVisit*>( pOutBaseProp );
		auto str1 = pDlg->m_strDialog;
		if( !str1.IsEmpty() ) {
			ID idText = TEXT_TBL->AddTextAutoID( str1, 20000, 10000 );
			pProp->idDialog = idText;
			TEXT_TBL->Save( _T( "text_ko.txt" ) );
		}
	} break;
	case XGAME::xSPOT_CASH: {
		auto pDlg = dynamic_cast<CDlgSubCash*>( dlg.m_pDlgSub );
		auto pProp = static_cast<XPropWorld::xCash*>( pOutBaseProp );
		if( pDlg->m_secRegen < 3.f ) {
			XALERT( "리스폰 주기가 너무 짧습니다." );
			return;
		}
		if( pDlg->m_numMin > 10 || pDlg->m_numMax > 10 ) {
			XALERT( "캐쉬스팟:캐쉬제공량이 넘 많은 거 아님?" );
			return;
		}
		pProp->secRegen = (float)pDlg->m_secRegen;
		pProp->produceMin = (float)pDlg->m_numMin;
		pProp->produceMax = (float)pDlg->m_numMax;
// 		pProp->secRegen = dlg.m_secRegen;
// 		pProp->produceMin = dlg.m_numProduce[ 0 ];
// 		pProp->produceMax = dlg.m_numProduce[ 1 ];
	} break;
	case XGAME::xSPOT_COMMON: {
		auto pDlg = dynamic_cast<CDlgSubCommon*>( dlg.m_pDlgSub );
		auto pProp = static_cast<XPropWorld::xCommon*>( pOutBaseProp );
		pProp->strType = pDlg->m_strType;
		pProp->strParam = pDlg->m_strParam;
		pProp->nParam[ 0 ] = pDlg->m_nParam1;
		pProp->nParam[ 1 ] = pDlg->m_nParam2;
		pProp->nParam[ 2 ] = pDlg->m_nParam3;
		pProp->nParam[ 3 ] = pDlg->m_nParam4;
// 		pProp->strType = dlg.m_strType;
// 		pProp->strParam = dlg.m_str1;
// 		int num = XNUM_ARRAY( dlg.m_nParam );
// 		for( int i = 0; i < num; ++i )
// 			pProp->nParam[ i ] = dlg.m_nParam[ i ];
	} break;
	default:
		XBREAK( 1 );
		break;
	}
}

void CaribeView::DoSpawnSquad( XGAME::xtUnit unit )
{
	bool bEnemy = XE::GetMain()->m_bCtrl == true;
	XE::VEC3 vwPos = XWndBattleField::sGet()->GetPosWindowToWorld( m_vRClick );
	XBattleField::sGet()->SpawnSquadByCheat( vwPos, unit, bEnemy );
}

void CaribeView::OnCreateSpearman()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	DoSpawnSquad( XGAME::xUNIT_SPEARMAN );
}


void CaribeView::OnCreateArcher()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	DoSpawnSquad( XGAME::xUNIT_ARCHER );
}


void CaribeView::OnCreatePaladin()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	DoSpawnSquad( XGAME::xUNIT_PALADIN );
}


void CaribeView::OnCreateMinotaur()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	DoSpawnSquad( XGAME::xUNIT_MINOTAUR );
}


void CaribeView::OnCreateCyclops()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	DoSpawnSquad( XGAME::xUNIT_CYCLOPS );
}


void CaribeView::OnCreateLycan()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	DoSpawnSquad( XGAME::xUNIT_LYCAN );
}


void CaribeView::OnCreateGolem()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	DoSpawnSquad( XGAME::xUNIT_GOLEM );
}


void CaribeView::OnCreateTreant()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	DoSpawnSquad( XGAME::xUNIT_TREANT );
}


void CaribeView::OnCreateFallenAngel()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	DoSpawnSquad( XGAME::xUNIT_FALLEN_ANGEL );
}

void CaribeView::OnCopySpot()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( SCENE_WORLD )
		SCENE_WORLD->CopySpotT();
#endif // _xIN_TOOL
}

void CaribeView::OnPasteSpot()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
// 	if( SCENE_WORLD )
// 		SCENE_WORLD->GetpTool()->PasteSelected();
	if( SCENE_WORLD && XBaseTool::s_pCurr )
		XBaseTool::s_pCurr->PasteSelected();
#endif // _xIN_TOOL
}

void CaribeView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	XE::xtKey key = XE::KEY_NONE;
	if( nChar == VK_LEFT ) {
		key = XE::KEY_LEFT;
	} else

	if( nChar == VK_RIGHT ) {
		key = XE::KEY_RIGHT;
	} else
	if( nChar == VK_UP ) {
		key = XE::KEY_UP;
	} else
	if( nChar == VK_DOWN ) {
		key = XE::KEY_DOWN;
	} else
	if( nChar == VK_DELETE ) {
		key = XE::KEY_DEL;
	}
	if( key ) {
		GAME->OnKeyDown( key );
	}
	__super::OnKeyDown( nChar, nRepCnt, nFlags );
}

int CaribeView::OnCreate(LPCREATESTRUCT lpcs )
{
	if( __super::OnCreate( lpcs) == -1 )
		return -1;
	return 0;
}

void CaribeView::OnViewBgObjBoundBox()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bViewBgObjBoundBox = !XAPP->m_bViewBgObjBoundBox;
	//	XAPP->XClientMain::SaveCheat();
}
void CaribeView::OnUpdateViewBgObjBoundBox( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->m_bViewBgObjBoundBox );
}
