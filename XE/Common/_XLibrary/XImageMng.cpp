#include "stdafx.h"
#include "XImageMng.h"
#include "etc/XSurface.h"
#include "etc/xGraphics.h"
#include "etc/XTimer2.h"
#include "etc/xLang.h"
#include "XFramework/client/XClientMain.h"
#include "XImage.h"


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

XImageMng::XImageMng()
{
	Init();
}

void XImageMng::Destroy()
{
	DoFlushCache();
	//
	CheckRelease();
	//	XLIST4_DESTROY(m_listSurface );
	for( auto img : m_listSurface ) {
		if( img.m_pSurface && !img.m_pSurface->IsbAtlas() )
			s_sizeTotalVMem -= img.m_pSurface->GetSizeByte();
		SAFE_DELETE( img.m_pSurface );
	}
}
// bool XImageMng::DoForceDestroy( XSurface *pSurface ) 
// {
// 	if( !pSurface )
// 		return false;
// 	return DoForceDestroy( pSurface->GetstrRes() );
// }

void XImageMng::CheckRelease( void )
{
	for( auto& img : m_listSurface ) {
		if( img.m_pSurface->GetnRefCnt() > 0 ) {
			XTRACE( "XImageMng: 해제되지 않은 이미지 발견. %s", img.m_strRes.c_str() );
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

bool XImageMng::LoadMap( const _tstring& strFile )
{
	XBREAK( CONSTANT == nullptr );
	CToken token;
	if( token.LoadFile( XE::MakePath( DIR_PROP, strFile ), XE::TXT_UTF16 ) == xFAIL )
		return false;
	while( 1 ) {
		token.GetToken();
		if( token.IsEof() )
			break;
		const _tstring strKey = token.m_Token;
		xImgMap imgMap;
		imgMap.m_strRes = strKey;
		const _tstring strFormat = token.GetToken();
		XE::xtPixelFormat format = XE::xPF_NONE;
		if( strFormat == _T("xPF_NONE") )
			format = XE::xPF_NONE;
		else if( strFormat == _T( "xPF_ARGB8888" ) )
			format = XE::xPF_ARGB8888;
		else if( strFormat == _T( "xPF_ARGB4444" ) )
			format = XE::xPF_ARGB4444;
		else if( strFormat == _T( "xPF_ARGB1555" ) )
			format = XE::xPF_ARGB1555;
		else if( strFormat == _T( "xPF_RGB565" ) )
			format = XE::xPF_RGB565;
		else if( strFormat == _T( "xPF_RGB555" ) )
			format = XE::xPF_RGB555;
		else {
			XBREAKF( 1, "%s: unknown pixelformat: %s", strFile.c_str(), strFormat.c_str() );
		}
		imgMap.m_Format = format;;
		m_mapImgInfo[ strKey ] = imgMap;
	}
	return true;
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
				if( !img.m_pSurface->IsbAtlas() )
					s_sizeTotalVMem -= img.m_pSurface->GetSizeByte();
				SAFE_DELETE( img.m_pSurface );
				m_listSurface.erase( itor++ );
			} else {
				++itor;
			}
	}
}

XImageMng::xImage* XImageMng::FindExist( XUINT64 idRes )
{
	for( auto& img : m_listSurface ) {
		if( img.m_pSurface->Getid64Res() == idRes )
			return &img;
	}
	return nullptr;
}

/**
 @brief szRes가 이미 생성된 서피스인지 검색
 @param bBatch 로딩요청받은 szRes의 로딩모드가 배치모드인지.
*/
XImageMng::xImage* XImageMng::FindExist( LPCTSTR szRes, bool bBatch )	// 로컬패스명으로 갖고있게 바뀜
{
	for( auto& img : m_listSurface ) {
		if( XE::IsSame( img.m_strRes.c_str(), szRes ) 
				&& img.m_bBatch == bBatch )
			return &img;
	}
	return nullptr;
}
/**
 @brief 내부에서만 쓰는 버전
*/
XImageMng::xImage* XImageMng::FindExist( XSurface *pSurface )	
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
// XSurface* XImageMng::Load( BOOL bHighReso, LPCTSTR szRes, BOOL bSrcKeep, BOOL bMakeMask, bool bAsync )
// {
// //	XBREAK( bHighReso == FALSE );		// 이제 이건 지원안함.
// 	// 전투배경같은건 bHighReso를 FALSE로 놓고 쓰는 편법을 쓰기 때문에 일단 bHighReso는 없애지 못함. 장차 bHighReso자체를 없애는 방향으로 가야할듯.
// // 	return _Load( bHighReso != FALSE, szRes, XE::xPF_ARGB4444, true, xBOOLToBool(bSrcKeep), xBOOLToBool(bMakeMask), bAsync );
// 	return _Load( bHighReso != FALSE, szRes, XE::xPF_NONE, true, xBOOLToBool( bSrcKeep ), xBOOLToBool( bMakeMask ), bAsync );
// }
/**
 @brief 기존에 bHighReso를 FALSE로 해서 크게 렌더링하는 편법을 쓰고 있어서 bHighReso가 그대로 살아있음.
*/
// XSurface* XImageMng::Load( bool bHighReso, 
// 													 LPCTSTR szRes, 
// 													 XE::xtPixelFormat format, 
// 													 bool bUseAtlas, 
// 													 bool bSrcKeep/*=false*/, 
// 													 bool bMakeMask/*=false*/, 
// 													 bool bAsync /*= false*/ )
// {
// 	const bool bKeepSrc = (bMakeMask == true);
// 	return _Load( bHighReso, szRes, format, bUseAtlas, bKeepSrc, bMakeMask, bAsync );
// }
XSurface* XImageMng::LoadByBatch( const _tstring& strRes,
																	XE::xtPixelFormat format,
																	bool bUseAtlas,
																	bool bSrcKeep, bool bMakeMask,
																	bool bAsync ) 
{
// 	auto prev = XGraphics::sSetEnableBatchLoading( true );
	auto psfc = _Load( true, strRes.c_str(), format, true, bUseAtlas, bSrcKeep, bMakeMask, bAsync );
// 	XGraphics::sSetEnableBatchLoading( prev );
	return psfc;
}

/**
 @brief 내부용 로딩모듈
 @param bUseAtlas 아틀라스를 사용하는 서피스인지
 @param bBatch 일괄렌더용 서피스인지
 @param bAsync 비동기 로딩
*/
XSurface* XImageMng::_Load( bool bHighReso, 
														LPCTSTR _szRes, 
														XE::xtPixelFormat format, 
														bool bBatch,
														bool bUseAtlas, 
														bool bSrcKeep, bool bMakeMask, 
														bool bAsync )
{
	if( !format ) {
		auto itor = m_mapImgInfo.find( _szRes );
		if( itor != m_mapImgInfo.end() ) {
			auto& imgInfo = itor->second;
			format = imgInfo.m_Format;
		} else {
			format = XE::xPF_ARGB4444;
		}
	}
	bAsync = false;		// 일단 버그수정을 위해 동기로딩만.
	const _tstring strRes = _szRes;
	//////////////////////////////////////////////////////////////////////////
	// 이미 로딩한것인지 이미지풀에서 szRes로 찾아봄
	{
		auto pImg = FindExist( strRes.c_str(), bBatch );
		if( pImg ) {
			// 재로딩이 되었다면 로딩한 시간을 다시 갱신시킴
			pImg->m_secLoaded = XTimer2::sGetTime();
			pImg->m_pSurface->IncRefCnt();
			DestroyOlderFile();		// 여기다 쓰지말고 씬로딩완료 직후에 한번 불러주는게 더 효율적일듯.
			return pImg->m_pSurface;
		}
	}
	if( bAsync ) {
		// 현재 배치로딩 활성화 여부에 따라 맞는 서피스를 생성해준다.
		auto pSurface = (bBatch)
			? GRAPHICS->CreateSurfaceAtlasBatch() 
			: GRAPHICS->CreateSurfaceAtlasNoBatch();
		if( XASSERT(pSurface) ) {		// 이건 빈 객체만 만드는거기땜에 반드시 있어야 함.
			xImage img;
			img.m_strRes = strRes;
			img.m_secLoaded = XTimer2::sGetTime();		// 비동기 미로딩상태라도 시간은 넣어야 한다. 로딩되기전에 다시 모두 해제되어 필요가 없는상태면 로딩하지 말아야 하기 때문.
			img.m_pSurface = pSurface;
			img.m_bAsyncLoad = true;
			img.m_bUseAtlas = bUseAtlas;
			img.m_bBatch = bBatch;
			m_listSurface.Add( img );
			xAsyncLoad asyncLoad;
			asyncLoad.m_strRes = strRes;
			asyncLoad.m_bHighReso = bHighReso;
			asyncLoad.m_Format = format;
			asyncLoad.m_bSrcKeep = bSrcKeep;
			asyncLoad.m_bMakeMask = bMakeMask;
			asyncLoad.m_bUseAtlas = bUseAtlas;
			asyncLoad.m_bBatch = bBatch;
			m_listAsync.Add( asyncLoad );		// 비동기 대기열에 등록.
			pSurface->IncRefCnt();
		}
		return pSurface;
	} else {
		//////////////////////////////////////////////////////////////////////////
		// 국가별 폴더에서 우선적으로 찾는다.
		_tstring strLoadTry;			// 로딩을 시도한 패스
		TCHAR szLangPath[1024];
		XE::LANG.ChangeToLangDir( strRes.c_str(), szLangPath );
		strLoadTry = szLangPath;
		auto llTime = XE::GetFreqTime();
		auto pSurface = GRAPHICS->CreateSurface( bHighReso, szLangPath, bBatch, format, 
																						 bUseAtlas, bSrcKeep, bMakeMask, bAsync );
		auto llPass = XE::GetFreqTime() - llTime;
		if( pSurface == nullptr ) {
			llPass = 0;
			strLoadTry = strRes;
			// 없으면 루트 폴더에서 찾는다.
			llTime = XE::GetFreqTime();
			pSurface = GRAPHICS->CreateSurface( bHighReso, 
																					strRes.c_str(), 
																					bBatch,
																					format, 
																					bUseAtlas,
																					bSrcKeep, 
																					bMakeMask,
																					bAsync );
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
			XBREAK( pSurface->IsHavestrRes() == FALSE );
			xImage img;
			img.m_strRes = strRes;		// 국가패스가 아닌 루트패스로 들어가고 검색시에도 루트패스로 찾아야함.
			img.m_secLoaded = XTimer2::sGetTime();
			img.m_pSurface = pSurface;
			img.m_bAsyncLoad = false;
			img.m_bUseAtlas = bUseAtlas;
			img.m_bBatch = bBatch;
			m_listSurface.Add( img );
			pSurface->IncRefCnt();
			if( !pSurface->IsbAtlas() )
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
	auto pImg = FindExist( idRes );
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
														, false, false, false );
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
		if( !img.m_pSurface->IsbAtlas() ) {
			if( pImgSrc )
				s_sizeTotalVMem += pSurface->GetSizeByte();
		}
		return pSurface;
	}
	return nullptr;
}

/**
 @brief 관리되는 빈서피스를 생성해서 돌려준다.
*/
XSurface* XImageMng::CreateSurface( const std::string& strKey )
{
	return CreateSurface( strKey.c_str(),
												XE::POINT(),
												XE::VEC2(),
												XE::xPF_NONE,
												nullptr,
												XE::POINT(),
												false, false );
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

void XImageMng::OnPause()
{
// 	for( auto& img : m_listSurface ) {
// 		img.m_pSurface->OnPause();
// 	}
}
// bool XImageMng::DoForceDestroy( const _tstring& strRes )
// {
// 	for( auto itor = m_listSurface.begin(); itor != m_listSurface.end(); ) {
// 		auto& img = ( *itor );
// 		XBREAK( img.m_pSurface == nullptr );
// //		if( img.m_pSurface->GetstrRes() == strRes ) {
// 		if( img.m_strRes == strRes ) {
// //			img.m_pSurface->_ClearRefCnt();
// 			// 다른데서 여전히 참조하고 있을수 있으니 refCnt 0일때만 파괴해야한다. 결국 ReleaseNow()기능을 만드는거랑 같네
// 			XBREAK( img.m_pSurface->GetnRefCnt() > 0 );		// 아직 어딘가에서 참조하고있다는것이다. 그러므로 지금 당장 이 리소스를 없애려면 그쪽부터 해제시켜야한다.
// 			XBREAK( img.m_pSurface->GetnRefCnt() < 0 );		// 
// 			// 아무곳에서도 참조하지 않는 refCnt 0이지만 시간땜에 아직 파괴되지 않은 리소스를 지금 즉시 파괴한다.
// 			if( img.m_pSurface->GetnRefCnt() == 0 ) {
// 				s_sizeTotalVMem -= img.m_pSurface->GetSizeByte();
// 				SAFE_DELETE( img.m_pSurface );
// 				m_listSurface.erase( itor++ );
// 				return true;
// 			} else
// 				++itor;
// 		} else
// 			++itor;
// 	}
// 	return false;
// }

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
#ifdef _DEBUG
				XTRACE( "flush img:[%s]", img.m_strRes.c_str() );
#endif // _DEBUG
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
	// 비동기로딩 프로세스
	if( m_listAsync.size() )
		AsyncLoadProcess();
	++m_Cnt;
}

/**
 @brief 비동기 로딩이 필요하면 주스레드에서 매 프레임마다 로딩
*/
void XImageMng::AsyncLoadProcess()
{
	XBREAK( m_listAsync.empty() );
	// 대기열이 있을때만.
	auto pFirst = m_listAsync.GetpFirst();
	if( XASSERT( pFirst ) ) {
		// 첫번째 대기열을 꺼냄
		const auto strRes = pFirst->m_strRes;
		// 국가별 폴더에서 우선적으로 찾는다.
		TCHAR szLangPath[1024];
		XE::LANG.ChangeToLangDir( strRes.c_str(), szLangPath );
		auto llTime = XE::GetFreqTime();
		auto _pSurface = GRAPHICS->CreateSurface( pFirst->m_bHighReso,
																							szLangPath,
																							pFirst->m_Format,
																							pFirst->m_bUseAtlas,		// atlas
																							pFirst->m_bSrcKeep,
																							pFirst->m_bMakeMask,
																							false );	// 비동기로 생성중이므로 false로 넘겨야 지금즉시 로딩함.
		auto llPass = XE::GetFreqTime() - llTime;
		if( _pSurface == nullptr ) {
			llPass = 0;
			// 없으면 루트 폴더에서 찾는다.
			llTime = XE::GetFreqTime();
			_pSurface = GRAPHICS->CreateSurface( pFirst->m_bHighReso,
																						strRes.c_str(),
																						pFirst->m_Format,
																						pFirst->m_bUseAtlas,
																						pFirst->m_bSrcKeep,
																						pFirst->m_bMakeMask,
																						false );
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
			XBREAK( _pSurface->IsHavestrRes() == FALSE );
			auto pImg = FindExist( pFirst->m_strRes, pFirst->m_bBatch );
			if( XASSERT( pImg ) ) {
				pImg->m_secLoaded = XTimer2::sGetTime();
				const auto refCnt = pImg->m_pSurface->GetnRefCnt();		// 편법. 레퍼런스 카운터는 바뀌면 안되므로 백업.
				// Load()시점에서 Surface객체는 이미 생성되었으므로 비동기로 로딩한 값만 채워넣어야 한다.
				GRAPHICS->CopyValueSurface( pImg->m_pSurface, _pSurface );		// 서피스 값 복사.(XSurface*로 복사하면 하위클래스값이 복사안되서 이렇게 함.)
				pImg->m_pSurface->_SetrefCnt( refCnt );		// 레퍼런스 카운트 복구.
				_pSurface->ClearCreated();		// 내부에서 메모리 삭제 방지.
				s_sizeTotalVMem += _pSurface->GetSizeByte();
				SAFE_DELETE( _pSurface );		// 임시객체인 원본은 삭제.
				m_Cnt = 0;		// 하나로딩하면 카운터 초기화
			}
		} else {// _pSurface
						// 로딩에 실패하면 대기열뒤로 보내야함.
			XBREAK( 1 );
		}
		// 로딩에 성공하면 대기열에서 삭제.
		m_listAsync.pop_front();
		pFirst = nullptr;
	}
}

/**
@brief 아틀라스 idTex의 크기변경으로 이 텍스쳐를 사용하는 모든 서피스의 uv를 갱신한다.
*/
void XImageMng::UpdateUV( ID idTex,
												const XE::POINT& sizePrev,
												const XE::POINT& sizeNew )
{
	for( auto img : m_listSurface ) {
		if( img.m_pSurface ) {
			img.m_pSurface->UpdateUV( idTex, sizePrev, sizeNew );
		}
	}
}
