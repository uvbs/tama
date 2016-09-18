#pragma once

#include "etc/XGraphicsDef.h"

class XSurface;
namespace XE {
struct xImage;
}
class XImageMng
{
public:
	static int s_sizeTotalVMem;
private:
	struct xAsyncLoad {
		_tstring m_strRes;			// 리소스 패스
		XE::xtPixelFormat m_Format = XE::xPF_NONE;
		bool m_bUseAtlas;
		bool m_bSrcKeep = false;
		bool m_bMakeMask = false;
		bool m_bHighReso = true;
		bool m_bBatch = false;		// 일괄렌더모드 서피스
	};
	struct xImage {
		_tstring m_strRes;		// 표준패스(국가패스아님) 비동기로 로딩할땐 pSurface의 szRes가 비어있기때문에 따로 둠.
		xSec m_secLoaded = 0;	// 이미지를 로딩한 시간
		XSurface *m_pSurface = nullptr;
		bool m_bAsyncLoad = false;		// 비동기로딩을 기다리고 있음.
		bool m_bUseAtlas = false;
		bool m_bBatch = false;
	};
private:
	XList4<xImage> m_listSurface;
	XList4<xAsyncLoad> m_listAsync;		// 비동기로딩 대기열
	int m_Cnt = 0;
	void Init() {}
	void Destroy();
public:
	XImageMng( int max );
	virtual ~XImageMng() { Destroy(); }
	//
	void CheckRelease( void );
private:
	xImage* FindExist( XUINT64 idRes );
	xImage* FindExist( LPCTSTR szFilename, bool bBatch );
	inline xImage* FindExist( const _tstring& strRes, bool bBatch ) {
		return FindExist( strRes.c_str(), bBatch );
	}
	XSurface* _Load( bool bHighReso,
									 LPCTSTR szRes, 
									 XE::xtPixelFormat format, 
									 bool bUseAtlas,
									 bool bSrcKeep, bool bMakeMask,
									 bool bAsync );
public:
	// 구코드 호환용
	XSurface* Load( BOOL bHighReso, LPCTSTR szRes, BOOL bSrcKeep = FALSE, BOOL bMakeMask=FALSE, bool bAsync = false );
	inline XSurface* Load( LPCTSTR szRes, BOOL bSrcKeep = FALSE, BOOL bMakeMask = FALSE, bool bAsync = false ) {
		return Load( TRUE, szRes, bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* Load( const _tstring& strRes, BOOL bSrcKeep = FALSE, BOOL bMakeMask = FALSE, bool bAsync = false ) {
		return Load( TRUE, strRes.c_str(), bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* Load( const _tstring&& strRes, BOOL bSrcKeep = FALSE, BOOL bMakeMask = FALSE, bool bAsync = false ) {
		return Load( TRUE, strRes.c_str(), bSrcKeep, bMakeMask, bAsync );
	}
	XSurface* Load( bool bHighReso, LPCTSTR szRes, BOOL bSrcKeep = FALSE, BOOL bMakeMask = FALSE, bool bAsync = false ) {
		return _Load( bHighReso, szRes, XE::xPF_ARGB4444, true, xBOOLToBool( bSrcKeep ), xBOOLToBool( bMakeMask ), bAsync );
	}
	XSurface* Load( bool bHighReso, const _tstring& strRes, BOOL bSrcKeep = FALSE, BOOL bMakeMask = FALSE, bool bAsync = false ) {
		return _Load( bHighReso, strRes.c_str(), XE::xPF_ARGB4444, true, xBOOLToBool( bSrcKeep ), xBOOLToBool( bMakeMask ), bAsync );
// 		return Load( bHighReso, strRes.c_str(), bSrcKeep, bMakeMask );
	}
	// 신코드
	XSurface* Load( bool bHighReso, 
									LPCTSTR szRes, 
									XE::xtPixelFormat format, 
									bool bUseAtlas, 
									bool bSrcKeep, bool bMakeMask, 
									bool bAsync );
	inline XSurface* Load( bool bHighReso, 
												 const _tstring& strRes, 
												 XE::xtPixelFormat format, 
												 bool bUseAtlas, 
												 bool bSrcKeep, bool bMakeMask, bool
												 bAsync ) {
		return Load( bHighReso, strRes.c_str(), format, bUseAtlas, bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* Load( const _tstring& strRes, 
												 XE::xtPixelFormat format, 
												 bool bUseAtlas, 
												 bool bSrcKeep, bool bMakeMask,
												 bool bAsync ) {
		return _Load( true, strRes.c_str(), format, bUseAtlas, bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* Load( const _tstring&& strRes, 
												 XE::xtPixelFormat format, 
												 bool bUseAtlas, 
												 bool bSrcKeep, bool bMakeMask, 
												 bool bAsync ) {
		return _Load( true, strRes.c_str(), format, bUseAtlas, bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* LoadByBatch( const _tstring&& strRes,
												 XE::xtPixelFormat format,
												 bool bUseAtlas,
												 bool bSrcKeep, bool bMakeMask,
												 bool bAsync ) {
		return _Load( true, strRes.c_str(), format, bUseAtlas, bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* Load( LPCTSTR szRes,
												 XE::xtPixelFormat format, 
												 bool bUseAtlas, 
												 bool bSrcKeep, bool bMakeMask,
												 bool bAsync ) {
		return _Load( true, szRes, format, bUseAtlas, bSrcKeep, bMakeMask, bAsync );
	}
	XSurface* CreateSurface( const char* cKey
													, const XE::POINT& sizeSurfaceOrig
													, const XE::VEC2& vAdj
													, XE::xtPixelFormat formatSurface								
													, void* const pImgSrc
													, const XE::POINT& sizeMemSrc
													, bool bSrcKeep, bool bMakeMask );
	XSurface* CreateSurface( const char* cKey, const XE::xImage& imgInfo );
	inline XSurface* CreateSurface( const std::string& strKey, const XE::xImage& imgInfo ) {
		return CreateSurface( strKey.c_str(), imgInfo );
	}
	void RestoreDevice();
	void DestroyDevice();
	void Release( XSurface* pSurface );
	template<int N>
	int GetArySurfaces( XArrayLinearN<XSurface*, N>& ary ) {
		int num = 0;
		for( auto img : m_listSurface ) {
			ary.Add( img.m_pSurface );
			++num;
		}
		return num;
	}
	bool DoForceDestroy( const _tstring& strRes );
	bool DoForceDestroy( XSurface *pSurface );
	void DoFlushCache();
// #ifdef WIN32
// 	void OnPauseByWin32();
// #endif // WIN32
	void OnPause();
	void Process( bool bTouching );
private:
	xImage* FindExist( XSurface *pSurface );
	void DestroyOlderFile();
	void AsyncLoadProcess();
};

extern XImageMng *IMAGE_MNG;
