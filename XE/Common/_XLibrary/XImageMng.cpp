#include "stdafx.h"
#include "XImageMng.h"
#include "etc/XSurface.h"
#include "etc/xGraphics.h"
#include "etc/XTimer2.h"
#include "etc/xLang.h"
#include "XFramework/client/XClientMain.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// #if !defined(WIN32) && defined(_CHEAT)
// #define XLOGP(F,...)		\
// 	if( XAPP->m_bDebugMode ) {\
// 		__xLog( XLOGTYPE_LOG, _T(F), ##__VA_ARGS__ ); \
// 	}
// #else
// #define XLOGP(F,...)		(0)
// #endif

#define _XIMAGEMNG_DEBUG

#ifdef _XIMAGEMNG_DEBUG
#define _IMLOGXN	XTRACE
#else
#define _IMLOGXN	{}
#endif

XImageMng *IMAGE_MNG = NULL;
int XImageMng::s_sizeTotalVMem = 0;

void XImageMng::Destroy()
{
	DoFlushCache();
	//
	CheckRelease();
//	XLIST4_DESTROY(m_listSurface );
	for( auto img : m_listSurface ) {
		if( img.m_pSurface )
			s_sizeTotalVMem -= img.m_pSurface->GetSizeByte();
		SAFE_DELETE( img.m_pSurface );
	}
}

XImageMng::XImageMng( int max )
{
	Init();
}

void XImageMng::CheckRelease( void )
{
	for( auto& img : m_listSurface ) {
		if( img.m_pSurface->GetnRefCnt() > 0 ) {
#ifdef _DEBUG
			XALERT( "XImageMng: 해제되지 않은 이미지 발견. %s", img.m_strRes.c_str() );
#else
			XLOGXN( "XImageMng: 해제되지 않은 이미지 발견. %s", img.m_strRes.c_str() );
#endif
		}
	}
}

void XImageMng::Release( XSurface* pSurface )
{
	XBREAK( pSurface == nullptr );
	XBREAK( pSurface->GetnRefCnt() == 0 );
	// 레퍼런스카운트를 떨어트리고 일단은 지우지 않는다.
	pSurface->DecRefCnt();
	// 어떤이미지를 릴리즈 할때 오래된파일삭제루틴을 한번 돌려준다.
	DestroyOlderFile();
}

/**
 @brief 레퍼런스카운트가 0이된것들중 시간이 오래된건 삭제시킨다.
*/
void XImageMng::DestroyOlderFile()
{
	for( auto itor = m_listSurface.begin(); itor != m_listSurface.end(); ) {
		auto& img = ( *itor );
			xSec secPass = XTimer2::sGetTime() - img.m_secLoaded;
			// 아무곳에도 사용하지 않고 시간이 오래된건 지운다.
			if( img.m_pSurface->GetnRefCnt() == 0 && secPass > 60 * 5 ) {
	//		if( img.m_pSurface->GetnRefCnt() == 0 && secPass > 60 ) {
	//			CONSOLE("%s destroyed.......", img.m_strRes.c_str() );
				s_sizeTotalVMem -= img.m_pSurface->GetSizeByte();
				SAFE_DELETE( img.m_pSurface );
				m_listSurface.erase( itor++ );
			} else {
				++itor;
			}
	}
}

XImageMng::xImage* XImageMng::Find( XUINT64 idRes )
{
	for( auto& img : m_listSurface ) {
		if( img.m_pSurface->Getid64Res() == idRes )
			return &img;
	}
	return nullptr;
}

XImageMng::xImage* XImageMng::Find( LPCTSTR szRes )	// 로컬패스명으로 갖고있게 바뀜
{
	for( auto& img : m_listSurface ) {
// 		if( XE::IsSame( img.m_pSurface->GetstrRes().c_str(), szRes ) )
		if( XE::IsSame( img.m_strRes.c_str(), szRes ) )
			return &img;
	}
	return nullptr;
}
/**
 @brief 내부에서만 쓰는 버전
*/
XImageMng::xImage* XImageMng::Find( XSurface *pSurface )	
{
	for( auto& img : m_listSurface ) {
//		if( img.m_pSurface->GetstrRes() == pSurface->GetstrRes() )
		if( img.m_strRes == pSurface->GetstrRes() )
			return &img;
	}
	return nullptr;
}
/**
 @brief 기존코드 호환용
*/
XSurface* XImageMng::Load( BOOL bHighReso
												, LPCTSTR szRes
												, BOOL bSrcKeep/*=FALSE*/
												, BOOL bMakeMask/*=FALSE*/
												, bool bAsync /*= false*/ )
{
//	XBREAK( bHighReso == FALSE );		// 이제 이건 지원안함.
	// 전투배경같은건 bHighReso를 FALSE로 놓고 쓰는 편법을 쓰기 때문에 일단 bHighReso는 없애지 못함. 장차 bHighReso자체를 없애는 방향으로 가야할듯.
	return _Load( bHighReso != FALSE, szRes, XE::xPF_ARGB4444, xBOOLToBool(bSrcKeep), xBOOLToBool(bMakeMask), bAsync );

}
/**
 @brief 기존에 bHighReso를 FALSE로 해서 크게 렌더링하는 편법을 쓰고 있어서 bHighReso가 그대로 살아있음.
*/
XSurface* XImageMng::Load( bool bHighReso
													, LPCTSTR szRes
													, XE::xtPixelFormat format
													, bool bSrcKeep/*=false*/
													, bool bMakeMask/*=false*/
													, bool bAsync /*= false*/ )
{
	const bool bKeepSrc = (bMakeMask == true);
	return _Load( bHighReso, szRes, format, bKeepSrc, bMakeMask, bAsync );
}

/**
 @brief 내부용 로딩모듈
*/
XSurface* XImageMng::_Load( bool bHighReso
													, LPCTSTR _szRes
													, XE::xtPixelFormat format
													, bool bSrcKeep
													, bool bMakeMask
													, bool bAsync )
{
	const _tstring strRes = _szRes;
	// 이미 로딩한것인지 이미지풀에서 szRes로 찾아봄
	{
		auto pImg = Find( strRes.c_str() );
		if( pImg ) {
			// 재로딩이 되었다면 로딩한 시간을 다시 갱신시킴
			pImg->m_secLoaded = XTimer2::sGetTime();
			pImg->m_pSurface->IncRefCnt();
			DestroyOlderFile();		// 여기다 쓰지말고 씬로딩완료 직후에 한번 불러주는게 더 효율적일듯.
			return pImg->m_pSurface;
		}
	}
//	AXLOGXN("XImageMng::Load(%s)", szRes);
	if( bAsync ) {
		auto pSurface = GRAPHICS->CreateSurface();
		if( XASSERT(pSurface) ) {		// 이건 빈 객체만 만드는거기땜에 반드시 있어야 함.
			xImage img;
			img.m_strRes = strRes;
			img.m_secLoaded = XTimer2::sGetTime();		// 비동기 미로딩상태라도 시간은 넣어야 한다. 로딩되기전에 다시 모두 해제되어 필요가 없는상태면 로딩하지 말아야 하기 때문.
			img.m_pSurface = pSurface;
			img.m_bAsyncLoad = true;
			m_listSurface.Add( img );
			xAsyncLoad asyncLoad;
			asyncLoad.m_strRes = strRes;
			asyncLoad.m_bHighReso = bHighReso;
			asyncLoad.m_Format = format;
			asyncLoad.m_bSrcKeep = bSrcKeep;
			asyncLoad.m_bMakeMask = bMakeMask;
			m_listAsync.Add( asyncLoad );		// 비동기 대기열에 등록.
			pSurface->IncRefCnt();
//			s_sizeTotalVMem += pSurface->GetSizeByte();
		}
		return pSurface;
	} else {
		// 국가별 폴더에서 우선적으로 찾는다.
		_tstring strLoadTry;			// 로딩을 시도한 패스
		TCHAR szLangPath[1024];
		XE::LANG.ChangeToLangDir( strRes.c_str(), szLangPath );
		strLoadTry = szLangPath;
		auto llTime = XE::GetFreqTime();
		auto pSurface = GRAPHICS->CreateSurface( bHighReso, szLangPath, format, bSrcKeep, bMakeMask );
		auto llPass = XE::GetFreqTime() - llTime;
		if( pSurface == nullptr ) {
			llPass = 0;
			strLoadTry = strRes;
			// 없으면 루트 폴더에서 찾는다.
			llTime = XE::GetFreqTime();
			pSurface = GRAPHICS->CreateSurface( bHighReso, strRes.c_str(), format, bSrcKeep, bMakeMask );
			if( pSurface ) {
				llPass = XE::GetFreqTime() - llTime;
				pSurface->SetstrRes( strRes.c_str() );
			} else {
				CONSOLE_TAG( "img", "file open failed:%s", strRes.c_str() );
			}
		} else {
			pSurface->SetstrRes( szLangPath );
		}
		if( pSurface ) {
//		XLOGP( "%s, %llu", XE::GetFileName( szRes ), llPass );
			XBREAK( pSurface->IsHavestrRes() == FALSE );
//		_IMLOGXN( "%s...loading", pSurface->GetstrRes().c_str() );
			xImage img;
			img.m_strRes = strRes;		// 국가패스가 아닌 루트패스로 들어가고 검색시에도 루트패스로 찾아야함.
			img.m_secLoaded = XTimer2::sGetTime();
			img.m_pSurface = pSurface;
			img.m_bAsyncLoad = false;
			m_listSurface.Add( img );
			pSurface->IncRefCnt();
			s_sizeTotalVMem += pSurface->GetSizeByte();
			DestroyOlderFile();
		}
		return pSurface;
	}
}

/**
 @brief RAW이미지 데이터를 managed되는 서피스로 만든다.
 @param cKey 이미지 고유의 키
*/

XSurface* XImageMng::CreateSurface( const char* cKey
																	, const XE::POINT& sizeSurfaceOrig
																	, const XE::VEC2& vAdj
																	, XE::xtPixelFormat formatSurface								
																	, void* const pImgSrc
																	, const XE::POINT& sizeMemSrc
																	, bool bSrcKeep, bool bMakeMask ) 
{
	// 어떤 이미지를 로딩할때마다 기존에 로딩한것중에서 안쓰고 오래된것은 삭제시킨다.
	DestroyOlderFile();
	// 이미 로딩한것인지 이미지풀에서 szRes로 찾아봄
	XUINT64 idRes = XE::GetCheckSum( cKey );
	auto pImg = Find( idRes );
	if( pImg ) {
		// 재로딩이 되었다면 로딩한 시간을 다시 갱신시킴
		pImg->m_secLoaded = XTimer2::sGetTime();
		XBREAK( pImg->m_pSurface == nullptr );
		pImg->m_pSurface->IncRefCnt();
		// 디바이스자원이 없는상태면 다시 생성시킴.
		if( pImg->m_pSurface->IsEmpty() ) {
			pImg->m_pSurface->Create( sizeSurfaceOrig
														, vAdj
														, formatSurface
														, pImgSrc
														, XE::xPF_ARGB8888
														, sizeMemSrc
														, false, false );
		}
		return pImg->m_pSurface;
	}
	auto pSurface = GRAPHICS->CreateSurface( sizeSurfaceOrig
																				, vAdj
																				, formatSurface
																				, pImgSrc
																				, sizeMemSrc
																				, bSrcKeep, bMakeMask );
	if( pSurface ) {
		pSurface->Setid64Res( idRes );
		xImage img;
		img.m_strRes = C2SZ(cKey);
		img.m_secLoaded = XTimer2::sGetTime();
		img.m_pSurface = pSurface;
		img.m_bAsyncLoad = false;
		m_listSurface.Add( img );
		pSurface->IncRefCnt();
		s_sizeTotalVMem += pSurface->GetSizeByte();
		return pSurface;
	}
	return nullptr;
}

XSurface* XImageMng::CreateSurface( const char* cKey, const XE::xImage& imgInfo )
{
	return XImageMng::CreateSurface( cKey
																, imgInfo.m_ptSize / 2
																, XE::VEC2(0)
																, XE::xPF_ARGB8888
																, imgInfo.m_pImg
																, imgInfo.m_ptSize
																, false, false );
}

void XImageMng::RestoreDevice( void )
{
	for( auto& img : m_listSurface ) {
#if defined(_DEBUG) && defined(_CHEAT)
		if( img.m_pSurface ) {
			// restore시에는 surface안에 있는 패스가 맞음(국가별 폴더)
			CONSOLE("restore img:[%s]", img.m_pSurface->GetstrRes().c_str() );
		}
#endif // _DEBUG
		img.m_pSurface->RestoreDevice();
	}
}

void XImageMng::DestroyDevice()
{
	for( auto& img : m_listSurface ) {
		img.m_pSurface->DestroyDevice();
	}
}
bool XImageMng::DoForceDestroy( const _tstring& strRes )
{
	for( auto itor = m_listSurface.begin(); itor != m_listSurface.end(); ) {
		auto& img = ( *itor );
		XBREAK( img.m_pSurface == nullptr );
//		if( img.m_pSurface->GetstrRes() == strRes ) {
		if( img.m_strRes == strRes ) {
//			img.m_pSurface->_ClearRefCnt();
			// 다른데서 여전히 참조하고 있을수 있으니 refCnt 0일때만 파괴해야한다. 결국 ReleaseNow()기능을 만드는거랑 같네
			XBREAK( img.m_pSurface->GetnRefCnt() > 0 );		// 아직 어딘가에서 참조하고있다는것이다. 그러므로 지금 당장 이 리소스를 없애려면 그쪽부터 해제시켜야한다.
			XBREAK( img.m_pSurface->GetnRefCnt() < 0 );		// 
			// 아무곳에서도 참조하지 않는 refCnt 0이지만 시간땜에 아직 파괴되지 않은 리소스를 지금 즉시 파괴한다.
			if( img.m_pSurface->GetnRefCnt() == 0 ) {
				s_sizeTotalVMem -= img.m_pSurface->GetSizeByte();
				SAFE_DELETE( img.m_pSurface );
				m_listSurface.erase( itor++ );
				return true;
			} else
				++itor;
		} else
			++itor;
	}
	return false;
}

/**
 @brief 아무도 참조하고 있지는 않지만 캐쉬타임에 걸려 아직 파괴되지 않은 리스스들을 모두 날린다.
*/
void XImageMng::DoFlushCache()
{
	for( auto itor = m_listSurface.begin(); itor != m_listSurface.end(); ) {
		auto& img = ( *itor );
		XBREAK( img.m_pSurface == nullptr );
		// 아무곳에서도 참조하지 않는 refCnt 0이지만 시간땜에 아직 파괴되지 않은 리소스를 지금 즉시 파괴한다.
		if( img.m_pSurface->GetnRefCnt() == 0 ) {
			s_sizeTotalVMem -= img.m_pSurface->GetSizeByte();
#if defined(_DEBUG) && defined(_CHEAT)
			if( img.m_pSurface ) {
// 				CONSOLE( "fush img:[%s]", img.m_pSurface->GetstrRes().c_str() );
				CONSOLE( "fush img:[%s]", img.m_strRes.c_str() );
			}
#endif // _DEBUG
			SAFE_DELETE( img.m_pSurface );
			m_listSurface.erase( itor++ );
		} else
			++itor;
	}
}


/**
 @brief 비동기로딩 등록된 파일을 한프레임당 하나씩 읽는다.
 .메인스레드에서 돌아감
 .한 프로세스에 하나의 파일만 읽는다.
 @param bTouching 현재 터치중인지
*/

void XImageMng::Process( bool bTouching )
{
	if( bTouching )
		return;
	if( m_listAsync.size() /*&& m_Cnt > 5*/ ) {		// 너무 빠르게 로딩하지 않기 위해.
		// 대기열이 있을때만.
		auto pFirst =  m_listAsync.GetpFirst();
		if( XASSERT(pFirst) ) {
			// 첫번째 대기열을 꺼냄
			const auto strRes = pFirst->m_strRes;
			// 국가별 폴더에서 우선적으로 찾는다.
			TCHAR szLangPath[1024];
			XE::LANG.ChangeToLangDir( strRes.c_str(), szLangPath );
			auto llTime = XE::GetFreqTime();
			auto _pSurface = GRAPHICS->CreateSurface( pFirst->m_bHighReso
																						, szLangPath
																						, pFirst->m_Format
																						, pFirst->m_bSrcKeep, pFirst->m_bMakeMask );
			auto llPass = XE::GetFreqTime() - llTime;
			if( _pSurface == nullptr ) {
				llPass = 0;
				// 없으면 루트 폴더에서 찾는다.
				llTime = XE::GetFreqTime();
				_pSurface = GRAPHICS->CreateSurface( pFirst->m_bHighReso
																					, strRes.c_str()
																					, pFirst->m_Format
																					, pFirst->m_bSrcKeep, pFirst->m_bMakeMask );
				if( _pSurface ) {
					llPass = XE::GetFreqTime() - llTime;
					_pSurface->SetstrRes( strRes.c_str() );
				} else {
					CONSOLE_TAG( "img", "(async)file open failed:%s", strRes.c_str() );
				}
			} else {
				_pSurface->SetstrRes( szLangPath );
			}
			if( _pSurface ) {
//		XLOGP( "%s, %llu", XE::GetFileName( szRes ), llPass );
				XBREAK( _pSurface->IsHavestrRes() == FALSE );
//		_IMLOGXN( "%s...loading", pSurface->GetstrRes().c_str() );
				auto pImg = Find( pFirst->m_strRes );
				if( XASSERT(pImg) ) {
					pImg->m_secLoaded = XTimer2::sGetTime();
					//pImg->m_pSurface = pSurface;  여기서 새로 할당한걸로 넣으면 안되고 기존 서피스에 내용만 채워야 한다.
					const auto refCnt = pImg->m_pSurface->GetnRefCnt();		// 편법. 레퍼런스 카운터는 바뀌면 안되므로 백업.
					GRAPHICS->CopyValueSurface( pImg->m_pSurface, _pSurface );		// 서피스 값 복사.
//					*(pImg->m_pSurface) = *_pSurface;		// 값 복사
					pImg->m_pSurface->_SetrefCnt( refCnt );		// 레퍼런스 카운트 복구.
					_pSurface->ClearCreated();
//				pSurface->IncRefCnt();		// _Load()때 이미 레퍼런스 카운트를 증가시켰으므로 여기서 올리면 안됨.
					s_sizeTotalVMem += _pSurface->GetSizeByte();
					SAFE_DELETE( _pSurface );		// 임시객체인 복사 원본은 삭제.
					m_Cnt = 0;		// 하나로딩하면 카운터 초기화
				}
			} else {// _pSurface
				// 로딩에 실패하면 대기열뒤로 보내야함.
				XBREAK(1);
			}
			// 로딩에 성공하면 대기열에서 삭제.
			m_listAsync.pop_front();
			pFirst = nullptr;
		}

	}
	++m_Cnt;
}

