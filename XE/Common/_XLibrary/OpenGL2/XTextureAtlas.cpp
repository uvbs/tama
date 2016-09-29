#include "stdafx.h"
#include "XTextureAtlas.h"
#include "XFramework/XSplitNode.h"
#include "XGraphicsOpenGL.h"
#include "Sprite/SprMng.h"
#include "XImageMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;
using namespace xnTexAtlas;

XList4<XSPAtlas> XTextureAtlas::s_listSurfaceAll;
XList4<XSPAtlasMng> XTextureAtlas::s_listAtlasLayer;
int XTextureAtlas::s_sizeVM = 0;

//////////////////////////////////////////////////////////////////////////
void XTextureAtlas::sRelease( ID idTex, ID idNode )
{
	if( idTex == 0 )
		return;
	XBREAK( idNode == 0 );
	for( auto pLayer : s_listAtlasLayer ) {
		pLayer->Release( idTex, idNode );
	}
}

/**
 @brief 참조하고 있는것이 없는 아틀라스 서피스를 삭제한다.
*/
void XTextureAtlas::sFlushAtlasSurface()
{
	
	for( auto itor = s_listSurfaceAll.begin(); itor != s_listSurfaceAll.end(); ) {
		auto& spAtlas = (*itor);
		const int useCnt = spAtlas.use_count();
		if( useCnt == 1 ) {
			const _tstring strTag = C2SZ( spAtlas->m_strTag );
			XTRACE( "delete no ref atlas:tag=%s, idTex=%d",
							strTag.c_str(),
							spAtlas->m_idTex );
			s_listSurfaceAll.erase( itor++ );
		} else {
			++itor;
		}
	}
}

/**
 @brief 아틀라스관리자 하나를 생성해주는 팩토리
*/
XSPAtlasMng XTextureAtlas::sCreateAtlasMng( const char* cTag )
{
	auto spAtlas = std::make_shared<XTextureAtlas>( cTag );
	s_listAtlasLayer.push_back( spAtlas );
	return spAtlas;
}

int XTextureAtlas::sGetBytesAll()
{
	int bytes = 0;
	for( auto spAtlas : s_listSurfaceAll ) {
		bytes += spAtlas->GetBytes();
	}
	return bytes;
}

////////////////////////////////////////////////////////////////
XSPAtlasMng XTextureAtlas::s_spCurrAtlasMng;
XTextureAtlas::XTextureAtlas( const char* cTag )
	: m_idMng( XE::GenerateID() )
	, m_strTag( cTag )
{
	Init();
}

void XTextureAtlas::Destroy()
{

}

/**
 @brief 텍스쳐(idTex) 한장을 아틀라스 서피스에서 해제시킨다.
 텍스쳐 참조카운터가 0이되면 아틀라스 서피스한장을 삭제시킨다.
*/
void XTextureAtlas::Release( ID idTex, ID idNode )
{
	if( idTex ) {
		auto spAtlas = GetspAtlas( idTex );
		if( spAtlas ) {
			auto pReleaseNode = spAtlas->m_pRoot->ReleaseNode( idNode );
			if( pReleaseNode ) {
				const XE::xRECT rect = pReleaseNode->GetRectTex();
				// 실제 디바이스 텍스쳐에 갱신.
				void* pImgSrc = nullptr;
				const int size = rect.GetSize().ToPoint().Size();
// 				const int bpp = XE::GetBpp( spAtlas->m_FormatSurface );
// 				if( bpp == 2 ) {
// 					pImgSrc = new WORD[ size ];
// 					memset( pImgSrc, 0, sizeof(WORD) * size );
// 				} else {
					pImgSrc = new DWORD[ size ];
					memset( pImgSrc, 0, sizeof( DWORD ) * size );
// 				}
				spAtlas->UpdateSubToDevice( pImgSrc,
																		rect.vLT,
																		rect.GetSize(),
																		XE::xPF_ARGB8888 );
				SAFE_DELETE_ARRAY( pImgSrc );
				pReleaseNode->Clear();
			}
			--spAtlas->m_refCnt;		// 텍스쳐 참조 해제
			XBREAK( spAtlas->m_refCnt < 0 );
			if( spAtlas->m_refCnt == 0 ) {
				DestroyAtlas( spAtlas );		// 아틀라스 서피스 1장 삭제
			}
		}
	}
	// 참조하고 있는것이 없는 아틀라스 서피스를 삭제
	sFlushAtlasSurface();
}

void XTextureAtlas::DestroyAtlas( XSPAtlas spAtlas )
{
	const ID idDestroy = spAtlas->m_idTex;
	// 아틀라스 서피스들의 목록에서 제거
	for( auto itor = m_listAtlas.begin(); itor != m_listAtlas.end(); ) {
		auto pAtlas = (*itor);
		if( pAtlas->m_idTex == idDestroy ) {
			m_listAtlas.erase( itor++ );
//			pAtlas->m_bDestroy = true;
			break;
		} else {
			++itor;
		}
	}
}

/**
 @brief 디바이스 자원만 날린다.
*/
void XTextureAtlas::DestroyDevice()
{
	for( auto spAtlas : m_listAtlas ) {
		spAtlas->DestroyDevice();
	}
}

/**
 @brief 메모리 자원을 날린다.
 홈으로가면 모든 자원이 삭제되어서 맵을 새로만들어야 하므로 아틀라스들을 모두 날린다.
*/
void XTextureAtlas::OnPause()
{
	m_listAtlas.clear();
}

/**
 @brief 이미지를 아틀라스에 배치하고 그 아이디와 위치를 얻는다.
 @param idTex 지정한 아틀라스에 배치한다. 0이면 적당한 아틀라스를 찾아서 배치한후 그 아이디를 리턴한다.
*/
ID XTextureAtlas::ArrangeImg( ID idTex,
															XE::xRect2* pOut,
															ID* pOutID,					// 아틀라스내 각 서피스의 고유아이디
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
			AddAtlas( XE::VEC2(0), fmtSurface );
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
			spAtlas = AddAtlas( XE::VEC2( 0 ), fmtSurface );
			if( spAtlas ) {
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
			const XE::xRECT rect = pNewNode->GetRectTex();
			pOut->vLT = rect.vLT;
			pOut->vRB = rect.vRB + XE::VEC2( 1, 1 );
			*pOutID = pNewNode->GetidNode();
			++spAtlas->m_refCnt;
			// 실제 디바이스 텍스쳐에 갱신.
			spAtlas->UpdateSubToDevice( pImgSrc,
																	rect.vLT,
																	rect.GetSize(),
																	fmtImgSrc );
			*pOutSizeAtlas = spAtlas->m_Size;
			return spAtlas->m_idTex;
		}
	}
	return 0;
}

/**
 @brief spAtlas에 sizeElem크기의 공간을 확보한다.
*/
xSplit::XNode* XTextureAtlas::InsertElem( XSPAtlas spAtlas,
																					const XE::VEC2& sizeElem ) const
{
	bool bResized = false;
	xSplit::XNode* pNewNode = nullptr;
	do {
		auto sizePrev = spAtlas->m_Size;
		pNewNode = spAtlas->m_pRoot->Insert( sizeElem );
		if( pNewNode ) {
			// 배치성공
			pNewNode->SetidImg( spAtlas->m_idTex );
			pNewNode->SetidNode( XE::GenerateID() );
			if( pNewNode->GetRectTex().vRB.x > spAtlas->m_maxFill.x )
				spAtlas->m_maxFill.x = pNewNode->GetRectTex().vRB.x;
			if( pNewNode->GetRectTex().vRB.y > spAtlas->m_maxFill.y )
				spAtlas->m_maxFill.y = pNewNode->GetRectTex().vRB.y;
		} else {
			s_sizeVM -= spAtlas->GetBytes();
			bResized = spAtlas->ResizeAtlas();
			s_sizeVM += spAtlas->GetBytes();
			if( bResized) {
				spAtlas->m_pRoot->ResizeRoot( spAtlas->m_Size );
				SPRMNG->UpdateUV( spAtlas->m_idTex, sizePrev.ToPoint(), spAtlas->m_Size.ToPoint() );
				IMAGE_MNG->UpdateUV( spAtlas->m_idTex, sizePrev.ToPoint(), spAtlas->m_Size.ToPoint() );
			} else {
				break;			// 더이상 사이즈를 늘일수 없음. 새 아틀라스에 넣어야 함.
			}
		}
	} while( pNewNode == nullptr );
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
 @brief 새 아틀라스를 생성하고 그 아틀라스를 리턴한다.
*/
XSPAtlas XTextureAtlas::AddAtlas( const XE::VEC2& _size, XE::xtPixelFormat formatSurface )
{
	auto size = _size;
#ifdef _XTEST
	size.Set( 256, 256 );
#else
//#pragma message("===================size.Set( 2048, 2048 )=======================")
 	size.Set( 128, 128 ); // 64로 했더니 텍스쳐 확장이 많이 일어나면서 유닛밑에 그림자가 깨지더라
//	size.Set( 2048, 2048 );
#endif // _XTEST
	XSPAtlas spAtlas = std::make_shared<xAtlas>( size, formatSurface );
	// PBO버퍼와 텍스쳐를 만든다.
	CHECK_GL_ERROR();
	spAtlas->m_idTex = GRAPHICS_GL->CreateTextureGL( nullptr,
																									 spAtlas->m_Size,
																									 XE::xPF_ARGB8888,
																									 spAtlas->m_Size,	// aligned
																									 formatSurface );
	s_sizeVM += spAtlas->GetBytes();
	extern int s_glFmt, s_glType;
	spAtlas->m_glFmt = s_glFmt;
	spAtlas->m_glType = s_glType;
	spAtlas->m_strTag = m_strTag;
	XBREAK( spAtlas->m_idTex == 0 );
	m_listAtlas.push_back( spAtlas );
	s_listSurfaceAll.push_back( spAtlas );
	return spAtlas;
}

/**
 @brief this 아틀라스의 크기를 두배로 늘인다.
 @return 성공하면 true, false는 더이상 크기를 늘일수 없다.
*/
bool xnTexAtlas::xAtlas::ResizeAtlas()
{
	const int maxTex = XSurface::GetMaxSurfaceWidth();
	XBREAK( maxTex == 0 );
	const auto sizePrev = m_Size;
	if( m_Size.w < maxTex && m_Size.h < maxTex ) {
		m_Size *= 2.f;
	} else {
		return false;
	}
	GRAPHICS_GL->ResizeTexture( m_idTex,
															sizePrev.ToPoint(),
															m_Size.ToPoint(),
															m_glType,
															m_glFmt );
	return true;
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

/**
 @brief 현재 아틀라스를 this로 선택한다.
*/
void XTextureAtlas::PushAtlasMng()
{
	m_spMngPrev = XTextureAtlas::_sSetpCurrMng( shared_from_this() );
}

/**
 @brief 현재 아틀라스를 이전 아틀라스로 반환한다.
*/
void XTextureAtlas::PopAtlasMng()
{
	XTextureAtlas::_sSetpCurrMng( m_spMngPrev );
}
