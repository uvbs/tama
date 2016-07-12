#include "StdAfx.h"
#include "XESceneSlide.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XESceneSlide::Destroy() 
{	
}

XESceneSlide::XESceneSlide( XEContent *pGame, ID idScene ) 
	: XEBaseScene( pGame, idScene, FALSE )	// transition off
{ 
	Init(); 
}

void XESceneSlide::Create( int idxStart )
{
	SetSlideRight( idxStart );
}

int XESceneSlide::Process( float dt ) 
{ 
	if( m_Trans == xTR_RIGHT )
	{
		float lerpTime = m_timerTrans.GetSlerp();
		if( lerpTime > 1.0f )
			lerpTime = 1.0f;
//		float lerp = XE::xiCatmullrom( lerpTime, -10.f, 0, 1.f, 1.f );
		float lerp = XE::xiHigherPowerDeAccel( lerpTime, 1.0f, 0 );
		if( m_pCurr )
		{
			float x = XE::GetGameWidth() * lerp; 
			m_pCurr->SetPosLocal( x, 0.f );
		}
		if( m_pNext )
		{
			float x = XE::GetGameWidth() * lerp; 
			m_pNext->SetPosLocal( -XE::GetGameWidth() + x, 0.f );
		}
		if( lerpTime == 1.0f )
		{
			m_Trans = xTR_NONE;
			XWnd::DestroyWnd( m_pCurr );		// 오른쪽으로 사라진 현재씬은 지우고
			m_pCurr = m_pNext;					// 현재씬을 새로운 씬으로 바꿈
//			m_pCurr->SetbActive( TRUE );
			GetWndTop()->SetbActive( TRUE );	// 입력을 다시 풀어줌.
			m_pNext = NULL;
			CallScriptTemplate<XESceneSlide*>( "OnFinishSlide", m_pCurr, m_pCurr->GetstrIdentifier().c_str() );
			OnFinishSlide();
			CallEventHandler( XWM_FINISH_SLIDE, m_idxScene );
		}
	} else
	if( m_Trans == xTR_LEFT )
	{
		float lerpTime = m_timerTrans.GetSlerp();
		if( lerpTime > 1.0f )
			lerpTime = 1.0f;
//		float lerp = XE::xiCatmullrom( lerpTime, -10.f, 0, 1.f, 1.f );
		float lerp = XE::xiHigherPowerDeAccel( lerpTime, 1.0f, 0 );
		if( m_pCurr )
		{
			float x = -XE::GetGameWidth() * lerp; 
			m_pCurr->SetPosLocal( x, 0.f );
		}
		if( m_pNext )
		{
			float x = XE::GetGameWidth() * (1.f - lerp); 
			m_pNext->SetPosLocal( x, 0.f );
		}
		if( lerpTime == 1.0f )
		{
			m_Trans = xTR_NONE;
			XWnd::DestroyWnd( m_pCurr );		// 왼쪽으로 사라진 현재씬은 지우고
			m_pCurr = m_pNext;					// 현재씬을 새로운 씬으로 바꿈
//			m_pCurr->SetbActive( TRUE );		// 슬라이딩이 끝나면 다시 풀어줌.
			GetWndTop()->SetbActive( TRUE );	// 입력을 다시 풀어줌.
			m_pNext = NULL;
			CallScriptTemplate<XESceneSlide*>( "OnFinishSlide", m_pCurr, m_pCurr->GetstrIdentifier().c_str() );
			OnFinishSlide();
			CallEventHandler( XWM_FINISH_SLIDE, m_idxScene );
		}
	}
	return XEBaseScene::Process( dt );
}

//
void XESceneSlide::Draw( void ) 
{
	XEBaseScene::Draw();
	if( m_timerSpeed.IsOn() )
	{
		m_timeLoad = m_timerSpeed.GetPassTime();
		m_timerSpeed.Off();
	}
#ifdef _CHEAT
//	PUT_STRINGF( 0.f, 465.f, XCOLOR_WHITE, "%d", m_timeLoad );
#endif
}

void XESceneSlide::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XESceneSlide::OnLButtonUp( float lx, float ly ) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XESceneSlide::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );
}

// idx: 새로 들어올 씬 번호
BOOL XESceneSlide::SetSlideLeft( int idx, DWORD p1, DWORD p2 )
{
	if( m_Trans != xTR_NONE )
		return FALSE;
	int idxOld = m_idxScene;
	m_idxScene = -1;	// 이걸 CreateScene에서 쓰고 있는지 알아보려고 이렇게 해봄 이상없으면 지울것.
	m_pNext = CreateScene( idx, p1, p2 );
	if( m_pNext == NULL )
	{
		m_idxScene = idxOld;
		return FALSE;
	}
	m_idxScene = idx;
	m_timerSpeed.Set(0, TRUE );
	GetWndTop()->SetbActive( FALSE );	// 최상위 윈도우를 정지시켜 모든 윈도우의 입력을 막음.
	Add( m_pNext );
	m_pNext->SetPosLocal( XE::GetGameWidth(), 0.f );
	m_timerTrans.Set( 0.15f );
	m_Trans = xTR_LEFT;
	m_Param[0] = p1;
	m_Param[1] = p2;
	return TRUE;
}
BOOL XESceneSlide::SetSlideRight( int idx, DWORD p1, DWORD p2 )
{
	if( m_Trans != xTR_NONE )	// 슬라이딩중에는 다시 슬라이딩 명령을 받지 못함.
		return FALSE;
	int idxOld = m_idxScene;
	m_idxScene = -1;	// 이걸 CreateScene에서 쓰고 있는지 알아보려고 이렇게 해봄 이상없으면 지울것.
	m_pNext = CreateScene( idx, p1, p2 );
	if( m_pNext == NULL )
	{
		// 씬 생성 실패상황
		m_idxScene = idxOld;
		return FALSE;
	}
	m_idxScene = idx;
	m_timerSpeed.Set(0, TRUE );
	GetWndTop()->SetbActive( FALSE );	// 최상위 윈도우를 정지시켜 모든 윈도우의 입력을 막음.
	Add( m_pNext );
	m_pNext->SetPosLocal( -XE::GetGameWidth(), 0.f );
	m_timerTrans.Set( 0.15f );
	m_Trans = xTR_RIGHT;
	m_Param[0] = p1;
	m_Param[1] = p2;
	return TRUE;
}

BOOL XESceneSlide::OnKeyUp( int keyCode )
{
	if( keyCode == XE::KEY_BACK )
	{
		// 백버튼 누르면 현재 씬에도 돌아가기 버튼이 있는지 보고 있으면 OnBack을 불러준다. 
		XTRACE("OnKeyUp==back");
		XWnd *pButt = Find( "butt.back" );
		if( pButt )
		{
			OnBack( pButt, 0, 0 );
			return TRUE;
		}
	}
	return FALSE;
}

