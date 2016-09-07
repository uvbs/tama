/*
 *  SprObj.cpp
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "SprObj.h"
#include "SprDat.h"
#include "SprMng.h"
#include "XArchive.h"
#include "XDelegateSprObj.h"
#include "XFramework/client/XApp.h"
#include "XFramework/client/XClientMain.h"
#include "etc/Debug.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define LAYER_LOOP( K )	int K##i;		\
						XBaseLayer **ppLayers = m_ppLayers;	\
						K##i = m_nNumLayers;		\
						while( K##i-- )			\
						{						\
							XBaseLayer *K = *ppLayers++;	\
							if( K == NULL )	continue;				// 이벤트레이어같은건 널이기때문에 이게 필요하다
#define LAYER_LOOP_REV( K )	int K##i;		\
							XBaseLayer **ppLayers = m_ppLayers + (m_nNumLayers - 1);	\
							K##i = m_nNumLayers;		\
							while( K##i-- )			\
							{						\
								XBaseLayer *K = *ppLayers--;	\
								if( K == NULL )	continue;				// 이벤트레이어같은건 널이기때문에 이게 필요하다
#define LOOP_END		}

#ifdef _SPR_USE_LUA
#pragma message("--------------------sprite use Lua!!")
#endif
///////////////////////////////////////////////////////////////
//
// XObjAct
//
////////////////////////////////////////////////////////////////
XObjAct::XObjAct( XSprObj *pSprObj, XAniAction *pAction ) 
{ 
	m_pAction = pAction; 
	m_pSprObj = pSprObj; 
	m_nNumLayers = pAction->GetnNumLayerInfo();
	m_ppLayers = new XBaseLayer*[ m_nNumLayers ];
	memset( m_ppLayers, 0, sizeof(XBaseLayer*) * m_nNumLayers );
}
void XObjAct::Destroy( void )
{
	// layer 삭제
	DestroyLayer();
}

void XObjAct::ClearLayer( void )
{
	LAYER_LOOP( pLayer )
		pLayer->Clear();
	LOOP_END
}
void XObjAct::FrameMove( float dt, float fFrmCurr )
{
	LAYER_LOOP( pLayer )
		pLayer->FrameMove( m_pSprObj, dt, fFrmCurr );
	LOOP_END
}
#ifdef _VER_OPENGL
void XObjAct::Draw( float x, float y, const MATRIX &m, XEFFECT_PARAM *pEffectParam )
{
	LAYER_LOOP( pLayer )
		pLayer->Draw( m_pSprObj, x, y, m, pEffectParam );
	LOOP_END
}
#else
void XObjAct::Draw( float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam )
{
	LAYER_LOOP( pLayer )
		pLayer->Draw( m_pSprObj, x, y, m, pEffectParam );
	LOOP_END
}
#endif
void XObjAct::DestroyLayer( void )
{
	int i;
	for( i = 0; i < m_nNumLayers; i ++ )
		SAFE_DELETE( m_ppLayers[i] );
	SAFE_DELETE_ARRAY( m_ppLayers );
}
XBaseLayer *XObjAct::CreateLayer( int idx, LAYER_INFO* pLayerInfo )
{
	auto pLayer = CreateLayer( idx
													, pLayerInfo->type
													, pLayerInfo->nLayer
													, pLayerInfo->fAdjustAxisX
													, pLayerInfo->fAdjustAxisY );		// 실제 레이어를 만들어준다
	if( pLayer ) {
		XBREAK( pLayerInfo->idLayer == 0 );
		pLayer->SetidLayer( pLayerInfo->idLayer );
	}
	return pLayer;
}
// pLayer가 널이면 레이어를 초기화 상태로 만든다.
XBaseLayer *XObjAct::CreateLayer( int idx, xSpr::xtLayer type, int nLayer, float fAdjAxisX, float fAdjAxisY )
{
	const XE::VEC2 vAdjAxis( fAdjAxisX, fAdjAxisY );
#ifdef _XDEBUG
	if( idx >= m_nNumLayers ) {
		XLOG( "%s id=%d ACT %s(%d) idx(%d) >= m_nNumLayers(%d)", m_pSprObj->GetSprFilename(), m_pSprObj->GetdwID(),
												m_pAction->GetszActName(), m_pAction->GetID(), idx, m_nNumLayers );
		return NULL;
	}
																
#endif
		
	XBaseLayer *pLayer = nullptr;
	{
		// 이거 나중에 XBaseLayer의스태틱 함수로 집어넣자. 안그러면 레이어종류가 추가될때마다 일일히 이런곳 찾아서 수정해줘야 한다
		switch( type ) {
			case xSpr::xLT_IMAGE:
				pLayer = new XLayerImage;
				((XLayerImage *)pLayer)->SetvAdjAxis( vAdjAxis );
				break;
			case xSpr::xLT_OBJ:
				pLayer = new XLayerObject;
				((XLayerObject *)pLayer)->SetvAdjAxis( vAdjAxis );
				break;
			case xSpr::xLT_SOUND:
				pLayer = new XLayerSound;
				break;
			case xSpr::xLT_EVENT:
				// 이벤트레이어는 게임에서 생성시킬 필요 없다
				break;
			case xSpr::xLT_DUMMY:
				pLayer = new XLayerDummy;
				((XLayerMove *)pLayer)->SetvAdjAxis( vAdjAxis );
				break;
			default:
				pLayer = NULL;
				XERROR( "layer 생성타입이 잘못되었음. %d", (int)type );
				break;
		}
		if( pLayer ) {
			pLayer->SetnLayer( nLayer );								// 지정한 레이어번호로 생성
		}
		m_ppLayers[ idx ] = pLayer;		// 널이 들어갈수도 있다
	}
	return pLayer;
}
XBaseLayer *XObjAct::GetLayer( xSpr::xtLayer type, int nLayer )
{
	LAYER_LOOP( pLayer )
		if( pLayer->GetType() == type && pLayer->GetnLayer() == nLayer )
			return pLayer;
	
	LOOP_END
	return NULL;
}

// lx, ly는 0,0센터 기준 로컬좌표
DWORD XObjAct::GetPixel( float cx, float cy, float mx, float my, const MATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb )
{
	DWORD pixel;
	LAYER_LOOP_REV( pLayer )
	{
		pixel = pLayer->GetPixel( cx, cy, mx, my, m, pa, pr, pg, pb );
		if( *pa > 0 )
			return pixel;			// 투명색이 아니면 픽셀이 있는것으로 간주하고 그 픽셀을 리턴한다
	} LOOP_END;
	return 0;
}

int XObjAct::Serialize( XArchive& ar )
{
	LAYER_LOOP( pLayer )
	{
		pLayer->Serialize( ar, m_pSprObj );
	}
	LOOP_END;
	return 1;
}

int XObjAct::DeSerialize( XArchive& ar )
{
	LAYER_LOOP( pLayer )
	{
		pLayer->DeSerialize( ar, m_pSprObj );
	}
	LOOP_END;
	return 1;
}

XBaseLayer* XObjAct::GetpLayerByidLocalInLayer( ID idLocalInLayer ) const 
{
	XBaseLayer **ppLayers = m_ppLayers;
	int num = m_nNumLayers;
	while( num-- ) {
		XBaseLayer *pLayer = *ppLayers++;
		if( pLayer == nullptr )	continue;				// 이벤트레이어같은건 널이기때문에 이게 필요하다
		if( pLayer->GetidLayer() == idLocalInLayer )
			return pLayer;
	}
	return nullptr;
}

///////////////////////////////////////////////////////////////
//
// XSprObj
//
////////////////////////////////////////////////////////////////
#define OBJACT_LOOP( K )	int K##i;		\
							XObjAct **ppObjActs = m_ppObjActs;	\
							K##i = m_nNumObjActs;		\
							while( K##i-- )			\
							{						\
								XObjAct *K = *ppObjActs++;	
#define OBJACT_LOOP_REV( K )	int K##i;		\
								XObjAct **ppObjActs = m_ppObjActs + (m_nNumObjActs - 1);	\
								K##i = m_nNumObjActs;		\
								while( K##i-- )			\
								{						\
									XObjAct *K = *ppObjActs--;	
// #define SPROBJ_LOOP( K )	int K##i;		\
// 							XSprObj **ppSprObjs = m_ppSprObjs;	\
// 							K##i = m_nNumSprObjs;		\
// 							while( K##i-- )			\
// 							{						\
// 								XSprObj *K = *ppSprObjs++;	
#define LOOP_END		}

float XSprObj::s_LuaDt = 0;		// LuaDraw()에서 씀. 이번프레임에 결정된 dt값을 저장한다

XSprObj::XSprObj( DWORD dwID ) 
{ 
	Init();
	m_dwID = dwID;
}
//#ifdef WIN32
XSprObj::XSprObj( LPCTSTR szFilename, XDelegateSprObj *pDelegate/* = nullptr*/ ) 
{ 
	Init(); 
	Load( szFilename, XE::xHSL(), false, FALSE, false ); 
	m_pDelegate = pDelegate; 
}
XSprObj::XSprObj( LPCTSTR szFilename, const XE::xHSL& hsl, XDelegateSprObj *pDelegate/* = nullptr*/ ) 
{
	Init();
	Load( szFilename, hsl, false, FALSE, false );
	m_pDelegate = pDelegate;
}
XSprObj::XSprObj( LPCTSTR szFilename, 
									const XE::xHSL& hsl, 
									bool bUseAtlas, 
									XDelegateSprObj *pDelegate/* = nullptr*/ )
{
	Init();
	Load( szFilename, hsl, bUseAtlas, FALSE, false );
	m_pDelegate = pDelegate;
}

// for lua
XSprObj::XSprObj( BOOL bKeepSrc, const char *cFilename ) 
{
	Init();
	Load( C2SZ(cFilename), XE::xHSL(), false, bKeepSrc, false );
}
#ifdef WIN32
XSprObj::XSprObj( BOOL bKeepSrc, LPCTSTR szFilename ) 
{
	Init();
	Load( szFilename, XE::xHSL(), false, bKeepSrc, false );
}
#endif // WIN32

void XSprObj::Destroy( void )
{
#ifdef _SPR_USE_LUA
	SAFE_DELETE( m_pLua );
#endif
// #ifdef _XSPR_LAZY_LOAD
// 	if( m_pSprDat == NULL )
// 		return;
// #endif // not spr_lazy_load
	// m_pSprDat를 삭제한다. 그러나 매니저를 통해 삭제해야한다. 그리고 RefCnt개념을 써야 한다
// 	XBREAK( m_pSprDat == NULL );
	if( SPRMNG && m_pSprDat )
		SPRMNG->Release( m_pSprDat );
	m_pSprDat = NULL;
	
	// 사전로드 sprobj 삭제. 
// 	{
// 		int i;
// 		for( i = 0; i < m_nNumSprObjs; i ++ )
// 			SAFE_DELETE( m_ppSprObjs[i] );
// 		SAFE_DELETE_ARRAY( m_ppSprObjs );
// 	}
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
#ifdef _XDEBUG
	if( !GetAction() )
		XLOG( "sprobj id=%d %s GetAction()=NULL", m_dwID, m_pSprDat->GetszFilename() );
#endif
	return GetAction()->GetfSpeed();
}

XSprite* XSprObj::GetSprite( int idx ) 
{
	if( XBREAK( m_pSprDat == NULL ) )
		return NULL;
	return m_pSprDat->GetSprite( idx );
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

BOOL XSprObj::IsHaveAction( ID idAct ) 
{
	if( m_pSprDat == nullptr )
		return FALSE;
	return m_pSprDat->IsHaveAction( idAct );
}

XAniAction* XSprObj::GetAction( ID idAct ) 
{
// #ifdef _XSPR_LAZY_LOAD
	if( m_pSprDat == NULL )
		return NULL;
// #endif
	if( XBREAK( idAct >= XSprDat::MAX_ID ) )		// id는 unsigned이므로 < 0은 검사할필요 없음
		return NULL;
	return m_pSprDat->GetAction( idAct );
}

XAniAction* XSprObj::GetAction() const 
{
// #ifdef _XSPR_LAZY_LOAD
	if( m_pSprDat == NULL )
		return NULL;
// #else
// 	XBREAKF( !GetpObjActCurr(), "sprobj id=%d %s m_pObjActCurr=NULL", m_dwID, m_pSprDat->GetszFilename() );
// #endif
	if( XBREAK( GetpObjActCurr() == nullptr ) )
		return nullptr;
	return GetpObjActCurr()->GetpAction();
}

ID XSprObj::GetActionID() 
{		// GetAction()->GetID()이렇게 쓰지말고 이걸쓸것
	XAniAction *pAction = GetAction();
	return ( pAction ) ? pAction->GetID() : 0;
}

float XSprObj::GetPlayTime() 
{
	XAniAction *pAction = GetAction();
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
XObjAct *XSprObj::AddObjAct( int idx, XAniAction *pAction )
{
#ifdef _XDEBUG
	if( idx >= m_nNumObjActs ) {
		XLOG( "%s id=%d ACT %s(%d) idx(%d) >= m_nNumObjActs(%d)", m_pSprDat->GetszFilename(), GetdwID(), pAction->GetszActName(), pAction->GetID(), idx, m_nNumObjActs );
		return NULL;
	}
#endif
	XObjAct *pObjAct = new XObjAct( this, pAction );
	m_ppObjActs[ idx ] = pObjAct;
	return pObjAct;
}
/**
 @brief 
*/
void XSprObj::SetAction( DWORD id, xRPT_TYPE playType, BOOL bExecFrameMove )
{
  XBREAK( m_bCallHandler == true );   // 콜백실행중 SetAction금지;
	XBREAK( id == 0 );
#ifdef _XSPR_LAZY_LOAD
	if( m_pSprDat == nullptr ) {
		// 아직 SprDat가 없어서 예약만 걸어둠.
		m_LazyInfo.idAct = id;
		m_LazyInfo.playType = playType;
		return;
	}
#else
	if( m_pSprDat == nullptr ) {
		XSprMng::sGet()->AsyncSetAction(  id, playType, bExecFrameMove );
		return;
	}
#endif
	if( _m_pObjActCurr && id == GetAction()->GetID() &&	m_PlayType == playType )		// 이미 셋된 액션아이디로 다시 셋시킬순 없다
		return;
	m_fFrameCurrent = 0;
	m_bFinish = FALSE;
	m_multiplySpeed = 1.0f;	// 스피드 배수도 초기화
	XAniAction *pAction = m_pSprDat->GetAction( id );
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
	// 레이어 내용 초기화(이부분을 왜 지웠었지?)
// 	GetpObjActCurr()->ClearLayer();
// 	if( bExecFrameMove )
// 		FrameMove( 0 );	// 액션바꾼후 젤첫프레임의 키는 한번 실행해줘야 한다 안그러면 그림이 사라지는 상태가 된다
// 	GetpObjActCurr()->ClearLayer();		
// 	if( bExecFrameMove )
// 		FrameMove(0);	// 액션바꾼후 젤첫프레임의 키는 한번 실행해줘야 한다 안그러면 그림이 사라지는 상태가 된다
	
//	pAction->SetAction( GetpObjActCurr() );	// Load에서 한번만 하도록 바뀜
//	Update();			// 로딩된 직후에 레이어를 한번 업데이트 해줘야 함. 안그러면 
}

////////////////////////////////////////////////////////////////////
/**
 @brief 현재 플레이중인 애니메이션의 전체 프레임중 랜덤위치로 이동한다.
*/
void XSprObj::JumpToRandomFrame()
{
	auto pAction = GetAction();
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
#ifdef _XSPR_LAZY_LOAD
	auto& lazy = m_LazyInfo;
	if( m_pSprDat == nullptr && lazy.strFilename.empty() == false ) {
#ifndef _XASYNC_SPR
		// SprDat가 없을땐 1초마다 한번씩 파일이 생겼는지 확인해본다.
		if( lazy.timerLazyLoad.IsOver() ) 
#endif // not _XASYNC_SPR
		{
			if( Load( lazy.strFilename.c_str(), lazy.m_HSL, lazy.m_bUseAtlas, lazy.bKeepSrc, false ) ) {
				SetAction( lazy.idAct, lazy.playType );
				lazy.timerLazyLoad.Off();
			} else
				lazy.timerLazyLoad.Reset();
		}
		if( lazy.timerLazyLoad.IsOff() )
			lazy.timerLazyLoad.Set( 1.f );
		return;
	}
#endif
	if( m_bFinish )		
		return;		// 애니메이션이 끝났으면 더이상 실행하지 않음
	XAniAction *pAction = GetAction();
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
void XSprObj::JumpKeyPos( XAniAction *pAction, float fJumpFrame )
{
	SetKeyCurrStart();		// 키를 첨으로 돌린다
	// 키위치를 점프프레임위치의 키로 바로 이동
	pAction->JumpKey( this, m_nKeyCurr, fJumpFrame );
}
/**
 스프라이트 객체를 생성하고 로딩한다.
*/
BOOL XSprObj::Load( LPCTSTR szFilename, const XE::xHSL& hsl, bool bUseAtlas, BOOL bKeepSrc, bool bAsyncLoad )
{ 
	XAUTO_LOCK2( XGraphics::s_spLock );
#ifdef _XDEBUG
	if( m_pSprDat ) {
		XLOG( "m_pSprDat(%s)가 해제되지 않았다 %s", m_pSprDat->GetszFilename(), szFilename );
		SPRMNG->Release( m_pSprDat );
		m_pSprDat = nullptr;
	}
#endif
	XBREAK( SPRMNG == NULL );
	m_pSprDat = SPRMNG->Load( szFilename, hsl, bUseAtlas, TRUE, bKeepSrc, bAsyncLoad );
	if( m_pSprDat == nullptr ) {
		// spr파일을 못읽었으면 SprObj::Process내에서 비동기로딩을 시작한다.
		// 알림창 띄우지 말것.
#ifdef _XSPR_LAZY_LOAD
		// 이제 SprObj를 생성했을때 sprDat가 없어도 실행은된다. 
		//파일을 실시간으로 패치받은 후에 파일이 생기면 그때 SprDat를 로딩한다.
		// 파일을 못찾았으면 파일정보를 기록해둠
		m_LazyInfo.strFilename = szFilename;
		m_LazyInfo.m_HSL = hsl;
		m_LazyInfo.m_bUseAtlas = bUseAtlas;
		if( XE::GetMain()->m_bDebugMode ) {
			CONSOLE("spr not found:%s", szFilename);
		}
#endif
		return FALSE;
	}
	// SprDat에 루아코드가 있다면 루아쓰레드를 만들어야 한다
	if( m_pSprDat->GetpcLuaAll() ) {
#ifdef _SPR_USE_LUA
		m_pLua = CreateScript();
//		m_pLua = FACTORY->CreateScript();	// virtual
		m_pLua->DoString( m_pSprDat->GetpcLuaAll() );
#endif
	}
	//
	int i, j;
	m_nNumObjActs = m_pSprDat->GetnNumActions();
	m_ppObjActs = new XObjAct*[ m_nNumObjActs ];
	memset( m_ppObjActs, 0, sizeof(XObjAct*) * m_nNumObjActs );
	for( i = 0; i < m_nNumObjActs; i++ ) {
		XAniAction *pAction = m_pSprDat->GetActionIndex( i );		// 순차적으로 액션을 읽어온다
		XObjAct *pObjAct = AddObjAct( i, pAction );		// 추가 액션정보를 만든다.
		for( j = 0; j < pAction->GetnNumLayerInfo(); j++ ) {
			LAYER_INFO *pLayerInfo = pAction->GetLayer( j );
			pObjAct->CreateLayer( j, pLayerInfo );
		}
	}
	return TRUE;
}

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
	if( m_pSprDat->GetpcLuaAll() )		
	{
#ifdef _SPR_USE_LUA
		m_pLua = CreateScript();
		//		m_pLua = FACTORY->CreateScript();	// virtual
		m_pLua->DoString( m_pSprDat->GetpcLuaAll() );
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
void XSprObj::Draw( float x, float y, const MATRIX& mParent )
{ 
#ifdef _XSPR_LAZY_LOAD
	if( m_pSprDat == NULL )		// SprDat가 없으면 일단 찍지 않음.
		return;
#endif
	auto pObjAct = GetpObjActCurr();
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
	if( m_DrawMode != xDM_ERROR || m_fAlpha < 1.0f )	// 외부지정 드로우 모드가 있으면 그 파라메터를 넘김
	{
		XEFFECT_PARAM EffectParam;
		EffectParam.drawMode = m_DrawMode;
		EffectParam.fAlpha = m_fAlpha;
		GetpObjActCurr()->Draw( 0, 0, mWorld, &EffectParam );
		m_DrawMode = xDM_ERROR;		// 드로우모드는 한번쓰고 다시 초기화시킴
	} else
		GetpObjActCurr()->Draw( 0, 0, mWorld, NULL );
}
#endif // GL
#ifdef _VER_DX
void XSprObj::Draw( float x, float y, const D3DXMATRIX &m ) 
{ 
#ifdef _XSPR_LAZY_LOAD
	if( m_pSprDat == NULL )		// SprDat가 없으면 일단 찍지 않음.
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
	XAniAction *pAction = GetAction();
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
	XAniAction *pAction = GetAction();
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
	XAniAction *pAction = GetAction();
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
	XAniAction *pAction = GetAction();
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
	XAniAction *pAction = GetAction();
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
	XObjAct *pObjAct = GetpObjActCurr();
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
	XObjAct *pObjAct = GetpObjActCurr();
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
XBaseLayer* XSprObj::GetpLayerByidLocalInLayer( XObjAct* pActObj, ID idLocalInLayer ) 
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

XObjAct* XSprObj::GetpObjAct( ID idAct ) const 
{
	if( XBREAK( idAct >= XSprDat::MAX_ID ) )
		return NULL;
	return m_ppObjActs[ idAct ];
}
