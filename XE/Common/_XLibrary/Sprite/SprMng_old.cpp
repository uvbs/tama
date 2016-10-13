#include "stdafx.h"
#ifndef _XASYNC_SPR
// 비동기 로딩 버전.
#include "SprMng.h"
#include "VersionXE.h"
#include "SprDat.h"
#include "XHSLMap.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xSpr;

XSprMng *SPRMNG = NULL;
int XSprMng::s_sizeTotalVM = 0;
xSec XSprMng::s_secCache = 180;

inline ID xSpr::xDat::getid() {
	return ( m_pSprDat ) ? m_pSprDat->GetsnDat() : 0;
}
bool xSpr::xDat::IsOverTime() const {
	return XTimer2::sGetTime() - m_secLoaded > XSprMng::s_secCache;
}

xSpr::xDat::~xDat() {
	SAFE_DELETE( m_pSprDat );
}


XSprMng::xAsync::xAsync() 
/*: m_playType( xRPT_LOOP )*/ {
	m_idAsync = XE::GenerateID();
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
	XAUTO_LOCK2( m_spLock );
	DoFlushCache();
	// 매니저가 최종 삭제될때까지 release되지 않은 객체가 있으면 경고알려줌
	CheckRelease();
	// 실제 삭제
	// 해제하지 않은 스프라이트가 있을때 어디서 릭났는지 알기 힘들어 삭제하는부분 뺌.
//	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
#ifndef _XASYNC_SPR
	for( auto& spDat : m_listSprDat ) {
		s_sizeTotalVM -= spDat->m_pSprDat->GetSizeByte();
		SAFE_DELETE( spDat->m_pSprDat );
// 		m_listSprDat.erase( itor++ );
	}
#endif // not _XASYNC_SPR
	m_listSprDat.clear();
	m_mapTable.clear();
//	m_listCache.clear();
	s_pInstance = nullptr;
}

// 해제되지 않은 객체가 있는지 검사한다
void XSprMng::CheckRelease( void )
{
	XAUTO_LOCK2( m_spLock );
	// 	for( auto& spDat : m_listSprDat ) {
	for( auto itor : m_mapTable ) {
		auto& spDat = itor.second;
		XALERT( "SprMng: 해제되지 않은 스프라이트 발견. %s"
						, spDat->m_pSprDat->GetszFilename() );
	}
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
void XSprMng::DestroyOverTimeFile()
{
	XAUTO_LOCK2( m_spLock );
	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
		auto& spDat = (*itor);
		auto pSprDat = spDat->m_pSprDat;
		//		xSec secPass = XTimer2::sGetTime() - dat.m_secLoaded;
		// 아무도 참조하지 않고 로딩한지 x분이 넘었으면 삭제
		// 		if( pSprDat->GetnRefCnt() == 0 && secPass > 60 * 3 ) {
		const auto refCnt = spDat.use_count();
// 		if( pSprDat->GetnRefCnt() == 0 && dat.IsOverTime() ) {
		if( refCnt == 1 && spDat->IsOverTime() ) {
			auto itorMap = m_mapTable.find( spDat->m_strKey );
			XASSERT( itorMap != m_mapTable.end() );
			s_sizeTotalVM -= pSprDat->GetSizeByte();
//			SAFE_DELETE( pSprDat );
			m_listSprDat.erase( itor++ );
		} else {
			++itor;
		}
	}
}

void XSprMng::Release( XSprDat *pSprDat )
{
	XAUTO_LOCK2( m_spLock );
	//
	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
		auto& spDat = (*itor);
		auto p = spDat->m_pSprDat;
		if( p && p->GetsnDat() == pSprDat->GetsnDat() ) {
			m_listSprDat.erase( itor++ );		// 맵에도 있으므로 파괴되지 않는다.
// 			p->DecRefCnt();
// 			XBREAK( p->GetnRefCnt() < 0 );
//			// 레퍼런스 카운트를 떨어트리고 일단은 지우지 않는다.
//			++itor;
			return;
		} else
			itor++;
	}
	// refCnt==0인것중 오래된파일은 삭제한다.
	DestroyOverTimeFile();
}

/**
@brief 아무도 참조하고 있지는 않지만 캐쉬타임에 걸려 아직 파괴되지 않은 리스스들을 모두 날린다.
스프라이트는 캐시타임을 쓰지 않으므로 해당없음. 그냥 코딩만 해둠.
*/
void XSprMng::DoFlushCache()
{
	XAUTO_LOCK2( m_spLock );
	for( auto itor = m_mapTable.begin(); itor != m_mapTable.end(); ) {
		auto& spDat = (*itor).second;
		const auto cntRef = spDat.use_count();
		// map에만 남아있는 자원을 최종 파괴한다.
		if( cntRef == 1 ) {
// 			SAFE_DELETE( spDat->m_pSprDat );
			m_mapTable.erase( itor++ );
		} else {
			++itor;
		}
	}
	
// 	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
// 		auto& spDat = (*itor);
// 		if( spDat->pSprDat->GetnRefCnt() == 0 ) {
// 			s_sizeTotalVM -= spDat.pSprDat->GetSizeByte();
// 			SAFE_DELETE( spDat.pSprDat );
// 			m_listSprDat.erase( itor++ );
// 		} else
// 			++itor;
// 	}
}

xSpr::XSPDat XSprMng::FindByKey( LPCTSTR szFilename, BOOL bSrcKeep, const XE::xHSL& hsl )
{
	XAUTO_LOCK2( m_spLock );
	Release까지는 문제없지만 문제는 재 검색이다.
	캐시(맵)에 남아있는 자원을 검색하려면 맵을 순회해야하는데 느리지 않을까?
	최적화 방법은 있을듯 하다 최적화는 나중에.
	for( auto& itor : m_mapTable ) {
		auto& spDat = itor.second;
		auto pSprDat = spDat->m_pSprDat;
		if( _tcsicmp( spDat->m_strKey.c_str(), szFilename ) == 0 ) {
#ifdef _XASYNC_SPR
			if( pSprDat == nullptr )
				return spDat;
#endif // _XASYNC_SPR
			if( pSprDat->GetbKeepSrc() == TRUE )
				return spDat;
			// bSrcKeep이 true인데 pSprDat가 false라면 새로 만든다.
			if( pSprDat->GetbKeepSrc() == bSrcKeep )		// 만약 이미 로딩한 파일이 있으나 메모리 보존상황이 다르면 새로 생성하도록 한다
				return spDat;		// 가급적 이쪽으로 오지 않게끔 로딩을 배치하자
		}
	}
	return nullptr;
}

/**
 @brief 비동기 로딩중인 spr을 얻는다.
*/
xSpr::XSPDat XSprMng::FindByidAsync( ID idAsync )
{
	XAUTO_LOCK2( m_spLock );
	return m_listSprDat.FindIf( [ idAsync ]( XSPDat& spDat )->bool {
		return spDat->m_idAsync == idAsync;
	});
}

void XSprMng::CompleteAsyncLoad( ID idAsync )
{
	auto spDat = FindByidAsync( idAsync );
	if( XASSERT(spDat) ) {
		XBREAK( spDat->m_pSprDat == nullptr );
		spDat->m_idAsync = 0;
	}
}
/**
@brief .spr파 일을 읽어 XSprDat* 객체로 만든다.
@param bAsyncLoad 파일을 비동기로 읽는다
@param pOutidAsync 비동기 로딩을 하게 되면 비동기로딩정보의 아이디가 담긴다.
*/
XSprDat *XSprMng::Load( LPCTSTR szFilename,
												const XE::xHSL& /*hsl*/,
												bool bUseAtlas,
//												BOOL bAddRefCnt,
												BOOL bSrcKeep,
												bool bAsyncLoad,
												ID* pOutidAsync )
{
	XAUTO_LOCK2( m_spLock );
	XE::xHSL hsl;
	_tstring strFile = szFilename;
	// HSL맵을 먼저 검색한다.
	const _tstring strKey = XE::GetFileName( szFilename );			// 파일명을 키로 사용함.
	const XE::xHSLFile* pHslFile = XHSLMap::sGet()->GetInfo( strKey );
	if( pHslFile ) {
		// treant -> treant2.spr
		strFile = pHslFile->m_strFile;
		hsl = pHslFile->m_HSL;
	} else {
	}
	{
		// 리스트에 szFilename으로 생성된게 있는지 찾는다.
		auto spDat = FindByKey( szFilename, bSrcKeep, hsl );
		XSprDat* pSprDat = nullptr;
		if( spDat ) {
			pSprDat = spDat->m_pSprDat;
			if( pSprDat == nullptr && spDat->m_idAsync )
				*pOutidAsync = spDat->m_idAsync;
			spDat->m_secLoaded = XTimer2::sGetTime();
			DestroyOverTimeFile();			// 여기다 쓰지말고 씬로딩완료 직후에 한번 불러주는게 더 효율적일듯.
			return pSprDat;
		}
	}
#ifdef _XASYNC_SPR
	// 로딩을 요청받으면 리스트에만 받아둔다.
	xAsync asyncSpr;
	asyncSpr.m_strFilename = szFilename;		// 가상 파일명(treant.spr)
	asyncSpr.m_strLoadFile = strFile;	// 실제 파일명(treant2.spr)
	asyncSpr.m_HSL = hsl;
	asyncSpr.m_bSrcKeep = bSrcKeep;
	asyncSpr.m_bUseAtlas = bUseAtlas;
	{
		XAUTO_LOCK2( m_spLock );
		m_listAsync.Add( asyncSpr );
		XBREAK( pOutidAsync == nullptr );
		*pOutidAsync = asyncSpr.m_idAsync;
		auto spDat = AddNew( szFilename, nullptr, hsl );
		if( spDat ) {
			spDat->m_idAsync = asyncSpr.m_idAsync;
		}
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
		AddNew( _tstring( szFilename ), pSprDat, hsl );
// 		if( bAddRefCnt )
// 			pSprDat->AddRefCnt();
		DestroyOverTimeFile();
		return pSprDat;
	}
	// failed
	SAFE_DELETE( pSprDat );
	return nullptr;
#endif // not _XASYNC_SPR
} // Load

xSpr::XSPDat XSprMng::AddNew( const _tstring& strKey, XSprDat *pSprDat, const XE::xHSL& hsl )
{
	auto spDat = std::make_shared<xDat>( strKey, pSprDat, hsl );
	spDat->m_secLoaded = XTimer2::sGetTime();
#ifdef _DEBUG
	for( auto& spDat : m_listSprDat ) {
		XBREAK( spDat->m_strKey == strKey );
	}
#endif // _DEBUG
	m_listSprDat.push_back( spDat );
//	m_listCache.push_back( spDat );
	m_mapTable[strKey] = spDat;
	return spDat;
}

void XSprMng::RestoreDevice()
{
	XAUTO_LOCK2( m_spLock );
	DoFlushCache();
	for( auto& spDat : m_listSprDat ) {
		XSprDat *pSprDat = spDat->m_pSprDat;
		XSPR_TRACE( "XSprMng::RestoreDevice(): %s", pSprDat->GetszFilename() );
#if defined(_DEBUG) && defined(_CHEAT)
		if( pSprDat ) {
			CONSOLE( "restore spr:[%s]", pSprDat->GetszFilename() );
		}
#endif // _DEBUG
//		if( pSprDat->GetnRefCnt() != 0 )		// refcnt가 0인것은 restore하지 않음.
//		if( spDat.use_count() == 
			pSprDat->RestoreDevice();
	}
}

#ifdef WIN32
void XSprMng::Reload()
{
	XAUTO_LOCK2( m_spLock );
	DoFlushCache();
	for( auto& spDat : m_listSprDat ) {
		spDat->m_pSprDat->Reload();
	}
}

#endif // WIN32

void XSprMng::OnPause()
{
	DoFlushCache();
	for( auto& spDat : m_listSprDat ) {
		spDat->m_pSprDat->DestroyDevice();
	}
}

#ifdef _XASYNC_SPR
#pragma message("ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ")


// void XSprMng::AsyncSetAction( ID idAsync, ID idAct, xRPT_TYPE playType )
// {
// 	XAUTO_LOCK2( m_spLock );
// 	auto pAsyncInfo = SPRMNG->GetpAsyncInfo( idAsync );
// 	if( pAsyncInfo ) {
// 		// 파일 로딩이 완료되면 setAction을 작동한다.
// 		pAsyncInfo->m_idAct = idAct;
// 		pAsyncInfo->m_playType = playType;
// 	}
// }

// XSprMng::xAsync* XSprMng::GetpAsyncInfo( ID idAsync )
// {
// 	XAUTO_LOCK2( m_spLock );
// 	for( auto& async : m_listAsync ) {
// 		if( async.m_idAsync == idAsync ) {
// 			return &async;
// 		}
// 	}
// 	return nullptr;
// }

/**
 @brief 비동기 로딩이 완료된 pSprDat를 찾는다.
*/
// const XSprMng::xAsync* XSprMng::GetpAsyncComplete( ID idAsync )
// {
// 	XAUTO_LOCK2( m_spLock );
// 	for( auto& async : m_listAsyncComplete ) {
// 		if( async.m_idAsync == idAsync ) {
// 			return &async;
// 		}
// 	}
// 	return nullptr;
// }

// void XSprMng::DeleteAsyncComplete( ID idAsync )
// {
// 	XAUTO_LOCK2( m_spLock );
// 	for( auto itor = m_listAsyncComplete.begin(); itor != m_listAsyncComplete.end(); ) {
// 		const auto& async = (*itor);
// 		m_listAsyncComplete.erase( itor++ );
// 	}
// }

void XSprMng::Process()
{
	XAUTO_LOCK2( m_spLock );
	// 파일로부터 비동기로딩이 끝나면 디바이스 버퍼를 생성시킨다.
	for( auto itor = m_listAsyncComplete.begin(); itor != m_listAsyncComplete.end(); ) {
		const auto& async = (*itor);
		XBREAK( async.m_pSprDat == nullptr );
		async.m_pSprDat->CreateDevice();		// 버텍스버퍼, 텍스쳐등을 생성시킨다.
		auto spDat = FindByidAsync( async.m_idAsync );
		if( spDat ) {
			spDat->m_pSprDat = async.m_pSprDat;
		}
// 		XSprObj* pSprObj = async.m_pSprObj;
// 		pSprObj->OnCompleteAsyncLoad( async.m_pSprDat );
// 		if( async.m_idAct ) {
// 			pSprObj->SetAction( async.m_idAct, async.m_playType );
//		}
		m_listAsyncComplete.erase( itor++ );
		break;		// 한프레임에 하나씩만 처리하도록.
	}
}

unsigned int __stdcall XSprMng::_WorkThread( void *param )
{
	SPRMNG->WorkThread();
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void XSprMng::WorkThread()
{
	while( 1 ) {
		xAsync asyncLoad;
		while( 1 ) {
			{
				XAUTO_LOCK2( m_spLock );
				if( m_listAsync.size() > 0 ) {
					// 앞에거 하나 빼냄
					asyncLoad = m_listAsync.front();
					m_listAsync.pop_front();
					break;
				}
			}
			Sleep( 100 );
		} // while
			//
		{
			auto pSprDat = new XSprDat;
			//			XSprite::sSetHSL( asyncLoad.m_HSL );
			const bool bAsyncLoad = true;
			// 비동기로딩모드로 읽어서 파일데이터를 메모리에만 올린다.
			XTrace( _T( "load %s........" ), asyncLoad.m_strFilename.c_str() );
			auto bOk = pSprDat->Load( asyncLoad.m_strLoadFile.c_str(),
																asyncLoad.m_bUseAtlas,
																bAsyncLoad,
																asyncLoad.m_HSL,
																asyncLoad.m_bSrcKeep );
			if( bOk ) {
				XAUTO_LOCK2( m_spLock );
				XTRACE( "complete\n", asyncLoad.m_strFilename.c_str() );
				auto spDat = FindByKey( asyncLoad.m_strFilename.c_str(), asyncLoad.m_bSrcKeep, asyncLoad.m_HSL );
				XBREAK( spDat == nullptr );
				asyncLoad.m_pSprDat = pSprDat;
				m_listAsyncComplete.push_back( asyncLoad );
			} else {
				XTrace( _T( "failed\n" ), asyncLoad.m_strFilename.c_str() );
				// failed
				SAFE_DELETE( pSprDat );
			}
		}
		Sleep( 100 );
	} // while

}

#endif // _XASYNC_SPR







#endif // not _XASYNC_SPR
