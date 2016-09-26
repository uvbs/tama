#include "stdafx.h"
#include "XFramework/XSplitNode.h"
#include "XGraphicsOpenGL.h"
#include "etc/XSurface.h"
#include "XTextureAtlas.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;

XE_NAMESPACE_START( xnTexAtlas )
//
xAtlas::xAtlas( const XE::VEC2& size, xtPixelFormat formatSurface )
	: m_Size( size )
	, m_idTex( 0 )
	, m_pRoot( nullptr )
	, m_FormatSurface( formatSurface ) {
	//
	m_pRoot = new xSplit::XNode( m_Size );
}

xAtlas::~xAtlas() {
	SAFE_DELETE( m_pRoot );
	const GLuint glTex = (GLuint)m_idTex;
	if( glTex )
		::glDeleteTextures( 1, &glTex );
	const int bpp = XE::GetBpp( m_FormatSurface );
	XSurface::sAddSizeTotalVMem( (int)(-m_Size.Size() * bpp) );
#ifdef WIN32
	CONSOLE("destroy atlas: id=%d\n", m_idTex );
#endif // WIN32
}

void xAtlas::DestroyDevice()
{
	if( m_idTex ) {
		const GLuint glTex = (GLuint)m_idTex;
		::glDeleteTextures( 1, &glTex );
		m_idTex = 0;
	}
}

/**
 @brief 실제텍스쳐에 새로 추가된 노드를 부분 갱신
 @fmtImg gl서피스의 포맷이 아니고 pImg의 픽셀포맷.
*/
void xAtlas::UpdateSubToDevice( const void* _pImg,
																const XE::VEC2& vLT,
																const XE::VEC2& sizeImg,
																xtPixelFormat fmtImg ) {
	const auto glFmtImg = XGraphicsOpenGL::sToGLFormat( fmtImg );
	const auto glTypeImg = XGraphicsOpenGL::sToGLType( fmtImg );
	CHECK_GL_ERROR();
	XGraphicsOpenGL::sBindTexture( m_idTex );
	CHECK_GL_ERROR();
	const void* pImg = _pImg;
	XBREAK( fmtImg != XE::xPF_ARGB8888 );	// 아직은 이것만 지원.

	if( fmtImg && fmtImg != m_FormatSurface ) {
		// 이미지소스가 서피스 포맷과 다르다면 변환해야한다.
		XE::CreateConvertPixels( _pImg, sizeImg, fmtImg, &pImg, m_FormatSurface );
	}
	// formatSurface로 변환시켰으므로.
	auto _glFmtImg = XGraphicsOpenGL::sToGLFormat( m_FormatSurface );
	auto _glTypeImg = XGraphicsOpenGL::sToGLType( m_FormatSurface );
	glTexSubImage2D( GL_TEXTURE_2D,
									 0,		// mipmap level
									 (GLint)vLT.x,
									 (GLint)vLT.y,
									 (GLsizei)sizeImg.w,
									 (GLsizei)sizeImg.h,
									 _glFmtImg,		// gl텍스쳐의 포맷과 픽셀데이타의 포맷이 맞지않으면 내부에서 변환해주는듯 하다.
									 _glTypeImg,
									 pImg );
	CHECK_GL_ERROR();
	if( fmtImg && fmtImg != m_FormatSurface ) {
		SAFE_DELETE_ARRAY( pImg );
	}
}

int xAtlas::GetBytes() const
{
	const int bpp = XE::GetBpp( m_FormatSurface );
	return (int)m_Size.Size() * bpp;
}

//
XE_NAMESPACE_END; // xTextureAtlas

