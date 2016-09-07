#include "stdafx.h"
#include "sprite/SprMng.h"
#include "VersionXE.h"
#include "Sprite/SprDat.h"
#include "XHSLMap.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSprMng *SPRMNG = NULL;
int XSprMng::s_sizeTotalVM = 0;
xSec XSprMng::s_secCache = 180;

inline ID XSprMng::xDat::getid() {
	return ( pSprDat ) ? pSprDat->GetsnDat() : 0;
}
bool XSprMng::xDat::IsOverTime() const {
	return XTimer2::sGetTime() - m_secLoaded > XSprMng::s_secCache;
}

//////////////////////////////////////////////////////////////////////////
XSprMng* XSprMng::s_pInstance = nullptr;
XSprMng::XSprMng()
{
	XBREAK( s_pInstance != nullptr );
	s_pInstance = this;
	Init();
	XTRACE( "create sprmng" );
}

void XSprMng::Destroy( void )
{
	DoFlushCache();
	// 매니저가 최종 삭제될때까지 release되지 않은 객체가 있으면 경고알려줌
	CheckRelease();
	// 실제 삭제
	// 해제하지 않은 스프라이트가 있을때 어디서 릭났는지 알기 힘들어 삭제하는부분 뺌.
	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
		auto& dat = (*itor);
		s_sizeTotalVM -= dat.pSprDat->GetSizeByte();
		SAFE_DELETE( dat.pSprDat );
		m_listSprDat.erase( itor++ );
	}
	s_pInstance = nullptr;
}

void XSprMng::OnCreate()
{
#ifdef _XASYNC_SPR
	m_spLock = std::make_shared<XLock>();
	//
	m_hThread = _beginthreadex( nullptr
															, 0
															, _WorkThread
															, nullptr
															, 0
															, (unsigned*)&m_idThread );
	CONSOLE( "spr thread: hThread=%d, idThread=%d", (DWORD)m_hThread, m_idThread );
#endif // _XASYNC_SPR
}


/**
@brief 레퍼런스 카운트가 0이된것들중 시간이 오래된건 삭제시킨다.
*/
void XSprMng::DestroyOlderFile()
{
	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
		auto& dat = (*itor);
		auto pSprDat = dat.pSprDat;
		//		xSec secPass = XTimer2::sGetTime() - dat.m_secLoaded;
		// 아무도 참조하지 않고 로딩한지 x분이 넘었으면 삭제
		// 		if( pSprDat->GetnRefCnt() == 0 && secPass > 60 * 3 ) {
		if( pSprDat->GetnRefCnt() == 0 && dat.IsOverTime() ) {
			s_sizeTotalVM -= pSprDat->GetSizeByte();
			SAFE_DELETE( pSprDat );
			m_listSprDat.erase( itor++ );
		} else {
			++itor;
		}
	}
}

// 해제되지 않은 객체가 있는지 검사한다
void XSprMng::CheckRelease( void )
{
	for( auto& dat : m_listSprDat ) {
		XALERT( "SprMng: 해제되지 않은 스프라이트 발견. %s", dat.pSprDat->GetszFilename() );
	}
}

void XSprMng::Release( XSprDat *pSprDat )
{
	//
	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
		auto& dat = (*itor);
		auto p = dat.pSprDat;
		if( p->GetsnDat() == pSprDat->GetsnDat() ) {
			p->DecRefCnt();
			XBREAK( p->GetnRefCnt() < 0 );
			// 레퍼런스 카운트를 떨어트리고 일단은 지우지 않는다.
			++itor;
			return;
		} else
			itor++;
	}
	// refCnt==0인것중 오래된파일은 삭제한다.
	DestroyOlderFile();
}

/**
@brief 아무도 참조하고 있지는 않지만 캐쉬타임에 걸려 아직 파괴되지 않은 리스스들을 모두 날린다.
스프라이트는 캐시타임을 쓰지 않으므로 해당없음. 그냥 코딩만 해둠.
*/
void XSprMng::DoFlushCache()
{
	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
		auto& dat = (*itor);
		if( dat.pSprDat->GetnRefCnt() == 0 ) {
			s_sizeTotalVM -= dat.pSprDat->GetSizeByte();
			SAFE_DELETE( dat.pSprDat );
			m_listSprDat.erase( itor++ );
		} else
			++itor;
	}
}

XSprMng::xDat* XSprMng::Find( LPCTSTR szFilename, BOOL bSrcKeep, const XE::xHSL& hsl )
{
	for( auto& dat : m_listSprDat ) {
		auto pSprDat = dat.pSprDat;
		if( _tcsicmp( dat.m_strKey.c_str(), szFilename ) == 0 ) {
			if( pSprDat->GetbKeepSrc() == TRUE )
				return &dat;
			if( pSprDat->GetbKeepSrc() == bSrcKeep )		// 만약 이미 로딩한 파일이 있으나 메모리 보존상황이 다르면 새로 생성하도록 한다
				return &dat;		// 가급적 이쪽으로 오지 않게끔 로딩을 배치하자
		}
	}
	return nullptr;
}

/**
@brief .spr파 일을 읽어 XSprDat* 객체로 만든다.
@param bAsyncLoad 파일을 비동기로 읽는다
*/
XSprDat *XSprMng::Load( LPCTSTR szFilename,
												const XE::xHSL& /*hsl*/,
												bool bUseAtlas,
												BOOL bAddRefCnt,
												BOOL bSrcKeep,
												bool bAsyncLoad )
{
	XE::xHSL hsl;
	_tstring strFile = szFilename;
	// HSL맵을 먼저 검색한다.
	const _tstring strKey = XE::GetFileName( szFilename );			// 파일명을 키로 사용함.
	const XE::xHSLFile* pHslFile = XHSLMap::sGet()->GetInfo( strKey );
	if( pHslFile ) {
		strFile = pHslFile->m_strFile;
		hsl = pHslFile->m_HSL;
	} else {
	}
	/*
	비동기로딩은 무조건 스레드를 써야한다. 프로세스에서 실시간으로 읽으면 파일읽는 시간은 여전히 존재하기때문에
	전체화면은 나타났지만 터치가 버벅되는 문제가 생긴다.
	*/
	XBREAK( bAddRefCnt == FALSE );		// 이제 이 옵션은 필요없게 됬다. 사전로딩을 위한거였는데 사전로딩도 TRUE로 읽어야 하게 바뀜
																		// 리스트에 szFilename으로 생성된게 있는지 찾는다.
	xDat* pDat = Find( szFilename, bSrcKeep, hsl );
	XSprDat* pSprDat = nullptr;
	if( pDat ) {
		pSprDat = pDat->pSprDat;
		if( bAddRefCnt )
			pSprDat->AddRefCnt();
		pDat->m_secLoaded = XTimer2::sGetTime();
		DestroyOlderFile();			// 여기다 쓰지말고 씬로딩완료 직후에 한번 불러주는게 더 효율적일듯.
		return pSprDat;
	}
#ifdef _XASYNC_SPR
	// 로딩을 요청받으면 리스트에만 받아둔다.
	xAsync asyncSpr;
	asyncSpr.m_strRes = szFilename;
	asyncSpr.m_HSL = hsl;
	asyncSpr.m_bSrcKeep = bSrcKeep;
	asyncSpr.m_bAddRefCnt = bAddRefCnt;
	asyncSpr.m_bUseAtlas = bUseAtlas;
	{
		XAUTO_LOCK2( m_spLock );
		m_listAsync.Add( asyncSpr );
	}
	return nullptr;
#else // _XSYNC_SPR
	if( bAsyncLoad )	// 비동기 로딩이면 sprDat객체를 지금 생성하지 않음.
		return nullptr;
	pSprDat = new XSprDat;
	XSprite::sSetHSL( hsl );
	if( pSprDat->Load( strFile.c_str(), bUseAtlas, bSrcKeep, FALSE ) ) {
		if( !bUseAtlas )
			s_sizeTotalVM += pSprDat->GetSizeByte();
		Add( _tstring( szFilename ), pSprDat, hsl );
		if( bAddRefCnt )
			pSprDat->AddRefCnt();
		DestroyOlderFile();
		return pSprDat;
	}
	// failed
	SAFE_DELETE( pSprDat );
	return nullptr;
#endif // not _XASYNC_SPR
} // Load

void XSprMng::Add( const _tstring& strKey, XSprDat *pSprDat, const XE::xHSL& hsl )
{
	xDat dat( strKey, pSprDat, hsl );
	dat.m_secLoaded = XTimer2::sGetTime();
	m_listSprDat.push_back( dat );
	m_mapTable[strKey] = dat;
}

void XSprMng::RestoreDevice()
{
	DoFlushCache();
	for( auto& dat : m_listSprDat ) {
		XSprDat *pSprDat = dat.pSprDat;
		XSPR_TRACE( "XSprMng::RestoreDevice(): %s", pSprDat->GetszFilename() );
#if defined(_DEBUG) && defined(_CHEAT)
		if( pSprDat ) {
			CONSOLE( "restore spr:[%s]", pSprDat->GetszFilename() );
		}
#endif // _DEBUG
		if( pSprDat->GetnRefCnt() != 0 )		// refcnt가 0인것은 restore하지 않음.
			pSprDat->RestoreDevice();
	}
}

#ifdef WIN32
void XSprMng::Reload()
{
	DoFlushCache();
	for( auto& dat : m_listSprDat ) {
		dat.pSprDat->Reload();
	}
}

#endif // WIN32

void XSprMng::OnPause()
{
	DoFlushCache();
	for( auto& dat : m_listSprDat ) {
		dat.pSprDat->DestroyDevice();
	}
}

#ifdef _XASYNC_SPR
#pragma message("ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ")

unsigned int __stdcall XSprMng::_WorkThread( void *param ) 
{
	SPRMNG->WorkThread();
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void XSprMng::WorkThread()
{
	while(1) {
		xAsync asyncLoad;
		while(1) {
			XAUTO_LOCK2( m_spLock );
			if( m_listAsync.size() > 0 ) {
				// 앞에거 하나 빼냄
				auto itor = m_listAsync.begin();
				asyncLoad = (*itor);
				m_listAsync.erase( itor++ );
				break;
			}
			Sleep(10);
		} // while
		//
		{
			XAUTO_LOCK2( XGraphics::s_spLock );
			auto pSprDat = new XSprDat;
			XSprite::sSetHSL( asyncLoad.m_HSL );
			auto bOk = pSprDat->Load( asyncLoad.m_strRes.c_str(),
																asyncLoad.m_bUseAtlas,
																asyncLoad.m_bSrcKeep );
			if( bOk ) {
				s_sizeTotalVM += pSprDat->GetSizeByte();
				Add( asyncLoad.m_strRes, pSprDat, asyncLoad.m_HSL );
				if( asyncLoad.m_bAddRefCnt )
					pSprDat->AddRefCnt();
			} else {
				// failed
				SAFE_DELETE( pSprDat );
			}
		}
		Sleep(10);
	} // while
}

void XSprMng::AsyncSetAction( ID idAct, xRPT_TYPE playType, BOOL bExecFrameMove )
{

}

#endif // _XASYNC_SPR







