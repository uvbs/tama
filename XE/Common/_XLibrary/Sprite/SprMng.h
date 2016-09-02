#pragma once
//#include "SprDat.h"
#include "VersionXE.h"
#include "etc/XGraphicsDef.h"
class XSprDat;

#ifndef _XASYNC_SPR
class XSprMng
{
public:
	static int s_sizeTotalVM;
	static XSprMng* sGet() { return s_pInstance; }
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
			: m_strKey(strKey), pSprDat(_pSprDat), m_HSL(hsl) { }
		xDat( const _tstring& strKey, XSprDat* _pSprDat ) 
			: m_strKey(strKey), pSprDat(_pSprDat) { }
		inline ID getid();
		bool IsOverTime() const;
	};
	//	XSprDat_List m_listSprDat;
	XList4<xDat> m_listSprDat;
	XList4<xDat> m_listPreLoad;
	std::map<_tstring, xDat> m_mapTable;
	void Init( void ) {
	}
	void Destroy( void );
public:
	XSprMng();
	virtual ~XSprMng() { XTRACE("destroy sprmng"); Destroy(); }

	void CheckRelease( void );
	XSprDat *Load( LPCTSTR szFilename, const XE::xHSL& hsl = XE::xHSL(), BOOL bAddRefCnt=TRUE, BOOL bSrcKeep=FALSE, bool bAsyncLoad = false );	
	void RestoreDevice();
	void Add( const _tstring& strKey, XSprDat *pSprDat, const XE::xHSL& hsl );
//	XSprDat *New( void );
	void Release( XSprDat *pSprDat );
	// 	void PreLoad( LPCTSTR szSpr, const XE::VEC3& vHSL, BOOL bSrcKeep=FALSE );
//	void ReleasePreload( void );
	void DoFlushCache();
#ifdef WIN32
	//	void OnPauseForWin32();
	void Reload();
#endif // WIN32
	void OnPause();
	void OnCreate();
private:
	xDat* Find( LPCTSTR szFilename, BOOL bSrcKeep, const XE::xHSL& hsl );
	void DestroyOlderFile();
};

#else
// class XSprMng
// {
// public:
// 	static int s_sizeTotalVM;
// private:
// 	struct xDat {
// 		XSprDat *pSprDat = nullptr;
// 		XE::VEC3 vHSL;
// 		xDat() {}
// 		xDat( XSprDat* _pSprDat, const XE::VEC3& _vHSL )
// 			: pSprDat( _pSprDat ), vHSL( _vHSL ) { }
// 		xDat( XSprDat* _pSprDat )
// 			: pSprDat( _pSprDat ) { }
// 		inline ID getid() {
// 			return ( pSprDat ) ? pSprDat->GetsnDat() : 0;
// 		}
// 	};
// 	struct xAsync {
// 		_tstring m_strRes;
// 		XE::VEC3 m_vHSL;
// 		BOOL m_bSrcKeep = FALSE;
// 		BOOL m_bAddRefCnt = FALSE;
// 	};
// 	//	XSprDat_List m_listSprDat;
// 	XList4<xDat> m_listSprDat;
// 	XList4<xDat> m_listPreLoad;
// 	XList4<xAsync> m_listAsync;		// 비동기로 로딩이 예약된 스프라이트들
// 	XSPLock m_spLock;
// 	uintptr_t m_hThread = 0;
// 	ID m_idThread = 0;
// 	void Init( void ) {
// 	}
// 	void Destroy( void );
// public:
// 	XSprMng();
// 	virtual ~XSprMng() {
// 		XTRACE( "destroy sprmng" ); Destroy();
// 	}
// 
// 	static unsigned int __stdcall _WorkThread( void *param );
// 	void WorkThread();
// 	void OnCreate();
// 	void CheckRelease( void );
// 	XSprDat *Load( LPCTSTR szFilename, const XE::VEC3& vHSL = XE::VEC3(), BOOL bAddRefCnt = TRUE, BOOL bSrcKeep = FALSE, bool bAsyncLoad = false );
// 	void RestoreDevice();
// 	void Add( XSprDat *pSprDat, const XE::VEC3& vHSL );
// 	XSprDat *Find( LPCTSTR szFilename, BOOL bSrcKeep, const XE::VEC3& vHSL );
// 	XSprDat *New( void );
// 	void Release( XSprDat *pSprDat );
// 	// 	void PreLoad( LPCTSTR szSpr, const XE::VEC3& vHSL, BOOL bSrcKeep=FALSE );
// 	void ReleasePreload( void );
// 	void DoFlushCache();
// #ifdef WIN32
// 	//	void OnPauseForWin32();
// 	void Reload();
// #endif // WIN32
// 	void OnPause();
// };
// 
// 
#endif // _XASYNC_SPR

extern XSprMng *SPRMNG;
//inline XSprMng* GetSprMng( void ) { return SPRMNG; }

