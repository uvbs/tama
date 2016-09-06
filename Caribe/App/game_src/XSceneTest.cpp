#include "stdafx.h"
#include "XSceneTest.h"
#include "XGame.h"
#include "OpenGL2/XGraphicsOpenGL.h"
#include "XFramework/XSplitNode.h"
#include "OpenGL2/XTextureAtlas.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xSplit;

/////////////////////////////////////////////////////////////////////////////////////////
XSceneTest* XSceneTest::s_pSingleton = nullptr;
XSceneTest* XSceneTest::sGet(){	return s_pSingleton;}
//static const XE::VEC2 c_sizeAtlas = XE::VEC2( 4096, 4096 );
//////////////////////////////////////////////////////////////////////////
void XSceneTest::Destroy() 
{	
//	SAFE_DELETE_ARRAY( m_pAtlas );
	XBREAK( s_pSingleton == nullptr );	// 이미 사라져있다면 잘못된것임
	XBREAK( s_pSingleton != this );		// 싱글톤이 this가 아니면 잘못된것임.
	s_pSingleton = nullptr;
}

XSceneTest::XSceneTest( XGame *pGame, SceneParamPtr& spParam ) 
	: XSceneBase( pGame, XGAME::xSC_TEST )
//	, m_Layout(_T("scene_Sample.xml"))
{ 
	XBREAK( s_pSingleton != nullptr );	// 이미 생성되어있다면 잘못된것임.
	s_pSingleton = this;
	Init(); 
//	m_Layout.CreateLayout("main", this);
	//
	// TODO: 이곳에 코딩하시오
	//
// 	m_pAtlas = new DWORD[ (int)c_sizeAtlas.Size() ];
// 	::memset( m_pAtlas, 0, (int)c_sizeAtlas.Size() * sizeof(DWORD) );
	SetbUpdate( true );
}

void XSceneTest::Create( void )
{
	XSceneBase::Create();
}

/**
 @brief UI의 생성과 업데이트는 이곳에 넣습니다.
*/
void XSceneTest::Update()
{
	XSceneBase::Update();
}

/**
 @brief 매 프레임 실행됩니다.
*/
int XSceneTest::Process( float dt ) 
{ 
	//
	return XSceneBase::Process( dt );
}

//
void XSceneTest::Draw() 
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

void XSceneTest::OnLButtonDown( float lx, float ly ) 
{
	XSceneBase::OnLButtonDown( lx, ly );
}

void XSceneTest::OnLButtonUp( float lx, float ly ) 
{
	XSceneBase::OnLButtonUp( lx, ly );
	//
	if( m_pRoot == nullptr ) {
		m_pRoot = new xSplit::XNode( XE::VEC2( 256 ) );
// 		m_pRoot->SetRect( XE::VEC2(0), XE::VEC2(255) );
	}
	const XE::VEC2 sizeImg( (float)xRandom( 32 ) + 32.f, (float)xRandom( 32 ) + 32.f);
	DWORD* pCurrImg = new DWORD[ (int)sizeImg.Size() ];
	// fill color
	{
		XCOLOR col = XCOLOR_RGBA( xRandom( 50, 255 ), xRandom( 50, 255 ), xRandom( 50, 255 ), 255 );
		for( int i = 0; i < sizeImg.Size(); ++i ) {
			pCurrImg[i] = col;
		}
	}
	XE::xRect2 rc;
	// glTexture아틀라스에 새 사각형을 배치
	auto glTexture = XTextureAtlas::sGet()->ArrangeImg( m_glTexture,
																											&rc,
																											pCurrImg,
																											sizeImg,
																											XE::xPF_ARGB8888,
																											XE::xPF_ARGB8888 );
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
	if( glTexture ) {
		m_glTexture = glTexture;
		CONSOLE( "%.1fx%.1f, addNode=%.1fx%.1f-%.1fx%.1f", 
						 sizeImg.x, sizeImg.y,
						 rc.vLT.x, rc.vLT.y,
						 rc.vRB.x, rc.vRB.y );
	} else {
		CONSOLE( "%.1fx%.1f", sizeImg.x, sizeImg.y );
	}
	SAFE_DELETE_ARRAY( pCurrImg );
	// 기존 아틀라스 삭제하고 다시 생성
// 	if( m_glTexture ) {
// 		::glDeleteTextures( 1, &m_glTexture );
// 		m_glTexture = 0;
// 	}
// 	if( m_glTexture == 0 ) {
// 		m_glTexture = XGraphicsOpenGL::sGet()->CreateTextureGL( m_pAtlas,
// 																														(int)c_sizeAtlas.x,
// 																														(int)c_sizeAtlas.y,
// 																														XE::xPF_ARGB8888,
// 																														(int)c_sizeAtlas.x,
// 																														(int)c_sizeAtlas.y,
// 																														XE::xPF_ARGB8888 );
// 		XBREAK( m_glTexture == 0 );
// 	}

}
void XSceneTest::OnMouseMove( float lx, float ly ) {
	XSceneBase::OnMouseMove( lx, ly );
}

// void XSceneTest::ReplaceTexture( DWORD* pAtlas, const XE::xRECT& rect, int wMax )
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

void XSceneTest::OnRButtonUp( float lx, float ly ) {
 	XSceneBase::OnRButtonUp( lx, ly );
// 	::memset( m_pAtlas, 0, (int)c_sizeAtlas.Size() * sizeof( DWORD ) );
// 	m_idCurr = 0;
// 	::glDeleteTextures( 1, &m_glTexture );
// 	m_glTexture = 0;
// 	SAFE_DELETE( m_pRoot );
}

