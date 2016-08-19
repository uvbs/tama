#include "stdafx.h"
#include "XWnd.h"
#include "xUtil.h"
#include "XWndMng.h"
#include "SprObj.h"
#include "XFontSpr.h"
#ifdef _XTOOLTIP			// 툴팁 써야할때 다시 살릴것.
#include "XToolTip.h"		
#endif
#ifdef WIN32
#include "XGraphicsD3DTool.h"
#endif
#include "XWndButton.h"
#include "XImage.h"
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
void XWnd::SetScaleLocal( float sx, float sy ) 
{ 
	if( m_pWndMng )
	{
		m_vScale.x = m_pWndMng->GetScale().x * sx; 
		m_vScale.y = m_pWndMng->GetScale().y * sy; 
	} else
	{
		m_vScale.Set( sx, sy );
	}
}
void XWnd::SetRotateLocal( float dRotZ )
{
	if( m_pWndMng )
		m_dRotateZ = m_pWndMng->GetRotate() + dRotZ;
	else
		m_dRotateZ = dRotZ;
}

XE::VEC2 XWnd::GetAdjust( void ) {	return m_pWndMng->GetpSprObj()->GetAdjust(); 	}

XBaseFontDat *XWnd::s_pFontDat = NULL;

void XWnd::Destroy( void ) 
{ 
	list<XWnd*>::iterator iter;
	for( iter = m_listItems.begin(); iter != m_listItems.end(); )
	{
		delete (*iter);
		m_listItems.erase( iter++ );
	}
	SAFE_DELETE( m_pModalDlg );	// 삭제책임은 XWnd가 갖는다(잘하는짓인지 몰겠음). 그러므로 ModalDlg를 루아내부에서 생성하면 안됨. 루아파괴될때 그곳에서 파괴되니까
}


XWnd *XWnd::Add( int id, XWnd *pChild ) 
{
	XBREAK( id == 0 );
	pChild->m_pWndMng = m_pWndMng;
	pChild->SetID( id );
	pChild->m_pParent = this;
	m_listItems.push_back( pChild );
	return pChild;
}

int XWnd::Process( float dt ) 
{
	if( m_timerPush.IsOver(0.15f) )	// 누른지 0.2초가 지났으면		
	{
#ifdef _XTOOLTIP
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
#endif
		m_timerPush.Off();
	}
	if( m_pModalDlg )		// 모달다이얼로그가 있으면 다른 윈도우들은 처리하지 않는다
	{
		return m_pModalDlg->Process( dt );
	}
	LIST_MANUAL_LOOP( m_listItems, XWnd*, itor, pWnd )
	{
		pWnd->Process(dt);
		if( pWnd->GetbDestroy() )
		{
			//SAFE_DELETE( pWnd );		아직 구현안됨
			if( pWnd->CallEventHandler( XWM_DESTROY ) )		// 파괴 이벤트 핸들러가 있다면 호출
			{
				SAFE_DELETE( pWnd );
				m_listItems.erase( itor++ );
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

int XWnd::CallEventHandler( ID msg, DWORD dwParam )
{
	int ret=1;
	XWND_MESSAGE_MAP msgMap = FindMsgMap( msg );
	if( msgMap.pOwner && msgMap.pHandler )
		ret = (msgMap.pOwner->*msgMap.pHandler)( this, msgMap.param, dwParam );		// 롤러가 돌아가다 멈추면 이벤트가 발생하며 멈춘아이템의 인덱스를 건넨다
	return ret;
}


XWnd* XWnd::OnLButtonDown( float lx, float ly ) 
{ 
	if( GetbShow() == FALSE )		return NULL;	// hide상태에선 본인과 자식들모두 감춤
	XE::VEC2 vMouseLocal( lx, ly );
	if( m_pModalDlg ) {
		XE::VEC2 vChildLocal;
		if( m_pParent )
			vChildLocal = vMouseLocal - (m_pModalDlg->GetPosLocal() * GetScaleLocal());
		else
			vChildLocal = vMouseLocal - m_pModalDlg->GetPosLocal();
			if( m_pModalDlg->IsWndAreaIn( vChildLocal ) )
				return m_pModalDlg->OnLButtonDown( vChildLocal.x, vChildLocal.y );
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
            if( pWnd->GetID() == 10005 )
            {
                int a=0;
            }
			if( pWnd->IsWndAreaIn( vChildLocal ) || pWnd->GetSizeLocal().IsMinus() )	// 라디오그룹같은건 사이즈가 -1이다
			{
				if( pWnd->GettimerPush().IsOff() )	// 타이머가 꺼져있을때만
					pWnd->GettimerPush().Set(0);	// 타이머 돌기 시작
				if( pWnd->GetbActive() )
				{
					pPushWnd = pWnd->OnLButtonDown( vChildLocal.x, vChildLocal.y );	// 차일드에는 상대좌표로 넘겨줌
					return (pPushWnd)? pPushWnd : pWnd;
				}
			}
		}
	}
	return NULL;
}
XWnd* XWnd::OnMouseMove( float lx, float ly ) 
{
	if( GetbShow() == FALSE )		return NULL;	// hide상태에선 본인과 자식들모두 감춤
	XE::VEC2 vMouseLocal( lx, ly );
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
				if( pWnd->GetbActive() )
				{
					pWndMove = pWnd->OnMouseMove( vChildLocal.x, vChildLocal.y );
					return (pWndMove)? pWndMove : pWnd;
				}
//						return pWndMove;
			} else
			{
				m_bToolTip = FALSE;
			}
			// NC이벤트는 영역체크에 상관없이 호출된다
			if( pWnd->GetbActive() )	
				pWnd->OnNCMouseMove( vChildLocal.x, vChildLocal.y );
		}
	}
	return NULL;
}
ID XWnd::OnLButtonUp( float lx, float ly ) 
{ 
	if( GetbShow() == FALSE )		return NULL;	// hide상태에선 본인과 자식들모두 감춤
	XE::VEC2 vMouseLocal( lx, ly );
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
				id = m_pModalDlg->OnLButtonUp( vChildLocal.x, vChildLocal.y );
			if( m_pModalDlg->GetbDestroy() )	// 다이얼로그는 ok버튼 등을 누르면 자동으로 꺼짐
				DestroyModalDlg();
		}
		return id;
	}
	// tooltip clear
	m_timerPush.Off();
	m_bToolTip = FALSE;
#ifdef _XTOOLTIP
	TOOLTIP = NULL;
#endif
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
				if( pWnd->GetbActive() )
				{
					pWnd->OnLButtonUp( vChildLocal.x, vChildLocal.y ); 
					return pWnd->GetID();
				}
			}
			// NC이벤트는 영역체크에 상관없이 호출된다
			if( pWnd->GetbActive() )
				pWnd->OnNCLButtonUp( vChildLocal.x, vChildLocal.y ); 
		}
	}
	return 0;
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
#ifdef WIN32
	XImageD3D<DWORD> image( TRUE );		// UI리소스는 모두 고해상도 리소스를 사용
#else
	XImageiOS<DWORD> image( TRUE );
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



////////////////////////////////////////////////////////////////
// dialog
XWndButton* XWndDialog::AddButton( ID id, float x, float y, LPCTSTR szText, LPCTSTR szSpr, ID idActUp, ID idActDown  )
{
	XWndButtonString *pButton = new XWndButtonString( x, y, szText, XWnd::s_pFontDat, GetpWndMng()->GetpSprObj()->GetszFilename(), idActUp, idActDown );
	pButton->SetColorText( XCOLOR_WHITE );
	Add( id, pButton );
	return pButton;
}
XWndButton* XWndDialog::AddButton( ID id, const XE::VEC2& vPos, const XE::VEC2& size, LPCTSTR szText )
{
	XWndDynamicButtonString *pButton = new XWndDynamicButtonString( vPos.x, vPos.y, size.w, size.h, szText, m_pFontDat );
	pButton->SetColorText( XCOLOR_WHITE );
	Add( id, pButton );
	return pButton;
}
XWndButton* XWndDialog::AddButton( ID id, float x, float y, LPCTSTR szSpr, ID idActUp, ID idActDown  )
{
	XWndButton *pButton = new XWndButton( x, y, szSpr, idActUp, idActDown );
	Add( id, pButton );
	return pButton;
}
// static text
XWnd* XWndDialog::AddStaticText( ID id, float x, float y, float w, float h, LPCTSTR szText, XBaseFontDat *pFontDat, xAlign align, XCOLOR col, xFONT::xtStyle style )
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
	XWND_RES_FRAME *pRes;
	if( m_frameImage.nBlockSize )
		pRes = &m_frameImage;
	else
		pRes = &GetpWndMng()->GetRes().frameDialog;
	XWnd::DrawFrame( *pRes );
	if( m_nStateAppear == 1 )		// Appear상태가 끝나면 차일드를 그린다
		XWnd::Draw();	// child draw
}


ID XWndDialog::OnLButtonUp( float lx, float ly ) 
{
	ID id = XWnd::OnLButtonUp( lx, ly );
	// Dialog는 OK, yes, no버튼등을 누르면 자동으로 다이얼로그가 꺼지게 한다
	switch( id )
	{
	case xYES:
	case xOK:
		SetbDestroy( TRUE );
		OnOk();
		CallEventHandler( XWM_OK );		// 다이얼로그의 ok눌렀을때 핸들러
		break;
	case xNO:
	case xCANCEL:
		SetbDestroy( TRUE );
		OnCancel();
		break;
	}
	return id;
}
//
int XWndDialog::LuaDoModal( lua_State *L )
{
	GetpWndMng()->SetModalDialog( this );		// 자신(Dialog)을 모달다이얼로그로 등록시킴
	return lua_yield( L, 0 );
}
//----------------------------------------------------------
void XWndImage::Create( BOOL bHighReso, LPCTSTR szImg )
{
	m_pSurface = GRAPHICS->CreateSurface( bHighReso, szImg );
	XBREAK( m_pSurface == NULL );
}
void XWndImage::Draw( void )
{
	m_pSurface->Draw( m_vPos );
}
//----------------------------------------------------------
BOOL XWndView::LoadRes( LPCTSTR szRes )
{
	XBREAK( szRes == NULL );
    if( szRes )
	{
		XWnd::LoadResFrame( szRes, &m_frameImage );
		XBREAKF( m_frameImage.nBlockSize == 0, "UI 리소스가 로딩되지 않았습니다" );
		m_nBlockSize = m_frameImage.nBlockSize;	// 편의상 받아옴
	}
    return TRUE;
}
int XWndView::Process( float dt )
{
	float slerp = m_timerAppear.GetSlerp();
	if( m_nStateAppear == 0 )		// 창 팝업효과 Init
	{
		SetScaleLocal( 0 );		// 스케일 0부터 시작
		m_timerAppear.Set( 0.10f, TRUE );	// 0.15초동안 커진다
		m_nStateAppear = 2;	// start
	} else
	if( m_nStateAppear == 2 )		// init
	{
		float scale = sinf( D2R(slerp * 90.f) );		// 0 ~ 1까지 사인파형으로 커진다
		SetScaleLocal( scale );	
		if( m_timerAppear.IsOver() ) {
			m_timerAppear.Set( 0.3f, TRUE );	// 100% +- x%구역에서 0.3초간 띠요요옹 한다
			m_nStateAppear = 3;		
		}
	} else
	if( m_nStateAppear == 3 )
	{
		float scale = 1.0f + (sinf( D2R(slerp * (360.f*1.f)) ) * 0.2f) * (1.f - slerp);		// 0 -> 1 -> 0 -> -1 -> 0.. slerp에 따라 진폭이 점점 작아진다. 360도까지는 아래위한파동이므로 두파동을 만들기 위해 720도까지 돌림
		SetScaleLocal( scale );	// 110% -> 100%로 작아진다
		if( m_timerAppear.IsOver() ) {
			m_nStateAppear = 1;		// 1이 Appear끝
			SetScaleLocal( 1.0f );		// 최종크기 100%
			OnFinishAppear();
		}
	} 

	XSprObj::s_LuaDt = dt;		// 루아글루에 사용
	return XWnd::Process( dt );
}

void XWndView::Draw( void )
{
	if( m_bDrawFrame )
	{
		if( m_frameImage.nBlockSize )
		{
			XWND_RES_FRAME *pRes = &m_frameImage;
			XWnd::DrawFrame( *pRes );
		}
	}
//	if( m_bDrawFrame )
//		XWnd::DrawFrame( GetpWndMng()->GetRes().frameView );
	if( m_nStateAppear == 1 )		// Appear상태가 끝나면 차일드를 그린다
	{
		if( m_prefLua )
			m_prefLua->Call<void,XWndView*>( m_cDrawHandler, this );	// lua의 draw handler를 호출
		//
		XWnd::Draw();		// child 그림
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
XWnd* XWndSlider::OnLButtonDown( float lx, float ly )
{
	m_Push = 1;
	XE::VEC2 size = GetSizeFinal();
	float lerp = lx / size.w;		// 0 ~ 1로 환산
	float w = m_Max - m_Min;
	m_Curr = m_Min + w * lerp;
	return this;
}
void XWndSlider::OnNCMouseMove( float lx, float ly )
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
		CallEventHandler( XWM_SLIDING );
	}
}
void XWndSlider::OnNCLButtonUp( float lx, float ly )
{
	m_Push = 0;
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
		pWnd1->SetAlign( xALIGN_CENTER );
		XE::VEC3 size = pWnd1->GetpFont()->GetLayoutSize( szTitle );
		lw = xmax( lw, size.x );
	}
	if( szMsg ) 
	{
//		_tcscpy_s( m_szMsg, szMsg );
		pWnd2 = new XWndTextString( w/2.f, 20.f, szMsg, pFontDat, XCOLOR_WHITE );
		Add( 2, pWnd2 );
		pWnd2->SetAlign( xALIGN_CENTER );
		XE::VEC3 size = pWnd2->GetpFont()->GetLayoutSize( szMsg );
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

XWnd* XWndPopupMenuText::OnLButtonDown( float lx, float ly )
{
	XWndPopupMenu::OnLButtonDown( lx, ly );
	return this;
}
XWnd* XWndPopupMenuText::OnMouseMove( float lx, float ly )
{
	XWndPopupMenu::OnMouseMove( lx, ly );
	return this;
}

ID XWndPopupMenuText::OnLButtonUp( float lx, float ly )
{
	XWndPopupMenu::OnLButtonUp( lx, ly );
	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////
void XWndScrollView::Scroll( float sx, float sy ) 
{
	m_vAdjust.x -= sx;
	m_vAdjust.y -= sy;
	if( m_vAdjust.x > 0 )
		m_vAdjust.x = 0;
	if( m_vAdjust.y > 0 )
		m_vAdjust.y = 0;
}
////////////////////////////////////////////////////////////////////////////////////////
XWndList::XWndList( float x, float y, float w, float h ) 
	: XWndScrollView( x, y, w, h )
{ 
	Init(); 
	XE::VEC2 v( x, y );
	XE::VEC2 vSize( w, h );
	if( v.y < 3 )
		v.y = 3;
	if( v.x < 3 )
		v.x = 3;
	if( v.y + vSize.h > GRAPHICS->GetScreenHeight() )
		v.y -= v.y + vSize.h - GRAPHICS->GetScreenHeight() - 3;
	if( v.x + vSize.w > GRAPHICS->GetScreenWidth() )
		v.x -= v.x + vSize.w - GRAPHICS->GetScreenWidth() - 3;
	SetPosLocal( v );
}
XWndList::XWndList( const XE::VEC2& vPos, const XE::VEC2& vSize )
	: XWndScrollView( vPos.x, vPos.y, vSize.w, vSize.h )
{
	Init();
	XE::VEC2 v = vPos;
	if( v.y < 3 )
		v.y = 3;
	if( v.x < 3 )
		v.x = 3;
	if( v.y + vSize.h > GRAPHICS->GetScreenHeight() )
		v.y -= v.y + vSize.h - GRAPHICS->GetScreenHeight() - 3;
	if( v.x + vSize.w > GRAPHICS->GetScreenWidth() )
		v.x -= v.x + vSize.w - GRAPHICS->GetScreenWidth() - 3;
	SetPosLocal( v );
}

void XWndList::Destroy()
{
//	XLIST_DESTROY( m_listItem, XWnd* );
}

XWnd* XWndList::AddItem( ID idWnd, XWnd *pWnd ) 
{
	m_listItem.Add( pWnd );
	Add( idWnd, pWnd );
	// 추가한 객체의 세로크기를 계산해서 리스트내의 y좌표를 지정한다.
	XE::VEC2 vSize = pWnd->GetSizeFinal();
	pWnd->SetPosLocal( 0.f, m_Y );
	m_Y += vSize.h;
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

//#include "Mainfrm.h"
void XWndList::Draw( void )
{
	XE::VEC2 v = XWnd::GetPosFinal();
	XE::VEC2 size = XWnd::GetSizeFinal();
//	GRAPHICS->FillRectSize( v, size, XCOLOR_RGBA( 0, 0, 0, 192 ) );		// frame
//	GRAPHICS->DrawRectSize( v, size, XCOLOR_YELLOW );		// frame
	// viewport 설정
	GRAPHICS->BackupViewport();
	GRAPHICS->SetViewport( XE::POINT( v ), XE::POINT( size ) );
	// 자식들 그림
	XWnd::Draw();
	// 각 아이템마다 외곽선 그려줌
	{
		XE::VEC2 vpos, vsize;
		XLIST_LOOP( m_listItem, XWnd*, pItem )
		{
			vpos = pItem->GetPosFinal();
			vsize = pItem->GetSizeFinal();
			GRAPHICS->DrawRectSize( vpos, vsize, XCOLOR_BLACK );
		} END_LOOP;
	}
	// viewport 해제	 
	GRAPHICS->RestoreViewport();
}

XWnd* XWndList::OnLButtonDown( float lx, float ly )
{
	m_pClickedWnd = XWndScrollView::OnLButtonDown( lx, ly );
	return this;
}

XWnd* XWndList::OnMouseMove( float lx, float ly )
{
	m_pClickedWnd = NULL;
	return XWndScrollView::OnMouseMove( lx, ly );
}

ID XWndList::OnLButtonUp( float lx, float ly )
{
	if( m_pClickedWnd )
	{
		ID idWnd = m_pClickedWnd->GetID();
		CallEventHandler( XWM_SELECT_MENU, idWnd );
	}
	m_pClickedWnd = NULL;
	return XWndScrollView::OnLButtonUp( lx, ly );
}


