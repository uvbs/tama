#include "stdafx.h"
#include "Sprite.h"
#ifdef _XASYNC_SPR
#include "SprMng.h"
#include "VersionXE.h"
#include "SprDat.h"
#include "XHSLMap.h"
#include "XSystem.h"
#include "opengl2/XTextureAtlas.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xSpr;

XSprMng *SPRMNG = NULL;
//int XSprMng::s_sizeTotalVM = 0;
xSec XSprMng::s_secCache = 180;
static volatile bool s_bThread = true;

XE_NAMESPACE_START( xSpr )
//
// struct Deleter {
// 	void operator()( xDat* p ) {
// 		auto spLock = SPRMNG->FindSprDatByidAsync( 1 );
// 		XTRACE("1");
// 	}
// };


//
XE_NAMESPACE_END; // xSpr


inline ID xSpr::xDat::getid() {
	return ( m_pSprDat ) ? m_pSprDat->GetsnDat() : 0;
}
bool xSpr::xDat::IsOverTime() const {
	return XTimer2::sGetTime() - m_secLoaded > XSprMng::s_secCache;
}

xSpr::xDat::~xDat() {
	SAFE_DELETE( m_pSprDat );
}
LPCTSTR xDat::GetszFilename() const {
	return (m_pSprDat) ? m_pSprDat->GetszFilename() : _T( "" );
}

const XActDat* xDat::GetAction( ID idAct ) const {
	return (m_pSprDat) ? m_pSprDat->GetAction( idAct ) : nullptr;
}

int xDat::GetnNumActions() const {
	return (m_pSprDat) ? m_pSprDat->GetnNumActions() : 0;
}

const XActDat* xDat::GetActionIndex( int index ) const {
	return (m_pSprDat) ? m_pSprDat->GetActionIndex( index ) : nullptr;
}

const XSprite* xDat::GetSprite( int nSpr ) const {
	return (m_pSprDat) ? m_pSprDat->GetSprite( nSpr ) : nullptr;
}

XSprite* xDat::GetSpriteMutable( int nSpr ) {
	return (m_pSprDat) ? m_pSprDat->GetSpriteMutable( nSpr ) : nullptr;
}


//////////////////////////////////////////////////////////////////////////
XSprMng::xAsync::xAsync() {
	m_idAsync = XE::GenerateID();
}

//////////////////////////////////////////////////////////////////////////
static void _WorkThread( void *param )
{
	SPRMNG->WorkThread();
}

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
	// 스레드부터 없애서 스레드간 충돌을 막는다.
	DestroyThread();
	//
	m_listAsync.clear();
	m_listAsyncComplete.clear();
	// 캐시에 있는거 다 날림.
	DoFlushCache();

	// 매니저가 최종 삭제될때까지 release되지 않은 객체가 있으면 경고알려줌
#ifdef _XDEBUG
	for( auto itor = m_mapDat.begin(); itor != m_mapDat.end(); ) {
		auto& spDat = itor->second;
		const auto useCnt = spDat.use_count();
		if( useCnt > 1 ) {
			XALERT( "SprMng: 해제되지 않은 spr 발견. %s"
							, spDat->m_pSprDat->GetszFilename() );
		}
	}
#endif // _XDEBUG
	m_mapDat.clear();		// 최종삭제
	s_pInstance = nullptr;
}

void XSprMng::DestroyThread()
{
	{
//		XAUTO_LOCK2( m_spLock );
		s_bThread = false;
		for( auto& spThread : m_aryThread )
			spThread->join();
		m_aryThread.clear();
	}
}

void XSprMng::CreateThreadx()
{
	s_bThread = true;
	m_spLock = std::make_shared<XLock>();
	//
	for( int i = 0; i < 2; ++i )
		m_aryThread.push_back( std::make_shared<std::thread>( _WorkThread, nullptr ) );
}
void XSprMng::OnCreate()
{
	CreateThreadx();
}

/**
@brief 레퍼런스 카운트가 1이된것들중 시간이 오래된건 삭제시킨다.
*/
void XSprMng::DestroyOverTimeFile()
{
	XAUTO_LOCK2( m_spLock );
	//
	for( auto itor = m_mapDat.begin(); itor != m_mapDat.end(); ) {
		auto& spDat = (*itor).second;
		auto pSprDat = spDat->m_pSprDat;
		const auto useCnt = spDat.use_count();
		if( useCnt == 1 ) {
			if( spDat->IsOverTime() ) {
				m_mapDat.erase( itor++ );
				continue;
			}
		}
		++itor;
	} // for
}

/**
 @brief 참조되고 있던 자원을 해제시킨다.
 해제된 자원은 캐시맵으로 들어가 일정시간후에 파괴된다.
 만약 그전에 그 자원에 대한 요청이 있다면 캐시에서 빠지고 다시 리스트에 올라간다.
*/
void XSprMng::Release( const XSPDat& spDatRelease )
{
	ReleaseByID( spDatRelease->m_idDat );
// 	XAUTO_LOCK2( m_spLock );
// 	// 캐시에 있는것 중 오래된파일은 삭제한다.
// 	DestroyOlderFile();
// 	//
// 	for( auto& itor = m_mapDat.begin(); itor != m_mapDat.end(); ) {
// 		auto& spDat = itor->second;
// 		auto pSprDat = spDat->m_pSprDat;
// 		if( pSprDat && spDat->m_idDat == spDatRelease->m_idDat ) {
// 			const int useCnt = spDat.use_count();
// 			if( useCnt == 2 ) {
// 				// 캐시로 이동시켜 파괴되지 않도록 한다.
// #ifdef _XDEBUG
// 				auto itorCache = m_mapCache.find( spDat->m_strKey );
// 				if( XASSERT( itorCache == m_mapCache.end() ) )
// #endif // _XDEBUG
// 					m_mapCache[spDat->m_strKey] = spDat;
// 				// 참조맵에선 삭제
// 				m_mapDat.erase( itor++ );
// //				XBREAK( spDat.use_count() != 2 );
// 			}
// 			return;
// 		} else
// 			itor++;
// 	}
}
void XSprMng::ReleaseByID( ID snDat )
{
// 	XAUTO_LOCK2( m_spLock );
// 	// 캐시에 있는것 중 오래된파일은 삭제한다.
// 	DestroyOlderFile();
// 	//
// 	for( auto& itor = m_mapDat.begin(); itor != m_mapDat.end(); ) {
// 		auto& spDat = itor->second;
// 		auto pSprDat = spDat->m_pSprDat;
// 		if( pSprDat && spDat->m_idDat == snDat ) {
// 			const int useCnt = spDat.use_count();
// 			if( useCnt == 1 ) {
// 				// 캐시로 이동시켜 파괴되지 않도록 한다.
// #ifdef _XDEBUG
// 				auto itorCache = m_mapCache.find( spDat->m_strKey );
// 				if( XASSERT( itorCache == m_mapCache.end() ) )
// #endif // _XDEBUG
// 					m_mapCache[spDat->m_strKey] = spDat;
// 				// 참조맵에선 삭제
// 				m_mapDat.erase( itor++ );
// 				//				XBREAK( spDat.use_count() != 2 );
// 			}
// 			return;
// 		} else
// 			itor++;
// 	}
}
// void XSprMng::Release( XSprDat *pSprDat )
// {
// 	XAUTO_LOCK2( m_spLock );
// 	// 캐시에 있는것 중 오래된파일은 삭제한다.
// 	DestroyOlderFile();
// 	//
// 	for( auto itor = m_mapDat.begin(); itor != m_mapDat.end(); ) {
// 		auto spDat = itor->second;
// 		auto pSprDat = spDat->m_pSprDat;
// 		if( pSprDat && pSprDat->GetsnDat() == pSprDat->GetsnDat() ) {
// 			// 캐시로 이동시켜 파괴되지 않도록 한다.
// #ifdef _XDEBUG
// 			auto itorCache = m_mapCache.find( spDat->m_strKey );
// 			if( XASSERT(itorCache == m_mapCache.end()) )
// #endif // _XDEBUG
// 				m_mapCache[ spDat->m_strKey ] = spDat;
// 			// 참조맵에선 삭제
// 			m_mapDat.erase( itor++ );
// 			XBREAK( spDat.use_count() != 2 );
// 			return;
// 		} else
// 			itor++;
// 	}
// }

/**
@brief 아무도 참조하고 있지는 않지만 캐쉬타임에 걸려 아직 파괴되지 않은 리스스들을 모두 날린다.
스프라이트는 캐시타임을 쓰지 않으므로 해당없음. 그냥 코딩만 해둠.
*/
void XSprMng::DoFlushCache()
{
	XAUTO_LOCK2( m_spLock );
	for( auto itor = m_mapDat.begin(); itor != m_mapDat.end(); ) {
		auto& spDat = (*itor).second;
		auto pSprDat = spDat->m_pSprDat;
		const int useCnt = spDat.use_count();
		if( useCnt == 1 ) {
			SAFE_DELETE( spDat->m_pSprDat );
			m_mapDat.erase( itor++ );
			continue;
		}
		++itor;
	}
}

/**
 @brief 비동기 로딩중인 spr을 얻는다.
*/
xSpr::XSPDat XSprMng::FindByidAsync( ID idAsync )
{
	XAUTO_LOCK2( m_spLock );
	for( auto itor : m_mapDat ) {
		const XSPDat spDat = itor.second;
		if( spDat->m_idAsync == idAsync )
			return spDat;
	}
	return nullptr;
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
XSPDat XSprMng::Load( LPCTSTR szFileKey,
											const XE::xHSL& /*hsl*/,
											bool bUseAtlas,
											BOOL bSrcKeep,
											bool bAsyncLoad,
											bool bBatch,
											ID* pOutidAsync )
{
	// 로딩전에 아무도 참조하고 있지 않은 "오래된" 자원을 해제시킨다.
	DestroyOverTimeFile();
	//
	XE::xHSL hsl;
	_tstring strFile = szFileKey;
	// HSL맵을 먼저 검색한다.
	const _tstring strKey = XE::GetFileName( szFileKey );			// 파일명을 키로 사용함.
	const XE::xHSLFile* pHslFile = XHSLMap::sGet()->GetInfo( strKey );
	if( pHslFile ) {
		// treant -> treant2.spr
		strFile = pHslFile->m_strFile;
		hsl = pHslFile->m_HSL;
	}
	// 이미 로딩된것이 있는지 찾는다.
	auto pDat = DoFindExist( szFileKey, pOutidAsync );
	if( pDat ) {
		return pDat;
	}
	if( bAsyncLoad ) {
		// 이미 로딩된게 없으면 비동기 로딩을 준비한다
		return LoadAsync( szFileKey, strFile, hsl, bUseAtlas, 
											bSrcKeep, bBatch, false, nullptr, pOutidAsync );
	} else {
		const bool bRestore = false;
		return LoadSync( szFileKey, strFile, hsl, bUseAtlas, 
										 bSrcKeep, bBatch, bRestore );
	}
} // Load

/**
 @brief 동기 로딩
*/
xSpr::XSPDat XSprMng::LoadSync( LPCTSTR szFileKey,
																const _tstring& strFile,
																const XE::xHSL& hsl,
																bool bUseAtlas,
																BOOL bSrcKeep,
																bool bBatch,
																bool bRestore )
{
	auto pSprDat = new XSprDat;
	m_pNowLoad = pSprDat;
	XTrace( _T( "loadSync %s........" ), szFileKey );
	auto bOk = pSprDat->Load( strFile.c_str(),
														bUseAtlas,
														false,		// bAsync
														hsl,
														bSrcKeep,
														bRestore,
														bBatch );
	m_pNowLoad = nullptr;
	if( bOk ) {
		XTRACE( "completed\n" );
		return AddNew( szFileKey, strFile, pSprDat, hsl, bUseAtlas, bBatch );
	} else {
		XTrace( _T( "failed\n" ) );
		// failed
		SAFE_DELETE( pSprDat );
	}
	return nullptr;
}

/**
 @brief 비동기 로딩
*/
xSpr::XSPDat XSprMng::LoadAsync( LPCTSTR szFileKey, 
																 const _tstring& strFile,
																 const XE::xHSL& hsl,
																 bool bUseAtlas,
																 BOOL bSrcKeep,
																 bool bBatch,
																 bool bRestore,
																 XSprDat* pSprDat,
																 ID* pOutidAsync )
{
	// 이미 로딩된게 없으면 비동기 로딩을 준비한다
	xAsync asyncSpr;
	asyncSpr.m_strFilename = szFileKey;		// 가상 파일명(treant.spr)
	asyncSpr.m_strLoadFile = strFile;	// 실제 파일명(treant2.spr)
	XBREAK( strFile.empty() );
	asyncSpr.m_HSL = hsl;
	asyncSpr.m_bSrcKeep = bSrcKeep;
	asyncSpr.m_bRestore = bRestore;
	asyncSpr.m_bUseAtlas = bUseAtlas;
	asyncSpr.m_bBatch = bBatch;
	asyncSpr.m_spAtlasMng = XTextureAtlas::sGetspCurrMng();
	asyncSpr.m_pSprDat = pSprDat;
	//
	XAUTO_LOCK2( m_spLock );
	// 스레드에서 하나씩 로딩을 시킴.
	m_listAsync.Add( asyncSpr );
	XBREAK( pOutidAsync == nullptr );
	*pOutidAsync = asyncSpr.m_idAsync;
	XSPDat spDat;
	if( !bRestore ) {
		spDat = AddNew( szFileKey, strFile, nullptr, hsl, bUseAtlas, bBatch );
		if( spDat ) {
			spDat->m_idAsync = asyncSpr.m_idAsync;
		}
	}
	return spDat;
}

void XSprMng::LoadAsync( XSPDat spDat, bool bRestore )
{
	LoadAsync( spDat->m_strKey.c_str(),
						 spDat->m_strLoadFile,
						 spDat->m_HSL,
						 spDat->m_bUseAtlas,
						 FALSE,
						 spDat->m_bBatch,
						 bRestore,
						 spDat->m_pSprDat,
						 &spDat->m_idAsync );
}

/**
@brief spr을 로딩하기전에 이미 참조되고 있다면 그것을 사용한다.
@note 키이름은 같지만 아틀라스 여부가 다를수 있다. 하지만 이제 비 아틀라스 spr은 쓰지 않을 예정이므로 아틀라스는 모두 true라고 가정한다.
*/
xSpr::XSPDat XSprMng::FindByKey( LPCTSTR szFilename )
{
	XAUTO_LOCK2( m_spLock );

	// 일단 카리브에선 bKeepSrc를 쓰지 않으니 검색에서 무시함.
	TCHAR szLwr[256];
	_tcscpy_s( szLwr, szFilename );
	_tcslwr_s( szLwr );
	auto itor = m_mapDat.find( szLwr );
	if( itor != m_mapDat.end() ) {
		return itor->second;
	}
	return nullptr;
}

xSpr::XSPDat XSprMng::DoFindExist( const _tstring& strFileKey, ID* pOutidAsync )
{
	XAUTO_LOCK2( m_spLock );
	bool bExistAtCache = false;
	// 리스트에 szFilename으로 생성된게 있는지 찾는다.
	auto spDat = FindByKey( strFileKey.c_str() );
	if( spDat == nullptr ) {
		return nullptr;
	}
	if( spDat->m_idAsync ) {
		*pOutidAsync = spDat->m_idAsync;
	}
	spDat->m_secLoaded = XTimer2::sGetTime();		// 캐시에있던것을 다시로드해서 시간 갱신
	*pOutidAsync = spDat->m_idAsync;
	return spDat;
}

xSpr::XSPDat XSprMng::AddNew( const _tstring& strKey, 
															const _tstring& strFile, 
															XSprDat *pSprDat, 
															const XE::xHSL& hsl,
															bool bUseAtlas,
															bool bBatch )
{
	TCHAR szLwrKey[256];
	_tcscpy_s( szLwrKey, strKey.c_str() );
	_tcslwr_s( szLwrKey );
	auto pDat = std::make_shared<xDat>( strKey, 
																			 strFile, 
																			 pSprDat, 
																			 hsl, 
																			 bBatch,
																			 bUseAtlas);
	pDat->m_secLoaded = XTimer2::sGetTime();
#ifdef _DEBUG
	XBREAK( (FindByKey( szLwrKey ) != nullptr) );
#endif // _DEBUG
	{
		XAUTO_LOCK2( m_spLock );
		m_mapDat[_tstring( szLwrKey )] = pDat;
	}
	return pDat;
}

void XSprMng::RestoreDevice()
{
	CreateThreadx();
	//
	XAUTO_LOCK2( m_spLock );
	// 캐시에 있는건 다 날림
	DoFlushCache();
	// 
	const bool bRestore = true;
	for( auto itor : m_mapDat ) {
		auto& spDat = itor.second;
		XSprDat *pSprDat = spDat->m_pSprDat;
		XSPR_TRACE( "XSprMng::RestoreDevice(): %s", pSprDat->GetszFilename() );
#if defined(_DEBUG) && defined(_CHEAT)
		if( pSprDat ) {
			CONSOLE( "restore spr:[%s]", pSprDat->GetszFilename() );
		}
#endif // _DEBUG
#pragma message("이것도 스레드로 읽어야 할듯.")
		pSprDat->RestoreDevice();
		// restore모드로 비동기 로딩
		LoadAsync( spDat, bRestore );
	}
}

#ifdef WIN32
void XSprMng::Reload()
{
	XAUTO_LOCK2( m_spLock );
	DoFlushCache();
	for( auto itor : m_mapDat ) {
		auto& spDat = itor.second;
		spDat->m_pSprDat->Reload();
	}
}

#endif // WIN32

// 스맛폰 홈으로 전환
void XSprMng::OnPause()
{
	DoFlushCache();
	for( auto itor : m_mapDat ) {
		auto& spDat = itor.second;
		spDat->m_pSprDat->DestroyDevice();
	}
	DestroyThread();
}

#pragma message("ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ASYNC_SPR ")

void XSprMng::Process()
{
	XAUTO_LOCK2( m_spLock );
	// 파일로부터 비동기로딩이 끝나면 디바이스 버퍼를 생성시킨다.
	for( auto itor = m_listAsyncComplete.begin(); itor != m_listAsyncComplete.end(); ) {
		const auto& async = (*itor);
		XBREAK( async.m_pSprDat == nullptr );
// 		SET_ATLASES( async.m_pAtlasMng ) {
// 			async.m_pSprDat->CreateDevice();		// 버텍스버퍼, 텍스쳐등을 생성시킨다.
// 		} END_ATLASES;
		auto spDat = FindByidAsync( async.m_idAsync );
		if( spDat ) {
			spDat->m_pSprDat = async.m_pSprDat;
			spDat->m_idAsync = 0;
//			SET_ATLASES( async.m_pAtlasMng ) {
			async.m_spAtlasMng->PushAtlasMng();
			async.m_pSprDat->CreateDevice();		// 버텍스버퍼, 텍스쳐등을 생성시킨다.
			async.m_spAtlasMng->PopAtlasMng();
//			} END_ATLASES;
		}
		m_listAsyncComplete.erase( itor++ );
		break;		// 한프레임에 하나씩만 처리하도록.
	}
}

/**
 @brief 아틀라스 idTex의 크기변경으로 이 텍스쳐를 사용하는 모든 서피스의 uv를 갱신한다.
*/
void XSprMng::UpdateUV( ID idTex, 
												const XE::POINT& sizePrev, 
												const XE::POINT& sizeNew )
{
	XAUTO_LOCK2( m_spLock );
	for( auto itor : m_mapDat ) {
		xSpr::XSPDat spDat = itor.second;
		if( spDat->m_pSprDat ) {
			spDat->m_pSprDat->UpdateUV( idTex, sizePrev, sizeNew );
		}
	}
	if( m_pNowLoad ) {
		m_pNowLoad->UpdateUV( idTex, sizePrev, sizeNew );
	}
}

//////////////////////////////////////////////////////////////////////////
void XSprMng::WorkThread()
{
	while( s_bThread ) {
		xAsync asyncLoad;
		while( s_bThread ) {
			{
				XAUTO_LOCK2( m_spLock );
				if( m_listAsync.size() > 0 ) {
					// 앞에거 하나 빼냄
					asyncLoad = m_listAsync.front();
					m_listAsync.pop_front();
					break;
				}
			}
			std::this_thread::sleep_for( std::chrono::milliseconds(1) );
			if( !s_bThread )
				return;
		} // while
			//
		{
			XSprDat* pSprDat = nullptr;
			if( asyncLoad.m_pSprDat == nullptr )
				pSprDat = new XSprDat;
			else
				pSprDat = asyncLoad.m_pSprDat;
			const bool bAsyncLoad = true;
			// 비동기로딩모드로 읽어서 파일데이터를 메모리에만 올린다.
			XTrace( _T( "load %s........" ), asyncLoad.m_strFilename.c_str() );
 			auto bOk = pSprDat->Load( asyncLoad.m_strLoadFile.c_str(),
																asyncLoad.m_bUseAtlas,
																bAsyncLoad,
																asyncLoad.m_HSL,
																asyncLoad.m_bSrcKeep,
																asyncLoad.m_bRestore,
																asyncLoad.m_bBatch );
			if( bOk ) {
				XAUTO_LOCK2( m_spLock );
				if( !asyncLoad.m_bRestore ) {
					XTRACE( "complete\n", asyncLoad.m_strFilename.c_str() );
					auto spDat = FindByKey( asyncLoad.m_strFilename.c_str() );
					XBREAK( spDat == nullptr );
					asyncLoad.m_pSprDat = pSprDat;
				}
				// 로딩완료 리스트
				m_listAsyncComplete.push_back( asyncLoad );
			} else {
				XTrace( _T( "failed\n" ), asyncLoad.m_strFilename.c_str() );
				// failed
				SAFE_DELETE( pSprDat );
			}
		}
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	} // while

}








#endif // _XASYNC_SPR
