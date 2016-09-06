#include "stdafx.h"
#include "SprDat.h"
#include "XActObj.h"
#include "SprObj.h"
#include "etc/types.h"
#include "etc/Debug.h"
#include "etc/xLang.h"
#include "XResObj.h"
#include "XResMng.h"
#include "XAutoPtr.h"
#include "XFramework/client/XApp.h"
#include "XFramework/client/XClientMain.h"

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
void XSprDat::Destroy( void )
{
	int i;
	// 액션리스트 삭제
	for( i = 0; i < GetnNumActions(); i++ )
		SAFE_DELETE( m_ppActions[i] );
	SAFE_DELETE_ARRAY( m_ppActions );
	// 스프라이트 삭제
	for( i = 0; i < GetnNumSprites(); i++ )
		SAFE_DELETE( m_ppSprites[i] );
	SAFE_DELETE_ARRAY( m_ppSprites );

	SAFE_DELETE_ARRAY( m_pcLuaAll );
}

// 액션을 하나추가하고 그 액션인덱스를 리턴한다.
void XSprDat::AddAction( int idx, XAniAction *pAction )
{
#ifdef _XDEBUG
	if( idx >= GetnNumActions() ) {
		XLOG( "%s idx(%d) >= GetnNumActions(%d)", GetszFilename(), idx, GetnNumActions() );
		return;
	}
#endif
	m_ppActions[ idx ] = pAction;
}


void XSprDat::AddSprite( int idx, XSprite *pSpr )
{
#ifdef _XDEBUG
	if( idx >= GetnNumSprites() ) {
		XLOG( "%s idx(%d) >= GetnNumSprites(%d)", GetszFilename(), idx, GetnNumSprites() );
		return;
	}
#endif
	m_ppSprites[ idx ] = pSpr;
}
//
BOOL XSprDat::Load( LPCTSTR _szFilename, bool bUseAtlas, BOOL bSrcKeep, BOOL bRestore )
{
	XLP1;
	LPCTSTR szFilename = NULL;
	if( bRestore )	{
		szFilename = static_cast<LPCTSTR>( m_szFilename );
		XBREAK( XE::IsEmpty(szFilename) == TRUE );
	} else {
		szFilename = _szFilename;
	}
	m_bUseAtlas = bUseAtlas;
	XSPR_TRACE( "%s load start", MakePath( DIR_SPR, szFilename ) );
	XSPR_TRACE("XSprDat::Load: try find lang folder");
	// 국가폴더 우선으로 읽도록 바뀜
	XBaseRes *pRes = XE::CreateResHandle( XE::MakePathLang( DIR_SPR, szFilename ) );
	XAutoPtr<XBaseRes*> _ptr( pRes );		// pRes는 블럭을 벗어날때 자동 삭제됨
	if( pRes == NULL )	{
		XSPR_TRACE("XSprDat::Load: try find main folder");
		pRes = XE::CreateResHandle( XE::MakePath( DIR_SPR, szFilename ) );
		_ptr.Setptr( pRes );
		if( pRes == NULL )	{
#if defined(WIN32) && defined(_DEBUG)
			XBREAK(1);		// lazyload 상황이 아닌데 spr파일 없을때 그냥 지나가버려서 문제점 찾는데 오래걸려서 일단 넣어봄
#endif
#ifndef _XSPR_LAZY_LOAD
			XERROR( "sprite %s open failed", szFilename );
#endif // not spr lazy load
			XSprite::sClearHSL();
			return FALSE;
		}
	}
	if( bRestore == FALSE )
		_tcscpy_s( m_szFilename, szFilename );
	WORD wData;
	DWORD dw1;
	pRes->Read( &wData, 2 );					// version
	if( wData > XSprDat::SPR_VER ) {	// 툴에서 저장된 버전은 높은데 xcode소스는 버전이 낮을때 이런게 생긴다. 반면 오히려 낮은버전의 파일이면 읽기를 취소하지 않고 호환되게 읽기를 시도한다
		XALERT( "%s 파일의 버전(%d)이 최신버전인 %d보다 크다", szFilename, (int)wData, XSprDat::SPR_VER );
		XSprite::sClearHSL();
		return FALSE;
	}
	if( bRestore == FALSE )
		m_nVersion = wData;
	dw1 = 0;
	DWORD reserved[4];
	pRes->Read( &dw1, 4 );		// 고해상도 플래그
	if( bRestore == FALSE ) {
//		m_bHighReso = (BOOL)dw1;
		BOOL bHighReso = (BOOL)dw1;
		XBREAK( bHighReso == FALSE );		// 혹시나 저해상도로 쓰고있는 파일이 있으면 bHighReso속성은 없애면 안됨.
	}
	pRes->Read( reserved, 4, 3 );	//reserved
	pRes->Read( &dw1, 4 );			// layer global id(게임에선 걍 스킵)
	// 합쳐진 lua코드
	if( IsUpperVersion(19) ) {
		int len;
		pRes->Read( &len, 4 );		// lua length 널포함 길이
		XBREAK( len < 0 );
		if( len > 0 ) {		// 루아코드가 있을때만
			if( bRestore )
				pRes->Seek( len );	// restore모드에선 건너뜀
			else {
				m_pcLuaAll = new char[ len ];
				pRes->Read( m_pcLuaAll, len );		// lua code read
			}
		}
	}
	// sprites
	int numSpr;
	pRes->Read( &numSpr, 4 );		// 스프라이트 개수
	if( bRestore == FALSE ) {
		m_nNumSprites = numSpr;
		m_ppSprites = new XSprite*[ numSpr ];
	} else {
		XBREAKF( numSpr != m_nNumSprites, "numSpr(%d) != m_nNumSprites(%d)", numSpr, m_nNumSprites );
		XBREAK( m_ppSprites == NULL );
	}
	XSPR_TRACE("SprDat: num sprites:%d", m_nNumSprites );
	int i;
	// sprite list load
	for( i = 0; i < numSpr; i ++ ) {
		XSPR_TRACE("SprDat: spr%d", i );
		XSprite *pSpr = NULL;
		if( bRestore == FALSE ) {
			pSpr = new XSprite( i );
			pSpr->Load( this, pRes, bUseAtlas, bSrcKeep, FALSE );
			AddSprite( i, pSpr );
			XE::VEC2 vSize = pSpr->GetsizeMemAligned();
			int byteSize = (int)(vSize.w * vSize.h * 2);
			XSprite::s_sizeTotalMem += byteSize;
			m_SizeByte += byteSize;
		} else {
			// restore device mode
			pSpr = m_ppSprites[ i ];
			XBREAK( pSpr == NULL );
			pSpr->Load( this, pRes, bUseAtlas, bSrcKeep, TRUE );
		}
	}
	if( bRestore == FALSE )	{
		// action list load
		pRes->Read( &m_nNumActions, 4 );
		m_ppActions = new XAniAction*[ m_nNumActions ];
		for( i = 0; i < m_nNumActions; i ++ ) {
			DWORD id=0;
			pRes->Read( &id, 4 );			// action id
			if( id == 0 )
				XLOG( "%s index %d의 action id가 0이다", GetszFilename(), id );
			auto pAction = new XAniAction( this, id );
			pAction->Load( this, pRes, m_nVersion );
			AddAction( i, pAction );
			if( pAction->GetID() >= MAX_ID )
				XLOG( "%s act idx=%d id=%d id > MAX_ID ", szFilename, i, pAction->GetID() );
			else
				m_indexFromID[ pAction->GetID() ] = i;
		}
		m_bKeepSrc = bSrcKeep;
	}
	XLP2;
	XLOGP( "%s, %llu",  XE::GetFileName( m_szFilename ), __llPass );
	XSprite::sClearHSL();
	return TRUE;
}

#ifdef WIN32
void XSprDat::Reload()
{
	Destroy();
	if( XE::IsHave(m_szFilename) )
		Load( m_szFilename, m_bUseAtlas, m_bKeepSrc, FALSE );
}

#endif // WIN32

/**
 @brief spr이 보유하고 있던 각 서피스들의 디바이스자원을 날리고 클리어 한다.
*/
void XSprDat::DestroyDevice()
{
	for( auto i = 0; i < GetnNumSprites(); i++ ) {
		auto pSprite = m_ppSprites[ i ];
		if( pSprite )
			pSprite->GetpSurface()->DestroyDevice();
	}
}




