/*
 *  SprObj.cpp
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "etc/XGraphicsDef.h"
#include "Sprite.h"
#include "SprDat.h"
#include "XActDat.h"
#include "SprMng.h"
#include "XArchive.h"
#include "XDelegateSprObj.h"
#include "XFramework/client/XApp.h"
#include "XFramework/client/XClientMain.h"
#include "etc/Debug.h"
#include "XActObj2.h"
#include "SprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xSpr;

#ifdef _SPR_USE_LUA
#pragma message("--------------------sprite use Lua!!")
#endif


///////////////////////////////////////////////////////////////
//
// XSprObj
//
////////////////////////////////////////////////////////////////
#define OBJACT_LOOP( K )	int K##i;		\
							XActObj **ppObjActs = m_ppObjActs;	\
							K##i = m_nNumObjActs;		\
							while( K##i-- )			\
							{						\
								XActObj *K = *ppObjActs++;	
#define OBJACT_LOOP_REV( K )	int K##i;		\
								XActObj **ppObjActs = m_ppObjActs + (m_nNumObjActs - 1);	\
								K##i = m_nNumObjActs;		\
								while( K##i-- )			\
								{						\
									XActObj *K = *ppObjActs--;	
#define LOOP_END		}

float XSprObj::s_LuaDt = 0;		// LuaDraw()에서 씀. 이번프레임에 결정된 dt값을 저장한다

XSprObj::XSprObj( DWORD dwID ) 
{ 
	Init();
	m_dwID = dwID;
}

/**
 @brief 
*/
XSprObj::XSprObj( LPCTSTR szFilename, 
									XDelegateSprObj *pDelegate/* = nullptr*/ ) 
{ 
	Init(); 
	const bool bBatch = false;		// 이 생성자는 모두 UI쓰는걸로 가정하고 일단 false
	m_spDat = LoadInternal( szFilename,
													XE::xHSL(),
													true,
													bBatch,
													&m_Async.m_idAsyncLoad );
	// 	Load( szFilename, XE::xHSL(), false, FALSE, false );
	if( m_spDat->m_pSprDat )
		OnFinishLoad( m_spDat->m_pSprDat );
	m_pDelegate = pDelegate; 
}

/**
 @brief 
*/
// XSprObj::XSprObj( LPCTSTR szFilename, 
// 									const XE::xHSL& hsl, 
// 									XDelegateSprObj *pDelegate/* = nullptr*/ ) 
// {
// 	Init();
// 	m_spDat = LoadInternal( szFilename, hsl, true, &m_Async.m_idAsyncLoad );
// // 	Load( szFilename, hsl, false, FALSE, false );
// 	if( m_spDat->m_pSprDat )
// 		OnFinishLoad( m_spDat->m_pSprDat );
// 	m_pDelegate = pDelegate;
// }

/**
 @brief 
*/
XSprObj::XSprObj( LPCTSTR szFilename, 
									const XE::xHSL& hsl, 
									bool bUseAtlas, 
									bool bBatch,
									XDelegateSprObj *pDelegate/* = nullptr*/ )
{
	Init();
	m_spDat = LoadInternal( szFilename, hsl, bUseAtlas, bBatch, &m_Async.m_idAsyncLoad );
// 	Load( szFilename, hsl, bUseAtlas, FALSE, false );
	if( m_spDat->m_pSprDat )
		OnFinishLoad( m_spDat->m_pSprDat );
	m_pDelegate = pDelegate;
}

// for lua
// XSprObj::XSprObj( BOOL bKeepSrc, const char *cFilename ) 
// {
// 	Init();
// 	m_spDat = LoadInternal( cFilename, XE::xHSL(), true, &m_Async.m_idAsyncLoad );
// // 	Load( C2SZ(cFilename), XE::xHSL(), false, bKeepSrc, false );
// 	if( m_spDat->m_pSprDat )
// 		OnFinishLoad( m_spDat->m_pSprDat );
// }
#ifdef WIN32
// XSprObj::XSprObj( BOOL bKeepSrc, LPCTSTR szFilename ) 
// {
// 	Init();
// 	m_spDat = LoadInternal( szFilename, XE::xHSL(), true, &m_Async.m_idAsyncLoad );
// //	Load( szFilename, XE::xHSL(), false, bKeepSrc, false );
// 	if( m_spDat->m_pSprDat )
// 		OnFinishLoad( m_spDat->m_pSprDat );
// }
#endif // WIN32

void XSprObj::Destroy( void )
{
#ifdef _SPR_USE_LUA
	SAFE_DELETE( m_pLua );
#endif
	// m_pSprDat를 삭제한다. 그러나 매니저를 통해 삭제해야한다. 그리고 RefCnt개념을 써야 한다
	SPRMNG->Release( m_spDat );
	
	for( auto& useSpr : m_aryUseSprObj ) {
		SAFE_DELETE( useSpr.m_pSprObj );
	}
	m_aryUseSprObj.clear();
	for( int i = 0; i < m_nNumObjActs; i ++ ) {
		SAFE_DELETE( m_ppObjActs[i] );
	}
	SAFE_DELETE_ARRAY( m_ppObjActs );

	
}
LPCTSTR XSprObj::GetSprFilename() 
{
	return ( GetpSprDat() ) ? GetpSprDat()->GetszFilename() : _T( "" );
}

float XSprObj::GetSpeedCurrentAction() 
{
	XBREAKF( !GetAction()
					 , "sprobj id=%d %s GetAction()=NULL"
							, m_dwID, m_spDat->m_pSprDat->GetszFilename() );
	return GetAction()->GetfSpeed();
}

const XSprite* XSprObj::GetSprite( int idx ) const
{
	if( XBREAK( IsError() ) )
		return nullptr;
	if( IsAsyncLoading() ) {
		return nullptr;
	}
	return m_spDat->m_pSprDat->GetSprite( idx );
}

XSprite* XSprObj::GetSpriteMutable( int idx )
{
	if( XBREAK( IsError() ) )
		return nullptr;
	if( IsAsyncLoading() ) {
		return nullptr;
	}
	return m_spDat->m_pSprDat->GetSpriteMutable( idx );
}

float XSprObj::GetWidth() 
{
	XBREAK( GetpObjActCurr() == NULL );
	XBREAK( GetpObjActCurr()->GetpAction() == NULL );
	XE::VEC2 vLT = GetpObjActCurr()->GetpAction()->GetBoundBoxLT();
	XE::VEC2 vRB = GetpObjActCurr()->GetpAction()->GetBoundBoxRB();
	return ( vRB.x - vLT.x ) * GetScaleX();
}
float XSprObj::GetHeight() 
{
	XBREAK( GetpObjActCurr() == NULL );
	XBREAK( GetpObjActCurr()->GetpAction() == NULL );
	XE::VEC2 vLT = GetpObjActCurr()->GetpAction()->GetBoundBoxLT();
	XE::VEC2 vRB = GetpObjActCurr()->GetpAction()->GetBoundBoxRB();
	return ( vRB.y - vLT.y ) * GetScaleY();
}

XE::VEC2 XSprObj::GetAdjust() 
{
	XE::VEC2 v;
	XBREAK( GetpObjActCurr() == NULL );
	XLayerImage *pLayer = (XLayerImage *)GetpObjActCurr()->GetLayer( xSpr::xLT_IMAGE, 0 );
	if( pLayer )
		if( pLayer->GetpSpriteCurr() ) {
			//				return pLayer->GetpSpriteCurr()->GetAdjust() * m_fScaleY * pLayer->GetcnScale().fScaleY;
			return pLayer->GetpSpriteCurr()->GetAdjust() * GetScale() * pLayer->GetcnScale().m_vScale;
		}
	return v;
}

bool XSprObj::IsHaveAction( ID idAct ) 
{
	if( IsError() ) {
		return false;
	}
	if( IsAsyncLoading() ) {
		return false;
	}
	return m_spDat->m_pSprDat->IsHaveAction( idAct );
}

const XActDat* XSprObj::GetAction( ID idAct )  const
{
	if( IsError() ) {
		return nullptr;
	}
	if( IsAsyncLoading() ) {
		return nullptr;
	}
	if( XBREAK( idAct >= XSprDat::MAX_ID ) )		// id는 unsigned이므로 < 0은 검사할필요 없음
		return NULL;
	return m_spDat->m_pSprDat->GetAction( idAct );
}

const XActDat* XSprObj::GetAction() const 
{
	if( IsError() ) {
		return nullptr;
	}
	if( IsAsyncLoading() ) {
		return nullptr;
	}
	if( XBREAK( GetpObjActCurr() == nullptr ) )
		return nullptr;
	return GetpObjActCurr()->GetpAction();
}

ID XSprObj::GetActionID() const
{		// GetAction()->GetID()이렇게 쓰지말고 이걸쓸것
	auto pAction = GetAction();
	if( pAction ) {
		return pAction->GetID();
	}
	return m_Async.m_idAct;
}


float XSprObj::GetPlayTime() const
{
	auto pAction = GetAction();
	if( pAction )
		return pAction->GetPlayTime();
	return 0.0f;
}

XLayerMove* XSprObj::GetpLayerMove( xSpr::xtLayer type, int nLayer ) 
{
	XBaseLayer *pLayer = GetLayer( type, nLayer );
#ifdef _XDEBUG
	if( pLayer && pLayer->IsTypeLayerMove() == FALSE ) {
		XERROR( "LayerMove타입이 아니다" );
		return NULL;
	}
#endif
	return (XLayerMove *)pLayer;
}
XLayerImage* XSprObj::GetpLayerImage( xSpr::xtLayer type, int nLayer ) 
{
	XBaseLayer *pLayer = GetLayer( type, nLayer );
#ifdef _XDEBUG
	if( pLayer->IsTypeLayerImage() == FALSE ) {
		XERROR( "LayerImage타입이 아니다" );
		return NULL;
	}
#endif
	return (XLayerImage *)pLayer;
}
XLayerObject* XSprObj::GetpLayerObj( xSpr::xtLayer type, int nLayer ) 
{
	XBaseLayer *pLayer = GetLayer( type, nLayer );
#ifdef _XDEBUG
	if( pLayer->IsTypeLayerObj() == FALSE ) {
		XERROR( "LayerObj타입이 아니다" );
		return NULL;
	}
#endif
	return (XLayerObject *)pLayer;
}

XBaseLayer* XSprObj::GetLayer( xSpr::xtLayer type, int nLayer ) 
{
	XBaseLayer *pLayer = GetpObjActCurr()->GetLayer( type, nLayer );
#ifdef _XDEBUG
	if( pLayer == NULL ) {
		XERROR( "레이어를 찾을수 없다. %d %d", (int)type, nLayer );
		return NULL;
	}
#endif
	return pLayer;
} // lx, ly는 this가 화면에 출력된 좌표를 0,0으로 하는 로컬좌표

/**
 @brief 
*/
XActObj *XSprObj::AddObjAct( int idx, const XActDat* pAction )
{
#ifdef _XDEBUG
	XBREAKF( idx >= m_nNumObjActs,
						"%s id=%d ACT %s(%d) idx(%d) >= m_nNumObjActs(%d)"
							, m_spDat->GetszFilename(), GetdwID(), pAction->GetszActName(), pAction->GetID(), idx, m_nNumObjActs );
#endif
	auto pObjAct = new XActObj( this, pAction );
	m_ppObjActs[ idx ] = pObjAct;
	return pObjAct;
}
/**
 @brief 
*/
void XSprObj::SetAction( DWORD idAct, xRPT_TYPE playType, BOOL bExecFrameMove )
{
  XBREAK( m_bCallHandler == true );   // 콜백실행중 SetAction금지;
	XBREAK( idAct == 0 );
	if( m_spDat->m_pSprDat == nullptr ) {
#ifdef _XASYNC_SPR
		XBREAK( m_Async.m_idAsyncLoad == 0 );
		m_Async.m_idAct = idAct;
		m_Async.m_playType = playType;
#endif // _XASYNC_SPR
		return;
	}
	if( _m_pObjActCurr 
			&& idAct == GetAction()->GetID() 
			&&	m_PlayType == playType ) {		// 이미 셋된 액션아이디로 다시 셋시킬순 없다
		return;
	}
	m_fFrameCurrent = 0;
	m_bFinish = FALSE;
	m_multiplySpeed = 1.0f;	// 스피드 배수도 초기화
	auto pAction = m_spDat->GetAction( idAct );
	OBJACT_LOOP( pObjAct )
		if( pObjAct->GetpAction() == pAction ) {
			SetpObjActCurr( pObjAct );
			break;
		}
	LOOP_END
	SetKeyCurrStart();					// 키포지션을 첨으로 되돌려주고
	if( playType == xRPT_DEFAULT )
		m_PlayType = pAction->GetPlayMode();
	else
		m_PlayType = playType;

}

////////////////////////////////////////////////////////////////////
/**
 @brief 현재 플레이중인 애니메이션의 전체 프레임중 랜덤위치로 이동한다.
*/
void XSprObj::JumpToRandomFrame()
{
	auto pAction = GetActionMutable();
	if( !pAction )
		return;
	const auto maxFrame = pAction->GetfMaxFrame();
	m_fFrameCurrent = xRandomF( maxFrame );
// 	JumpKeyPos( pAction, m_fFrameCurrent );
// 	pAction->ExecuteKey( this, m_nKeyCurr, m_fFrameCurrent );
// 앞쪽키를 실행하지 않고 바로 중간으로 건너뛰어버리면
// 앞쪽키에서 수행해야할 작업(blendfunc같은)을 건너뛰어버려서 잘못 표시된다.
// 따라서 중간으로 건너뛰더라도 앞쪽의 키는 모두 수행해야 한다.
	int idxKey = 0;	// 첫키부터 실행
	pAction->ExecuteKey( this, idxKey, m_fFrameCurrent );		// frameCurr까지 모든 키 실행
}

/**
 @brief 
*/
void XSprObj::FrameMove( float dt )
{
#ifdef _XASYNC_SPR
	// 비동기 로딩중이면 진행안함. 로딩이 끝나면 SprMng쪽에서 pSprDat을 세팅함.
	// 비동기로 예약된 setAction
	if( m_Async.m_idAsyncLoad ) {
		if( m_spDat->m_pSprDat ) {
			OnFinishLoad( m_spDat->m_pSprDat );
			if( m_Async.m_idAct ) {
				SetAction( m_Async.m_idAct, m_Async.m_playType );
			}
			// 비동기 로딩 완료
			m_Async.Clear();
		}
	}
#endif // _XASYNC_SPR
	if( m_spDat->m_pSprDat == nullptr ) {
		return;
	}
	if( m_bFinish )
		return;		// 애니메이션이 끝났으면 더이상 실행하지 않음
#ifdef _XTEST
	if( GetpObjActCurr() == nullptr )
		return;
#endif // _XTEST
	auto pAction = GetActionMutable();
	if( pAction == nullptr )
		return;
	BOOL bPlay = IsPlaying();
	if( m_pParentSprObj )
		bPlay = m_pParentSprObj->IsPlaying();
	if( bPlay ) {			// 멈춤 모드가 아닐때만 프레임을 진행시킨다
		m_fFrameCurrent += pAction->GetfSpeed() * dt * m_multiplySpeed;
	}
	if( m_fFrameCurrent >= pAction->GetfMaxFrame() ) {	// 맥스 프레임을 넘어가면
		// 프레임스키핑으로 가장마지막 프레임에 있는 키를 건너뛸수도 있으므로 프레임을 첨으로 감기전에 마지막으로 키를 실행해준다
		pAction->ExecuteKey( this, m_nKeyCurr, m_fFrameCurrent );
		GetpObjActCurr()->FrameMove( dt, m_fFrameCurrent );
		if( m_PlayType == xRPT_LOOP ) {
//			m_bFinish = TRUE;	// 루핑은 finish라는게 없다
			m_fFrameCurrent = pAction->GetRepeatMark() + (m_fFrameCurrent - pAction->GetfMaxFrame());
			if( pAction->GetRepeatMark() > 0 ) {	// 도돌이표 마크가 있으면
				JumpKeyPos( pAction, pAction->GetRepeatMark() );		// 돌아가는 위치로 키 실행없이 바로 점프함
				// 도돌이표 마크 이후키만 실행해줘야함
				pAction->ExecuteKey( this, m_nKeyCurr, m_fFrameCurrent );
				GetpObjActCurr()->FrameMove( dt, m_fFrameCurrent );
			} else {
				SetKeyCurrStart();
				// 프레임이 첨으로 감겼으므로 키를 한번 실행해줘야 한다
				GetpObjActCurr()->ClearLayer();
				pAction->ExecuteKey( this, m_nKeyCurr, m_fFrameCurrent );
				GetpObjActCurr()->FrameMove( dt, m_fFrameCurrent );
			}
		} else
		if( m_PlayType == xRPT_1PLAY ) {
			m_bFinish = TRUE;
			m_fFrameCurrent = pAction->GetfMaxFrame();
			if( m_pDelegate )
				m_pDelegate->OnFinishPlayAction( this, pAction->GetID(), 0 );
//			GetpObjActCurr()->ClearLayer();	// 1PLAY라도 SprObj객체를 파괴하지 않는한 마지막 프레임에 머물러 있는게 좋을듯.
		} else 
		if( m_PlayType == xRPT_1PLAY_CONT )	{// 이건 쓰지말자 끝에서 계속 돌아야 한다면 도돌이표를 써도 된다
			m_bFinish = TRUE;
			m_fFrameCurrent = pAction->GetfMaxFrame();
		} else  {
			m_bFinish = TRUE;
			m_fFrameCurrent = pAction->GetfMaxFrame();
		}
	} else {
		pAction->ExecuteKey( this, m_nKeyCurr, m_fFrameCurrent );
		GetpObjActCurr()->FrameMove( dt, m_fFrameCurrent );
	}
}

// fFrameCurrent위치로 바로 점프한다. 그사이의 키는 실행하지 않는다
void XSprObj::JumpKeyPos( XActDat *pAction, float fJumpFrame )
{
	SetKeyCurrStart();		// 키를 첨으로 돌린다
	// 키위치를 점프프레임위치의 키로 바로 이동
	pAction->JumpKey( this, m_nKeyCurr, fJumpFrame );
}
/**
 스프라이트 객체를 생성하고 로딩한다.
*/
// BOOL XSprObj::Load( LPCTSTR szFilename, const XE::xHSL& hsl, bool bUseAtlas, BOOL bKeepSrc, bool bAsyncLoad )
// { 
// 	
// 	XBREAK( SPRMNG == nullptr );
// 	if( XBREAK( m_spDat != nullptr ) ) {
// 		SPRMNG->Release( m_spDat );
// 		m_spDat = nullptr;
// 	}
// 	// 비동기로딩시에는 파일I/O만 예약한다.
// 	m_spDat = SPRMNG->Load( szFilename,
// 													hsl,
// 													bUseAtlas,
// 													bKeepSrc,
// 													bAsyncLoad,
// 													&m_Async.m_idAsyncLoad );
// 	XBREAK( spDat == nullptr );
// 	// 레이어등을 생성한다.
// 	if( spDat->m_pSprDat )
// 		OnFinishLoad( spDat->m_pSprDat );
// 	return TRUE;
// } // Load

XSPDat XSprObj::LoadInternal( LPCTSTR szFilename, 
															const XE::xHSL& hsl, 
															bool bUseAtlas, 
															bool bBatch, 
															ID* pOutidAsync ) const
{
	// 비동기로딩시에는 파일I/O만 예약한다.
	return SPRMNG->Load( szFilename,
											 hsl,
											 bUseAtlas,
											 FALSE,
											 true,
											 bBatch,
											 pOutidAsync );
} // Load

#ifdef WIN32
XSPDat XSprObj::LoadInternal( const char* cFilename, 
															const XE::xHSL& hsl, 
															bool bUseAtlas, 
															bool bBatch,
															ID* pOutidAsync ) const
{
	const _tstring strFile = C2SZ(cFilename);
	return LoadInternal( strFile.c_str(), hsl, bUseAtlas, bBatch, pOutidAsync );
}
#endif // WIN32

/**
 루아객체를 다시 만든다(완전 땜빵)
 Load안에서 불린 CreateScript()가 virtual로 작동을 못해서 XSprObj생성자가 끝난후
 루아객체를 제대로 생성해주기위해 다시한번 재생성한다. 땜빵코드
*/
BOOL XSprObj::RecreateLua( void ) 
{
#ifdef _SPR_USE_LUA
	SAFE_DELETE( m_pLua );
#endif
	// SprDat에 루아코드가 있다면 루아쓰레드를 만들어야 한다
	if( m_spDat->m_pSprDat->GetpcLuaAll() )	{
#ifdef _SPR_USE_LUA
		m_pLua = CreateScript();
		//		m_pLua = FACTORY->CreateScript();	// virtual
		m_pLua->DoString( m_spDat->GetpcLuaAll() );
#endif
	}
	return TRUE;
}

XSprObj* XSprObj::AddSprObj( LPCTSTR szSpr
 														, ID idAct
														, xRPT_TYPE playMode
														, ID idBase, ID idLocal
														, ID idActParent
														, ID idLayerParent )
{
	if( idActParent == 0 )
		return nullptr;
	if( idBase != 0 && idLocal == 0 ) {
		XLOG( "AddSprObj: 로컬 아이디를 지정하지 않았습니다." );
		return NULL;
	}
	ID idSprObj = idBase + idLocal;
	if( XBREAK( idSprObj == 0 ) )		// 게임에선 이런경우가 없어야 한다.
		return NULL;
	for( auto& useSpr : m_aryUseSprObj ) {
		if( useSpr.m_idActParent == idActParent 
			&& useSpr.m_idLayerParent == idLayerParent ) {
			useSpr.m_pSprObj->Reset();		// 새로 생성되지 않고 캐쉬에서 가져온 경우도 기본사항은 리셋되어 있어야 함
 			useSpr.m_pSprObj->SetAction( idAct, playMode );
			return useSpr.m_pSprObj;
		}
	}
#ifdef _XDEBUG
	if( m_nNumSprObjs >= 10 ) {
		XLOG( "m_nNumSprObjs(%d)가 범위를 벗어났다", m_nNumSprObjs );
		return NULL;
	}
#endif
	XSprObj *pSprObj = CreateSprObj( szSpr );
	pSprObj->SetdwID( idSprObj );
	pSprObj->SetAction( idAct, playMode );
	// 일단 최대 CreateObj키는 10개로 제한하자. SprDat가 로드될때 사전로드SprObj를 모든 액션에서 다 로드하면 효율이 안좋을것 같다
	xUseSprObj useSpr;
	useSpr.m_idActParent = idActParent;
	useSpr.m_idLayerParent = idLayerParent;
	useSpr.m_pSprObj = pSprObj;
	m_aryUseSprObj.Add( useSpr );
	return pSprObj;	
}

#ifdef _VER_OPENGL
// void XSprObj::Draw( const MATRIX& mWorld )
// {
// 	MATRIX m;
// 	MatrixIdentity( m );
// 	Draw( 0, 0, m );
// }
void XSprObj::Draw( float x, float y )
{
	MATRIX m;
	MatrixIdentity( m );
	Draw( x, y, m );
}
void XSprObj::Draw( float x, float y, const MATRIX& mParent )
{ 
#if defined(_XSPR_LAZY_LOAD) || defined(_XASYNC_SPR)
	if( XBREAK( IsError() ) )
		return;
	if( IsAsyncLoading() ) {
		return;
	}
#endif // defined(_XSPR_LAZY_LOAD) || defined(_XASYNC_SPR)
	auto pObjAct = GetpObjActCurr();
	if( pObjAct == nullptr )
		return;
	auto pAct = pObjAct->GetpAction();
	MATRIX mWorld, m;
	if( m_DrawMode == xDM_NONE )		// 아무것도 그리지 않는 모드
		return;
	x += m_fAdjustAxisX;		// 좌표축 보정
	y += m_fAdjustAxisY;
	MatrixIdentity( mWorld );
	if( m_fAdjustAxisX || m_fAdjustAxisY )    {
		MatrixTranslation( m, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
		MatrixMultiply( mWorld, mWorld, m );
	}
	const auto vScaleAct = pAct->GetvScale();
	const XE::VEC2 vScale = XE::VEC2( m_fScaleX, m_fScaleY ) * vScaleAct;
//	if( m_fScaleX != 1.0f || m_fScaleY != 1.0f || vScaleAct != XE::VEC2(1) ) {
	if( vScale != XE::VEC2(1,1) ) {
		MatrixScaling( m, vScale.x, vScale.y, 1.0f );
		MatrixMultiply( mWorld, mWorld, m );
	}
	const auto v3RotAct = pAct->GetvRotate();
	float dRotZ = m_fRotZ + v3RotAct.z;
	float dRotY = m_fRotY + v3RotAct.y;
	if( dRotZ || dRotY ) {
		MatrixRotationY( m, D2R( dRotY ) );
		MatrixRotationZ( m, D2R( dRotZ ) );
		MatrixMultiply( mWorld, mWorld, m );
	}
	if( m_fRotY )	{
		MatrixRotationY( m, D2R( m_fRotY ) );
		MatrixMultiply( mWorld, mWorld, m );
	}
	MatrixTranslation( m, x, y, 0 );
	MatrixMultiply( mWorld, mWorld, m );
	MatrixMultiply( mWorld, mWorld, mParent );
	XEFFECT_PARAM EffectParam;
	if( m_DrawMode != xDM_ERROR || m_fAlpha < 1.0f ) {	// 외부지정 드로우 모드가 있으면 그 파라메터를 넘김
		EffectParam.drawMode = m_DrawMode;
		EffectParam.fAlpha = m_fAlpha;
		GetpObjActCurr()->Draw( 0, 0, mWorld, &EffectParam );
		m_DrawMode = xDM_ERROR;		// 드로우모드는 한번쓰고 다시 초기화시킴
	} else {
		GetpObjActCurr()->Draw( 0, 0, mWorld, &EffectParam );
	}
}
#endif // GL
#ifdef _VER_DX
void XSprObj::Draw( float x, float y, const D3DXMATRIX &m ) 
{ 
#if defined(_XSPR_LAZY_LOAD) || defined(_XASYNC_SPR)
	if( m_spDat->m_pSprDat == nullptr )		// SprDat가 없으면 일단 찍지 않음.
		return;
#endif
	D3DXMATRIX mTrans, mScale, mRot, mWorld, mAxis;
	if( m_DrawMode == xDM_NONE )		// 아무것도 그리지 않는 모드
		return;
	x += m_fAdjustAxisX;		// 좌표축 보정
	y += m_fAdjustAxisY;
	XE::VEC2 vScale = XE::VEC2( m_fScaleX, m_fScaleY );
	vScale *= GetpObjActCurr()->GetpAction()->GetvScale();
	float dRotZ = m_fRotZ + GetpObjActCurr()->GetpAction()->GetvRotate().z;
	float dRotY = m_fRotY + GetpObjActCurr()->GetpAction()->GetvRotate().y;

	D3DXMatrixTranslation( &mTrans, x, y, 0 );
	D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(dRotY), D3DXToRadian(m_fRotX), D3DXToRadian(dRotZ) );
	D3DXMatrixScaling( &mScale, vScale.x, vScale.y, 1.0f );
//	D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
	D3DXMatrixTranslation( &mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
	mWorld = mAxis * mScale * mRot * mTrans;
	mWorld *= m;
	if( m_DrawMode != xDM_ERROR || m_fAlpha < 1.0f )	// 외부지정 드로우 모드가 있으면 그 파라메터를 넘김
	{
		XEFFECT_PARAM EffectParam;
		EffectParam.drawMode = m_DrawMode;
		EffectParam.fAlpha = m_fAlpha;
		GetpObjActCurr()->Draw( 0, 0, mWorld, &EffectParam );		// x, y를 매트릭스에 반영했기때문에 로컬좌표 0을 넘겨준다
		m_DrawMode = xDM_ERROR;		// 드로우모드는 한번쓰고 다시 초기화시킴
	}
	else
		GetpObjActCurr()->Draw( 0, 0, mWorld, NULL );		// x, y를 매트릭스에 반영했기때문에 로컬좌표 0을 넘겨준다
}
#endif // dx

MATRIX* XSprObj::GetMatrix( MATRIX *pOut, float lx, float ly )
{
	lx += m_fAdjustAxisX;		// 좌표축 보정
	ly += m_fAdjustAxisY;
	MATRIX mTrans, mScale, mAxis;
	MatrixTranslation( mTrans, lx, ly, 0 );
	MATRIX mRx, mRy;
	MatrixRotationY( mRy, D2R(m_fRotY) );
	MatrixRotationX( mRx, D2R(m_fRotX) );
	MatrixMultiply( mRx, mRx, mRy );		// 순서가 XY인가 YX인가 ㅡ.ㅡ?
//	MatrixRotationYawPitchRoll( &mRot, D3DXToRadian(m_fRotY), D3DXToRadian(m_fRotX), D3DXToRadian(m_fRotZ) );
	MatrixScaling( mScale, m_fScaleX, m_fScaleY, 1.0f );
	MatrixTranslation( mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
	// * mRx를 살리니까 릴리즈에서 mRx계산이 제대로 안된다. 이유는 모름
//	*pOut = mAxis * mScale * /*mRx * */mTrans;	
	MatrixMultiply( *pOut, mAxis, mScale );
	MatrixMultiply( *pOut, *pOut, mTrans );
	return pOut;
}

// lx, ly는 this가 화면에 출력된 좌표를 0,0으로 하는 로컬좌표
DWORD XSprObj::GetPixel( float cx, float cy, float mx, float my, const MATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb )
{
	*pa = 0;
	if( pr )		*pr = 0;
	if( pg )	*pg = 0;
	if( pb )	*pb = 0;
	MATRIX mWorld, m2;
	if( m_DrawMode == xDM_NONE )		// 아무것도 그리지 않는 모드
		return 0;
	GetMatrix( &m2, cx, cy );
	MatrixMultiply( mWorld, m2, m );
//	mWorld = m2 * m;
	return GetpObjActCurr()->GetPixel( cx, cy, mx, my, mWorld, pa, pr, pg, pb );

}
// static.
// 이 클래스에서 쓰는 루아글루함수들을 pLua에 등록시킨다
#ifdef _SPR_USE_LUA
void XSprObj::RegisterLua( XLua *pLua )
{
	// SprObj
	pLua->Register_Class<XSprObj>("XSprObj");
#ifdef WIN32
//	pLua->Register_ClassCon<XSprObj,const char*>();		// 
#else
//	pLua->Register_ClassCon<XSprObj,const char*,XBaseObj*>();		// 
//	pLua->Register_ClassCon<XSprObj,const char*>();		// 
#endif
	pLua->RegisterCPPFunc<XSprObj>("SetAction", &XSprObj::LuaSetAction );
	pLua->RegisterCPPFunc<XSprObj>("Draw", &XSprObj::LuaDraw );
	pLua->RegisterCPPFunc<XSprObj>("SetScale", &XSprObj::LuaSetScale );
	pLua->RegisterCPPFunc<XSprObj>("SetScaleXY", &XSprObj::LuaSetScaleXY );
	pLua->RegisterCPPFunc<XSprObj>("SetRotate", &XSprObj::LuaSetRotate );
	pLua->RegisterCPPFunc<XSprObj>("SetAlpha", &XSprObj::SetfAlpha );
	pLua->RegisterCPPFunc<XSprObj>("AddSprObj", &XSprObj::LuaAddSprObj );
	pLua->RegisterVar( "PLAY_ONE", xRPT_1PLAY );
	pLua->RegisterVar( "PLAY_ONE_CONT", xRPT_1PLAY_CONT );
	pLua->RegisterVar( "PLAY_LOOP", xRPT_LOOP );
}
#endif // not use

// 현재 액션을 화면에 draw하면 화면을 벗어나는 부분이 있는가?
BOOL XSprObj::IsDrawOutPartly( const XE::VEC2& vPos )
{
	auto pAction = GetAction();
	XBREAK( pAction == NULL );
	if( pAction )
	{
		XE::VEC2 vlLT = pAction->GetBoundBoxLT();		// 로컬
		XE::VEC2 vLT = vPos + vlLT;
		if( vLT.x < 0 )
			return TRUE;
		if( vLT.y < 0 )
			return TRUE;
		XE::VEC2 vlRB = pAction->GetBoundBoxRB();
		XE::VEC2 vRB = vPos + vlRB;
		if( vRB.x > XE::GetGameWidth() )
			return TRUE;
		if( vRB.y > XE::GetGameHeight() )
			return TRUE;
	}
	return FALSE;
}

// vPos에 draw할때 왼쪽을 벗어나는 부분이 있는가.
BOOL XSprObj::IsDrawOutPartlyLeft( const XE::VEC2& vPos )
{
	auto pAction = GetAction();
	XBREAK( pAction == NULL );
	if( pAction )
	{
		XE::VEC2 vlLT = pAction->GetBoundBoxLT();		// 로컬
		XE::VEC2 vLT = vPos + vlLT;
		if( vLT.x < 0 )
			return TRUE;
	}
	return FALSE;
}
// vPos에 draw할때 오른쪽을 벗어나는 부분이 있는가.
BOOL XSprObj::IsDrawOutPartlyRight( const XE::VEC2& vPos )
{
	auto pAction = GetAction();
	XBREAK( pAction == NULL );
	if( pAction )
	{
		XE::VEC2 vlRB = pAction->GetBoundBoxRB();
		XE::VEC2 vRB = vPos + vlRB;
		if( vRB.x > XE::GetGameWidth() )
			return TRUE;
	}
	return FALSE;
}

// vPos에 draw할때 아래쪽을 벗어나는 부분이 있는가.
BOOL XSprObj::IsDrawOutPartlyBottom( const XE::VEC2& vPos )
{
	auto pAction = GetAction();
	XBREAK( pAction == NULL );
	if( pAction )
	{
		XE::VEC2 vlRB = pAction->GetBoundBoxRB();
		XE::VEC2 vRB = vPos + vlRB;
		if( vRB.y > XE::GetGameHeight() )
			return TRUE;
	}
	return FALSE;
}

// vPos에 draw할때 위쪽을 벗어나는 부분이 있는가.
BOOL XSprObj::IsDrawOutPartlyTop( const XE::VEC2& vPos )
{
	auto pAction = GetAction();
	XBREAK( pAction == NULL );
	if( pAction )
	{
		XE::VEC2 vlLT = pAction->GetBoundBoxLT();		// 로컬
		XE::VEC2 vLT = vPos + vlLT;
		if( vLT.y < 0 )
			return TRUE;
	}
	return FALSE;
}

// 현재 애니메이션을 sec시간동안 플레이되게 한다.
void XSprObj::SetPlayTime( float secPlay )
{
	// 애니메이션의 최대 애니메이션 시간
	float secMax = (GetAction()->GetfMaxFrame() / 
					GetAction()->GetfSpeed()) / 60.f;
	float multiply = secMax / secPlay;
	SetmultiplySpeed( multiply );

}

int XSprObj::Serialize( XArchive& ar )
{
	XActObj *pObjAct = GetpObjActCurr();
	if( pObjAct )
	{
		ID idAct = GetActionID();
		ar << idAct;
		pObjAct->Serialize( ar );
	}

	return 1;
}

int XSprObj::DeSerialize( XArchive& ar )
{
	ID idAct;
	ar >> idAct;
	SetAction( idAct );
	XActObj *pObjAct = GetpObjActCurr();
	if( pObjAct )
		pObjAct->DeSerialize( ar );
	// $(SolutionDir)$(Configuration)
	return 1;
}
#ifdef _SPR_USE_LUA
XLua* XSprObj::CreateScript( void ) 
{ 
	return new XLua; 
}

XSprObj* XSprObj::CreateSprObj( LPCTSTR szSpr, XDelegateSprObj *pDelegate ) 
{
	return new XSprObj( szSpr, pDelegate );
}
#endif // _SPR_USE_LUA

XSprObj* XSprObj::CreateSprObj( LPCTSTR szSpr, XDelegateSprObj *pDelegate ) 
{
	return new XSprObj( szSpr, pDelegate );
}

// 새버전
XBaseLayer* XSprObj::GetpLayerByidLocalInLayer( XActObj* pActObj, ID idLocalInLayer ) 
{
	auto pLayer = pActObj->GetpLayerByidLocalInLayer( idLocalInLayer );
#ifdef _XDEBUG
	if( pLayer == nullptr ) {
		XERROR( "레이어를 찾을수 없다. idLocalInLayer=%d", idLocalInLayer );
		return nullptr;
	}
#endif
	return pLayer;
} 

XActObj* XSprObj::GetpObjAct( ID idAct ) const 
{
	if( XBREAK( idAct >= XSprDat::MAX_ID ) )
		return NULL;
	return m_ppObjActs[ idAct ];
}

#ifdef _XASYNC_SPR
// void XSprObj::OnCompleteAsyncLoad( XSprDat* pSprDat ) 
// {
// 	OnFinishLoad( pSprDat );
// }
bool XSprObj::IsAsyncLoading() const 
{
	return m_Async.m_idAsyncLoad != 0;
}
#endif // _XASYNC_SPR

void XSprObj::OnFinishLoad( XSprDat* pSprDat )
{
	XBREAK( pSprDat == nullptr );
	//
	// SprDat에 루아코드가 있다면 루아쓰레드를 만들어야 한다
	if( m_spDat->m_pSprDat->GetpcLuaAll() ) {
#ifdef _SPR_USE_LUA
		m_pLua = CreateScript();
		m_pLua->DoString( m_spDat->GetpcLuaAll() );
#endif
	}
	//
	int i, j;
	m_nNumObjActs = m_spDat->GetnNumActions();
	m_ppObjActs = new XActObj*[m_nNumObjActs];
	memset( m_ppObjActs, 0, sizeof( XActObj* ) * m_nNumObjActs );
	for( i = 0; i < m_nNumObjActs; i++ ) {
		auto pAction = m_spDat->GetActionIndex( i );		// 순차적으로 액션을 읽어온다
		auto pObjAct = AddObjAct( i, pAction );		// 추가 액션정보를 만든다.
		for( j = 0; j < pAction->GetnNumLayerInfo(); j++ ) {
			auto pLayerInfo = pAction->GetLayer( j );
			pObjAct->CreateLayer( j, pLayerInfo );
		}
	}

}

XActObj* XSprObj::GetpObjActCurr() const 
{
#if defined(_XSPR_LAZY_LOAD) || defined(_XASYNC_SPR)
	if( XBREAK( IsError() ) )
		return nullptr;
	if( IsAsyncLoading() ) {
		return nullptr;
}
#else
	XBREAK( _m_pObjActCurr == NULL );
#endif
	return _m_pObjActCurr;
}

bool XSprObj::IsError() const 
{
#ifdef _XASYNC_SPR
	return m_spDat->m_pSprDat == nullptr && m_Async.m_idAsyncLoad == 0;
#else
	return m_spDat->m_pSprDat == nullptr;
#endif // _XASYNC_SPR
}

void XSprObj::Transform( float *lx, float *ly ) 
{
	Vec3 v = Vec3( *lx, *ly, 0 );
	MATRIX m, mRotX, mRotY, mRotZ, mScale;
	MatrixRotationX( mRotX, D2R( m_fRotX ) );
	MatrixRotationY( mRotY, D2R( m_fRotY ) );
	MatrixRotationZ( mRotZ, D2R( m_fRotZ ) );
	MatrixScaling( mScale, GetScaleX(), GetScaleY(), 1.0f );
	MatrixIdentity( m );
	MatrixMultiply( m, m, mScale );
	MatrixMultiply( m, m, mRotX );
	MatrixMultiply( m, m, mRotY );
	MatrixMultiply( m, m, mRotZ );
	Vec4 v4d;
	MatrixVec4Multiply( v4d, v, m );
	*lx = v4d.x;
	*ly = v4d.y;
}

void XSprObj::Transform( float *fAngle ) 
{
	// 좌우플립됐을경우는....
	if( m_fRotY == 180.0f ) {
		float a = *fAngle;
		if( a > 180.0f )		// 일단 각도계를 -180~+180기준으로 바꾼다음
			a -= 360.0f;
		else if( a < -180.0f )
			a += 360.0f;
		*fAngle = -a;		// 부호를 바꿔준다
	}
	// 다른 각도변환은 일단은 하지 않는다.
}

void XSprObj::ResetAction() 
{	// 현재설정된 액션그대로 초기화만 시킨다
	m_fFrameCurrent = 0;
	SetKeyCurrStart();
	m_bFinish = FALSE;
}

XSprDat* XSprObj::GetpSprDat() 
{
	return m_spDat->m_pSprDat;
}

ID XSprObj::GetidActByRandom() const 
{
	return (m_spDat->m_pSprDat) ? m_spDat->m_pSprDat->GetidActByRandom() : 0;
}

bool XSprObj::IsBatchRender() const
{
	return m_spDat->m_bBatch;
}
