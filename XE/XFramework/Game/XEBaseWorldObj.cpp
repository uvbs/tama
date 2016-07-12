#include "stdafx.h"
#include "XEBaseWorldObj.h"
#include "XFramework/Game/XEWorld.h"
#include "XFramework/Game/XEWndWorld.h"
#include "XImageMng.h"
#include "XArchive.h"
#include "Sprite/SprObj.h"
#include "Sprite/SprDat.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

DWORD XEBaseWorldObj::s_idSerial = 1;

XEBaseWorldObj::XEBaseWorldObj( XEWndWorld *pWndWorld, 
								int type,
								const XE::VEC3& vPos, 
								LPCTSTR szImg )
{ 
	Init(); 
	m_Type = type;
	LoadImage( szImg );
	m_vwPos = vPos;
	m_pWndWorld = pWndWorld;
}

XEBaseWorldObj::XEBaseWorldObj( XEWndWorld *pWndWorld, int type, LPCTSTR szSpr, ID idAct ) 
{
	Init();
	m_Type = type;
	m_pWndWorld = pWndWorld;
	LoadSpr( szSpr, idAct );
}

XEBaseWorldObj::XEBaseWorldObj( XEWndWorld *pWndWorld, int type, const XE::VEC3& vPos, LPCTSTR szSpr, ID idAct ) 
{
	Init();
	m_Type = type;
	m_vwPos = vPos;
	m_pWndWorld = pWndWorld;
	LoadSpr( szSpr, idAct );;
}

void XEBaseWorldObj::Destroy() 
{
	if( m_pSprObj )
	{
//		XTRACE("SAFE_DELETE( m_pSprObj ) %s", m_pSprObj->GetSprFilename() );
	}
	SAFE_DELETE( m_pSprObj );
	SAFE_RELEASE2( IMAGE_MNG, m_pSurface );
}

/**
 @brief 
*/
BOOL XEBaseWorldObj::LoadSpr( LPCTSTR szSpr, const XE::xHSL& hsl, ID idAct, xRPT_TYPE typeLoop )
{
	if( XBREAK( XE::IsEmpty( szSpr ) == TRUE ) )
		return FALSE;
	m_strSpr = szSpr;
	m_pSprObj = new XSprObj( szSpr, hsl, this );
	//	XTRACE( "new XSprObj(%s)", szSpr );
	if( XBREAK( m_pSprObj == NULL ) )
		return FALSE;
	// 비동기 로딩땜에 assert를 내지 않는게 맞지만 현재 그 기능이 없으므로 assert를 냄
	XBREAK( m_pSprObj->IsError() );
	if( m_pSprObj->IsError() ) {
		m_snObj = 0;
		SAFE_DELETE( m_pSprObj );
		return FALSE;
	}
	m_pSprObj->SetAction( idAct, typeLoop );
	return TRUE;
}
// BOOL XEBaseWorldObj::LoadSpr( LPCTSTR szSpr, const XE::VEC3& vHSL, ID idAct, xRPT_TYPE typeLoop )
// {
// 	if( XBREAK( XE::IsEmpty(szSpr) == TRUE ) )
// 		return FALSE;
// 	m_strSpr = szSpr;
// 	m_pSprObj = new XSprObj( szSpr, vHSL, this );
// //	XTRACE( "new XSprObj(%s)", szSpr );
// 	if( XBREAK(m_pSprObj == NULL) )
// 		return FALSE;
// 	// 비동기 로딩땜에 assert를 내지 않는게 맞지만 현재 그 기능이 없으므로 assert를 냄
// 	XBREAK( m_pSprObj->IsError() );
// 	if( m_pSprObj->IsError() ) {
// 		m_snObj = 0;
// 		SAFE_DELETE( m_pSprObj );
// 		return FALSE;
// 	}
// 	m_pSprObj->SetAction( idAct, typeLoop );
// 	return TRUE;
// }

void XEBaseWorldObj::LoadImage( LPCTSTR szImg )
{
	if( XE::IsHave( szImg ) )
	{
		m_pSurface = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_IMG, szImg ) );
		XBREAK( m_pSurface == NULL );
	}
}


void XEBaseWorldObj::FrameMove( float dt )
{
	if( m_pSprObj )
		m_pSprObj->FrameMove( dt );
}
/**
 @param vPos 스크린 좌표
 @param scale 카메라 스케일값
*/
void XEBaseWorldObj::Draw( const XE::VEC2& vPos, float scale/*=1.f*/, float alpha )
{
	if( m_pSprObj )
	{
		// 카메라스케일과 오브젝트스케일을 곱해서 최종스케일값을 세팅한다.
		XE::VEC2 vScale;
		vScale.x = m_vScale.x;
		vScale.y = m_vScale.z;
		vScale *= scale;
		m_pSprObj->SetScale( vScale );
		m_pSprObj->SetfAlpha( m_Alpha * alpha );
		m_pSprObj->Draw( vPos );
	}
	if( m_pSurface )
	{
		float w = m_pSurface->GetWidth();
		float h = m_pSurface->GetHeight();
		m_pSurface->Draw( vPos.x - w/2.f, vPos.y - h );
	}
}

XE::VEC2 XEBaseWorldObj::GetScaleImage() {
	// 높이는 z이므로 z를 사용함.
	return XE::VEC2( GetScaleObj().x, GetScaleObj().z ) * m_pSprObj->GetScale();
}

void XEBaseWorldObj::SetRotateY( float dAng ) {
	XBREAK( m_pSprObj == NULL );
	m_pSprObj->SetRotateY( dAng );
}

// 오브젝트의 바운딩 영역을 스크린좌표로 만들어 돌려준다.
XE::xRECT XEBaseWorldObj::GetBoundBoxScreen( void ) 
{
	XBREAK( m_pWndWorld == NULL );
	XE::xRECT rect = GetBoundBoxWorld();
	XE::VEC3 v3LT( rect.vLT );
	XE::VEC3 v3RB( rect.vRB );
	rect.vLT = m_pWndWorld->GetPosWorldToScreen( v3LT );
	rect.vRB = m_pWndWorld->GetPosWorldToScreen( v3RB );
	return rect;
}

// 오브젝트의 바운딩 영역을 스크린좌표로 만들어 돌려준다.
XE::xRECT XEBaseWorldObj::GetBoundBoxWindow( void ) 
{
	XBREAK( m_pWndWorld == NULL );
	XE::xRECT rect = GetBoundBoxWorld();
	rect.vLT = m_pWndWorld->GetPosWorldToWindow( rect.vLT );
	rect.vRB = m_pWndWorld->GetPosWorldToWindow( rect.vRB );
	return rect;
}

void XEBaseWorldObj::OnLButtonUp( float lx, float ly )
{
	XE::xRECT vBB = GetBoundBoxWindow();	// 오브젝트의 바운딩영역을 윈도우내 좌표로 돌려준다.
	if( XE::IsArea( vBB.vLT, vBB.GetSize(), XE::VEC2(lx,ly) ) )
	{
		OnTouch( vBB, XE::VEC2(lx, ly) );	// virtual
	}
}

// 오브젝트 중심기준 좌표계로 바운딩박스를 만든다.
XE::xRECT XEBaseWorldObj::GetBoundBoxLocal( void ) 
{
	if( GetpSprObj() )
	{
		XE::xRECT rect;
		rect.vLT = GetpSprObj()->GetAction()->GetBoundBoxLT();
		rect.vRB = GetpSprObj()->GetAction()->GetBoundBoxRB();
		rect.vLT *= GetpSprObj()->GetScale();
		rect.vRB *= GetpSprObj()->GetScale();
		return rect;
	} else
	if( m_pSurface )
	{
		XE::xRECT rect;
		XE::VEC2 vSize = m_pSurface->GetSize();
		rect.vLT.x = -(vSize.w / 2.f);
		rect.vLT.y = -vSize.h;
		rect.vRB.x = vSize.w / 2.f;
		rect.vRB.y = 0;
		rect.vLT *= m_pSurface->GetfScaleX();
		rect.vRB *= m_pSurface->GetfScaleX();
		return rect;
	}
	return XE::xRECT();
}

int XEBaseWorldObj::Serialize( XArchive& ar )
{
	// m_idObj는 타겟측의 고유아이디를 보존해야하므로 시리얼라이즈 시키지 않음.
	ar << (char) m_Destroy;
	ar << (char) 0;
	ar << (char) 0;
	ar << (char) 0;
	ar << m_vwPos;
	// 일단은 pvp에서만 쓸거니까 간단하게...
	return 1;
}

int XEBaseWorldObj::DeSerialize( XArchive& ar )
{
	char c1;
	ar >> c1;	m_Destroy = (int)c1;
	ar >> c1;
	ar >> c1;
	ar >> c1;
	ar >> m_vwPos;
	return 1;
}

/**
 @brief 현재 윈도우 좌표를 계산해서 돌려준다.
*/
XE::VEC2 XEBaseWorldObj::GetWindowPos( void ) 
{
	return m_pWndWorld->GetPosWorldToScreen( GetvwPos() );
}

XE::VEC2 XEBaseWorldObj::GetvsCenter( void ) 
{
	return GetpWndWorld()->GetPosWorldToScreen( GetvwPos() + GetvCenterLocal() );
}

XE::VEC2 XEBaseWorldObj::GetvsTop( float adjZ/* = 0.f*/ ) 
{
	return GetpWndWorld()->GetPosWorldToScreen( GetvwPos() + GetvTopLocal( adjZ ) );
}






