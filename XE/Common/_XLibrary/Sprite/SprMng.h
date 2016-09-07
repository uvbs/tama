#pragma once
#include "VersionXE.h"
#include "etc/XGraphicsDef.h"
class XSprDat;

class XSprMng {
public:
	static int s_sizeTotalVM;
	static XSprMng* sGet() {
		return s_pInstance;
	}
	static xSec s_secCache;
private:
	static XSprMng* s_pInstance;
	struct xDat {
		_tstring m_strKey;			// HSL로딩이 있기때문에 pSprDat의 파일명과 일치하지 않을 수 있음.
		XSprDat *pSprDat = nullptr;
		XE::xHSL m_HSL;
		xSec m_secLoaded = 0;			// 파일을 로드한 시간
		xDat() {}
		xDat( const _tstring& strKey, XSprDat* _pSprDat, const XE::xHSL& hsl )
			: m_strKey( strKey ), pSprDat( _pSprDat ), m_HSL( hsl ) { }
		xDat( const _tstring& strKey, XSprDat* _pSprDat )
			: m_strKey( strKey ), pSprDat( _pSprDat ) { }
		inline ID getid();
		bool IsOverTime() const;
	};
#ifdef _XASYNC_SPR
	struct xAsync {
		_tstring m_strRes;
		XE::xHSL m_HSL;
		//		XE::VEC3 m_vHSL;
		BOOL m_bSrcKeep = FALSE;
		BOOL m_bAddRefCnt = FALSE;
		bool m_bUseAtlas = false;
	};
#endif // _XASYNC_SPR
	XList4<xDat> m_listSprDat;
	XList4<xDat> m_listPreLoad;
	std::map<_tstring, xDat> m_mapTable;
#ifdef _XASYNC_SPR
	XList4<xAsync> m_listAsync;		// 비동기로 로딩이 예약된 스프라이트들
	XSPLock m_spLock;
	uintptr_t m_hThread = 0;
	ID m_idThread = 0;
#endif // _XASYNC_SPR
	void Init( void ) {
	}
	void Destroy( void );
public:
	XSprMng();
	~XSprMng() {
		XTRACE( "destroy sprmng" ); Destroy();
	}

#ifdef _XASYNC_SPR
	static unsigned int __stdcall _WorkThread( void *param );
	void WorkThread();
	void AsyncSetAction( ID idAct, xRPT_TYPE playType, BOOL bExecFrameMove );
	void OnCreate();
#endif // _XASYNC_SPR
	void CheckRelease( void );
	XSprDat *Load( LPCTSTR szFilename,
								 const XE::xHSL& hsl/* = XE::xHSL()*/,
								 bool bUseAtlas,
								 BOOL bAddRefCnt = TRUE,
								 BOOL bSrcKeep = FALSE,
								 bool bAsyncLoad = false );
	void RestoreDevice();
	void Add( const _tstring& strKey, XSprDat *pSprDat, const XE::xHSL& hsl );
	void Release( XSprDat *pSprDat );
	void DoFlushCache();
#ifdef WIN32
	void Reload();
#endif // WIN32
	void OnPause();
private:
	xDat* Find( LPCTSTR szFilename, BOOL bSrcKeep, const XE::xHSL& hsl );
	void DestroyOlderFile();
};

extern XSprMng *SPRMNG;

