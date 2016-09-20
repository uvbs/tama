#include "stdafx.h"
#include "Key.h"
#include "Sprite.h"
#include "XActDat.h"
#include "XActDat.h"
#include "SprObj.h"
#include "etc/types.h"
#include "etc/Debug.h"
#include "etc/xLang.h"
#include "XResObj.h"
#include "XResMng.h"
#include "XAutoPtr.h"
#include "XFramework/client/XApp.h"
#include "XFramework/client/XClientMain.h"
#include "Sprite/SprMng.h"
#include "SprDat.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;

/////////////////////////////////////////////////////////
//
// class XSprObj
//
/////////////////////////////////////////////////////////
void XSprDat::Destroy()
{
	int i;
	// 액션리스트 삭제
	for( i = 0; i < GetnNumActions(); i++ )
		SAFE_DELETE( m_ppActions[i] );
	SAFE_DELETE_ARRAY( m_ppActions );
	// 스프라이트 삭제
	XVECTOR_DESTROY( m_arySprite );
// 	for( i = 0; i < GetnNumSprites(); i++ )
// 		SAFE_DELETE( m_ppSprites[i] );
// 	SAFE_DELETE_ARRAY( m_ppSprites );

	SAFE_DELETE_ARRAY( m_pcLuaAll );
}

// 액션을 하나추가하고 그 액션인덱스를 리턴한다.
void XSprDat::AddAction( int idx, XActDat *pAction )
{
#ifdef _XDEBUG
	if( idx >= GetnNumActions() ) {
		XLOG( "%s idx(%d) >= GetnNumActions(%d)", GetszFilename(), idx, GetnNumActions() );
		return;
	}
#endif
	m_ppActions[ idx ] = pAction;
}


void XSprDat::AddSprite( /*int idx, */XSprite *pSpr )
{
// #ifdef _XDEBUG
// 	if( idx >= GetnNumSprites() ) {
// 		XLOG( "%s idx(%d) >= GetnNumSprites(%d)", GetszFilename(), idx, GetnNumSprites() );
// 		return;
// 	}
// #endif
//	m_ppSprites[ idx ] = pSpr;
	m_arySprite.push_back( pSpr );
}
//
BOOL XSprDat::Load( LPCTSTR _szFilename, 
										bool bUseAtlas, 
										bool bAsyncLoad,
										const XE::xHSL& hsl,
										BOOL bSrcKeep, 
										BOOL bRestore,
										bool bBatch )
{
	XLOAD_PROFILE1;
	m_HSL = hsl;
	m_bBatch = bBatch;
	LPCTSTR szFilename = _T("");
	if( bRestore )	{
		szFilename = m_strFile.c_str();
		XBREAK( m_strFile.empty() );
	} else {
		szFilename = _szFilename;
	}
	m_bUseAtlas = bUseAtlas;
	XSPR_TRACE( "%s load start", MakePath2( DIR_SPR, szFilename ) );
	XSPR_TRACE("XSprDat::Load: try find lang folder");
	// 국가폴더 우선으로 읽도록 바뀜
	std::shared_ptr<XBaseRes> spRes( XE::CreateResHandle( XE::MakePathLang2( DIR_SPR, szFilename ) ) );
//	XAutoPtr<XBaseRes*> _ptr( pRes );		// pRes는 블럭을 벗어날때 자동 삭제됨
	if( spRes == nullptr )	{
		XSPR_TRACE("XSprDat::Load: try find main folder");
		spRes = std::shared_ptr<XBaseRes>( XE::CreateResHandle( XE::MakePath2( DIR_SPR, szFilename ) ) );
// 		_ptr.Setptr( pRes );
		if( spRes == nullptr )	{
#if  !defined(_XSPR_LAZY_LOAD) || defined(_XASYNC_SPR)
			XERROR( "sprite %s open failed", szFilename );
#elif (defined(WIN32) && defined(_DEBUG))
			XBREAK( 1 );		// lazyload 상황이 아닌데 spr파일 없을때 그냥 지나가버려서 문제점 찾는데 오래걸려서 일단 넣어봄
#endif // not spr lazy load
			return FALSE;
		}
	}
	if( !bRestore ) {
		m_strFile = szFilename;
	}
	WORD wData;
	DWORD dw1;
	spRes->Read( &wData, 2 );					// version
	if( wData > XSprDat::SPR_VER ) {	// 툴에서 저장된 버전은 높은데 xcode소스는 버전이 낮을때 이런게 생긴다. 반면 오히려 낮은버전의 파일이면 읽기를 취소하지 않고 호환되게 읽기를 시도한다
#ifdef _XASYNC_SPR
		XTRACE( "%s 파일의 버전(%d)이 최신버전인 %d보다 크다", szFilename, (int)wData, XSprDat::SPR_VER );
#else
		XALERT( "%s 파일의 버전(%d)이 최신버전인 %d보다 크다", szFilename, (int)wData, XSprDat::SPR_VER );
#endif // _XASYNC_SPR
		return FALSE;
	}
	if( bRestore == FALSE )
		m_nVersion = wData;
	dw1 = 0;
	DWORD reserved[4];
	spRes->Read( &dw1, 4 );		// 고해상도 플래그
	if( bRestore == FALSE ) {
//		m_bHighReso = (BOOL)dw1;
		BOOL bHighReso = (BOOL)dw1;
		XBREAK( bHighReso == FALSE );		// 혹시나 저해상도로 쓰고있는 파일이 있으면 bHighReso속성은 없애면 안됨.
	}
	spRes->Read( reserved, 4, 3 );	//reserved
	spRes->Read( &dw1, 4 );			// layer global id(게임에선 걍 스킵)
	// 합쳐진 lua코드
	if( IsUpperVersion(19) ) {
		int len;
		spRes->Read( &len, 4 );		// lua length 널포함 길이
		XBREAK( len < 0 );
		if( len > 0 ) {		// 루아코드가 있을때만
			if( bRestore )
				spRes->Seek( len );	// restore모드에선 건너뜀
			else {
				m_pcLuaAll = new char[ len ];
				spRes->Read( m_pcLuaAll, len );		// lua code read
			}
		}
	}
	// sprites
	int numSpr;
	spRes->Read( &numSpr, 4 );		// 스프라이트 개수
	if( bRestore ) {
		XBREAK( m_arySprite.empty() );
	}
	XSPR_TRACE("SprDat: num sprites:%d", GetnNumSprites() );
	int i;
	// sprite list load
	for( i = 0; i < numSpr; i ++ ) {
		XSPR_TRACE("SprDat: spr%d", i );
		XSprite *pSpr = nullptr;
		if( bRestore == FALSE ) {
			pSpr = new XSprite( i );
			pSpr->Load( this, spRes.get(), bUseAtlas, bAsyncLoad, m_HSL, bSrcKeep, bBatch, FALSE );
			AddSprite( /*i, */pSpr );
			if( !bAsyncLoad ) {
				XE::VEC2 vSize = pSpr->GetsizeMemAligned();
				int byteSize = (int)(vSize.w * vSize.h * 2);
				XSprite::s_sizeTotalMem += byteSize;
				if( !bUseAtlas )
					m_SizeByte += byteSize;
			}
		} else {
			// restore device mode
			pSpr = m_arySprite[ i ];
			XBREAK( pSpr == nullptr );
			pSpr->Load( this, spRes.get(), bUseAtlas, bUseAtlas, hsl, bSrcKeep, bBatch, TRUE );
		}
	}
	if( bRestore == FALSE )	{
		// action list load
		spRes->Read( &m_nNumActions, 4 );
		m_ppActions = new XActDat*[ m_nNumActions ];
		for( i = 0; i < m_nNumActions; i ++ ) {
			DWORD id=0;
			spRes->Read( &id, 4 );			// action id
			if( id == 0 )
				XLOG( "%s index %d의 action id가 0이다", GetszFilename(), id );
			auto pAction = new XActDat( this, id );
			pAction->Load( this, spRes.get(), m_nVersion );
			AddAction( i, pAction );
			if( pAction->GetID() >= MAX_ID )
				XLOG( "%s act idx=%d id=%d id > MAX_ID ", szFilename, i, pAction->GetID() );
			else
				m_indexFromID[ pAction->GetID() ] = i;
		}
		m_bKeepSrc = bSrcKeep;
	}
	XLOAD_PROFILE2;
	XLOGP( "%s, %llu",  XE::GetFileName( m_strFile ), __llPass );
//	XSprite::sClearHSL();
	return TRUE;
}

/**
@brief 비동기로딩으로 메모리에 올라온 데이터를 바탕으로 디바이스 자원을 생성한다.
*/
void XSprDat::CreateDevice()
{
	for( auto pSpr : m_arySprite )
		pSpr->CreateDevice();
//	m_bLoadComplete = true;
}



#ifdef WIN32
void XSprDat::Reload()
{
	Destroy();
	if( !m_strFile.empty() )
#ifdef _XASYNC_SPR
		Load( m_strFile.c_str(), m_bUseAtlas, true, m_HSL, m_bKeepSrc, FALSE, m_bBatch );
#else
		Load( m_szFilename, m_bUseAtlas, false, m_HSL, m_bKeepSrc, FALSE, m_bBatch );
#endif // _XASYNC_SPR
}

#endif // WIN32

/**
 @brief spr이 보유하고 있던 각 서피스들의 디바이스자원을 날리고 클리어 한다.
*/
void XSprDat::DestroyDevice()
{
	for( auto pSpr : m_arySprite ) {
		if( pSpr ) {
			if( pSpr->GetpSurface() )
				pSpr->DestroyDevice();
		}
	}
}

ID XSprDat::GetidActByRandom() const
{
	ID idxAct = xRandom( m_nNumActions );
	const auto pActDat = m_ppActions[ idxAct ];
	return (pActDat)? pActDat->GetID() : 0;
}

XSurface* XSprDat::GetSpriteSurface( int idxSpr ) {
	if( XBREAK( idxSpr >= GetnNumSprites() || idxSpr < 0 ) ) {
		return nullptr;
	}
	//		XSprite *pSpr = m_ppSprites[ nSpr ];
	auto pSpr = m_arySprite[idxSpr];
	XBREAK( pSpr == nullptr );
	if( pSpr )
		return pSpr->GetpSurface();
	return nullptr;
}

void XSprDat::UpdateUV( ID idTex,
												const XE::POINT& sizePrev,
												const XE::POINT& sizeNew )
{
	for( auto pSpr : m_arySprite ) {
		pSpr->UpdateUV( idTex, sizePrev, sizeNew );
	}
}
