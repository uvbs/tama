#include "stdafx.h"
//#include "sprite/SprDat.h"
#include "XFramework/XEProfile.h"
#include "sprite/SprObj.h"
#include "sprite/XActObj2.h"
#include "Sprite/XActDat.h"
#include "_Wnd2/XWndSprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
XWndSprObj* XWndSprObj::sUpdateCtrl( XWnd *pRoot
																	, const XE::VEC2& vPos
																	, const char *cIdentifier )
{
	auto pCtrl = SafeCast2<XWndSprObj*>( pRoot->Find( cIdentifier ) );
	if( pCtrl == nullptr ) {
		pCtrl = new XWndSprObj( vPos );
		pCtrl->SetstrIdentifier( cIdentifier );
		XBREAK( pCtrl == nullptr );
		pRoot->Add( pCtrl );
	}
	pCtrl->SetPosLocal( vPos );
	return pCtrl;
}

XWndSprObj* XWndSprObj::sUpdateCtrl( XWnd *pRoot
																	, LPCTSTR szSpr
																	, ID idAct
																	, const XE::VEC2& vPos
																	, const char *cIdentifier )
{
	auto pCtrl = SafeCast2<XWndSprObj*>( pRoot->Find( cIdentifier ) );
	if( pCtrl == nullptr ) {
		pCtrl = new XWndSprObj( szSpr, idAct, vPos );
		pCtrl->SetstrIdentifier( cIdentifier );
		XBREAK( pCtrl == nullptr );
		pRoot->Add( pCtrl );
	}
	pCtrl->SetPosLocal( vPos );
	pCtrl->SetSprObj( szSpr, idAct );
	return pCtrl;
}

XWndSprObj::XWndSprObj( LPCTSTR szSpr, 
												ID idAct, 
												const XE::VEC2& vPos, 
												xRPT_TYPE loopType/*=xRPT_LOOP*/ ) 
	: XWnd(vPos.x, vPos.y) 
{
	Init();
	SetPosLocal( vPos );
	if( XE::IsHave(szSpr) ) {
		CreateSprObj( szSpr, idAct, true, false, false, loopType );
		m_loopType = loopType;
		m_idAct = idAct;
	}
}
void XWndSprObj::Destroy() 
{
	SAFE_DELETE( m_pSprObj );
}

void XWndSprObj::CreateSprObj( LPCTSTR szSpr, 
																	 ID idAct, 
																	 bool bUseAtlas,
																	 bool bBatch,
																	 bool bAsyncLoad,
																	 xRPT_TYPE loopType )
{
	if( XE::IsEmpty( szSpr ) ) {
		SetbDestroy( true );
		return;
	}
	m_pSprObj = new XSprObj( szSpr, XE::xHSL(), idAct, xRPT_LOOP,
													 bUseAtlas, bBatch, bAsyncLoad,
													 [this]( XSprObj* pSprObj ) {
		XBREAK( pSprObj->GetpObjActCurr() == nullptr );
		if( pSprObj->GetpObjActCurr() ) {
			// 캐시에 있다가 불려온것이면 데이타가 있다.
			const XE::VEC2 vSize = pSprObj->GetSize();
			SetSizeLocal( vSize );
		} else {
			const XE::VEC2 vSize( 2, 2 );		// 비동기로 로딩해서 크기를 일단 이렇게 맞춤.
			SetSizeLocal( vSize );
		}
	} );
	m_pSprObj->SetAction( idAct, loopType );
	m_idAct = idAct;
	m_loopType = loopType;
	if( m_pSprObj->IsError() )
		return;
	m_strSpr = szSpr;
// 	if( m_pSprObj->GetpObjActCurr() ) {
// 		// 캐시에 있다가 불려온것이면 데이타가 있다.
// 		const XE::VEC2 vSize = m_pSprObj->GetSize();
// 		SetSizeLocal( vSize );
// 	} else {
// 		const XE::VEC2 vSize( 2, 2 );		// 비동기로 로딩해서 크기를 일단 이렇게 맞춤.
// 		SetSizeLocal( vSize );
// 	}
}

/**
 @brief spr파일을 로딩한다.
 @param bAsyncLoad 비동기식으로 파일을 로딩한다.
*/
void XWndSprObj::SetSprObj( LPCTSTR szSpr, ID idAct, xRPT_TYPE loopType, bool bASyncLoad ) 
{
	SetSprObj( szSpr, idAct, true, false, bASyncLoad, loopType );
}

void XWndSprObj::SetSprObj( LPCTSTR szSpr, 
														ID idAct, 
														bool bUseAtlas,
														bool bBatch,
														bool bASyncLoad,
														xRPT_TYPE loopType )
{
	if( XE::IsEmpty( szSpr ) ) {
		return;
	}
	if( idAct )
		m_idAct = idAct;
	bool bRecreate = false;
	if( m_pSprObj == nullptr
			|| (m_pSprObj && !XE::IsSame( m_pSprObj->GetszFilename(), szSpr )) )
		bRecreate = true;
	if( bRecreate ) {
		// 다른파일을 지정해서 객체를 새로 생성해야함.
		SAFE_DELETE( m_pSprObj );
		if( XE::IsHave( szSpr ) ) {
			CreateSprObj( szSpr, idAct, bUseAtlas, bBatch, bASyncLoad, loopType );
		}
	} else {
		if( m_pSprObj )
			m_pSprObj->SetAction( idAct, loopType );
	}
}

void XWndSprObj::DestroyForReload()
{
	if( !m_strSpr.empty() ) {
		XBREAK( m_pSprObj == nullptr );
		SAFE_DELETE( m_pSprObj );
	}
	XWnd::DestroyForReload();
}
/**
 @brief 현재 SprObj를 파괴하고 재로딩한다.
*/
void XWndSprObj::Reload()
{
	if( !m_strSpr.empty() ) {
		XBREAK( m_pSprObj != nullptr );		// DestroyForReload()부터 하고 와야함.
		SetSprObj( m_strSpr.c_str(), m_idAct, m_loopType, false );
	}
	XWnd::Reload();
}

int XWndSprObj::Process( float dt ) 
{
	if( m_pSprObj ) {
		auto bLoaded = m_pSprObj->IsLoaded();
		m_pSprObj->FrameMove( dt );
		if( !bLoaded && m_pSprObj->IsLoaded() ) {
			// 위 FrameMove에서 파일로딩이 완료되었다.
			if( m_pSprObj->GetpObjActCurr() ) {
				auto sizeSpr = m_pSprObj->GetSize();
				SetSizeLocal( sizeSpr );
			}
		}
		if( m_timerLife.IsOn() ) {
			if( m_timerLife.IsOver() )
				SetbDestroy( true );
		} else {
			if( m_loopType == xRPT_1PLAY && m_pSprObj->IsFinish() ) {
				OnFinishAni();		// virtual
				if( m_pDelegate ) {
					auto bDestroy = m_pDelegate->DelegateSprObjFinishAni( this );
					if( bDestroy )
						SetbDestroy( TRUE );
					else
						m_pSprObj->ResetAction();
				} else
					SetbDestroy( TRUE );
			}
		}
	}
	XWnd::Process( dt );
	return 1;
}

void XWndSprObj::Draw( void ) 
{
	XPROF_OBJ_AUTO();
//	XWnd::Draw();	// << 이걸왜 먼저 불렀지?
	if( m_pSprObj && m_bDraw ) {
		XE::VEC2 vScale = GetScaleFinal();
		float alphaWin = GetAlphaFinal();
		m_pSprObj->SetRotateX( m_vRotate.x );
		m_pSprObj->SetRotateY( m_vRotate.y );
		m_pSprObj->SetRotateZ( m_vRotate.z );
		m_pSprObj->SetfAlpha( alphaWin );
		m_pSprObj->SetScale( vScale );
		m_pSprObj->Draw( GetPosFinal() );
	}
	XWnd::Draw();
}


BOOL XWndSprObj::IsWndAreaIn( float lx, float ly ) 
{
	if( m_pSprObj && m_pSprObj->GetpObjActCurr() ) {
		XE::VEC2 vLT = m_pSprObj->GetpObjActCurr()->GetpAction()->GetBoundBoxLT();
		vLT *= m_scaleBB;
		lx -= vLT.x;
		ly -= vLT.y;
	}
	int w = (int)(GetSizeFinal().w * m_scaleBB);
	int h = (int)(GetSizeFinal().h * m_scaleBB);
	if( XE::IsArea2( 0, 0, w, h, lx, ly ) )
		return TRUE;
	return FALSE;
}

bool XWndSprObj::IsWndAreaInByScreen( float x, float y )
{
	auto vPos = GetPosScreen();
	auto vLocal = XE::VEC2(x,y) - vPos;
	return IsWndAreaIn( vLocal.x, vLocal.y ) != FALSE;
}

XE::xRECT XWndSprObj::GetBoundBoxByVisibleNoTrans()
{
	if( m_pSprObj && m_pSprObj->GetAction() ) {
		const auto pAct = m_pSprObj->GetAction();
		return XE::xRECT( pAct->GetBoundBoxLT(), pAct->GetBoundBoxSize() ) * m_scaleBB;
	} else {
		return XWnd::GetBoundBoxByVisibleNoTrans();
	}
}

ID XWndSprObj::GetidAct() 
{
	return (m_pSprObj) ? m_pSprObj->GetActionID() : 0;
}

void XWndSprObj::SetpDelegateBySprObj( XDelegateSprObj *pDelegate ) {
	if( m_pSprObj )
		m_pSprObj->SetpDelegate( pDelegate );
}

void XWndSprObj::SetColor( XCOLOR col ) {
	if( m_pSprObj )
		m_pSprObj->SetColor( col );
}
void XWndSprObj::SetColor( float r, float g, float b ) {
	if( m_pSprObj )
		m_pSprObj->SetColor( r, g, b );
}
void XWndSprObj::SetFlipHoriz( BOOL bFlag ) {
	if( m_pSprObj )
		m_pSprObj->SetFlipHoriz( bFlag );
}
void XWndSprObj::SetFlipVert( BOOL bFlag ) {
	if( m_pSprObj )
		m_pSprObj->SetFlipVert( bFlag );
}
void XWndSprObj::SetFlipHoriz( bool bFlag ) {
	if( m_pSprObj )
		m_pSprObj->SetFlipHoriz( xboolToBOOL( bFlag ) );
}
void XWndSprObj::SetFlipVert( bool bFlag ) {
	if( m_pSprObj )
		m_pSprObj->SetFlipVert( xboolToBOOL( bFlag ) );
}

void XWndSprObj::SetAction( ID idAct, xRPT_TYPE typeLoop ) {
	if( m_pSprObj )
		m_pSprObj->SetAction( idAct, typeLoop );
}
void XWndSprObj::GoFirstFrame( void ) {
	if( m_pSprObj )
		m_pSprObj->ResetAction();
}
void XWndSprObj::GoRandomFrame() {
	if( m_pSprObj )
		m_pSprObj->JumpToRandomFrame();
}
void XWndSprObj::SetSizeSprObjHeight( float h ) {
	float ratio = h / m_pSprObj->GetHeight();
	SetScaleLocal( ratio );
}
