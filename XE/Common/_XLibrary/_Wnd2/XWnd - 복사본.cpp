#include "stdafx.h"
#include "XWnd.h"
#include "xUtil.h"
#include "XWndMng.h"
#include "SprObj.h"
#include "XFontSpr.h"
#include "XToolTip.h"		
#ifdef WIN32
#include "XGraphicsD3DTool.h"
#endif
#include "XWndButton.h"
#ifdef _NEW_IMAGE
#include "XImage.h"
#else
#ifdef _VER_IOS
#include "XImageiOS.h"
#endif
#endif
#include "XClientMain.h"
#include "XLayout.h"
using namespace XE;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XWnd* XWnd::s_pFocusWindow = NULL;
XWnd* XWnd::s_pDropWnd = NULL;
XList<ID> XWnd::s_listAllowClick;		// 이게 세팅되어 있으면 이 윈도우들외에 다른 윈도우는 입력을 막아야 한다.

//////////////////////////////////////////////////////////////////////////
// static
// pCompare윈도우가 클릭해도 되는 윈도우인지 검사한다.
/// <summary>
/// Saves the person.
/// </summary>
/// <param name="person">Person.</param>
BOOL XWnd::IsAllowClickWnd( XWnd *pCompare )
{
	if( s_listAllowClick.size() == 0 )
		return TRUE;
	BOOL bFind = FALSE;
	XLIST_LOOP2( s_listAllowClick, ID, idAllow )
	{
		XWnd *pAllowClick = XE::GetMain()->GetpGame()->Find( idAllow );
		if( pAllowClick )
		{
			bFind = TRUE;
			if( pAllowClick->IsHierarchyWnd( pCompare ) )
				return TRUE;
			if( pAllowClick->Find( pCompare ) )		// 허용된 윈도우의 자식이라도 허용함.
				return TRUE;
		}
	} END_LOOP;
	if( bFind )
		return FALSE;
	// 입력을 허용해야할 윈도우가 없으면 모두다 클릭 허용
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void XWnd::SetScaleLocal( float sx, float sy ) 
{ 
	m_vScale.Set( sx, sy );
}
void XWnd::SetRotateLocal( float dRotZ )
{
	m_dRotateZ = dRotZ;
}

XBaseFontDat *XWnd::s_pFontDat = NULL;

void XWnd::Destroy( void ) 
{ 
//	list<XWnd*>::iterator iter;
	int size = m_listItems.size();
//	for( iter = m_listItems.begin(); iter != m_listItems.end(); iter++ )
	XLIST_LOOP( m_listItems, XWnd*, pWnd );
	{
//		XWnd *pWnd = (*iter);
		SAFE_DELETE( pWnd );
//		m_listItems.erase( iter++ );
	} END_LOOP;
	SAFE_DELETE( m_pModalDlg );	// 삭제책임은 XWnd가 갖는다(잘하는짓인지 몰겠음). 그러므로 ModalDlg를 루아내부에서 생성하면 안됨. 루아파괴될때 그곳에서 파괴되니까
}

void XWnd::DestroyChildAll( void )
{
//	list<XWnd*>::iterator iter;
//	int size = m_listItems.size();
//	for( iter = m_listItems.begin(); iter != m_listItems.end(); iter++ )
	XLIST_LOOP( m_listItems, XWnd*, pWnd );
	{
//		XWnd *pWnd = (*iter);
		pWnd->SetbDestroy( TRUE );
	} END_LOOP;
//	m_listItems.clear();	 // ?? <-이거 클리어 시키면 안되는거 아님?
}

XWnd *XWnd::Add( int id, XWnd *pChild ) 
{
	XBREAK( id == 0 );
//	pChild->m_pWndMng = m_pWndMng;
	pChild->SetID( id );
	pChild->m_pParent = this;
	// 중복된게 있는지 검사.
	XBREAK( Find( pChild ) );
//	m_listItems.push_back( pChild );
	m_listItems.Add( pChild );
	pChild->OnAddWnd();
	return pChild;
}

// this의 최상위윈도우에 pWnd를 추가시킨다.
XWnd* XWnd::AddWndTop( ID idWnd, XWnd *pWnd )
{
	XWnd *pTop = GetWndTop();
	pTop->Add( idWnd, pWnd );
	return pWnd;
}

XWnd* XWnd::Find( ID id ) 
{
	XBREAK( id == 0 );
	if( GetDestroyFinal() )
		return NULL;
	XLIST_LOOP( m_listItems, XWnd*, pWnd )
	{
		if( pWnd->GetDestroyFinal() )	// 파괴된건 찾지 않음.
			continue;
		if( pWnd->GetID() == id )	
			return pWnd;
		XWnd *pChild;
		if( pChild = pWnd->Find( id ) )	// child에서도 검사
			return pChild;
	} END_LOOP;
	return NULL;
}
//
XWnd* XWnd::Find( const char *cIdentifier ) 
{
	XBREAK( XE::IsEmpty( cIdentifier ) == TRUE );
	if( GetDestroyFinal() )
		return NULL;
//	LIST_LOOP( m_listItems, XWnd*, itor, pWnd )
	XLIST_LOOP( m_listItems, XWnd*, pWnd )
	{
		if( pWnd->GetDestroyFinal() )	// 파괴된건 찾지 않음.
			continue;
		if( XE::IsSame( pWnd->GetszIdentifier(), cIdentifier ) )	
			return pWnd;
		XWnd *pChild;
		if( pChild = pWnd->Find( cIdentifier ) )	// child에서도 검사
			return pChild;
	} END_LOOP;
	return NULL;
}
//
XWnd* XWnd::Find( XWnd *_pWnd ) 
{
	if( _pWnd == NULL )
		return NULL;
	if( GetDestroyFinal() )
		return NULL;
	LIST_LOOP( m_listItems, XWnd*, itor, pWnd )
	{
		if( pWnd->GetDestroyFinal() )	// 파괴된건 찾지 않음.
			continue;
		if( pWnd->Find( _pWnd ) )	// child에서도 검사
			return _pWnd;
		if( _pWnd == pWnd )
			return pWnd;
	} END_LOOP;
	return NULL;
}

int XWnd::Process( float dt ) 
{
	if( GetDestroyFinal() )
		return 0;
	if( GetbProcess() == FALSE )
		return 0;
	// 생성/삭제시에 애니메이션이 있으면 처리하고 애니메이션을 수행하는 중이면 아래 process()들은 실행 안함.
	if( ProcessEnterLeaveAnimation() == TRUE )
		return 1;
	if( m_timerPush.IsOver(0.15f) )	// 누른지 0.2초가 지났으면		
	{
		if( m_prefToolTip == NULL )
			m_prefToolTip = CreateToolTip();		// virtual
		if( m_prefToolTip )
		{
			XE::VEC2 vPos = GetPosFinal();
			XE::VEC2 vTT( vPos.x, vPos.y - GetprefToolTip()->GetSize().h );
			m_prefToolTip->SetvPos( vTT );
			m_bToolTip = TRUE;
			TOOLTIP = m_prefToolTip;
			m_prefToolTip->Update(); 
		}
		m_timerPush.Off();
	}
	if( m_pModalDlg )		// 모달다이얼로그가 있으면 다른 윈도우들은 처리하지 않는다
	{
		return m_pModalDlg->Process( dt );
	}
	LIST_MANUAL_LOOP( m_listItems, XWnd*, itor, pWnd )
	{
		pWnd->Process(dt);
		if( pWnd->GetDestroyFinal() )
		{
			if( pWnd->CallEventHandler( XWM_DESTROY ) )		// 파괴 이벤트 핸들러가 있다면 호출
			{
				pWnd->OnDestroy();
				ID idDestroy = pWnd->GetID();
				XWnd *pDestroyAfterWnd = pWnd->GetpAfterDestroyEvent();
                pWnd->PrivateDestroyChild();
				SAFE_DELETE( pWnd );
				m_listItems.erase( itor++ );
				// 파괴된 직후에 이벤트 발생시킬 윈도우가 있으면 발생시킴
				if( pDestroyAfterWnd )
					pDestroyAfterWnd->CallEventHandler( XWM_DESTROY_AFTER, idDestroy );
			} else
			{
				++itor;
				pWnd->SetbDestroy( FALSE );
			}
		} else
			++itor;
	} END_LOOP;
	return 0;
}

// private전용 자식 삭제
// 모든 윈도우는 자신이 SAFE_DELETE되기 전에 이것을 먼저 불러야 한다.
// virturl 파괴자로 인해 부모의 리소스가 먼저삭제되고 자식들의 리소스가 나중에 파괴되어
// 생성순서와 파괴순서가 바뀌는 문제를 해결하려 했다.
void XWnd::PrivateDestroyChild( void )
{
    LIST_LOOP( m_listItems, XWnd*, itor, pChild )
    {
        pChild->PrivateDestroyChild();
        SAFE_DELETE( pChild );
    } END_LOOP;
    m_listItems.clear();
    int size = m_listItems.size();
    size = 1;
}

// 윈도우의 등장/퇴장 애니메이션
BOOL XWnd::ProcessEnterLeaveAnimation( void )
{

	if( m_bAnimationEnterLeave )
	{
		if( m_stateAppear == xAP_NONE )
		{
			m_stateAppear = xAP_ENTER_INIT;		// 나타나기 시작
			SetbActive( FALSE );		// 애니메이션 중엔 비활성 시킴
//			m_bEnter = !m_bEnter;			// 사라져있는상태면 나타나는 상태로 나타나있는상태면 사라지는 상태로
		}
	}
	if( m_stateAppear )
	{
		if( m_stateAppear == xAP_ENTER_INIT )
		{
			m_timerAppear.Set( m_secAppear );
			m_stateAppear = xAP_ENTER_ING;
		} else
		if( m_stateAppear == xAP_LEAVE_INIT )
		{
			m_timerAppear.Set( m_secAppear );
			m_stateAppear = xAP_LEAVE_ING;
		}
		if( m_timerAppear.IsOn() )
		{
			float timeLerp = m_timerAppear.GetSlerp();
			if( timeLerp > 1.0f )
				timeLerp = 1.0f;
			float lerp = XE::xiHigherPowerDeAccel( timeLerp, 1.0f, 0 );
			if( m_stateAppear == xAP_LEAVE_ING )
				lerp = 1.0f - lerp;
			BOOL bEnter = (m_stateAppear == xAP_ENTER_ING )? TRUE : FALSE;
			ProcessAnimationLerp( m_bEnter, timeLerp, lerp );		// virtual
			if( timeLerp >= 1.0f )
			{
				if( m_stateAppear == xAP_ENTER_ING )
				{
					OnFinishAppear();
					m_stateAppear = xAP_ENTER_END;
				} else
				{
					m_stateAppear = xAP_LEAVE_END;
					SetDestroyFinal();
				}
				SetbActive( TRUE );
				m_timerAppear.Off();
				return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

// aniLerp: bEnter가 true이면 0->1.0로 변화하고 false이면 1.0->0으로 변화한다.
void XWnd::ProcessAnimationLerp( BOOL bEnter, float timeLerp, float aniLerp )
{
	SetAlphaLocal( aniLerp );
}

void XWnd::Draw( void ) 
{
	if( GetbShow() == FALSE )	return;	// hide상태에선 본인과 자식들모두 감춤
	LIST_LOOP( m_listItems, XWnd*, itor, pWnd )
	{
		if( pWnd->GetbShow() && pWnd->GetDestroyFinal() == FALSE )
		{
/*			XE::VEC2 vPos = pWnd->GetPosFinal();
			XE::VEC2 vSize = pWnd->GetSizeFinal();
			if( vPos.x > (float)GRAPHICS->GetViewportRight() )
                continue;
			if( vPos.y > (float)GRAPHICS->GetViewportBottom() )
                continue;
			if( vPos.x + vSize.w < (float)GRAPHICS->GetViewportLeft() )
                continue;
			if( vPos.y + vSize.h < (float)GRAPHICS->GetViewportTop() )
                continue; */
			// 모달타입의 경우 그리기 전에 배경을 어둡게 해준다.
			if( pWnd->GetbModal() )
				GRAPHICS->FillRectSize( XE::VEC2(0), GRAPHICS->GetScreenSize(), XCOLOR_RGBA( 0, 0, 0, 200 ) );
			pWnd->Draw();
		}
	} END_LOOP;
	if( m_pModalDlg ) {		// 모달다이얼로그는 최상위에 찍힌다
		m_pModalDlg->Draw();
	}
}

void XWnd::Draw( const XE::VEC2& vParent ) 
{
	XE::VEC2 vViewportLT = XE::VEC2(0);;
	XE::VEC2 vViewportRB = GRAPHICS->GetViewportSize();

	if( GetbShow() == FALSE )	return;	// hide상태에선 본인과 자식들모두 감춤
	LIST_LOOP( m_listItems, XWnd*, itor, pWnd )
	{
		if( pWnd->GetbShow() && pWnd->GetDestroyFinal() == FALSE )
		{
			XE::VEC2 vPos = vParent + pWnd->GetPosLocal() * this->GetScaleLocal();
			XE::VEC2 vSize = pWnd->GetSizeFinal();
			if( vPos.x > vViewportRB.x )
				continue;
			if( vPos.y > vViewportRB.y )
				continue;
			if( vPos.x + vSize.w < vViewportLT.x )
				continue;
			if( vPos.y + vSize.h < vViewportLT.y )
				continue;
			// 모달타입의 경우 그리기 전에 배경을 어둡게 해준다.
			if( pWnd->GetbModal() )
				GRAPHICS->FillRectSize( XE::VEC2(0), GRAPHICS->GetScreenSize(), XCOLOR_RGBA( 0, 0, 0, 200 ) );
			pWnd->Draw( vPos );
		}
	} END_LOOP;
	if( m_pModalDlg ) {		// 모달다이얼로그는 최상위에 찍힌다
		m_pModalDlg->Draw();
	}
}

void XWnd::GenerateLoopEvent( void )
{
	XWND_MESSAGE_MAP msgMap = FindMsgMap( XWM_LOOP_EVENT );
	if( msgMap.pOwner && msgMap.pHandler )
	{
		LIST_LOOP( m_listItems, XWnd*, itor, pWnd )
		{
			(msgMap.pOwner->*msgMap.pHandler)( pWnd, msgMap.param, 0 );		// 롤러가 돌아가다 멈추면 이벤트가 발생하며 멈춘아이템의 인덱스를 건넨다
		} END_LOOP;
	}
}

int XWnd::CallEventHandler( ID msg, DWORD dwParam2 )
{
	int ret=1;
	XWND_MESSAGE_MAP msgMap = FindMsgMap( msg );
	if( msgMap.pOwner && msgMap.pHandler )
		ret = (msgMap.pOwner->*msgMap.pHandler)( this, msgMap.param, dwParam2 );		// 롤러가 돌아가다 멈추면 이벤트가 발생하며 멈춘아이템의 인덱스를 건넨다
	return ret;
}

void XWnd::CallEvent( const char *cFunc )
{
	XLua *pLua = XE::GetMain()->GetpGame()->GetpLua();
	if( pLua )
	{
		if( pLua->IsHaveFunc( cFunc ) )
		{
			CONSOLE( "%s: idWnd=%d", Convert_char_To_TCHAR( cFunc ), GetID() );
			pLua->Call<void, XWnd*>( cFunc, this );
		}
	}
}

void XWnd::OnLButtonDown( float lx, float ly ) 
{ 
	if( GetbShow() == FALSE )		return;	// hide상태에선 본인과 자식들모두 감춤
	if( GetbActive() == FALSE )		return;	// deActive상태에선 본인과 자식들의 입력을 모두 막음.
	if( GetbEnable() == FALSE )	return;
	if( GetDestroy() )		return;		// 파괴 예약된 상태에선 입력도 모두 막음
	XE::VEC2 vMouseLocal( lx, ly );
	if( m_pModalDlg ) 
	{
		XE::VEC2 vChildLocal;
		if( m_pParent )
			vChildLocal = vMouseLocal - (m_pModalDlg->GetPosLocal() * GetScaleLocal());
		else
			vChildLocal = vMouseLocal - m_pModalDlg->GetPosLocal();
		if( m_pModalDlg->IsWndAreaIn( vChildLocal ) )
			m_pModalDlg->OnLButtonDown( vChildLocal.x, vChildLocal.y );
		return;
	}
	list<XWnd*>::reverse_iterator iter;
	for( iter = m_listItems.rbegin(); iter != m_listItems.rend(); iter++ )
	{
		XWnd *pWnd = (*iter);
		if( pWnd->GetbShow() ) 
		{
			XWnd *pPushWnd = NULL;
			XE::VEC2 vChildLocal;
			if( m_pParent )
				vChildLocal = vMouseLocal - (pWnd->GetPosLocal() * GetScaleLocal());
			else
				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
			if( pWnd->IsWndAreaIn( vChildLocal ) || pWnd->GetSizeLocal().IsMinus() )	// 라디오그룹같은건 사이즈가 -1이다
			{
				if( pWnd->GettimerPush().IsOff() )	// 타이머가 꺼져있을때만
					pWnd->GettimerPush().Set(0);	// 타이머 돌기 시작
				if( pWnd->GetbActive() && pWnd->GetbEnable() && pWnd->GetDestroy() == FALSE )
				{
					m_pwndLastLDown = pWnd;
					// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
					if( XWnd::IsAllowClickWnd( pWnd ) )
					{
						pWnd->OnLButtonDown( vChildLocal.x, vChildLocal.y );	// 차일드에는 상대좌표로 넘겨줌
						if( pWnd->GetSizeLocal().IsMinus() == FALSE )
							return;
						else
						{
							int a = 0;
						}
					}
				}
			}
			if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				return;
		}
	}
	m_pwndLastLDown = NULL;
}

void XWnd::OnMouseMove( float lx, float ly ) 
{
	if( GetbShow() == FALSE )		return;	// hide상태에선 본인과 자식들모두 감춤
	if( GetbActive() == FALSE )		return;	// deActive상태에선 본인과 자식들의 입력을 모두 막음.
	if( GetbEnable() == FALSE )	return;
	XE::VEC2 vMouseLocal( lx, ly );
	if( m_pCapture )
	{
		XBREAK( m_pParent != NULL );
		XE::VEC2 vChildLocal = vMouseLocal - m_pCapture->GetPosFinal();
		m_pCapture->OnMouseMove( vChildLocal.x, vChildLocal.y );
		return;
	}
	if( m_pModalDlg ) 
	{
		XE::VEC2 vChildLocal;
		if( m_pParent )
			vChildLocal = vMouseLocal - (m_pModalDlg->GetPosLocal() * GetScaleLocal());
		else
		{
			vChildLocal = vMouseLocal - m_pModalDlg->GetPosLocal();
			if( m_pModalDlg->IsWndAreaIn( vChildLocal ) )
				return m_pModalDlg->OnMouseMove( vChildLocal.x, vChildLocal.y );
		}
	}
	list<XWnd*>::reverse_iterator iter;
	for( iter = m_listItems.rbegin(); iter != m_listItems.rend(); iter++ )
	{
		XWnd *pWnd = (*iter);
		if( pWnd->GetbShow() ) 
		{
			XWnd *pWndMove = NULL;
			XE::VEC2 vChildLocal;
			if( m_pParent )
				vChildLocal = vMouseLocal - (pWnd->GetPosLocal() * GetScaleLocal());
			else
				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
			if( pWnd->IsWndAreaIn( vChildLocal ) || pWnd->GetSizeLocal().IsMinus()  )
			{
				if( pWnd->GetbActive() && pWnd->GetbEnable() )
				{
					if( GetCapture() != pWnd )		// 캡쳐윈도우가 있다면 최상위윈도우에서 처리했을것이므로 중복 실행 하지 않음.
						pWnd->OnMouseMove( vChildLocal.x, vChildLocal.y );
					return;
				}
//						return pWndMove;
			} else
			{
				m_bToolTip = FALSE;
			}
			// NC이벤트는 영역체크에 상관없이 호출된다
			if( pWnd->GetbActive() && pWnd->GetbEnable() )	
				pWnd->OnNCMouseMove( vChildLocal.x, vChildLocal.y );
			if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				return;
		}
	}
}
void XWnd::OnLButtonUp( float lx, float ly ) 
{ 
	m_pwndLastLDown = NULL;
	if( GetbShow() == FALSE )		return;	// hide상태에선 본인과 자식들모두 감춤
	if( GetbActive() == FALSE )		return;	// deActive상태에선 본인과 자식들의 입력을 모두 막음.
	if( GetbEnable() == FALSE )	return;
	XE::VEC2 vMouseLocal( lx, ly );
	if( m_pCapture )
	{
		XBREAK( m_pParent != NULL );
		XE::VEC2 vChildLocal = vMouseLocal - m_pCapture->GetPosFinal();
		m_pCapture->OnLButtonUp( vChildLocal.x, vChildLocal.y );
		return;
	}
	if( m_pModalDlg ) 
	{
		ID id=0;
		XE::VEC2 vChildLocal;
		if( m_pParent )
			vChildLocal = vMouseLocal - (m_pModalDlg->GetPosLocal() * GetScaleLocal());
		else
		{
			vChildLocal = vMouseLocal - m_pModalDlg->GetPosLocal();
			if( m_pModalDlg->IsWndAreaIn( vChildLocal ) )
				m_pModalDlg->OnLButtonUp( vChildLocal.x, vChildLocal.y );
			if( m_pModalDlg->GetDestroyFinal() )	// 다이얼로그는 ok버튼 등을 누르면 자동으로 꺼짐
				DestroyModalDlg();
		}
		return;
	}
	// tooltip clear
	m_timerPush.Off();
	m_bToolTip = FALSE;
	TOOLTIP = NULL;
	// child
	list<XWnd*>::reverse_iterator iter;
	for( iter = m_listItems.rbegin(); iter != m_listItems.rend(); iter++ )
	{
		XWnd *pWnd = (*iter);
		if( pWnd->GetbShow() )
		{
			XE::VEC2 vChildLocal;
			if( m_pParent )
				vChildLocal = vMouseLocal - (pWnd->GetPosLocal() * GetScaleLocal());
			else
				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
			pWnd->GettimerPush().Off();
			pWnd->SetbToolTip( FALSE );
			if( pWnd->IsWndAreaIn( vChildLocal ) || pWnd->GetSizeLocal().IsMinus() )
			{
				if( pWnd->GetbActive() && pWnd->GetbEnable() )
				{
					m_pwndLastLUp = pWnd;
					if( GetCapture() != pWnd )
					{
						// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
						if( XWnd::IsAllowClickWnd( pWnd ) )
							pWnd->OnLButtonUp( vChildLocal.x, vChildLocal.y ); 
					}
					if( pWnd->GetSizeLocal().IsMinus() == FALSE )
						return;
					else
					{
						int a = 0;
					}
				}
			}
			// NC이벤트는 영역체크에 상관없이 호출된다
			if( pWnd->GetbActive() && pWnd->GetbEnable() )
				pWnd->OnNCLButtonUp( vChildLocal.x, vChildLocal.y ); 
			if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				return;
		}
	}
	m_pwndLastLUp = NULL;
	return;
}

BOOL XWnd::OnKeyDown( int keyCode ) 
{ 
	return FALSE;
}
BOOL XWnd::OnKeyUp( int keyCode ) 
{ 
	if( GetbShow() == FALSE )		return FALSE;	// hide상태에선 입력받지 않음.
	if( GetbActive() == FALSE )		return FALSE;	// deActive상태에선 입력받지 않음.
	if( GetbEnable() == FALSE )	return FALSE;
	if( GetDestroy() )		return FALSE;		// 파괴 예약된 상태에선 입력도 모두 막음
	list<XWnd*>::reverse_iterator iter;
	for( iter = m_listItems.rbegin(); iter != m_listItems.rend(); iter++ )
	{
		XWnd *pWnd = (*iter);
		if( pWnd->GetbShow() ) 
		{
			if( pWnd->GetbActive() && pWnd->GetbEnable() && pWnd->GetDestroy() == FALSE )
			{
				// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
				if( XWnd::IsAllowClickWnd( pWnd ) )
				{
					BOOL bCatched = pWnd->OnKeyUp( keyCode );	
					// pWnd가 키다운 이벤트를 캐치해서 썼다.
					if( bCatched )
						return TRUE;
				}
			}
			if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				return FALSE;
		}
	}
	return FALSE;
}


// frame리소스로 this의 크기에 맞는 프레임을 그려준다
void XWnd::DrawFrame( const XWND_RES_FRAME& frame )
{
	// 블럭으로 쪼개진 Frame리소스를 타일화 해서 찍음
	XE::VEC2 vPos = GetPosFinal();		// 좌상귀 좌표(기준좌표)는 스케일에 관계없이 똑같다
	XE::VEC2 vScale = GetScaleFinal();		// 이 윈도우 최종스케일값
	XE::VEC2 vSize = GetSizeFinal();		// 이 윈도우 최종사이즈(스케일이 적용된)
	XE::VEC2 vOrigSize = m_vSize;

	XE::VEC2 vBlockSize;
	vBlockSize.Set( frame.nBlockSize );	// 계산을 단순화하기 위해 블럭사이즈도 스케일1.0을 기준으로 좌표를 지정하고 최종적으로 스케일을 적용하자
	if( (vOrigSize.w - (float)((int)(vOrigSize.w / vBlockSize.w) * vBlockSize.w)) > 0 )
		vOrigSize.w = ((int)(vOrigSize.w / vBlockSize.w) + 1) * vBlockSize.w;
	if( (vOrigSize.h - (float)((int)(vOrigSize.h / vBlockSize.h) * vBlockSize.h)) > 0 )
		vOrigSize.h = ((int)(vOrigSize.h / vBlockSize.h) + 1) * vBlockSize.h;
	XE::VEC2 vSec = (vOrigSize / vBlockSize);		// 스케일등을 고려하지 않은 순수한 오리지날 사이즈
	vSec.x = (float)((int)vSec.x);
	vSec.y = (float)((int)vSec.y);
	XE::VEC2 vSizeHalf = vOrigSize / 2;
	XSurface *psfc = NULL;
	float lx, ly;
	// 프레임은 빼고 중간 블럭으로 size를 채운다
	for( int i = 0; i < (int)vSec.y; i ++ )	// 블럭사이즈로 나누고 남을수 있으니 +1을 더한다
	{
		for( int j = 0; j < (int)vSec.x; j ++ )
		{
			psfc = frame.psfcFrame[ 4 ];	// 중앙 블럭
			psfc->SetScale( vScale );
			lx = j * vBlockSize.x;
			ly = i * vBlockSize.y;
			psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
			psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
		}
	}
	// 좌상귀
	psfc = frame.psfcFrame[ 0 ];	
	psfc->SetScale( vScale );
	lx = 0 * vBlockSize.x - vBlockSize.x;
	ly = 0 * vBlockSize.y - vBlockSize.y;
	psfc->SetAdjustAxis( vSizeHalf );				// 창의 중간으로 변환축을 옮김
	psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	// 우상귀
	psfc = frame.psfcFrame[ 2 ];	
	psfc->SetScale( vScale );
	lx = vSec.x * vBlockSize.x;
	ly = 0 * vBlockSize.y - vBlockSize.y;
	psfc->SetAdjustAxis( vSizeHalf );				// 창의 중간으로 변환축을 옮김
	psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	// 좌하귀
	psfc = frame.psfcFrame[ 6 ];	
	psfc->SetScale( vScale );
	lx = 0 * vBlockSize.x - vBlockSize.x;
	ly = vSec.y * vBlockSize.y;
	psfc->SetAdjustAxis( vSizeHalf );				// 창의 중간으로 변환축을 옮김
	psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	// 우하귀
	psfc = frame.psfcFrame[ 8 ];	
	psfc->SetScale( vScale );
	lx = vSec.x * vBlockSize.x;
	ly = vSec.y * vBlockSize.y;
	psfc->SetAdjustAxis( vSizeHalf );				// 창의 중간으로 변환축을 옮김
	psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	
	// 위/아래
	for( int i = 0; i < (int)vSec.x; i ++ )
	{
		psfc = frame.psfcFrame[ 1 ];	// 위
		psfc->SetScale( vScale );
		lx = i * vBlockSize.x;
		ly = 0 * vBlockSize.y - vBlockSize.y;
		psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
		psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
		//
		psfc = frame.psfcFrame[ 7 ];	// 아래
		psfc->SetScale( vScale );
		ly = vSec.y * vBlockSize.y;
		psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
		psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	}
	// 왼쪽/오른쪽
	for( int j = 0; j < (int)vSec.y; j ++ )
	{
		psfc = frame.psfcFrame[ 3 ];	// 왼쪽
		psfc->SetScale( vScale );
		lx = 0 * vBlockSize.x - vBlockSize.x;
		ly = j * vBlockSize.y;
		psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
		psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
		//
		psfc = frame.psfcFrame[ 5 ];	// 아래
		psfc->SetScale( vScale );
		lx = vSec.x * vBlockSize.x;
		psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
		psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	}
} // drawframe

BOOL XWnd::LoadResFrame( LPCTSTR szFile, XWND_RES_FRAME *pFrame, int surfaceBlockSize )
{
	_tcscpy_s( pFrame->szFrame, szFile );
	LPCTSTR szRes = XE::MakePath( DIR_UI, szFile );
	XASSERT( surfaceBlockSize == 0 || surfaceBlockSize == 16 || surfaceBlockSize == 32 || surfaceBlockSize == 64 );
	pFrame->nBlockSize = surfaceBlockSize;
	// png를 읽음
#ifdef _NEW_IMAGE
    XImage image( TRUE );
#else
#ifdef WIN32
	XImageD3D<DWORD> image( TRUE );		// UI리소스는 모두 고해상도 리소스를 사용
#else
	XImageiOS<DWORD> image( TRUE );
#endif
#endif
	if( image.Load( szRes ) == FALSE )
		return FALSE;
	// 블럭사이즈 자동계산
	if( surfaceBlockSize == 0 )
	{
		XBREAKF( (image.GetWidth() % 3) != 0, "%s의 이미지가 3으로 나누어 떨어져야 합니다", szRes );
		pFrame->nBlockSize = surfaceBlockSize = image.GetWidth() / 3;
	}
	int iw = image.GetMemWidth();		// 이미지 메모리 크기
	int ih = image.GetMemHeight();
	int memBlockSize = image.ConvertToMemSize( (float)surfaceBlockSize );	// 실제 메모리 블럭 사이즈
	// 9개의 조각으로 서피스를 나눔
	DWORD *pImg = image.GetTextureData();
	pFrame->psfcFrame[0] = GRAPHICS->CreateSurface( TRUE, 0, 0, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );		// 첫째줄
	pFrame->psfcFrame[1] = GRAPHICS->CreateSurface( TRUE, memBlockSize, 0, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[2] = GRAPHICS->CreateSurface( TRUE, memBlockSize*2, 0, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[3] = GRAPHICS->CreateSurface( TRUE, 0, memBlockSize, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );		// 둘째줄
	pFrame->psfcFrame[4] = GRAPHICS->CreateSurface( TRUE, memBlockSize, memBlockSize, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[5] = GRAPHICS->CreateSurface( TRUE, memBlockSize*2, memBlockSize, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[6] = GRAPHICS->CreateSurface( TRUE, 0, memBlockSize*2, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );		// 셋째줄
	pFrame->psfcFrame[7] = GRAPHICS->CreateSurface( TRUE, memBlockSize, memBlockSize*2, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[8] = GRAPHICS->CreateSurface( TRUE, memBlockSize*2, memBlockSize*2, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	return TRUE;
}

static bool CompPriority( XWnd *p1, XWnd *p2 )
{
	if( p1->GetPriority() < p2->GetPriority() )
		return true;
	return false;
}
// draw priority순으로 차일드들을 소트한다.
void XWnd::SortPriority( void )
{
	m_listItems.sort( CompPriority );
}


////////////////////////////////////////////////////////////////
// dialog
/*XWndButton* XWndDialog::AddButton( ID id, float x, float y, LPCTSTR szText, LPCTSTR szSpr, ID idActUp, ID idActDown  )
{
	XWndButtonString *pButton = new XWndButtonString( x, y, szText, XCOLOR_WHITE, XWnd::s_pFontDat, GetpWndMng()->GetpSprObj()->GetszFilename(), idActUp, idActDown );
	pButton->SetColorText( XCOLOR_WHITE );
	Add( id, pButton );
	return pButton;
}*/
/*XWndButton* XWndDialog::AddButton( ID id, const XE::VEC2& vPos, const XE::VEC2& size, LPCTSTR szText )
{
	XWndDynamicButtonString *pButton = new XWndDynamicButtonString( vPos.x, vPos.y, size.w, size.h, szText, m_pFontDat );
	pButton->SetColorText( XCOLOR_WHITE );
	Add( id, pButton );
	return pButton;
}*/
XWndButton* XWndDialog::AddButton( ID id, float x, float y, LPCTSTR szSpr, ID idActUp, ID idActDown  )
{
	XWndButton *pButton = new XWndButton( x, y, szSpr, idActUp, idActDown );
	Add( id, pButton );
	return pButton;
}
// static text
XWnd* XWndDialog::AddStaticText( ID id, float x, float y, float w, float h, LPCTSTR szText, XBaseFontDat *pFontDat, XE::xAlign align, XCOLOR col, xFONT::xtStyle style )
{
	w = _tcslen( szText ) * 6.f;	// 10=font width(임시)
	h = 10;
	XWndTextString *wndStr = new XWndTextString( x, y, szText, pFontDat, col );
	wndStr->SetAlign( align );
	wndStr->SetStyle( style );
	return Add( id, wndStr );
}

int XWndDialog::Process( float dt )
{
	return XWndView::Process( dt );
}
void XWndDialog::Draw( void )
{
	if( m_psfcFrame )
	{
		XWndView::Draw();	
	} else
	{
		XWND_RES_FRAME *pRes;
		if( m_frameImage.nBlockSize )
			pRes = &m_frameImage;
		else
			XBREAK(1);
			//pRes = &GetpWndMng()->GetRes().frameDialog;
		XWnd::DrawFrame( *pRes );
		if( m_nStateAppear == 1 )		// Appear상태가 끝나면 차일드를 그린다
			XWnd::Draw();	
	}
}


void XWndDialog::OnLButtonUp( float lx, float ly ) 
{
	XWnd::OnLButtonUp( lx, ly );
}
//
/*int XWndDialog::LuaDoModal( lua_State *L )
{
	GetpWndMng()->SetModalDialog( this );		// 자신(Dialog)을 모달다이얼로그로 등록시킴
	return lua_yield( L, 0 );
}*/
//===========================================================
void XWndImage::Create( BOOL bHighReso, LPCTSTR szRes )
{
	m_pSurface = IMAGE_MNG->Load( bHighReso, szRes );
	XBREAKF( m_pSurface == NULL, "create fail. %s", szRes );
	m_bCreate = TRUE;
}
void XWndImage::Draw( void )
{
	if( m_pSurface )
	{
		XE::VEC2 vPos = GetPosFinal() + m_vAdjust;
		XE::VEC2 vScale = GetScaleFinal();
		m_pSurface->SetRotateY( m_dAngY );
		m_pSurface->SetRotateZ( m_dAngZ );
		m_pSurface->SetAdjustAxis( m_vAdjustAxis );
		m_pSurface->SetScale( vScale );
		m_pSurface->SetfAlpha( GetAlphaFinal() );
		m_pSurface->Draw( vPos );
	}
	XWnd::Draw();
}

void XWndImage::Draw( const XE::VEC2& vPos )
{
	if( m_pSurface )
	{
		XE::VEC2 v = vPos + m_vAdjust;
		XE::VEC2 vScale = GetScaleFinal();
		m_pSurface->SetAdjustAxis( m_vAdjustAxis );
		m_pSurface->SetScale( vScale );
		m_pSurface->SetfAlpha( GetAlphaFinal() );
		m_pSurface->Draw( v );
	}
	XWnd::Draw( vPos );
}

void XWndImage::Draw( XE::VEC2& vPos, xDM_TYPE drawMode )
{
	if( m_pSurface )
	{
		XE::VEC2 v = vPos + m_vAdjust;
		m_pSurface->SetAdjustAxis( m_vAdjustAxis );
		m_pSurface->SetfAlpha( GetAlphaFinal() );
		m_pSurface->SetDrawMode( drawMode );
		m_pSurface->Draw( v );
	}
//	XWnd::Draw();
}
//----------------------------------------------------------
XWndView::XWndView( XLayout *pLayout, const char *cKey, XWnd *pParent, const char *cGroup ) 
{
	Init();
	pLayout->CreateLayout( cKey, pParent, cGroup );
}

BOOL XWndView::LoadRes( LPCTSTR szFrame )
{
	XBREAK( szFrame == NULL );
    if( szFrame )
	{
		XWnd::LoadResFrame( szFrame, &m_frameImage );
		XBREAKF( m_frameImage.nBlockSize == 0, "UI 리소스가 로딩되지 않았습니다" );
		m_nBlockSize = m_frameImage.nBlockSize;	// 편의상 받아옴
		m_bDrawFrame = TRUE;
		XE::VEC2 vSize = GetSizeLocal();
		XE::VEC2 vSector = vSize / (float)m_frameImage.nBlockSize;
		vSector.x = (float)((int)vSector.x);
		vSector.y = (float)((int)vSector.y);
//		vSector += XE::VEC2( 2 );		// 양쪽 프레임까지 합해서 섹터단위 크기
		vSize = vSector * (float)m_frameImage.nBlockSize;
		SetSizeLocal( vSize );
	}
    return TRUE;
}

void XWndView::ProcessAnimationLerp( BOOL bEnter, float timeLerp, float aniLerp )
{
}

BOOL XWndView::ProcessEnterLeaveAnimation( void )
{
	// 애니메이션을 원하지 않는다면.
	if( GetbAnimationEnterLeave() == FALSE && m_nStateAppear == 0 )
	{
		m_nStateAppear = 1;		// 1이 Appear끝
		m_vLT = GetPosFinal();
		SetScaleLocal( 1.0f );		// 최종크기 100%
#ifdef _CHEAT
		m_timerSpeed.Set(0, TRUE );		// 로딩전에 타이머 작동시킴
#endif
		CallEvent( "OnEventAddWnd" );
		OnFinishAppear();
		CallEventHandler( XWM_FINISH_APPEAR );
		return FALSE;
	}
	float slerp = m_timerAppear.GetSlerp();
	if( m_nStateAppear == 0 )		// 창 팝업효과 Init
	{
		SetScaleLocal( 1.0f );
		m_vLT = GetPosFinal();
		SetScaleLocal( 0 );		// 스케일 0부터 시작
//		m_timerAppear.Set( 0.10f );	// 0.15초동안 커진다
		m_timerAppear.Set( 0.05f );	// 0.15초동안 커진다
		m_nStateAppear = 2;	// start
		SetbActive( FALSE );		// 애니메이션중엔 비활성 시킴
	} else
	if( m_nStateAppear == 2 )		// init
	{
		float scale = sinf( D2R(slerp * 90.f) );		// 0 ~ 1까지 사인파형으로 커진다
		SetScaleLocal( scale );	
		if( m_timerAppear.IsOver() ) {
			m_timerAppear.Set( 0.15f );	// 100% +- x%구역에서 0.3초간 띠요요옹 한다
//			m_timerAppear.Set( 0.3f );	// 100% +- x%구역에서 0.3초간 띠요요옹 한다
			//				m_timerAppear.Set( 5.f, TRUE );	// 100% +- x%구역에서 0.3초간 띠요요옹 한다
			m_nStateAppear = 3;		
		}
	} else
	if( m_nStateAppear == 3 )
	{
		float scale = 1.0f + (sinf( D2R(slerp * (360.f*1.f)) ) * 0.2f) * (1.f - slerp);		// 0 -> 1 -> 0 -> -1 -> 0.. slerp에 따라 진폭이 점점 작아진다. 360도까지는 아래위한파동이므로 두파동을 만들기 위해 720도까지 돌림
		SetScaleLocal( scale );	// 110% -> 100%로 작아진다
		if( m_timerAppear.IsOver() ) {
			m_nStateAppear = 1;		// 1이 Appear끝
			SetbActive( TRUE );
			SetbAnimationEnterLeave( FALSE );		// 사라질땐 바로 사라지도록
			SetScaleLocal( 1.0f );		// 최종크기 100%
#ifdef _CHEAT
			m_timerSpeed.Set(0, TRUE );		// 로딩전에 타이머 작동시킴
#endif
			CallEvent( "OnEventAddWnd" );
			OnFinishAppear();
			CallEventHandler( XWM_FINISH_APPEAR );
			return FALSE;
		}
	} else
	if( m_nStateAppear == 1 )
		return FALSE;
	return TRUE;
}

int XWndView::Process( float dt )
{
	{
	}

	XSprObj::s_LuaDt = dt;		// 루아글루에 사용
	return XWnd::Process( dt );
}

void XWndView::Draw( void )
{
	if( m_nStateAppear == 0 )		// 아직 Process를 거치지 않은 상태라서 찍지 않음.
		return;
	if( m_psfcFrame )
	{
		XE::VEC2 vScale = GetScaleFinal();
		XE::VEC2 vSize = m_psfcFrame->GetSize();
		m_psfcFrame->SetAdjustAxis( vSize / 2.f );
		m_psfcFrame->SetScale( vScale );
		m_psfcFrame->Draw( m_vLT );
	} else
	{
		if( m_bDrawFrame )
		{
			if( m_frameImage.nBlockSize )
			{
				XWND_RES_FRAME *pRes = &m_frameImage;
				XWnd::DrawFrame( *pRes );
			}
		} else
		if( m_colBg )
		{
			XE::VEC2 vPos = m_vLT; // GetPosFinal();
			XE::VEC2 vSize = GetSizeLocal(); // GetSizeFinal();
			XE::VEC2 vScale = GetScaleFinal();
			XE::VEC2 vCenter = vPos + vSize / 2.f;
			XE::VEC2 vLT = vCenter + (vPos - vCenter) * vScale;
			vSize *= vScale;
			GRAPHICS->FillRectSize( vLT, vSize, m_colBg );
			GRAPHICS->DrawRectSize( vLT, vSize, m_colBorder );
		}
	}
	if( m_nStateAppear == 1 )		// Appear상태가 끝나면 차일드를 그린다
	{
		if( m_prefLua )
			m_prefLua->Call<void,XWndView*>( m_cDrawHandler, this );	// lua의 draw handler를 호출
		//
		XWnd::Draw();		// child 그림
#ifdef _CHEAT
		if( m_timerSpeed.IsOn() )
		{
			m_timeLoad = m_timerSpeed.GetPassTime();
			m_timerSpeed.Off();
		}
		XE::VEC2 vPos = GetPosFinal();
		XE::VEC2 vSize = GetSizeFinal();
//		PUT_STRINGF( vPos.x, vPos.y + vSize.h - 20.f, XCOLOR_WHITE, "%d", m_timeLoad );
#endif
	}
}

//------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// 등장 이펙트

int XWndEffectGradually::Process( float dt )
{
	switch( m_State )
	{
	case 0:	break;		// 암것도 안하는 상태
	case 2:	break;		// 암것도 안하는 상태
	case 1:				// 진행중
		{
			XE::VEC2 vPosLocal = GetPosLocal();
			float lerpTime = (float)m_Timer.GetPassTime() / m_Timer.GetWaitTime();	// 진행 시간
			if( lerpTime > 1.0f )
				lerpTime = 1.0f;
			float slerp = XE::xiCatmullrom( lerpTime, -10.0f, 0, 1, 1 );		// 스플라인 방식 보간
			XE::VEC2 vLen = m_vPosEnd - m_vPosStart;		// 이동구간의 길이벡터
			XE::VEC2 vPos = m_vPosStart + vLen * slerp;	// 현재 좌표 계산
			float scale = slerp;		// 현재 스케일링
			SetPosLocal( vPos );		// 좌표 갱신
			SetScaleLocal( scale );	// 크기 갱신
			float lerpAlpha = XE::xiHigherPowerDeAccel( lerpTime, 1.0f, 0 );
			SetAlphaLocal( lerpAlpha );	// 알파 갱신. 알파는 선형보간 하자
			if( lerpTime >= 1.0f )
				m_State = 2;		// 끝
		}
		break;
	}
	return 1;		// 현재 사용안하는듯
}
//////////////////////////////////////////////////////////////////
// 슬라이더 컨트롤
void XWndSlider::OnLButtonDown( float lx, float ly )
{
	m_Push = 1;
	XE::VEC2 size = GetSizeFinal();
	float lerp = lx / size.w;		// 0 ~ 1로 환산
	float w = m_Max - m_Min;
	m_Curr = m_Min + w * lerp;
	CallEventHandler( XWM_SLIDING, *((DWORD*)(&m_Curr)) );
	SetCapture( this );
}
void XWndSlider::OnMouseMove( float lx, float ly )
{
	if( m_Push )
	{
		XE::VEC2 size = GetSizeFinal();
		float lerp = lx / size.w;		// 0 ~ 1로 환산
		float w = m_Max - m_Min;
		m_Curr = m_Min + w * lerp;
		if( m_Curr < m_Min )
			m_Curr = m_Min;
		else
		if( m_Curr > m_Max )
			m_Curr = m_Max;
		m_Push = 2;
	CallEventHandler( XWM_SLIDING, *((DWORD*)(&m_Curr)) );
	}
}
void XWndSlider::OnLButtonUp( float lx, float ly )
{
	ReleaseCapture();
	m_Push = 0;
	CallEventHandler( XWM_SLIDING, *((DWORD*)(&m_Curr)) );
}
/////////////////////////////////////////////////////////////////////////////////////
XWndNotification::XWndNotification( float w, float h, LPCTSTR szTitle, LPCTSTR szMsg, XBaseFontDat *pFontDat ) 
	: XWndView( 0, 0, 0, 0, NULL ) 
{ 
	Init(); 
	float lw=0;
	XWndTextString *pWnd1=NULL, *pWnd2=NULL; 
	if( szTitle )
	{
//		_tcscpy_s( m_szTitle, szTitle );
		pWnd1 = new XWndTextString( w/2.f, 5.f, szTitle, pFontDat, XCOLOR_WHITE );
		Add( 1, pWnd1 );
		pWnd1->SetAlign( XE::xALIGN_CENTER );
		XE::VEC2 size = pWnd1->GetpFont()->GetLayoutSize( szTitle );
		lw = xmax( lw, size.x );
	}
	if( szMsg ) 
	{
//		_tcscpy_s( m_szMsg, szMsg );
		pWnd2 = new XWndTextString( w/2.f, 20.f, szMsg, pFontDat, XCOLOR_WHITE );
		Add( 2, pWnd2 );
		pWnd2->SetAlign( XE::xALIGN_CENTER );
		XE::VEC2 size = pWnd2->GetpFont()->GetLayoutSize( szMsg );
		lw = xmax( lw, size.x );
	}
	//
	if( pWnd1 )
		pWnd1->SetPosLocal( lw/2.f, 5.f );
	if( pWnd2 )
		pWnd2->SetPosLocal( lw/2.f, 20.f );
	
	SetSize( lw, 32.f );
	m_Timer.Set( 3.0f );
	//
}
void XWndNotification::Destroy() 
{
//	SAFE_DELETE( m_pTitle );
//	SAFE_DELETE( m_pMsg );
}
void XWndNotification::Draw( void )
{
	XWndView::Draw();
//	if( m_pTitle )
//		m_pTitle->Draw();
//	if( m_pMsg )
//		m_pMsg->Draw();
}


// 팝업을 Yes No 메시지창 형태로 만든다
/*void XWndPopup::SetYesNo( LPCTSTR szText ) 
{
	SetText( szText );
	XE::VEC2 sizeFont( 3, 8 );	// freetype font의 폰트 사이즈
	SetDialogSize( _tcslen( szText ) * sizeFont.x, 50.f );		// 다이얼로그 크기를 지정하고
	XE::VEC2 vSize = GetSizeFinal();
	XE::VEC2 vScale = GetScaleFinal();
//		XE::VEC2 sizeButton = sizeFont * XE::VEC2( 8.0f, 2.0f ) * vScale;	// 스트링 길이. 1.5:'YES'각글자를 0.5씩으로 보고..
	XE::VEC2 sizeButton = XE::VEC2( 32, 16 ) * vScale;	// 스트링 길이. 1.5:'YES'각글자를 0.5씩으로 보고..
	XE::VEC2 vPos;
	vPos.x = vSize.x / 4 - sizeButton.w / 2;		// 팝업 가로크기를 4등분해서 왼쪽의 중앙에 위치
	vPos.y = vSize.y / 2 + (vSize.y / 2) / 2 - sizeButton.h / 2;	// 팝업 세로를 2등분해서 아랫부분의 중앙에 배치
	AddButton( XWndDialog::xYES, vPos, sizeButton, XTEXT(18) );		// yes button
	vPos.x = (vSize.x / 4) * 3 - sizeButton.w / 2;		// 팝업 가로크기를 4등분해서 오른쪽의 중앙에 위치
	AddButton( XWndDialog::xNO, vPos, sizeButton, XTEXT(19) );		// no button
}
// 팝업을 Yes No 메시지창 형태로 만든다
void XWndPopup::SetOkCancel( LPCTSTR szText ) 
{
	SetText( szText );
	XE::VEC2 sizeFont( 8, 8 );	// freetype font의 폰트 사이즈
	SetDialogSize( _tcslen( szText ) * sizeFont.x, 50.f );		// 다이얼로그 크기를 지정하고
	XE::VEC2 vSize = GetSizeFinal();
	XE::VEC2 vScale = GetScaleFinal();
	XE::VEC2 sizeButton = sizeFont * XE::VEC2( 3.0f, 1.0f ) * vScale;	// 스트링 길이. 1.5:'cancel'각글자를 0.5씩으로 보고..
	XE::VEC2 vPos;
	vPos.x = vSize.x / 4 - sizeButton.w / 2;		// 팝업 가로크기를 4등분해서 왼쪽의 중앙에 위치
	vPos.y = vSize.y / 2 + (vSize.y / 2) / 2 - sizeButton.h / 2;	// 팝업 세로를 2등분해서 아랫부분의 중앙에 배치
	AddButton( XWndDialog::xYES, vPos, sizeButton, XTEXT(20) );		// ok button
	vPos.x = (vSize.x / 4) * 3 - sizeButton.w / 2;		// 팝업 가로크기를 4등분해서 오른쪽의 중앙에 위치
	AddButton( XWndDialog::xNO, vPos, sizeButton, XTEXT(21) );		// cancel button
}
//
void XWndPopup::SetOk( LPCTSTR szText ) 
{
	SetText( szText );
	XE::VEC2 sizeFont( 8, 8 );	// freetype font의 폰트 사이즈
	SetDialogSize( _tcslen( szText ) * sizeFont.x, 50.f );		// 다이얼로그 크기를 지정하고
	XE::VEC2 vSize = GetSizeFinal();
	XE::VEC2 vScale = GetScaleFinal();
	XE::VEC2 sizeButton = sizeFont * XE::VEC2( 3.0f, 1.0f ) * vScale;	
	XE::VEC2 vPos;
	vPos.x = vSize.x / 4 - sizeButton.w / 2;		// 팝업 가로크기를 4등분해서 왼쪽의 중앙에 위치
	vPos.y = vSize.y / 2 + (vSize.y / 2) / 2 - sizeButton.h / 2;	// 팝업 세로를 2등분해서 아랫부분의 중앙에 배치
	AddButton( XWndDialog::xOK, vPos, sizeButton, XTEXT(20) );		// ok button
}*/
/*
XWndButton* XWndPopup::AddMenu( ID id, LPCTSTR szText, int idActUp, int idActDown )
{
	if( idActDown == 0 )
		idActDown = idActUp + 1;	// 특별히 지정하지 않으면 down은 up다음 아이디로 한다
	// 메뉴 버튼 생성	
	XWndButton *pButton = NULL;
	if( szText )
		pButton = AddButton( id, 5.f, m_yAuto, szText, idActUp, idActDown );		// 외부에서 폰트객체를 넘겨받을수 있게 해야할듯
	else
		pButton = AddButton( id, 5.f, m_yAuto, idActUp, idActDown );

	// 다음 메뉴를 위해 좌표 내려감.
	if( XASSERT( GetpWndMng()->GetpSprObj() ) )
	{
//		XE::VEC2 size = GetpWndMng()->GetpSprObj()->GetSize( idActUp );
		XE::VEC2 size( 140, 45 );
		m_yAuto += size.h + 1.0f;
		// Popup메뉴창 크기
		SetDialogSize( size.x + 10, m_yAuto+5 );
	}
	return pButton;
}
*/

// 팝업 메뉴
XWndPopupMenu::XWndPopupMenu( const XE::VEC2 vPos, const XE::VEC2 vSize, LPCTSTR szRes ) 
	: XWndView( vPos.x, vPos.y, vSize.w, vSize.h, szRes )
{ 
	Init(); 
	XE::VEC2 v = vPos;
	if( v.y < 3 )
		v.y = 3;
	if( v.x < 3 )
		v.x = 3;
	if( v.y + vSize.h > GRAPHICS->GetScreenHeight() )
		v.y -= vPos.y + vSize.h - GRAPHICS->GetScreenHeight() - 3;
	if( v.x + vSize.w > GRAPHICS->GetScreenWidth() )
		v.x -= v.x + vSize.w - GRAPHICS->GetScreenWidth() - 3;
	SetPosLocal( v );
	//
	m_ctrlList = new XWndList( 0, 0, vSize.w, vSize.h );
	Add( m_ctrlList );
}

void XWndPopupMenu::OnNCLButtonUp( float lx, float ly )
{
	XE::VEC2 vSize = GetSizeFinal();
	if( lx < 0 || ly < 0 || lx > vSize.w || ly > vSize.h )
	{
		// 윈도우 이외의 영역을 클릭함
		SetbDestroy( TRUE );			// 팝업메뉴는 메뉴영역 밖을 클릭하면 꺼진다.
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
void XWndPopupMenuText::Destroy()
{
//	XLIST_DESTROY( m_listText, XWndTextString* );
}
void XWndPopupMenuText::AddItem( ID idWnd, LPCTSTR szText )
{
	XE::VEC2 vSize = GetSizeFinal();
	vSize.h = m_hElem;
	XWndTextString *pText = new XWndTextString( vSize, szText, m_pFontDat, XCOLOR_WHITE );
	m_ctrlList->AddItem( idWnd, pText );
	m_ctrlList->SetbShow( FALSE );
}

void XWndPopupMenuText::Draw( void )
{
	XWndPopupMenu::Draw();
	if( XWndView::IsAnimation() == FALSE )
	{
		m_ctrlList->SetbShow( TRUE );
	}
}

void XWndPopupMenuText::OnLButtonDown( float lx, float ly )
{
	XWndPopupMenu::OnLButtonDown( lx, ly );
}
void XWndPopupMenuText::OnMouseMove( float lx, float ly )
{
	XWndPopupMenu::OnMouseMove( lx, ly );
}

void XWndPopupMenuText::OnLButtonUp( float lx, float ly )
{
	XWndPopupMenu::OnLButtonUp( lx, ly );
}

////////////////////////////////////////////////////////////////////////////////////////
int XWndScrollView::Process( float dt )
{
	Scroll();
	return XWnd::Process( dt );
}
void XWndScrollView::Scroll( void ) 
{
	XE::VEC2 vSize = GetSizeFinal();
	if( m_bLButtonDown )
	{
		m_vScroll.y = m_vMouse.y - m_vPrev.y;
		if( m_vScroll.y > 0 && m_vScroll.y > 4.f )
			m_vScroll.y = 15.0f;
		if( m_vScroll.y < 0 && m_vScroll.y < -4.f )
			m_vScroll.y = -15.0f;
		if( m_vAdjust.y > 0 )
			m_vScroll.y /= 2.0f;
		if( m_vAdjust.y < vSize.y - m_vTotalSize.y )
			m_vScroll.y /= 2.0f;
		if( m_vScroll.IsZero() == FALSE )
		{
			m_Timer.Set(0);		// 스크롤중에는 타이머가 계속 리셋된다
			m_vScrollLast = m_vScroll;
		}
	} else
	{
		if( m_bLButtonUp )
		{
			// 뗀순간에 타이머가 0.3초가 넘어있으면 스크롤값을 정지시키고.
			// 그 이하라면 마지막으로 스크롤되던 값을 넣어서 계속 스크롤 되게 한다.
			if( m_Timer.IsOver(0.1f) )
				m_vScroll.Set( 0 );
			else
			{
				if( m_vScroll.IsZero() )
					m_vScroll = m_vScrollLast;
			}
			m_bLButtonUp = FALSE;
		}
	}
	m_vPrev = m_vMouse;
	m_vAdjust.x += m_vScroll.x;
	m_vAdjust.y += m_vScroll.y;
	if( m_vAdjust.x > 0 )
		m_vAdjust.x = 0;
	//
	if( m_bLButtonDown == FALSE )		// 손을 놨을때
	{
		if( m_vAdjust.y > 0 )
		{
			m_vScroll.y -= 1.f;
			if( m_vScroll.y < 0 )
				m_vScroll.y = 0;
			m_vAdjust.y -= m_vAdjust.y / 5.f;
			if( m_vAdjust.y < 0 )
				m_vAdjust.y = 0;
		} else
		if( m_vAdjust.y < vSize.y - m_vTotalSize.y )
		{
			m_vScroll.y += 1.f;
			if( m_vScroll.y > 0 )
				m_vScroll.y = 0;
			m_vAdjust.y += ((vSize.y - m_vTotalSize.y) - m_vAdjust.y) / 5.f;
			if( m_vAdjust.y > 0 )
				m_vAdjust.y = 0;
		}
 
		if( m_vScroll.y > 0 )
		{
			m_vScroll.y -= 0.1f;
			if( m_vScroll.y < 0 )
				m_vScroll.y = 0;
		} else
		if( m_vScroll.y < 0 )
		{
			m_vScroll.y += 0.1f;
			if( m_vScroll.y > 0 )
				m_vScroll.y = 0;
		}
	}

}
void XWndScrollView::Draw( void )
{
	{
		XE::VEC2 vPos = XWnd::GetPosFinal();
		XE::VEC2 vSize = GetSizeFinal();
		float ratioH = vSize.h / m_vTotalSize.h;
		float hBar = vSize.h * ratioH;		// 스크롤바 길이
		float ratioY = -m_vAdjust.y / m_vTotalSize.h;
		float yBar = vSize.h * ratioY;
		XE::VEC2 v, vs;
		v.x = vSize.w - 3.f;
		v.y = yBar;
		vs.w = 3.f;
		vs.h = hBar;
		if( hBar < 0 )
			hBar = 0;
		if( v.y < 0 )
		{
			vs.h += v.y;
			v.y = 0;
		}
		if( v.y + vs.h > vSize.h )
		{
			vs.h -= (v.y + vs.h) - vSize.h;
		}
		if( vs.h < 0 )
			vs.h = 0;
		BYTE a = (BYTE)( m_Alpha * 255.f );
		GRAPHICS->FillRectSize( vPos + v, vs, XCOLOR_RGBA( 216, 223, 231, a ) );
		if( m_bLButtonDown || !m_vScroll.IsZero() )
		{
			m_Alpha += 0.03f;
			if( m_Alpha > 0.5f )
				m_Alpha = 0.5f;
		} else
		{
			m_Alpha -= 0.03f;
			if( m_Alpha < 0 )
				m_Alpha = 0;
		}
	}
}

void XWndScrollView::OnLButtonDown( float lx, float ly ) 
{
//	XE::VEC2 vMouse( lx, ly );
	XE::VEC2 vMouse = INPUTMNG->GetMousePos();
	m_vMouse = vMouse;
	m_vPrev = vMouse;
	m_bLButtonDown = TRUE;
	m_vScroll = XE::VEC2( 0 );
	XWnd::OnLButtonDown( -m_vAdjust.x + lx, -m_vAdjust.y + ly );
}
void XWndScrollView::OnMouseMove( float lx, float ly ) 
{
//	XE::VEC2 vDist = m_vMouse - m_vPrev;
	XWnd *pWndRet = this;
	if( m_bLButtonDown )
	{
		m_vMouse = INPUTMNG->GetMousePos();
//		m_vMouse = XE::VEC2( lx, ly );	// 터치에선 항상 눌린상태에서 Move가 되기땜에 일부러 안으로 옮김
		m_bScroll = TRUE;
	}
	// 기기에선 언제나 드래그 무브상태다.
	XWnd::OnMouseMove( -m_vAdjust.x + lx, -m_vAdjust.y + ly );
//	if( m_bScroll )
//		m_vScroll.y = -vDist.y;
//		XWndScrollView::Scroll( 0, -vDist.y );/
//	else
//		XWnd::OnMouseMove( -m_vAdjust.x + lx, -m_vAdjust.y + ly );
//	m_vPrev = m_vMouse;
}
void XWndScrollView::OnNCMouseMove( float lx, float ly )
{
//	m_vMouse = XE::VEC2( lx, ly );
}
void XWndScrollView::OnLButtonUp( float lx, float ly ) 
{
	m_bLButtonDown = FALSE;
	m_bLButtonUp = TRUE;
	if( m_bScroll == FALSE )
	{
		m_bScroll = FALSE;
		XWnd::OnLButtonUp( -m_vAdjust.x + lx, -m_vAdjust.y + ly );
	}
	m_bScroll = FALSE;
}
void XWndScrollView::OnNCLButtonUp( float lx, float ly )
{
	m_bLButtonDown = FALSE;
	m_bScroll = FALSE;
	XWnd::OnNCLButtonUp( lx, ly );

}

////////////////////////////////////////////////////////////////////////////////////////
XWndList::XWndList( LPCTSTR szBg, const XE::VEC2& vPos, const XE::VEC2& vLocalxy, 
							const XE::VEC2& vSize, xtType type )
	: XWndScrollView( vPos.x, vPos.y, vSize.w, vSize.h )
{
	Init();
	m_Type = type;

	SetPosLocal( vPos + vLocalxy );
	m_posBg -= vLocalxy;
	if( XE::IsHave( szBg ) )
		m_psfcBg = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szBg ) );
}

void XWndList::Destroy()
{
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
//	XLIST_DESTROY( m_listItem, XWnd* );
}

XWnd* XWndList::AddItem( ID idWnd, XWnd *pWnd ) 
{
	m_listItem.Add( pWnd );
	if( idWnd )
		Add( idWnd, pWnd );
	else
		Add( pWnd );
	XE::VEC2 vSize = pWnd->GetSizeFinal();
	if( m_Type == xTYPE_LIST )
	{
		// 추가한 객체의 세로크기를 계산해서 리스트내의 y좌표를 지정한다.
		pWnd->SetPosLocal( 0.f, m_Y );
		XWndScrollView::SetViewSize( XE::VEC2( GetSizeFinal().w, m_Y + vSize.h ) );
		m_Y += vSize.h;
		m_Y += m_vSpace.h;
	} else
	if( m_Type == xTYPE_THUMB )
	{
		if( m_X + vSize.w > GetSizeFinal().w )
		{
			m_X = 0;
			m_Y += vSize.h;
			m_Y += m_vSpace.h;
		}
		pWnd->SetPosLocal( m_X, m_Y );
		XWndScrollView::SetViewSize( XE::VEC2( m_X + vSize.w, m_Y + vSize.h ) );
		m_X += vSize.w;
		m_X += m_vSpace.w;
	} else
		XBREAK(1);

	return pWnd;
}

void XWndList::Del( XWnd* pWnd ) 
{ 
	m_listItem.Del( pWnd );				// 리스트에서도 지우고
	DestroyID( pWnd->GetID() );		// 윈도우도 지우고
}

void XWndList::DelFromID( ID idWnd ) 
{
	XWnd *pWnd = Find( idWnd );
	XBREAK( pWnd == NULL );
	if( pWnd == NULL )	return;
	Del( pWnd );
}

void XWndList::DestroyAllItem( void )
{
	m_listItem.Clear();
	DestroyChildAll();
	m_X = m_Y = 0;
	XWndScrollView::Clear();
}

//#include "Mainfrm.h"
void XWndList::Draw( void )
{
	XE::VEC2 v = XWnd::GetPosFinal();
	XE::VEC2 size = XWnd::GetSizeFinal();
	if( m_psfcBg )
		m_psfcBg->Draw( v + m_posBg );
	// viewport 설정
	GRAPHICS->BackupViewport();
	GRAPHICS->SetViewport( XE::POINT( v ), XE::POINT( size ) );
	XE::SetProjection( (int)size.w, (int)size.h );
	// 자식들 그림
	XWnd::Draw();
	// viewport 해제	 
	GRAPHICS->RestoreViewport();
	XE::SetProjection( GRAPHICS->GetScreenWidth(), GRAPHICS->GetScreenHeight() );
//	GRAPHICS->DrawRectSize( v, size, XCOLOR_WHITE );
	XWndScrollView::Draw();
}

void XWndList::OnLButtonDown( float lx, float ly )
{
	XWndScrollView::OnLButtonDown( lx, ly );
	m_pClickedWnd = GetpwndLastLDown();
	m_vDown = INPUTMNG->GetMousePos();		// 터치 절대 좌표
	SetCapture( this );
}

void XWndList::OnMouseMove( float lx, float ly )
{
	XE::VEC2 vDist = m_vDown - INPUTMNG->GetMousePos();	// 스크롤뷰는 터치무브상태일때는 상대좌표가 항상 제자리이기때문에 터치 절대좌표로 써야 한다.
	// 처음 누른곳에서 3픽셀이상 벗어나면 움직인것으로 인정
	if( fabs( vDist.Lengthsq() ) > 3.f * 3.f )
	{
		m_pClickedWnd = NULL;
	}
	XWndScrollView::OnMouseMove( lx, ly );
}

void XWndList::OnLButtonUp( float lx, float ly )
{
	ReleaseCapture();
	if( m_pClickedWnd )
	{
		if( m_pClickedWnd->GetbEnable() )
		{
			ID idWnd = m_pClickedWnd->GetID();
			CallEventHandler( XWM_SELECT_MENU, idWnd );
			m_pClickedWnd->CallEventHandler( XWM_HELP_CLICKED, idWnd );
		}
	}
	m_pClickedWnd = NULL;
	return XWndScrollView::OnLButtonUp( lx, ly );
}

//======================================================
// 탭 컨트롤에 탭을 추가한다. x,y,w,h는 탭클릭 영역이다
XWndTab::XWndTab( float x, float y, float w, float h, LPCTSTR szFont ) 
	: XWnd( NULL, x, y, w, h )	
{ 
	Init(); 
	m_pFontObj = FONTMNG->CreateFontObj( szFont, 20.f );
	m_pFontObj->SetAlign( XE::xALIGN_CENTER );
}
void XWndTab::Destroy( void )
{
	SAFE_DELETE( m_pFontObj );
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		SAFE_DELETE( pTab->psfcImg );
		SAFE_DELETE( pTab );
	} END_LOOP;
}
void XWndTab::AddTab( ID idTab, LPCTSTR szLabel, LPCTSTR szImg, float x, float y, float w, float h )
{
	TAB *pTab = new TAB;
	pTab->idTab = idTab;
	pTab->strImg = szImg;
	pTab->x = x;
	pTab->y = y;
	pTab->w = w;
	pTab->h = h;
	if( szLabel )
		pTab->strLabel = szLabel;
//	pTab->psfcImg = GRAPHICS->CreateSurfaceFromFile( XE::MakePath( DIR_UI, szImg ), 281, 414 );
	pTab->psfcImg = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImg ) );
	m_listTab.Add( pTab );
	if( m_pSelectedTab == NULL )
		m_pSelectedTab = pTab;
}

void XWndTab::OnLButtonDown( float lx, float ly )
{
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( XE::IsArea2( pTab->x, pTab->y, pTab->w, pTab->h, lx, ly ) )
		{
			m_pSelectedTab = pTab;
			OnClickedTab( pTab );
//			CallEventHandler( XWM_CLICKED, pTab->idTab );
			break;
		}
	} END_LOOP;
	XWnd::OnLButtonDown( lx, ly );
}

void XWndTab::Draw( void )
{
	XE::VEC2 vPos = GetPosFinal();
	m_pSelectedTab->psfcImg->Draw( vPos );
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( pTab->strLabel.empty() == false )
			DrawTabLabel( pTab );
	} END_LOOP;
	XWnd::Draw();
}

void XWndTab::DrawTabLabel( TAB *pTab )
{
	m_pFontObj->SetLineLength( pTab->w );
	float yCenter = pTab->y + (pTab->h / 2.f);
	XE::VEC2 vPos = GetPosFinal();
	vPos.x += pTab->x;
	vPos.y += yCenter - 5.f;
	m_pFontObj->DrawString( vPos.x, vPos.y, pTab->strLabel.c_str() );
}

XWndTab::TAB* XWndTab::GetTab( ID idTab )
{
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( pTab->idTab == idTab )
			return pTab;
	} END_LOOP;
	return NULL;
}
//======================================================
// 탭 컨트롤에 탭을 추가한다. x,y,w,h는 탭클릭 영역이다
XWndTab2::XWndTab2( float x, float y, LPCTSTR szImg, LPCTSTR szFont ) 
	: XWnd( NULL, x, y )	
{ 
	Init(); 
	// 뒷배경
	if( szImg )
	{
		m_psfcBg = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImg ) );
		SetSizeLocal( m_psfcBg->GetWidth(), m_psfcBg->GetHeight() );
	}
	// 폰트
	m_pFontObj = FONTMNG->CreateFontObj( szFont, 20.f );
	m_pFontObj->SetAlign( XE::xALIGN_CENTER );
	m_pFontObj->SetLineLength( GetSizeFinal().w );
}
XWndTab2::XWndTab2( float x, float y, float w, float h, LPCTSTR szFont ) 
	: XWnd( NULL, x, y, w, h )	
{ 
	Init(); 
	// 폰트
	m_pFontObj = FONTMNG->CreateFontObj( szFont, 20.f );
	m_pFontObj->SetAlign( XE::xALIGN_CENTER );
	m_pFontObj->SetLineLength( GetSizeFinal().w );
}
void XWndTab2::Destroy( void )
{
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
	SAFE_DELETE( m_pFontObj );
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		SAFE_RELEASE2(IMAGE_MNG, pTab->psfcImgOn );
		SAFE_RELEASE2(IMAGE_MNG, pTab->psfcImgOff );
		SAFE_DELETE( pTab );
	} END_LOOP;
}
// szImgOn : 활성탭 이미지
// szImgOff : 비활성탭 이미지(생략하면 szImgOn으로 같이 씀)
void XWndTab2::AddTab( ID idTab, LPCTSTR szLabel, LPCTSTR szImgOn, LPCTSTR szImgOff, float x, float y, float w, float h )
{
	TAB *pTab = new TAB;
	pTab->idTab = idTab;
	pTab->strImgOn = szImgOn;
	pTab->strImgOff = szImgOff;
	pTab->x = x;
	pTab->y = y;
	pTab->w = w;
	pTab->h = h;
	XE::VEC2 vSize = GetSizeLocal();
	// 탭이 붙으면서 크기가 더 커지는 경우는 전체 윈도우 영역을 더 늘여준다.
	if( pTab->x + pTab->w > vSize.w )
		vSize.w = pTab->x + pTab->w;
	if( pTab->y + pTab->h > vSize.h )
		vSize.h = pTab->y + pTab->h;
	SetSizeLocal( vSize );
	if( szLabel )
		pTab->strLabel = szLabel;
	pTab->psfcImgOn = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImgOn ) );
	if( szImgOff )
		pTab->psfcImgOff = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImgOff ) );

	m_listTab.Add( pTab );
	if( m_pSelectedTab == NULL )
		m_pSelectedTab = pTab;
}

void XWndTab2::OnLButtonDown( float lx, float ly )
{
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( XE::IsArea2( pTab->x, pTab->y, pTab->w, pTab->h, lx, ly ) )
		{
			m_pSelectedTab = pTab;
			OnClickedTab( pTab );
//			CallEventHandler( XWM_CLICKED, pTab->idTab );
			break;
		}
	} END_LOOP;
	XWnd::OnLButtonDown( lx, ly );
}

void XWndTab2::Draw( void )
{
	XE::VEC2 vPos = GetPosFinal();
	// 뒷배경 찍음
	if( m_psfcBg )
		m_psfcBg->Draw( vPos );
	// 탭 이미지 찍음
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( pTab != m_pSelectedTab )		// 현재 선택된 탭은 나중에 찍기위해 건너뜀
		{
			if( pTab->psfcImgOff )
				pTab->psfcImgOff->Draw( vPos.x + pTab->x, vPos.y + pTab->y );
			else
				pTab->psfcImgOn->Draw( vPos.x + pTab->x, vPos.y + pTab->y );
			if( pTab->strLabel.empty() == false )
				DrawTabLabel( pTab );
		}
	} END_LOOP;
	// 선택된 탭을 덮어 그림
	m_pSelectedTab->psfcImgOn->Draw( vPos.x + m_pSelectedTab->x, vPos.y + m_pSelectedTab->y );
	if( m_pSelectedTab->strLabel.empty() == false )
		DrawTabLabel( m_pSelectedTab );
	XWnd::Draw();
}

void XWndTab2::DrawTabLabel( TAB *pTab )
{
	m_pFontObj->SetLineLength( pTab->w );
	float yCenter = pTab->y + (pTab->h / 2.f);
	XE::VEC2 vPos = GetPosFinal();
	vPos.x += pTab->x;
	vPos.y += yCenter - 5.f;
	m_pFontObj->DrawString( vPos.x, vPos.y, pTab->strLabel.c_str() );
}

XWndTab2::TAB* XWndTab2::GetTab( ID idTab )
{
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( pTab->idTab == idTab )
			return pTab;
	} END_LOOP;
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
XWndPageSlideWithXML::XWndPageSlideWithXML( const XE::VEC2& vPos, 
													const XE::VEC2& vSize, 
													XLayout* pLayout, 
													TiXmlElement *elemSlide,
													LPCTSTR szImgPoint )
	: XWnd( NULL, vPos.x, vPos.y, vSize.w, vSize.h )
{ 
	Init(); 
	m_pLayout = pLayout;
	m_elemSlide = elemSlide;
	//
	if( XE::IsHave( szImgPoint ) )
	{
		_tstring strFile = XE::GetFileTitle( szImgPoint );
		_tstring strExt = XE::GetFileExt( szImgPoint );
		strFile += _T("_on.");
		strFile += strExt;
		m_sfcPointOn = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, strFile.c_str() ) );
		//
		strFile = XE::GetFileTitle( szImgPoint );
		strExt = XE::GetFileExt( szImgPoint );
		strFile += _T("_off.");
		strFile += strExt;
		m_sfcPointOff = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, strFile.c_str() ) );
	}
	//
	m_posPagePoint.y = vSize.h - 13.f;	// 디폴트 페이지포인트 위치
}

// idxBase페이지를 기준으로 다음페이지의 인덱스를 얻는다. 없으면 -1을 리턴한다.
int XWndPageSlideWithXML::GetNextPage( int idxBase )
{
	if( idxBase == m_listPageKey.size() - 1 )
		return -1;
	return idxBase + 1;
}
// idxBase페이지를 기준으로 이전페이지의 인덱스를 얻는다. 없으면 -1을 리턴한다.
int XWndPageSlideWithXML::GetPrevPage( int idxBase )
{
	if( idxBase == 0 )
		return -1;
	return idxBase - 1;
}

int XWndPageSlideWithXML::AddPageKey( const char *cKeyPage ) 
{
	m_listPageKey.Add( cKeyPage );
	int idx = m_listPageKey.size() - 1;
	// 페이지가 늘어날때마다 페이지 포인트시작 위치도 갱신
	float sizePagePoint = m_listPageKey.size() * m_distPagePoint;
	XE::VEC2 vSize = GetSizeLocal();
	// 슬라이드내 로컬좌표
	m_posPagePoint.x = (vSize.w / 2.f) - (sizePagePoint / 2.f);
	return idx;
}

/**
 현재 페이지를 idxPage번호로 세팅하고 xml에 세팅된 컨트롤들을 모두 생성한다.
 그와 동시에 왼쪽 오른쪽 페이지의 컨트롤들도 생성한다.
*/
void XWndPageSlideWithXML::SetCurrPage( int idxPage )
{
	XBREAK( idxPage < 0 || idxPage >= m_listPageKey.size() );
	if( m_pCurr )
	{
		if( m_pCurr->GetidxPage() == idxPage )		// 이미 같은페이지면 다시 만들필요 없다.
			return;
		XWnd::DestroyWnd( m_pCurr );
	}
	// 현재 페이지 생성
	m_pCurr = CreatePage( idxPage );
	Add( m_pCurr );
	// 왼쪽페이지 생성
	if( m_pLeft )
		XWnd::DestroyWnd( m_pLeft );
	int idxPrevPage = GetPrevPage( idxPage );
	if( idxPrevPage >= 0 )
	{
		m_pLeft = CreatePage( idxPrevPage );
		XE::VEC2 vPos( -GRAPHICS->GetScreenWidth(), 0 );
		m_pLeft->SetPosLocal( vPos );
		Add( m_pLeft );
	}
	// 오른족 페이지생성
	if( m_pRight )
		XWnd::DestroyWnd( m_pRight );
	int idxNextPage = GetNextPage( idxPage );
	if( idxNextPage >= 0 )
	{
		m_pRight = CreatePage( idxNextPage );
		XE::VEC2 vPos( GRAPHICS->GetScreenWidth(), 0 );
		m_pRight->SetPosLocal( vPos );
		Add( m_pRight );
	}
}

XWndPageForSlide* XWndPageSlideWithXML::CreatePage( int idxPage )
{
	XBREAK( idxPage < 0 || idxPage >= m_listPageKey.size() );
	// 셋팅하려는 페이지의 키문자열을 구해서 xml node를 구한다.
	string strKeyPage = m_listPageKey.GetFromIndex( idxPage );
	TiXmlElement *elemPage = m_elemSlide->FirstChildElement( strKeyPage.c_str() );
	// 빈 페이지를 만들어 현재 페이지에 붙인다.
//	XE::VEC2 vPos = GetPosFinal();
	XE::VEC2 vSize = GetSizeFinal();
	
	XWndPageForSlide *pPage = new XWndPageForSlide( idxPage,  XE::VEC2(0), vSize );
	// 현재페이지에 xml에 세팅된 컨트롤들을 생성해서 붙인다.
	m_pLayout->CreateLayout( elemPage, pPage );
	//
	return pPage;
}

void XWndPageSlideWithXML::OnLButtonDown( float lx, float ly )
{
	m_vPrev = XE::VEC2( lx, ly );
	if( m_AutoScrollDir == XE::xDIR_NONE )
	{
		m_vStartScroll = XE::VEC2( lx, ly );
		m_bTouchDown = TRUE;
	} else
		m_bTouchDown = FALSE;
}

void XWndPageSlideWithXML::OnMouseMove( float lx, float ly )
{
	if( m_AutoScrollDir == XE::xDIR_NONE && m_bTouchDown )
	{
		XE::VEC2 vDist = XE::VEC2( lx, ly ) - m_vPrev;	// 마우스 이동량
		if( m_pCurr )
		{
			XE::VEC2 vPos = m_pCurr->GetPosLocal();
			vPos.x += vDist.x;
			m_pCurr->SetPosLocal( vPos );
		}
		if( m_pRight )
		{
			XE::VEC2 vPos = m_pRight->GetPosLocal();
			vPos.x += vDist.x;
			m_pRight->SetPosLocal( vPos );
		}
		if( m_pLeft )
		{
			XE::VEC2 vPos = m_pLeft->GetPosLocal();
			vPos.x += vDist.x;
			m_pLeft->SetPosLocal( vPos );
		}
	}
	m_vPrev = XE::VEC2( lx, ly );
}
void XWndPageSlideWithXML::OnLButtonUp( float lx, float ly )
{
	m_bTouchDown = FALSE;
	if( m_AutoScrollDir == XE::xDIR_NONE && m_pCurr )
	{
//		float wHalf = GRAPHICS->GetScreenWidth() / 2.f;
		float wHalf = GRAPHICS->GetScreenWidth() * 0.3f;	// 30%
		XE::VEC2 vPos = m_pCurr->GetPosLocal();
		// 현재페이지의 좌상귀x좌표가 왼쪽으로 화면의 절반을 넘어간상태면
		if( vPos.x < -wHalf )
		{
			if( m_pRight == NULL )
				DoAutoScroll( XE::xDIR_CENTER, vPos );		// 오른쪽화면이 더이상 없으므로 제자리로 돌아가기
			else
				DoAutoScroll( XE::xDIR_RIGHT, vPos );
		} else
		if( vPos.x > wHalf )
		{
			if( m_pLeft == NULL )
				DoAutoScroll( XE::xDIR_CENTER, vPos );		// 왼쪽화면이 더이상 없으므로 제자리로 돌아가기
			else
				DoAutoScroll( XE::xDIR_LEFT, vPos );
		} else
		{
			// 아니면 제자리로 돌아가야 한다.
			DoAutoScroll( XE::xDIR_CENTER, vPos );
		}
	}
}

void XWndPageSlideWithXML::DoAutoScroll( XE::xDir dir, const XE::VEC2& vStart )
{
	m_AutoScrollDir = dir;
	m_vStartScroll = vStart;
}
#define xSEC_SLIDE		0.15f
int XWndPageSlideWithXML::Process( float dt )
{
	if( m_AutoScrollDir == XE::xDIR_RIGHT )
	{
		// ------->>>>
		// 오른쪽 화면이 나타나고 현재화면이 왼쪽으로 사라진다.
		float lerp = m_itpScroll.GetInterpolationValueWithSet( xSEC_SLIDE );
		XE::VEC2 vPos = m_pCurr->GetPosLocal();
		float wScreen = (float)GRAPHICS->GetScreenWidth();
		vPos.x = m_vStartScroll.x + ( (-wScreen) - m_vStartScroll.x ) * lerp;
		m_pCurr->SetPosLocal( vPos );
		if( m_pRight )
		{
			XE::VEC2 vPosRight = m_pRight->GetPosLocal();
			vPosRight.x = vPos.x + wScreen;
			m_pRight->SetPosLocal( vPosRight );
		}
		if( m_itpScroll.IsFinish() )
		{
			m_itpScroll.Off();
			m_AutoScrollDir = XE::xDIR_NONE;
			// 현재 화면이 왼쪽으로 완전히 사라졌다.
			if( m_pLeft )
				XWnd::DestroyWnd( m_pLeft );		// 원래있던 왼쪽페이지를 삭제
			m_pLeft = m_pCurr;
			m_pCurr = m_pRight;
			m_pRight = NULL;
			// 오른쪽 페이지 생성
			int idxRightPage = GetNextPage( m_pCurr->GetidxPage() );
			if( idxRightPage >= 0 )
			{
				m_pRight = CreatePage( idxRightPage );
				Add( m_pRight );
			}
			CallEventHandler( XWM_FINISH_SLIDE, m_pCurr->GetidxPage() );
		}
	} else
	if( m_AutoScrollDir == XE::xDIR_LEFT )
	{
		// <<<<-----
		// 왼쪽 화면이 나타나고 현재화면이 오른쪽으로 사라진다.
		float lerp = m_itpScroll.GetInterpolationValueWithSet( xSEC_SLIDE );
		XE::VEC2 vPos = m_pCurr->GetPosLocal();
		float wScreen = (float)GRAPHICS->GetScreenWidth();
		vPos.x = m_vStartScroll.x + ( wScreen - m_vStartScroll.x ) * lerp;
		m_pCurr->SetPosLocal( vPos );
		if( m_pLeft )
		{
			XE::VEC2 vPosLeft = m_pLeft->GetPosLocal();
			vPosLeft.x = vPos.x - wScreen;
			m_pLeft->SetPosLocal( vPosLeft );
		}
		if( m_itpScroll.IsFinish() )
		{
			m_itpScroll.Off();
			m_AutoScrollDir = XE::xDIR_NONE;
			// 현재 화면이 오른쪽으로 완전히 사라졌다.
			if( m_pRight )
				XWnd::DestroyWnd( m_pRight );		// 원래있던 오른쪽페이지를 삭제
			m_pRight = m_pCurr;
			m_pCurr = m_pLeft;
			m_pLeft = NULL;
			// 왼쪽 페이지 생성
			int idxLeftPage = GetPrevPage( m_pCurr->GetidxPage() );
			if( idxLeftPage >= 0 )
			{
				m_pLeft = CreatePage( idxLeftPage );
				Add( m_pLeft );
			}
			CallEventHandler( XWM_FINISH_SLIDE, m_pCurr->GetidxPage() );
		}
	} else
	if( m_AutoScrollDir == XE::xDIR_CENTER )
	{
		// 제자리로 돌아가기
		float lerp = m_itpScroll.GetInterpolationValueWithSet( xSEC_SLIDE );
		float wScreen = (float)GRAPHICS->GetScreenWidth();
		XE::VEC2 vPos = m_pCurr->GetPosLocal();
		vPos.x = m_vStartScroll.x + ( 0 - m_vStartScroll.x ) * lerp;
		m_pCurr->SetPosLocal( vPos );
		if( m_pRight )
		{
			XE::VEC2 vPosRight = m_pRight->GetPosLocal();
			vPosRight.x = vPos.x + wScreen;
			m_pRight->SetPosLocal( vPosRight );
		}
		if( m_pLeft )
		{
			XE::VEC2 vPosLeft = m_pLeft->GetPosLocal();
			vPosLeft.x = vPos.x - wScreen;
			m_pLeft->SetPosLocal( vPosLeft );
		}
		if( m_itpScroll.IsFinish() )
		{
			m_itpScroll.Off();
			m_AutoScrollDir = XE::xDIR_NONE;
		}
	}
	return 1;
}

void XWndPageSlideWithXML::Draw( void )
{
	XWnd::Draw();
	// 페이지 포인트 그림
	XE::VEC2 vPos = GetPosFinal();
	int numPages = m_listPageKey.size();
	XE::VEC2 posPoint = vPos + m_posPagePoint;
	for( int i = 0; i < numPages; ++i )
	{
		if( i == m_pCurr->GetidxPage() )
		{
			if( m_sfcPointOn )	
				m_sfcPointOn->Draw( posPoint );
		}
		else
		{
			if( m_sfcPointOff )
				m_sfcPointOff->Draw( posPoint );
		}
		posPoint.x += m_distPagePoint;
	}
}
//////////////////////////////////////////////////////////////////////////
//==========================================================
XWndProgressBar::XWndProgressBar( float x, float y, LPCTSTR szImg, LPCTSTR szImgBg )
	: XWnd( NULL, x, y ), 
	XProgressBar( XE::VEC2( x, y ), szImg, szImgBg )
{
	Init();
	if( GetpsfcBg() )
	{
		XE::VEC2 vSize = GetpsfcBg()->GetSize();
		SetSizeLocal( vSize );
	}
}

XWndProgressBar::XWndProgressBar( float x, float y, float w, float h, XCOLOR col )
	: XWnd( NULL, x, y, w, h ),
	XProgressBar( XE::VEC2( x, y ), XE::VEC2( w, h ) )
{
	Init();
}


void XWndProgressBar::Draw( void )
{
	XE::VEC2 vPos = GetPosFinal();
	XProgressBar::Draw( vPos, m_Lerp );
}
