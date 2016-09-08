﻿#pragma once
#ifdef _XASYNC_SPR
#include "VersionXE.h"
#include "etc/XGraphicsDef.h"
class XSprDat;
class XSprObj;
enum xRPT_TYPE : int;

XE_NAMESPACE_START( xSpr )
//
struct xDat {
	ID m_idDat = 0;
	_tstring m_strKey;			// HSL로딩이 있기때문에 pSprDat의 파일명과 일치하지 않을 수 있음.
	_tstring m_strLoadFile;		// 실제 읽는 파일의 파일명
	XSprDat *m_pSprDat = nullptr;
	XE::xHSL m_HSL;
	xSec m_secLoaded = 0;			// 파일을 로드한 시간
	ID m_idAsync = 0;					// 비동기로딩중이면 아이디가 있다.
	xDat() : m_idDat( XE::GenerateID() ) {}
	xDat( const _tstring& strKey, const _tstring& strFile, XSprDat* _pSprDat, const XE::xHSL& hsl )
		: m_idDat(XE::GenerateID())
		, m_strKey( strKey )
		, m_strLoadFile( strFile )
		, m_pSprDat( _pSprDat )
		, m_HSL( hsl ) { }
// 	xDat( const _tstring& strKey, XSprDat* _pSprDat )
// 		: m_idDat(XE::GenerateID()), m_strKey( strKey ), m_pSprDat( _pSprDat ) { }
	~xDat();
	inline ID getid();
	bool IsOverTime() const;
};
typedef std::shared_ptr<xDat> XSPDat;
//
XE_NAMESPACE_END; // XE


class XSprMng {
public:
	static int s_sizeTotalVM;
	static XSprMng* sGet() {
		return s_pInstance;
	}
	static xSec s_secCache;
private:
	static XSprMng* s_pInstance;
	struct xKey {

	};
	struct xAsync {
		ID m_idAsync = 0;
		_tstring m_strFilename;				// 가상 파일이름.
		_tstring m_strLoadFile;		// 실제 읽어야 하는 파일
		XE::xHSL m_HSL;
		BOOL m_bSrcKeep = FALSE;
		bool m_bUseAtlas = false;
		XSprDat* m_pSprDat = nullptr;		// 비동기로딩이 완료되었을때 그 sprdat
		xAsync();
	};
	std::map<_tstring, xSpr::XSPDat> m_mapDat;		// 현재 참조되고 있는 자원
	std::map<_tstring, xSpr::XSPDat> m_mapCache;	// 현재 참조되고 있지는 않지만 캐시에 남아있는 자원.
	XList4<xAsync> m_listAsync;		// 비동기로 로딩이 예약된 스프라이트들
	XList4<xAsync> m_listAsyncComplete;		// 비동기 로딩이 완료된 스프라이트
	XSPLock m_spLock;
	uintptr_t m_hThread = 0;
	ID m_idThread = 0;
	void Init( void ) {
	}
	void Destroy( void );
public:
	XSprMng();
	~XSprMng() {
		XTRACE( "destroy sprmng" ); Destroy();
	}
	GET_ACCESSOR( XSPLock, spLock );
	static unsigned int __stdcall _WorkThread( void *param );
	void WorkThread();
	void OnCreate();
	void Process();
	XSprDat *Load( LPCTSTR szFilename,
								 const XE::xHSL& hsl/* = XE::xHSL()*/,
								 bool bUseAtlas,
//								 BOOL bAddRefCnt = TRUE,
								 BOOL bSrcKeep = FALSE,
								 bool bAsyncLoad = false,
								 ID* pOutidAsync =nullptr	);
	void RestoreDevice();
	void Release( XSprDat *pSprDat );
	void DoFlushCache();
#ifdef WIN32
	void Reload();
#endif // WIN32
	void OnPause();
	XSprDat* FindSprDatByidAsync( ID idAsync ) {
		auto spDat = FindByidAsync( idAsync );
		if( spDat )
			return spDat->m_pSprDat;
		return nullptr;
	}
private:
//	void CheckRelease( void );
	xSpr::XSPDat AddNew( const _tstring& strKey, const _tstring& strFile, XSprDat *pSprDat, const XE::xHSL& hsl );
	void MoveCacheToRefMap( const xSpr::XSPDat& spDat );
	xSpr::XSPDat FindByKey( LPCTSTR szFilename );
	xSpr::XSPDat FindByKeyAtCache( LPCTSTR szFilename );
	inline xSpr::XSPDat FindByKey( const _tstring& strKeyFile ) {
		return FindByKey( strKeyFile.c_str() );
	}
	inline xSpr::XSPDat FindByKeyAtCache( const _tstring& strKeyFile ) {
		return FindByKeyAtCache( strKeyFile.c_str() );
	}
	xSpr::XSPDat FindByidAsync( ID idAsync );
	void CompleteAsyncLoad( ID idAsync );
	void DestroyOlderFile();
//	void DeleteAsyncComplete( ID idAsync );
};


extern XSprMng *SPRMNG;

#endif // _XASYNC_SPR
