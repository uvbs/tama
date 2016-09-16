#include "stdafx.h"
#ifdef _VER_OPENGL
#include "XFontDatFTGL.h"
#include "XFontObjFTGL.h"
#include "etc/xMath.h"
#include "XResMng.h"
#include "XFramework/client/XApp.h"
#include "XFramework/client/XClientMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XBaseFontObj* XFontDatFTGL::CreateFontObj( void )
{
	return new XFontObjFTGL( this );
}

void XFontDatFTGL::Destroy( void )
{
//	XTRACE("XFontDatFTGL destroy:%s", GetstrFont().c_str() );
	SAFE_DELETE( m_font );
}

XBaseFontDat* XFontDatFTGL::CreateFontDat( LPCTSTR szFont, float fontSize, float outset ) 
{
	return new XFontDatFTGL( szFont, fontSize, outset );
}

// outset지원하지 않음
BOOL XFontDatFTGL::Create( LPCTSTR szFontFile, float fontSize, float outset ) 
{
	RestoreDevice();
	if( m_font == NULL )
		return FALSE;
		
	return TRUE;
}

float XFontDatFTGL::GetFontHeight() 
{ 
#ifdef _XGAME
	return (GetFontSize() * 0.5f);// - 2.0f;		폰트 출력시 기준이 되는 y값이 폰트의 아래쪽이 되어야 하는데 위쪽으로 사용해서 생기는 근본적인 문제. 폰트의 좌표계를 바꾸지 않는한 이렇게 하드코딩으로 보정해야한다.
#else
	return GetFontSize(); 
#endif
}
float XFontDatFTGL::GetFontWidth() 
{
#ifdef _XGAME
	return GetFontSize() * 0.5f;
#else
	return GetFontSize(); 
#endif
}

void XFontDatFTGL::RestoreDevice( void )
{
//	XTRACE("FTTextureFont destroy");
	if( m_font )
		m_font->RestoreDevice();	// 내부에서 쓰던 gl텍스쳐 아이디를 모두 0으로 클리어 시켜 glDelete하지 않도록 한다.
	Reload2();
}

void XFontDatFTGL::Reload2()
{
	SAFE_DELETE( m_font );
	//
	XLOAD_PROFILE1;		// profiling
//	XTRACE("FTTextureFont create");
	TCHAR szFullpath[1024];
	TCHAR szLocalpath[256];
	_tcscpy_s( szLocalpath, XE::MakePathLang( DIR_FONT, GetstrFont().c_str() ) );
	if( XE::SetReadyRes( szFullpath, szLocalpath ) == FALSE ) {
		_tcscpy_s( szLocalpath, XE::MakePath( DIR_FONT, GetstrFont().c_str() ) );
		if( XE::SetReadyRes( szFullpath, szLocalpath ) == FALSE ) {
			XALERT( "not found font: %s", szLocalpath );
			return;
		}
	}
	// iOS번들이므로 풀패스로 넘겨주면 상관없다.
	std::string cstrFullpath = SZ2C( szFullpath );
	m_font = new FTTextureFont( cstrFullpath.c_str() );
	//#endif
	if( m_font->Error() ) {
		FT_Error er = m_font->Error();
		XALERT( "폰트파일 %s를 읽을 수 없습니다", szFullpath );
		SAFE_DELETE( m_font );
		return;
	}
	m_idFTGLFont = XE::GenerateID();	// m_font가 새로 할당될때마다 아이디는 바뀐다.
	m_font->FaceSize( (int)GetFontSize() );
	//	if( outset > 0 )
	//		m_font->Outset( outset );		// 뚱뚱함의 굵기
	XLOAD_PROFILE2;
	XLOGP( "%s(%.2f), %llu", XE::GetFileName( szFullpath ), (float)GetFontSize(), __llPass );
}

// 텍스트 레이아웃 기능이 없는 단순 출력루틴
float XFontDatFTGL::DrawString( float x, float y, LPCTSTR szString, XCOLOR color, xFONT::xtStyle style, float dist )
{
	if( XBREAK( m_font == NULL ) )
		return 0;
	if( XE::IsEmpty( szString ) )
		return 0;
	const float hFont = GetFontHeight();
	const float hFontAdj = (hFont * 0.8f);		// 폰트 출력시 기준이 되는 y값이 폰트의 아래쪽이 되어야 하는데 위쪽으로 사용해서 생기는 근본적인 문제. 폰트의 좌표계를 바꾸지 않는한 이렇게 하드코딩으로 보정해야한다.
	y += hFontAdj;		// FTGL은 좌표계가 글자의 아랫부분이므로 편의상 글자위 좌상귀가 좌표가 되도록 바꿈
										// 프리타입 폰트는 독자적인 좌표계를 씀. 아래쪽이 0임
	MATRIX mVP;
	MatrixMakeOrtho( mVP, 0, GRAPHICS->GetViewportWidth()*2.f, 0, GRAPHICS->GetViewportHeight()*2.f, -1.0f, 1.0f );

	XE::VEC2 vc( x, y );	// virtual coodinate
												// 480x320좌표계를 1024x768좌표계로 바꿈
												// 그리고 y좌표를 뒤집음
												//	To1024Cood( &vc );
	vc *= 2.f;
	vc.y = GRAPHICS->GetViewportHeight()*2.f - vc.y;
	float r, g, b, a;
	r = XCOLOR_RGB_R( color ) / 255.0f;
	g = XCOLOR_RGB_G( color ) / 255.0f;
	b = XCOLOR_RGB_B( color ) / 255.f;
	a = XCOLOR_RGB_A( color ) / 255.f;

	float adjStyle = 1.f;
	// 	if( GetFontSize() <= 25 )
	// 		adjStyle = 1.f;
	//	if( a == 1.f ) 
	{
		if( style == xFONT::xSTYLE_SHADOW ) {
			MATRIX mModel, mMVP, mWorld;
			MatrixIdentity( mWorld );
			MatrixScaling( mModel, m_vScale.x, m_vScale.y, 1.0f );
			MatrixMultiply( mWorld, mWorld, mModel );
			MatrixTranslation( mModel, vc.x + adjStyle, vc.y - adjStyle, 0 );
			MatrixMultiply( mWorld, mWorld, mModel );
			MatrixMultiply( mMVP, mWorld, mVP );
			// 			MatrixMultiply( mMVP, mModel, mVP );
			xFTGL::SetMatrixModelViewProjection( GetMatrixPtr( mMVP ) );
			if( color == XCOLOR_BLACK )
				xFTGL::SetColor( 1.0f, 1.0f, 1.0f, a );
			else
				xFTGL::SetColor( 0, 0, 0, a );
#ifdef _XPROFILE
			if( !(XGraphics::s_dwDraw & XE::xeBitNoFont) )
#endif // _XPROFILE
				m_font->Render( szString );
		} else
			if( style == xFONT::xSTYLE_STROKE ) {
				//			_RenderStroke( vc.x - adjStyle, vc.y - adjStyle, szString, mVP );
				_RenderStroke( vc.x - adjStyle, vc.y, szString, mVP, a );
				_RenderStroke( vc.x + adjStyle, vc.y, szString, mVP, a );
				_RenderStroke( vc.x, vc.y - adjStyle, szString, mVP, a );
				_RenderStroke( vc.x, vc.y + adjStyle, szString, mVP, a );
			}
	}
	xFTGL::SetColor( r, g, b, a );
	//     MATRIX mModel, mMVP;
	//     MatrixTranslation( mModel, vc.x, vc.y, 0 );
	//     MatrixMultiply( mMVP, mModel, mVP );
	MATRIX mModel, mMVP, mWorld;
	MatrixIdentity( mWorld );
	MatrixScaling( mModel, m_vScale.x, m_vScale.y, 1.0f );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixTranslation( mModel, vc.x, vc.y, 0 );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixMultiply( mMVP, mWorld, mVP );
	xFTGL::SetMatrixModelViewProjection( GetMatrixPtr( mMVP ) );
#ifdef _XPROFILE
	if( !(XGraphics::s_dwDraw & XE::xeBitNoFont) )
#endif // _XPROFILE
		m_font->Render( szString );

	return 0;
}

void XFontDatFTGL::_RenderStroke( float x, float y, LPCTSTR szStr, const MATRIX& mVP, float alpha )
{
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoFont )
		return;
#endif // _XPROFILE
		xFTGL::SetColor( 0, 0, 0, alpha );
// 	MATRIX mModel, mMVP;
// 	MatrixTranslation( mModel, x, y, 0 );
// 	MatrixMultiply( mMVP, mModel, mVP );
// 	const auto vScale = m_vScale * 1.01f;
	const auto vScale = m_vScale;
	MATRIX mModel, mMVP, mWorld;
	MatrixIdentity( mWorld );
	MatrixScaling( mModel, vScale.x, vScale.y, 1.0f );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixTranslation( mModel, x, y, 0 );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixMultiply( mMVP, mWorld, mVP );
	xFTGL::SetMatrixModelViewProjection( GetMatrixPtr(mMVP) );

	m_font->Render(szStr);
}

#endif // gl