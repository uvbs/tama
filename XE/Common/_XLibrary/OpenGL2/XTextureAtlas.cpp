﻿#include "stdafx.h"
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
static const XE::VEC2 c_sizeDefault = XE::VEC2(256, 256);

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
}

/**
 @brief 실제텍스쳐에 새로 추가된 노드를 부분 갱신
 @fmtImg gl서피스의 포맷이 아니고 pImg의 픽셀포맷.
*/
void xAtlas::UpdateSubToDevice( const void* pImg,
																const XE::VEC2& vLT,
																const XE::VEC2& sizeImg,
																xtPixelFormat fmtImg ) {
	const auto glFmtImg = XGraphicsOpenGL::sToGLFormat( fmtImg );
	const auto glTypeImg = XGraphicsOpenGL::sToGLType( fmtImg );
	glBindTexture( GL_TEXTURE_2D, (GLuint)m_idTex );
#ifdef _DEBUG
	auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR );
#endif // _DEBUG
	glTexSubImage2D( GL_TEXTURE_2D,
									 0,		// mipmap level
									 (GLint)vLT.x,
									 (GLint)vLT.y,
									 (GLsizei)sizeImg.w,
									 (GLsizei)sizeImg.h,
									 glFmtImg,		// gl텍스쳐의 포맷과 픽셀데이타의 포맷이 맞지않으면 내부에서 변환해주는듯 하다.
									 glTypeImg,
									 pImg );
#ifdef _DEBUG
	glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR );
#endif // _DEBUG
}

//
XE_NAMESPACE_END; // xTextureAtlas

using namespace xnTexAtlas;

std::shared_ptr<XTextureAtlas> XTextureAtlas::s_spInstance;
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

/**
 @brief 이미지를 아틀라스에 배치하고 그 아이디와 위치를 얻는다.
 @param idTex 지정한 아틀라스에 배치한다. 0이면 적당한 아틀라스를 찾아서 배치한후 그 아이디를 리턴한다.
*/
ID XTextureAtlas::ArrangeImg( ID idTex,
															XE::xRect2* pOut,
															const void* pImgSrc,
															const XE::VEC2& sizeMemSrc,
															XE::xtPixelFormat fmtImgSrc,
															XE::xtPixelFormat fmtSurface )
{
	xSplit::XNode* pNewNode = nullptr;
	XSPAtlas spAtlas;
	if( idTex == 0 ) {
		if( m_listAtlas.empty() ) {
			AddAtlas( xnTexAtlas::c_sizeDefault, fmtSurface );
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
			spAtlas = AddAtlas( xnTexAtlas::c_sizeDefault, fmtSurface );
			if( XASSERT( spAtlas ) ) {
				pNewNode = InsertElem( spAtlas, sizeMemSrc );
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
			// 실제 디바이스 텍스쳐에 갱신.
			spAtlas->UpdateSubToDevice( pImgSrc,
																	pNewNode->GetRect().vLT,
																	pNewNode->GetRect().GetSize(),
																	fmtImgSrc );
			return spAtlas->m_idTex;
		}
	}
	return 0;
}

xSplit::XNode* XTextureAtlas::InsertElem( XSPAtlasConst spAtlas,
																					const XE::VEC2& sizeElem )
{
	auto pNewNode = spAtlas->m_pRoot->Insert( sizeElem );
	if( pNewNode ) {
		// 배치성공
		pNewNode->SetidImg( spAtlas->m_idTex );
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
XSPAtlas XTextureAtlas::AddAtlas( const XE::VEC2& size, XE::xtPixelFormat formatSurface )
{
	XSPAtlas spAtlas = std::make_shared<xAtlas>( size, formatSurface );
	// PBO버퍼와 텍스쳐를 만든다.
	DWORD* pImg = new DWORD[(int)(spAtlas->m_Size.Size()) ];		// 일단이걸로 테스트후 null로 바꿔봄
	spAtlas->m_idTex = GRAPHICS_GL->CreateTextureGL( pImg,
																									 spAtlas->m_Size,
																									 formatSurface,
																									 spAtlas->m_Size,	// aligned
																									 formatSurface );
	XBREAK( spAtlas->m_idTex == 0 );
	m_listAtlas.push_back( spAtlas );
	SAFE_DELETE_ARRAY( pImg );
	return spAtlas;
}