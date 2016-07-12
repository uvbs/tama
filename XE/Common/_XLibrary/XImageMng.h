#pragma once

class XSurface;
class XImageMng
{
public:
	static int s_sizeTotalVMem;
private:
	struct xAsyncLoad {
		_tstring m_strRes;			// 리소스 패스
		XE::xtPixelFormat m_Format = XE::xPF_NONE;
		bool m_bSrcKeep = false;
		bool m_bMakeMask = false;
		bool m_bHighReso = true;
	};
	struct xImage {
		_tstring m_strRes;		// 표준패스(국가패스아님) 비동기로 로딩할땐 pSurface의 szRes가 비어있기때문에 따로 둠.
		xSec m_secLoaded = 0;	// 이미지를 로딩한 시간
		XSurface *m_pSurface = nullptr;
		bool m_bAsyncLoad = false;		// 비동기로딩을 기다리고 있음.
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
	xImage* Find( XUINT64 idRes );
	xImage* Find( LPCTSTR szFilename );
	inline xImage* Find( const _tstring& strRes ) {
		return Find( strRes.c_str() );
	}
private:
	XSurface* _Load( bool bHighReso, LPCTSTR szRes, XE::xtPixelFormat format, bool bSrcKeep, bool bMakeMask, bool bAsync );
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
		return _Load( bHighReso, szRes, XE::xPF_ARGB4444, xBOOLToBool( bSrcKeep ), xBOOLToBool( bMakeMask ), bAsync );
	}
	XSurface* Load( bool bHighReso, const _tstring& strRes, BOOL bSrcKeep = FALSE, BOOL bMakeMask = FALSE, bool bAsync = false ) {
		return _Load( bHighReso, strRes.c_str(), XE::xPF_ARGB4444, xBOOLToBool( bSrcKeep ), xBOOLToBool( bMakeMask ), bAsync );
// 		return Load( bHighReso, strRes.c_str(), bSrcKeep, bMakeMask );
	}
	// 신코드
	XSurface* Load( bool bHighReso, LPCTSTR szRes, XE::xtPixelFormat format, bool bSrcKeep = false, bool bMakeMask = false, bool bAsync = false );
	inline XSurface* Load( bool bHighReso, const _tstring& strRes, XE::xtPixelFormat format, bool bSrcKeep = false, bool bMakeMask = false, bool bAsync = false ) {
		return Load( bHighReso, strRes.c_str(), format, bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* Load( const _tstring& strRes, XE::xtPixelFormat format, bool bSrcKeep = false, bool bMakeMask = false, bool bAsync = false ) {
		return _Load( true, strRes.c_str(), format, bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* Load( const _tstring&& strRes, XE::xtPixelFormat format, bool bSrcKeep = false, bool bMakeMask = false, bool bAsync = false ) {
		return _Load( true, strRes.c_str(), format, bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* Load( LPCTSTR szRes, XE::xtPixelFormat format, bool bSrcKeep = false, bool bMakeMask = false, bool bAsync = false ) {
		return _Load( true, szRes, format, bSrcKeep, bMakeMask, bAsync );
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
	inline bool DoForceDestroy( XSurface *pSurface ) {
		if( !pSurface )
			return false;
		return DoForceDestroy( pSurface->GetstrRes() );
	}
	void DoFlushCache();
// #ifdef WIN32
// 	void OnPauseByWin32();
// #endif // WIN32
	void OnPause();
	void Process( bool bTouching );
private:
	xImage* Find( XSurface *pSurface );
	void DestroyOlderFile();
};

extern XImageMng *IMAGE_MNG;
