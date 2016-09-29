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
	// 각 png파일들을 어떤 포맷으로 읽어야 할지 정보가 들어있다.
	struct xImgMap {
		_tstring m_strRes;
		XE::xtPixelFormat m_Format = XE::xPF_ARGB4444;
	};
private:
	XList4<xImage> m_listSurface;
	XList4<xAsyncLoad> m_listAsync;		// 비동기로딩 대기열
	std::map<_tstring, xImgMap> m_mapImgInfo;
	int m_Cnt = 0;
	void Init() {}
	void Destroy();
public:
	XImageMng();
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
									 bool bBatch,
									 bool bUseAtlas,
									 bool bSrcKeep, bool bMakeMask,
									 bool bAsync );
public:
	inline XSurface* Load( const _tstring& strRes,
												 bool bBatch,
												 XE::xtPixelFormat format,
												 bool bUseAtlas,
												 bool bAsync ) {
		return _Load( true, strRes.c_str(), format, bBatch, bUseAtlas, false, false, bAsync );
	}
	inline XSurface* Load( const _tstring& strRes,
												 XE::xtPixelFormat format, 
												 bool bUseAtlas, 
												 bool bSrcKeep, bool bMakeMask,
												 bool bAsync ) {
		return _Load( true, strRes.c_str(), format, false, bUseAtlas, bSrcKeep, bMakeMask, bAsync );
	}
	inline XSurface* Load( const _tstring&& strRes, 
												 XE::xtPixelFormat format, 
												 bool bUseAtlas, 
												 bool bSrcKeep, bool bMakeMask, 
												 bool bAsync ) {
		return _Load( true, strRes.c_str(), format, false, bUseAtlas, bSrcKeep, bMakeMask, bAsync );
	}
	XSurface* LoadByBatch( const _tstring& strRes,
												 XE::xtPixelFormat format,
												 bool bUseAtlas,
												 bool bSrcKeep, bool bMakeMask,
												 bool bAsync );
	//
	inline XSurface* Load( const _tstring& strRes ) {
		return Load( strRes.c_str(), XE::xPF_NONE, true, false, false, true );
	}
	inline XSurface* LoadByBatch( const _tstring& strRes ) {
		return LoadByBatch( strRes, XE::xPF_ARGB4444, true, false, false, true );
	}
	inline XSurface* LoadByBatch( const _tstring& strRes, XE::xtPixelFormat format ) {
		return LoadByBatch( strRes, format, true, false, false, true );
	}
	inline XSurface* Load( const _tstring& strRes,
												 XE::xtPixelFormat format ) {
		return Load( strRes.c_str(), format, true, false, false, false );
	}
	inline XSurface* Load( const _tstring& strRes,
												 XE::xtPixelFormat format, bool bSrcKeep ) {
		return Load( strRes.c_str(), format, true, bSrcKeep, false, false );
	}
	inline XSurface* LoadByRetina( LPCTSTR szRes,
																 XE::xtPixelFormat format,
																 bool bUseAtlas,
																 bool bAsync ) {
		return _Load( false, szRes, format, false, bUseAtlas, false, false, bAsync );
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
	XSurface* CreateSurface( const std::string& strKey );
	void RestoreDevice();
	void DestroyDevice();
	void Release( XSurface* pSurface );
	bool LoadMap( const _tstring& strFile );
	template<int N>
	int GetArySurfaces( XArrayLinearN<XSurface*, N>& ary ) {
		int num = 0;
		for( auto img : m_listSurface ) {
			ary.Add( img.m_pSurface );
			++num;
		}
		return num;
	}
// 	bool DoForceDestroy( const _tstring& strRes );
// 	bool DoForceDestroy( XSurface *pSurface );
	void DoFlushCache();
// #ifdef WIN32
// 	void OnPauseByWin32();
// #endif // WIN32
	void OnPause();
	void Process( bool bTouching );
	void UpdateUV( ID idTex, const XE::POINT& sizePrev, const XE::POINT& sizeNew );
private:
	xImage* FindExist( XSurface *pSurface );
	void DestroyOlderFile();
	void AsyncLoadProcess();
};

extern XImageMng *IMAGE_MNG;
