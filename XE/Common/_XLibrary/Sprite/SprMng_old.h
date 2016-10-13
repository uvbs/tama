#pragma once

#ifndef _XASYNC_SPR
// 여기 있는 코드는 구코드를 참고할필요가 있을때만 사용한다.
// 신코드(비동기)가 완전히 완성되면 구코드를 버리도록 한 용도이다.
// 이곳에 새 코드를 넣지 말아야 한다.
#include "VersionXE.h"
#include "etc/XGraphicsDef.h"
#include "XFType.h"
class XSprDat;
class XSprObj;
enum xRPT_TYPE : int;

XE_NAMESPACE_START( xSpr )
//
struct xDat {
	ID m_idDat = 0;
	_tstring m_strKey;			// HSL로딩이 있기때문에 pSprDat의 파일명과 일치하지 않을 수 있음.
	XSprDat *m_pSprDat = nullptr;
	XE::xHSL m_HSL;
	xSec m_secLoaded = 0;			// 파일을 로드한 시간
	ID m_idAsync = 0;					// 비동기로딩중이면 아이디가 있다.
	xDat() : m_idDat( XE::GenerateID() ) {}
	xDat( const _tstring& strKey, XSprDat* _pSprDat, const XE::xHSL& hsl )
		: m_idDat(XE::GenerateID()), m_strKey( strKey ), m_pSprDat( _pSprDat ), m_HSL( hsl ) { }
	xDat( const _tstring& strKey, XSprDat* _pSprDat )
		: m_idDat(XE::GenerateID()), m_strKey( strKey ), m_pSprDat( _pSprDat ) { }
	~xDat();
	inline ID getid();
	bool IsOverTime() const;
};
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
#ifdef _XASYNC_SPR
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
#endif // _XASYNC_SPR
//	XList4<xSpr::XSPDat> m_listSprDat;
//	XList4<xSpr::XSPDat> m_listPreLoad;
//	XList4<xSpr::XSPDat> m_listCache;		// use_count=0가 되었을때 메인리스트에선 삭제되지만 캐시에는 남는다. 다시 로딩이 필요할때 여기서 찾아서 로딩해 파일I/O시간을 줄인다.
	std::map<_tstring, xSpr::XSPDat> m_mapDat;		// 현재 참조되고 있는 자원
	std::map<_tstring, xSpr::XSPDat> m_mapCache;	// 현재 참조되고 있지는 않지만 캐시에 남아있는 자원.
#ifdef _XASYNC_SPR
	XList4<xAsync> m_listAsync;		// 비동기로 로딩이 예약된 스프라이트들
	XList4<xAsync> m_listAsyncComplete;		// 비동기 로딩이 완료된 스프라이트
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
	GET_ACCESSOR( XSPLock, spLock );
	static unsigned int __stdcall _WorkThread( void *param );
	void WorkThread();
//	void AsyncSetAction( ID idAsync, ID idAct, xRPT_TYPE playType );
	void OnCreate();
//	xAsync* GetpAsyncInfo( ID idAsync );
//	const XSprMng::xAsync* GetpAsyncComplete( ID idAsync );
	void Process();
#endif // _XASYNC_SPR
	void CheckRelease( void );
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
	xSpr::XSPDat AddNew( const _tstring& strKey, XSprDat *pSprDat, const XE::xHSL& hsl );
	void MoveCacheToRefMap( const _tstring& strKey );
	xSpr::XSPDat FindByKey( LPCTSTR szFilename, BOOL bSrcKeep, const XE::xHSL& hsl );
	xSpr::XSPDat FindByKeyAtCache( LPCTSTR szFilename, BOOL bSrcKeep, const XE::xHSL& hsl );
	xSpr::XSPDat FindByidAsync( ID idAsync );
	void CompleteAsyncLoad( ID idAsync );
	void DestroyOverTimeFile();
//	void DeleteAsyncComplete( ID idAsync );
};


extern XSprMng *SPRMNG;

#endif // not _XASYNC_SPR
