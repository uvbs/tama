#include "stdafx.h"
#include "XWndList.h"
#include "XWndListH.h"
#include "XWndImage.h"
#include "XFramework/XEProfile.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef max
#undef  max;
#endif
#ifdef min
#undef  min;
#endif

XWndList::XWndList( const XE::VEC2& vPos,
										const XE::VEC2& vSize, xtType type )
	: XWndScrollView( vPos.x, vPos.y, vSize.w, vSize.h )
{
	Init();
	m_Type = type;

	SetType( type );
	SetPosLocal( vPos );
// 	m_posBg -= vLocalxy;
// 	if( XE::IsHave( szBg ) )
// 		m_psfcBg = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szBg ) );
	// 타입에 따라 스크롤방향을 잠근다.
	// 스크롤뷰의 크기가 가로 혹은 세로로만 긴 형태라도 당기면 조금 스크롤되는게 있으므로 강제로 스크롤을 막아야 한다.
}

void XWndList::Destroy()
{
//	SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
//	XLIST_DESTROY( m_listItem, XWnd* );
}

/**
 @brief 리스트에 아이템을 하나 추가한다.
*/
XWnd* XWndList::AddItem( ID idWnd, XWnd *pWnd ) 
{
	m_listElem.Add( pWnd );
	if( idWnd )
		Add( idWnd, pWnd );
	else
		Add( pWnd );
	// 각자 자신의 바운딩박스를 미리 계산하라고 메시지를 보냄.
//	pWnd->PreCalcLayoutSize();	// WndText때문에 들어간것이었으나 필요없어져서 삭제됨.
	// 자식도 없는데 사이즈도 없는 윈도우가 들어오면 경고(WndText외엔 이런일이 없을것으로 예상함.
	// WndText를 리스트에 넣으려면 미리 크기가 정해져 있어야 함.
	XBREAK( pWnd->GetNumChild() == 0 && pWnd->GetSizeLocal().IsInvalid() );
	// 추가한 아이템의 위치를 갱신한다.
	RecalcElem( pWnd );
	return pWnd;
}

/**
 @brief pWnd의 위치를 갱신한다.
*/
void XWndList::RecalcElem( XWnd *pWnd )
{
	if( pWnd->IsDestroy() )
		return;
	// this의 최대 사이즈로 this의 사이즈를 정한다.
	const auto vScale = pWnd->GetScaleLocal();
	auto sizeLocalByElem = pWnd->GetSizeNoTransLayout() * vScale;
	// 각 elem의 고정크기가 지정되어있을땐.
	if( m_sizeFixed.w > 0 )
		sizeLocalByElem.w = m_sizeFixed.w;
	if( m_sizeFixed.h > 0 )
		sizeLocalByElem.h = m_sizeFixed.h;
	XASSERT( sizeLocalByElem.IsValid() );
	if( m_Type == xLT_VERT ) {
		// 추가한 객체의 세로크기를 계산해서 리스트내의 y좌표를 지정한다.
		pWnd->SetPosLocal( m_X, m_Y );
		m_Y += sizeLocalByElem.h;
		m_Y += m_vSpace.h;
	} else
	if( m_Type == xLT_HORIZ ) {
		// 추가한 객체의 세로크기를 계산해서 리스트내의 y좌표를 지정한다.
		pWnd->SetPosLocal( m_X, m_Y );
		m_X += sizeLocalByElem.w;
		m_X += m_vSpace.w;
	} else
	if( m_Type == xLT_THUMB ) {
		// 스크롤뷰의 크기가 혀용하는곳 까지 가로로 우선 배열되고 다차면 세로로 내려간다.
//		if( m_X + vSize.w > GetSizeFinal().w ) {	// final이 아닐텐데.....
		if( m_X + sizeLocalByElem.w > GetSizeLocal().w ) {
			m_X = 0;
			m_Y += sizeLocalByElem.h;
			m_Y += m_vSpace.h;
		}
		pWnd->SetPosLocal( m_X, m_Y );
		m_X += sizeLocalByElem.w;
		m_X += m_vSpace.w;
	} else
		XBREAK( 1 );
	SetScrollViewAutoSize();
}

/**
 @brief idWnd element를 삭제하고 각 element위치를 재갱신한다.
*/
void XWndList::DelItem( ID idWnd )
{
	// 리스트에서 해당 아이템을 삭제한다.
	m_listElem.DelByID( idWnd );
	DestroyID( idWnd );
	SetbUpdate( TRUE );
}

/**
 @brief 리스트의 엘리먼트를 인덱스로 꺼낸다.
*/
XWnd* XWndList::GetElemByIndex( int idx ) 
{

	int i = 0;
// 	XLIST_LOOP_IDX( m_listElem, XWnd*, i, pElem )
// 	{
	for( auto pElem : m_listItems ) {
		if( idx == i++ )
			return pElem;
	}
	return nullptr;
}

void XWndList::DestroyAllItem( void )
{
	m_listElem.Clear();
	DestroyChildAll();
	m_X = m_Y = 0;
	XWndScrollView::Clear();
}

void XWndList::Update( void )
{
	// 위치값을 초기화시키고 AddItem을 다시 불러 각 위치값을 재 갱신한다.
	m_X = m_Y = 0;
	for( auto pElem : m_listItems ) {
		RecalcElem( pElem );
	}
	SetScrollViewAutoSize();
	if( m_bMultiSelect ) {
		// 멀티셀렉트 타입이면 셀렉트 된애들한테 셀렉트된걸 다시 보내준다.
		for( auto idWnd : m_listSelect ) {
			auto pDelegate = dynamic_cast<XWndListElemDelegate*>( Find( idWnd ) );
			if( pDelegate )
				pDelegate->SetListSelect( TRUE );
		}
	}
	XWndScrollView::Update();
}

void XWndList::OnStopScroll()
{
	XE::VEC2 vCurrLT = GetvCurrLT();			// 전체 스크롤영역중 현재 스크롤위치의 LT좌표
	XE::VEC2 vSizeScrl = GetsizeView();		// 스크롤 창문의 크기
	if( m_pDelegate )
		m_pDelegate->DelegateStopScroll( this, vCurrLT, vSizeScrl );
}

//#include "Mainfrm.h"
void XWndList::Draw( void )
{
	XPROF_OBJ_AUTO();
	XE::VEC2 v = XWnd::GetPosFinal();
//	auto v = GetPosFinal();
	XE::VEC2 size = XWnd::GetSizeFinal();
// 	if( m_psfcBg )
// 		m_psfcBg->Draw( v + m_posBg );
	// viewport 설정
	const XE::VEC2 vpLT = GRAPHICS->GetViewportLT();		// 스택에 백업.
	const XE::VEC2 vpSize = GRAPHICS->GetViewportSize();
//	GRAPHICS->BackupViewport();
	m_vAdjustByViewport.Set( 0 );
	if( v.x < vpLT.x ) {
		m_vAdjustByViewport.x = v.x - vpLT.x;
		v.x = vpLT.x;
		size.w += m_vAdjustByViewport.x;
	}
	if( v.y < vpLT.y ) {
		m_vAdjustByViewport.y = v.y - vpLT.y;
		v.y = vpLT.y;
		size.h += m_vAdjustByViewport.y;
	}
	GRAPHICS->SetViewport( v, size );
	XE::SetProjection( size.w, size.h );
	// 자식들 그림
	XWnd::Draw();
	// viewport 해제	 
//	GRAPHICS->RestoreViewport();
	GRAPHICS->SetViewport( vpLT.ToPoint(), vpSize.ToPoint() );		// 백업받았던 뷰포트 복구.
	XE::SetProjection( vpSize.w, vpSize.h );
//	XE::SetProjection( XE::GetGameWidth(), XE::GetGameHeight() );
//	GRAPHICS->DrawRectSize( v, size, XCOLOR_WHITE );
	XWndScrollView::DrawScrollBar();
}

void XWndList::OnLButtonDown( float lx, float ly )
{
	XWndScrollView::OnLButtonDown( lx, ly );
//	m_pClickedWnd = GetpwndLastLDown();
	auto pWndLastLDown = GetpwndLastLDown();
	if( pWndLastLDown )
		m_idClickedWnd = pWndLastLDown->getid();
	else
		m_idClickedWnd = 0;
	m_vDown = INPUTMNG->GetMousePos();		// 터치 절대 좌표
	m_bTouchDown = TRUE;
	m_bScrollExclusive = FALSE;
	SetCapture();
}

void XWndList::OnLButtonDownCancel( void )
{
//	m_pClickedWnd = NULL;
	m_idClickedWnd = 0;
	m_bTouchDown = FALSE;
	m_bScrollExclusive = FALSE;
	ReleaseCapture();
	XWndScrollView::OnLButtonDownCancel();
}

void XWndList::ProcessDrag( const XE::VEC2& vDistMove )
{
	// 한쪽방향으로만 스크롤되는 경우
	BOOL bDrag = FALSE;
	if( IsVertOnly() )
		// 가로로 더 많이 움직였으면
		if( fabs( vDistMove.x ) > fabs( vDistMove.y ) )
			bDrag = TRUE;
	if( IsHorizOnly() ) {
		// 세로로 더 많이 움직였으면
		if( fabs( vDistMove.y ) > fabs( vDistMove.x ) )
			bDrag = TRUE;
	}
	if( bDrag ) {
		auto pClickedWnd = GetpClickedWnd();
		if( pClickedWnd ) {
			// drag & drop중 drag이벤트 발생
			CallEventHandler( XWM_DRAG, pClickedWnd->GetID() );
			// 드래그한 윈도우를 받아둔다.
			XWnd::sSetDragWnd( pClickedWnd );
			OnLButtonDownCancel();
		}
	}
}

void XWndList::OnMouseMove( float lx, float ly )
{
	XWndScrollView::OnMouseMove( lx, ly );
	if( m_bTouchDown ) {
		XE::VEC2 vDist = m_vDown - INPUTMNG->GetMousePos();	// 스크롤뷰는 터치무브상태일때는 상대좌표가 항상 제자리이기때문에 터치 절대좌표로 써야 한다.
		if( GetbScroll() ) {
			// 세로로 더 많이 움직였으면...
			if( fabs( vDist.y ) > fabs( vDist.x ) && m_bScrollExclusive == FALSE ) {
				// 내가 독점했으니 부모들에게 터치다운 이벤트 캔슬하라고 알려줌.
				SendParentsLButtonDownCancel();
				m_bScrollExclusive = TRUE;		// 필요하다면 내 자식에게도 무브 이벤트 보내지 않는다.
			}
			m_idClickedWnd = 0;
		}
	}
}

void XWndList::OnLButtonUp( float lx, float ly )
{
	m_bScrollExclusive = FALSE;
	m_bTouchDown = FALSE;
	ReleaseCapture();
	auto pClickedWnd = GetpClickedWnd();
	if( pClickedWnd ) {
		if( pClickedWnd->GetbEnable() ) {
			const ID idWnd = pClickedWnd->GetID();
			if( m_bMultiSelect ) {
				if( !m_listSelect.Findp( idWnd ) ) {
					// 셀렉트 핸들러에서 1을 리턴해줘야 셀렉트를 허락받고 해당동작을 계속 실행할수 있다.
					if( CallEventHandler( XWM_SELECT_ELEM, idWnd ) ) {
						m_listSelect.Add( idWnd );
						auto pDelegate = dynamic_cast<XWndListElemDelegate*>( pClickedWnd );
						if( pDelegate )
							pDelegate->SetListSelect( TRUE );
					}
				} else {
					if( CallEventHandler( XWM_DESELECT_ELEM, idWnd ) ) {
						m_listSelect.Del( idWnd );
						auto pDelegate = dynamic_cast<XWndListElemDelegate*>( pClickedWnd );
						if( pDelegate )
							pDelegate->SetListSelect( FALSE );
					}
				}
			} else {
				CallEventHandler( XWM_SELECT_ELEM, idWnd );
				pClickedWnd->CallEventHandler( XWM_HELP_CLICKED, idWnd );
			}
		}
	}
	pClickedWnd = NULL;
	m_idClickedWnd = 0;
	XWndScrollView::OnLButtonUp( lx, ly );
}

// 자식중 하나가 크기가 변할것임을 알려왔다.
void XWndList::OnStartChangeSizeOfChild( XWnd *pChild, const XE::VEC2& vSize, float secSizing )
{
	if( m_Type == xLT_HORIZ || m_Type == xLT_VERT )	{
		// 아직은 리스트 타입만 지원됨
		m_State = 1;
		m_pResizingChild = pChild;
		m_sizeResizingChild = vSize;
		m_secRearrange = secSizing;		// 자식이 변하는 속도와 맞춰준다.
		// 재배열될 애들 골라냄.
		m_listRearrange.clear();
		bool bFind = false;
		for( auto pWnd : GetlistItems() ) {
			if( bFind ) {
				xINTERPOLATION_WND wnd;
				wnd.vBefore = pWnd->GetPosLocal();
				wnd.vAfter = wnd.vBefore;
				wnd.vAfter.y += m_sizeResizingChild.h;
				wnd.pWnd = pWnd;
				m_listRearrange.Add( wnd );
			}
			if( pWnd == m_pResizingChild )		// 크기가 변하는 넘 다음꺼부터 밀린다.
				bFind = true;
		}
// 		XList<XWnd*>::Itor itor;
// 		GetNextClear2( &itor );
// 		XWnd *pWnd;
// 		BOOL bFind = FALSE;
// 		while( ( pWnd = GetNext2( itor ) ) ) {
// 			if( bFind ) {
// 				xINTERPOLATION_WND wnd;
// 				wnd.vBefore = pWnd->GetPosLocal();
// 				wnd.vAfter = wnd.vBefore;
// 				wnd.vAfter.y += m_sizeResizingChild.h;
// 				wnd.pWnd = pWnd;
// 				m_listRearrange.Add( wnd );
// 			}
// 			if( pWnd == m_pResizingChild )		// 크기가 변하는 넘 다음꺼부터 밀린다.
// 				bFind = TRUE;
// 		}
		// 스크롤뷰의 크기도 조정한다.
		XE::VEC2 sizeScrollView = XWndScrollView::GetViewSize();
		sizeScrollView += m_sizeResizingChild;
		XWndScrollView::SetViewSize( sizeScrollView );
	}
	// 
}

int XWndList::Process( float dt )
{
	if( m_State == 1 ) {
		if( m_Type == xLT_HORIZ || m_Type == xLT_VERT ) {
			float lerp = m_ipoRearrange.GetInterpolationValueWithSet( m_secRearrange );
			// 자식의 크기변화로 리스트아이템들의 위치 재배열애니메이션 중.
			for( const auto& wnd : m_listRearrange ) {
				XE::VEC2 vCurr = wnd.vBefore + ( wnd.vAfter - wnd.vBefore ) * lerp;
				wnd.pWnd->SetPosLocal( vCurr );
			}
			if( m_ipoRearrange.IsFinish() ) {
				m_ipoRearrange.Off();
				m_State = 0;
				m_pResizingChild = NULL;
				m_sizeResizingChild.Set( 0 );
				m_secRearrange = 0;
				m_listRearrange.Clear();
			}
		}
	}
	return XWndScrollView::Process( dt );;
}

void XWndList::DoMoveTop()
{
	SetFocusTop();
}
void XWndList::DoMoveBottom()
{
	SetFocusBottom();
}

XE::VEC2 XWndList::GetSizeNoTransLayout()
{
	return GetSizeLocal();
}
