#include "stdafx.h"
#include "XTextureAtlas.h"
#include "XFramework/XSplitNode.h"
#include "XGraphicsOpenGL.h"

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
	::glDeleteTextures( 1, &glTex );
	const int bpp = XE::GetBpp( m_FormatSurface );
	XSurface::sAddSizeTotalVMem( (int)(-m_Size.Size() * bpp) );
#ifdef WIN32
	TRACE("destroy atlas: id=%d\n", m_idTex );


#endif // WIN32
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
#ifdef _DEBUG
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
#endif // _DEBUG
	XGraphicsOpenGL::sBindTexture( m_idTex );
#ifdef _DEBUG
	auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR );
#endif // _DEBUG
	const void* pImg = _pImg;
	XBREAK( fmtImg != XE::xPF_ARGB8888 );	// 아직은 이것만 지원.
	if( fmtImg != m_FormatSurface ) {
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
#ifdef _DEBUG
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); 
	}
#endif // _DEBUG
	if( fmtImg != m_FormatSurface ) {
		SAFE_DELETE_ARRAY( pImg );
	}
//	XGraphicsOpenGL::sBindTexture( 0 );		// 이거 하면 안됨
}

//
XE_NAMESPACE_END; // xTextureAtlas

using namespace xnTexAtlas;

std::shared_ptr<XTextureAtlas> XTextureAtlas::s_spInstance;
//XE::VEC2 XTextureAtlas::s_sizeDefault = XE::VEC2( 256, 256 );
////////////////////////////////////////////////////////////////
std::shared_ptr<XTextureAtlas>& XTextureAtlas::sGet() {	if( s_spInstance == nullptr )		s_spInstance = std::shared_ptr<XTextureAtlas>( new XTextureAtlas );	return s_spInstance;}
void XTextureAtlas::sDestroyInstance() {
	s_spInstance.reset();
}
////////////////////////////////////////////////////////////////
XTextureAtlas::XTextureAtlas()
{
	XBREAK( s_spInstance != nullptr );
	Init();
}

void XTextureAtlas::Release( ID idTex )
{
	auto spAtlas = GetspAtlas( idTex );
	if( spAtlas ) {
		--spAtlas->m_refCnt;
		XBREAK( spAtlas->m_refCnt < 0 );
		if( spAtlas->m_refCnt == 0 ) {
			DestroyAtlas( spAtlas );
		}
	}
}

void XTextureAtlas::DestroyAtlas( XSPAtlas spAtlas )
{
	const ID idDestroy = spAtlas->m_idTex;
	for( auto itor = m_listAtlas.begin(); itor != m_listAtlas.end(); ) {
		if( (*itor)->m_idTex == idDestroy ) {
			m_listAtlas.erase( itor++ );
			break;
		} else {
			++itor;
		}
	}
}

/**
 @brief 이미지를 아틀라스에 배치하고 그 아이디와 위치를 얻는다.
 @param idTex 지정한 아틀라스에 배치한다. 0이면 적당한 아틀라스를 찾아서 배치한후 그 아이디를 리턴한다.
*/
ID XTextureAtlas::ArrangeImg( ID idTex,
															XE::xRect2* pOut,
															const void* pImgSrc,
															const XE::VEC2& sizeMemSrc,
															XE::xtPixelFormat fmtImgSrc,
															XE::xtPixelFormat fmtSurface,
															XE::VEC2* pOutSizeAtlas )
{
	xSplit::XNode* pNewNode = nullptr;
	XSPAtlas spAtlas;
	if( idTex == 0 ) {
		if( m_listAtlas.empty() ) {
			AddAtlas( /*XTextureAtlas::s_sizeDefault*/XE::VEC2(0), fmtSurface );
		}
		//
		for( auto _spAtlas : m_listAtlas ) {
			// 비어있는 아틀라스에 배치(서피스 포맷이 같아야 함)
			if( _spAtlas->m_FormatSurface == fmtSurface ) {
				pNewNode = InsertElem( _spAtlas, sizeMemSrc );
				if( pNewNode ) {
					// 배치성공
					spAtlas = _spAtlas;
					break;
				}
			}
		}
		// 루프를 다 돌았는데도 삽입을 못했다면 모든 아틀라스에 공간이 없다는것임.
		if( !spAtlas ) {
			// 공간이 더이상 없음. 새 아틀라스 추가
			spAtlas = AddAtlas( /*XTextureAtlas::s_sizeDefault*/XE::VEC2( 0 ), fmtSurface );
			if( XASSERT( spAtlas ) ) {
				pNewNode = InsertElem( spAtlas, sizeMemSrc );
				XBREAK( pNewNode == nullptr );
			}
		}
	} else {
		spAtlas = GetspAtlas( idTex );
		if( spAtlas ) {
			pNewNode = InsertElem( spAtlas, sizeMemSrc );
			if( pNewNode == nullptr )
				return 0;		// 아틀라스 아이디가 명시된것은 배치실패하면 그냥 0리턴.
		}

	}
	if( XASSERT( spAtlas ) ) {
		if( XASSERT( pNewNode ) ) {
			XE::xRECT rect = pNewNode->GetRect();
			pOut->vLT = rect.vLT;
			pOut->vRB = rect.vRB + XE::VEC2( 1, 1 );
			++spAtlas->m_refCnt;
			// 실제 디바이스 텍스쳐에 갱신.
			spAtlas->UpdateSubToDevice( pImgSrc,
																	pNewNode->GetRect().vLT,
																	pNewNode->GetRect().GetSize(),
																	fmtImgSrc );
			*pOutSizeAtlas = spAtlas->m_Size;
			return spAtlas->m_idTex;
		}
	}
	return 0;
}

xSplit::XNode* XTextureAtlas::InsertElem( XSPAtlas spAtlas,
																					const XE::VEC2& sizeElem ) const
{
	auto pNewNode = spAtlas->m_pRoot->Insert( sizeElem );
	if( pNewNode ) {
		// 배치성공
		pNewNode->SetidImg( spAtlas->m_idTex );
		if( pNewNode->GetRect().vRB.x > spAtlas->m_maxFill.x )
			spAtlas->m_maxFill.x = pNewNode->GetRect().vRB.x;
		if( pNewNode->GetRect().vRB.y > spAtlas->m_maxFill.y )
			spAtlas->m_maxFill.y = pNewNode->GetRect().vRB.y;
	}
	return pNewNode;
}

/**
 @brief idTex를 가지는 아틀라스를 찾는다.
*/
XSPAtlas XTextureAtlas::GetspAtlas( ID idTex )
{
	for( auto& spAtlas : m_listAtlas ) {
		if( spAtlas->m_idTex == idTex )
			return spAtlas;
	}
	return nullptr;
}
XSPAtlasConst XTextureAtlas::GetspAtlasConst( ID idTex ) const
{
	for( auto& spAtlas : m_listAtlas ) {
		if( spAtlas->m_idTex == idTex )
			return spAtlas;
	}
	return nullptr;
}
/**
 @brief pbo버퍼에 pImg를 갱신하고 텍스쳐로 전송
*/
// xSplit::XNode* XTextureAtlas::UpdateSub( XSPAtlas spAtlas,
// 															 const DWORD* pImg, 
// 															 const XE::VEC2& vLT, 
// 															 const XE::VEC2& sizeImg, 
// 															XE::xtPixelFormat fmtImgSrc,
// 															XE::xtPixelFormat fmtSurface )
// {
// }
// 
/**
 @brief 새 아틀라스를 생성하고 그 아틀라스를 리턴한다.
*/
XSPAtlas XTextureAtlas::AddAtlas( const XE::VEC2& _size, XE::xtPixelFormat formatSurface )
{
	auto size = _size;
// 	if( formatSurface == XE::xPF_ARGB8888 )
		size.Set( 2048, 2048 );
// 	else
// 		size.Set( 512, 512 );
	XSPAtlas spAtlas = std::make_shared<xAtlas>( size, formatSurface );
	// PBO버퍼와 텍스쳐를 만든다.
// 	DWORD* pImg = new DWORD[(int)(spAtlas->m_Size.Size()) ];		// 일단이걸로 테스트후 null로 바꿔봄
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
// 	spAtlas->m_idTex = GRAPHICS_GL->CreateTextureGL( pImg,
// 																									 spAtlas->m_Size,
// 																									 XE::xPF_ARGB8888,
// 																									 spAtlas->m_Size,	// aligned
// 																									 formatSurface );
	extern int s_glFmt, s_glType;
	spAtlas->m_glFmt = s_glFmt;
	spAtlas->m_glType = s_glType;
	spAtlas->m_idTex = GRAPHICS_GL->CreateTextureGL( nullptr,
																									 spAtlas->m_Size,
																									 XE::xPF_ARGB8888,
																									 spAtlas->m_Size,	// aligned
																									 formatSurface );
	XBREAK( spAtlas->m_idTex == 0 );
	m_listAtlas.push_back( spAtlas );
//  	SAFE_DELETE_ARRAY( pImg );
	return spAtlas;
}

ID XTextureAtlas::GetidTex( int idxAtlas )
{
	if( m_listAtlas.empty() )
		return 0;
	auto spAtlas = m_listAtlas.GetByIndex( idxAtlas );
	if( spAtlas )
		return spAtlas->m_idTex;
	return 0;
}
