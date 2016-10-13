#include "stdafx.h"
#include "XSceneTest2.h"
#include "XGame.h"
#include "OpenGL2/XGraphicsOpenGL.h"
#include "XFramework/XSplitNode.h"
#include "OpenGL2/XTextureAtlas.h"
#include "Sprite/SprObj.h"
#include "etc/XSurfaceDef.h"
#include "Opengl2/XBatchRenderer.h"
#include "XGameWnd.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

const XE::VEC2 c_sizeAtlas( 256, 256 );

using namespace xSplit;

/////////////////////////////////////////////////////////////////////////////////////////
XSceneTest2* XSceneTest2::s_pSingleton = nullptr;
XSceneTest2* XSceneTest2::sGet(){	return s_pSingleton;}
//static const XE::VEC2 c_sizeAtlas = XE::VEC2( 4096, 4096 );
//////////////////////////////////////////////////////////////////////////
void XSceneTest2::Destroy() 
{	
	SAFE_DELETE_ARRAY( m_pAtlas );
	XBREAK( s_pSingleton == nullptr );	// 이미 사라져있다면 잘못된것임
	XBREAK( s_pSingleton != this );		// 싱글톤이 this가 아니면 잘못된것임.
	s_pSingleton = nullptr;
}

XSceneTest2::XSceneTest2( XGame *pGame, XSPSceneParam& spParam ) 
	: XSceneBase( pGame, XGAME::xSC_TEST2 )
{ 
	XBREAK( s_pSingleton != nullptr );	// 이미 생성되어있다면 잘못된것임.
	s_pSingleton = this;
	Init(); 
	//
	// TODO: 이곳에 코딩하시오
	//
	m_pAtlas = new DWORD[ (int)c_sizeAtlas.Size() ];
	::memset( m_pAtlas, 0, (int)c_sizeAtlas.Size() * sizeof(DWORD) );
	SetbUpdate( true );
}

/**
 @brief 
*/
void XSceneTest2::Create( void )
{
	XSceneBase::Create();
}

/**
 @brief UI의 생성과 업데이트는 이곳에 넣습니다.
*/
void XSceneTest2::Update()
{
	XSceneBase::Update();
}

/**
 @brief 매 프레임 실행됩니다.
*/
int XSceneTest2::Process( float dt ) 
{ 
	//
	return XSceneBase::Process( dt );
}

//
void XSceneTest2::Draw() 
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
	//
	if( m_glTexture ) {
		// 256으로 축소해서 찍음
 		GRAPHICS_GL->DrawTexture( m_glTexture, 0, 0, 256.f, 256.f, FALSE );
		{ auto glErr = glGetError();
		XASSERT( glErr == GL_NO_ERROR ); }
	}
}

void XSceneTest2::OnLButtonDown( float lx, float ly ) 
{
	XSceneBase::OnLButtonDown( lx, ly );
}

void XSceneTest2::OnLButtonUp( float lx, float ly ) 
{
	XSceneBase::OnLButtonUp( lx, ly );
	//
// 	if( m_pRoot == nullptr ) {
// 		m_pRoot = new xSplit::XNode( c_sizeAtlas );
// 	}
// 	const XE::VEC2 sizeImg( (float)xRandom( 32 ) + 32.f, (float)xRandom( 32 ) + 32.f);
// 	DWORD* pCurrImg = new DWORD[ (int)sizeImg.Size() ];
// 	// fill color
// 	{
// 		XCOLOR col = XCOLOR_RGBA( xRandom( 50, 255 ), xRandom( 50, 255 ), xRandom( 50, 255 ), 255 );
// 		for( int i = 0; i < sizeImg.Size(); ++i ) {
// 			pCurrImg[i] = col;
// 		}
// 	}
// 	XE::xRect2 rc;
// 	XE::VEC2 sizeAtlas;
// 	// glTexture아틀라스에 새 사각형을 배치
// 	auto glTexture = XTextureAtlas::sGet()->ArrangeImg( m_glTexture,
// 																											&rc,
// 																											pCurrImg,
// 																											sizeImg,
// 																											XE::xPF_ARGB8888,
// 																											XE::xPF_ARGB8888,
// 																											&sizeAtlas );
// 	{ auto glErr = glGetError();
// 	XASSERT( glErr == GL_NO_ERROR ); }
// 	if( glTexture ) {
// 		m_glTexture = glTexture;
// 		CONSOLE( "%.1fx%.1f, addNode=%.1fx%.1f-%.1fx%.1f", 
// 						 sizeImg.x, sizeImg.y,
// 						 rc.vLT.x, rc.vLT.y,
// 						 rc.vRB.x, rc.vRB.y );
// 	} else {
// 		CONSOLE( "%.1fx%.1f", sizeImg.x, sizeImg.y );
// 	}
// 	SAFE_DELETE_ARRAY( pCurrImg );

}
void XSceneTest2::OnMouseMove( float lx, float ly ) {
	XSceneBase::OnMouseMove( lx, ly );
}

// void XSceneTest2::ReplaceTexture( DWORD* pAtlas, const XE::xRECT& rect, int wMax )
// {
// 	XCOLOR col = XCOLOR_RGBA( xRandom(50, 255), xRandom(50, 255), xRandom(50, 255), 255 );
// 	const int w = (int)rect.GetWidth();
// 	const int h = (int)rect.GetHeight();
// 	for( int y = (int)rect.vLT.y; y < (int)rect.vLT.y + h; ++y ) {
// 		for( int x = (int)rect.vLT.x; x < (int)rect.vLT.x + w; ++x ) {
// 			m_pAtlas[ y * wMax + x ] = col;
// 		}
// 	}
// }

void XSceneTest2::OnRButtonUp( float lx, float ly ) {
 	XSceneBase::OnRButtonUp( lx, ly );
	XWND_ALERT( "%s", _T( "test" ) );
	// 	::memset( m_pAtlas, 0, (int)c_sizeAtlas.Size() * sizeof( DWORD ) );
// 	m_idCurr = 0;
// 	::glDeleteTextures( 1, &m_glTexture );
// 	m_glTexture = 0;
// 	SAFE_DELETE( m_pRoot );
}

