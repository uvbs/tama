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
#ifdef _VER_IPHONE
#include "SprMng/SprMng.h"
#else
#include "SprMng\\SprMng.h"
#endif
#include "XFactory.h"

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

///////////////////////////////////////////////////////////////
//
// XObjAct
//
////////////////////////////////////////////////////////////////
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
void XObjAct::Draw( float x, float y, XEFFECT_PARAM *pEffectParam )
{
	LAYER_LOOP( pLayer )
		pLayer->Draw( m_pSprObj, x, y, pEffectParam );
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
// pLayer가 널이면 레이어를 초기화 상태로 만든다.
XBaseLayer *XObjAct::CreateLayer( int idx, XBaseLayer::xTYPE type, int nLayer, float fAdjAxisX, float fAdjAxisY )
{
#ifdef _XDEBUG
	if( idx >= m_nNumLayers ) {
		XLOG( "%s id=%d ACT %s(%d) idx(%d) >= m_nNumLayers(%d)", m_pSprObj->GetpSprDat()->GetszFilename(), m_pSprObj->GetdwID(),
												m_pAction->GetszActName(), m_pAction->GetID(), idx, m_nNumLayers );
		return NULL;
	}
																
#endif
		
	XBaseLayer *pLayer = NULL;
	{
		// 이거 나중에 XBaseLayer의스태틱 함수로 집어넣자. 안그러면 레이어종류가 추가될때마다 일일히 이런곳 찾아서 수정해줘야 한다
		switch( type )
		{
			case XBaseLayer::xIMAGE_LAYER:
				pLayer = new XLayerImage;
				((XLayerImage *)pLayer)->SetAdjustAxis( fAdjAxisX, fAdjAxisY );
				break;
			case XBaseLayer::xOBJ_LAYER:
				pLayer = new XLayerObject;
				((XLayerObject *)pLayer)->SetAdjustAxis( fAdjAxisX, fAdjAxisY );
				break;
			case XBaseLayer::xSOUND_LAYER:
				pLayer = new XLayerSound;
				break;
			case XBaseLayer::xEVENT_LAYER:
				// 이벤트레이어는 게임에서 생성시킬 필요 없다
				break;
			case XBaseLayer::xDUMMY_LAYER:
				pLayer = new XLayerDummy;
				((XLayerMove *)pLayer)->SetAdjustAxis( fAdjAxisX, fAdjAxisY );
				break;
			default:
				pLayer = NULL;
				XERROR( "layer 생성타입이 잘못되었음. %d", (int)type );
				break;
		}
		if( pLayer )
			pLayer->SetnLayer( nLayer );								// 지정한 레이어번호로 생성
		m_ppLayers[ idx ] = pLayer;		// 널이 들어갈수도 있다
	}
	return pLayer;
}
XBaseLayer *XObjAct::GetLayer( XBaseLayer::xTYPE type, int nLayer )
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
#define SPROBJ_LOOP( K )	int K##i;		\
							XSprObj **ppSprObjs = m_ppSprObjs;	\
							K##i = m_nNumSprObjs;		\
							while( K##i-- )			\
							{						\
								XSprObj *K = *ppSprObjs++;	
#define LOOP_END		}

float XSprObj::s_LuaDt = 0;		// LuaDraw()에서 씀. 이번프레임에 결정된 dt값을 저장한다

void XSprObj::Destroy( void )
{
	SAFE_DELETE( m_pLua );
	// m_pSprDat를 삭제한다. 그러나 매니저를 통해 삭제해야한다. 그리고 RefCnt개념을 써야 한다
	XBREAK( m_pSprDat == NULL );
	if( SPRMNG )
		SPRMNG->Release( m_pSprDat );
	m_pSprDat = NULL;
	
	// 사전로드 sprobj 삭제. 
	{
		int i;
		for( i = 0; i < m_nNumSprObjs; i ++ )
			SAFE_DELETE( m_ppSprObjs[i] );
		SAFE_DELETE_ARRAY( m_ppSprObjs );
	}
	{
		for( int i = 0; i < m_nNumObjActs; i ++ )
			SAFE_DELETE( m_ppObjActs[i] );
		SAFE_DELETE_ARRAY( m_ppObjActs );
	}

	
}
////////////// F Action ////////////////////////////
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

void XSprObj::SetAction( DWORD id, xRPT_TYPE playType, BOOL bExecFrameMove )
{
	XBREAK( id == 0 );
	if( _m_pObjActCurr && id == GetAction()->GetID() )		// 이미 셋된 액션아이디로 다시 셋시킬순 없다
		return;
	m_fFrameCurrent = 0;
    XBREAK( m_pSprDat == NULL );
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
	m_bFinish = FALSE;
	// 레이어 내용 초기화
	GetpObjActCurr()->ClearLayer();
	if( bExecFrameMove )
		FrameMove(0);	// 액션바꾼후 젤첫프레임의 키는 한번 실행해줘야 한다 안그러면 그림이 사라지는 상태가 된다
	
//	pAction->SetAction( GetpObjActCurr() );	// Load에서 한번만 하도록 바뀜
}

////////////////////////////////////////////////////////////////////
void XSprObj::FrameMove( float dt )
{
	if( m_bFinish )		return;		// 애니메이션이 끝났으면 더이상 실행하지 않음
	XAniAction *pAction = GetAction();
	BOOL bPlay = IsPlaying();
	if( m_pParentSprObj )
		bPlay = m_pParentSprObj->IsPlaying();
	if( bPlay )			// 멈춤 모드가 아닐때만 프레임을 진행시킨다
		m_fFrameCurrent += pAction->GetfSpeed() * dt;
	if( m_fFrameCurrent >= pAction->GetfMaxFrame() )	// 맥스 프레임을 넘어가면
	{
		// 프레임스키핑으로 가장마지막 프레임에 있는 키를 건너뛸수도 있으므로 프레임을 첨으로 감기전에 마지막으로 키를 실행해준다
		pAction->ExecuteKey( this, m_nKeyCurr, m_fFrameCurrent );
		GetpObjActCurr()->FrameMove( dt, m_fFrameCurrent );
		if( m_PlayType == xRPT_LOOP )
		{
//			m_bFinish = TRUE;	// 루핑은 finish라는게 없다
			m_fFrameCurrent = pAction->GetRepeatMark() + (m_fFrameCurrent - pAction->GetfMaxFrame());
			if( pAction->GetRepeatMark() > 0 )	// 도돌이표 마크가 있으면
			{
				JumpKeyPos( pAction, pAction->GetRepeatMark() );		// 돌아가는 위치로 키 실행없이 바로 점프함
				// 도돌이표 마크 이후키만 실행해줘야함
//				GetpObjActCurr()->ClearLayer();
				pAction->ExecuteKey( this, m_nKeyCurr, m_fFrameCurrent );
				GetpObjActCurr()->FrameMove( dt, m_fFrameCurrent );
			}
			else {
				SetKeyCurrStart();
				// 프레임이 첨으로 감겼으므로 키를 한번 실행해줘야 한다
				GetpObjActCurr()->ClearLayer();
				pAction->ExecuteKey( this, m_nKeyCurr, m_fFrameCurrent );
				GetpObjActCurr()->FrameMove( dt, m_fFrameCurrent );
			}

//			SetKeyCurrStart();
//			GetpObjActCurr()->ClearLayer();
			// 프레임이 첨으로 감겼으므로 키를 한번 실행해줘야 한다
//			pAction->ExecuteKey( this, m_nKeyCurr, m_fFrameCurrent );
//			GetpObjActCurr()->FrameMove( dt, m_fFrameCurrent );
		} else
		if( m_PlayType == xRPT_1PLAY )
		{
			m_bFinish = TRUE;
			m_fFrameCurrent = pAction->GetfMaxFrame();
//			GetpObjActCurr()->ClearLayer();	// 1PLAY라도 SprObj객체를 파괴하지 않는한 마지막 프레임에 머물러 있는게 좋을듯.
		} else 
		if( m_PlayType == xRPT_1PLAY_CONT )	// 이건 쓰지말자 끝에서 계속 돌아야 한다면 도돌이표를 써도 된다
		{
			m_bFinish = TRUE;
			m_fFrameCurrent = pAction->GetfMaxFrame();
		} else 
		{
			m_bFinish = TRUE;
			m_fFrameCurrent = pAction->GetfMaxFrame();
		}
	} else 
	{
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
BOOL XSprObj::Load( LPCTSTR szFilename, BOOL bKeepSrc ) 
{ 
#ifdef _XDEBUG
	if( m_pSprDat ) {
		XLOG( "m_pSprDat(%s)가 해제되지 않았다 %s", m_pSprDat->GetszFilename(), szFilename );
		SPRMNG->Release( m_pSprDat );
		m_pSprDat = NULL;
	}
#endif
	XBREAK( SPRMNG == NULL );
	m_pSprDat = SPRMNG->Load( szFilename, TRUE, bKeepSrc );
	if( !m_pSprDat  )		// 일단 SprDat를 읽고
	{
		XERROR( "%s 읽기 실패", szFilename );
		return FALSE;
	}
	// SprDat에 루아코드가 있다면 루아쓰레드를 만들어야 한다
	if( m_pSprDat->GetpcLuaAll() )		
	{
		m_pLua = FACTORY->CreateScript();	// virtual
		m_pLua->DoString( m_pSprDat->GetpcLuaAll() );
	}
	//
	int i, j;
	m_nNumObjActs = m_pSprDat->GetnNumActions();
	m_ppObjActs = new XObjAct*[ m_nNumObjActs ];
	memset( m_ppObjActs, 0, sizeof(XObjAct*) * m_nNumObjActs );
	for( i = 0; i < m_nNumObjActs; i++ )
	{
		XAniAction *pAction = m_pSprDat->GetActionIndex( i );		// 순차적으로 액션을 읽어온다
		XObjAct *pObjAct = AddObjAct( i, pAction );		// 추가 액션정보를 만든다.
		for( j = 0; j < pAction->GetnNumLayerInfo(); j++ )
		{
			LAYER_INFO *pLayerInfo = pAction->GetLayer( j );
			pObjAct->CreateLayer( j, pLayerInfo->type, pLayerInfo->nLayer, pLayerInfo->fAdjustAxisX, pLayerInfo->fAdjustAxisY );		// 실제 레이어를 만들어준다
		}
//		pAction->KeyRemapLayer( pObjAct );
	}
	// 아래코드 하지 않도록 정책이 바뀜. sprObj.Load를 하면 무조건 SetAction을 외부에서 불러서 초기화 해줘야함
//	// 가장 처음에 있는 액션을 실행시킨다
//	if( m_pSprDat->GetActionIndex(0) )
//		SetAction( m_pSprDat->GetActionIndex(0)->GetID(), xRPT_LOOP, FALSE );		// 가장첫번째에 있는 액션을 활성화 시킨다
	return TRUE;
}

XSprObj* XSprObj::AddSprObj( LPCTSTR szSpr, ID idAct, xRPT_TYPE playMode, ID idBase, ID idLocal )
{
	if( idBase != 0 && idLocal == 0 )
	{
		XLOG( "AddSprObj: 로컬 아이디를 지정하지 않았습니다." );
		return NULL;
	}
	ID idSprObj = idBase + idLocal;
	if( XBREAK( idSprObj == 0 ) )		// 게임에선 이런경우가 없어야 한다.
		return NULL;
	SPROBJ_LOOP( pSprObj )
		if( pSprObj->GetidSprObj() == idSprObj )			// 이미 로드한것이므로 다시 생성하지 않는다
		{
			pSprObj->Reset();		// 새로 생성되지 않고 캐쉬에서 가져온 경우도 기본사항은 리셋되어 있어야 함
			pSprObj->SetAction( idAct, playMode );
			return pSprObj;
		}
	LOOP_END
		
#ifdef _XDEBUG
	if( m_nNumSprObjs >= 10 ) {
		XLOG( "m_nNumSprObjs(%d)가 범위를 벗어났다", m_nNumSprObjs );
		return NULL;
	}
#endif
//	XSprObj *pSprObj = CreateSprObj( szSpr );
	XSprObj *pSprObj = FACTORY->CreateSprObj( szSpr );
	pSprObj->SetdwID( idSprObj );
//	pSprObj->Load( szSpr );
	pSprObj->SetAction( idAct );
	if( m_ppSprObjs == NULL )
		m_ppSprObjs = new XSprObj*[ 10 ];		// 일단 최대 CreateObj키는 10개로 제한하자. SprDat가 로드될때 사전로드SprObj를 모든 액션에서 다 로드하면 효율이 안좋을것 같다
	m_ppSprObjs[ m_nNumSprObjs++ ] = pSprObj;
	return pSprObj;	
}

#ifdef _VER_OPENGL
void XSprObj::Draw( float x, float y ) 
{ 
	glPushMatrix();
	x += m_fAdjustAxisX;
	y += m_fAdjustAxisY;
	glTranslatef(x, y, 0);
	if( m_fRotX )
		glRotatef(m_fRotX, 1.0f, 0, 0);
	if( m_fRotY )
		glRotatef(m_fRotY, 0, 1.0f, 0);
	if( m_fRotZ )
		glRotatef(m_fRotZ, 0, 0, 1.0f);
	glScalef( GetScaleX(), GetScaleY(), 1.0f);
	glTranslatef( -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );		// 좌표축을 돌린 후 움직인다
	if( m_DrawMode != xDM_ERROR || m_fAlpha < 1.0f )	// 외부지정 드로우 모드가 있으면 그 파라메터를 넘김
	{
		XEFFECT_PARAM EffectParam;
		EffectParam.drawMode = m_DrawMode;
		EffectParam.fAlpha = m_fAlpha;
		GetpObjActCurr()->Draw( 0, 0, &EffectParam ); 
		m_DrawMode = xDM_ERROR;		// 드로우모드는 한번쓰고 다시 초기화시킴
	} else
		GetpObjActCurr()->Draw( 0, 0, NULL ); 

	glPopMatrix();
//		m_fRotX = m_fRotY = m_fRotZ = 0;
//		m_fScaleX = m_fScaleY = 1.0f;
}
#endif // GL
#ifdef WIN32
void XSprObj::Draw( float x, float y, const D3DXMATRIX &m ) 
{ 
	D3DXMATRIX mTrans, mScale, mRot, mWorld, mAxis;
	if( m_DrawMode == xDM_NONE )		// 아무것도 그리지 않는 모드
		return;
	x += m_fAdjustAxisX;		// 좌표축 보정
	y += m_fAdjustAxisY;
	D3DXMatrixTranslation( &mTrans, x, y, 0 );
	D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(m_fRotY), D3DXToRadian(m_fRotX), D3DXToRadian(m_fRotZ) );
	D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
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
#endif // WIN32

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


