#pragma once
#ifdef _XASYNC_SPR
#include "VersionXE.h"
#include "etc/XGraphicsDef.h"
#include <thread>
class XSprDat;
class XSprObj;
class XActDat;
class XSprite;
class XTextureAtlas;
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
	bool m_bBatch = false;		// 배치모드로 서피스가 생성된것인지
	bool m_bUseAtlas = false;	// 아틀라스 사용여부. (m_bBatch가 false이어도 이게 true인 경우도 있다) 그러나 배치모드인데 아틀라스가 아닐수는 없다.
	xDat() : m_idDat( XE::GenerateID() ) {}
	xDat( const _tstring& strKey, const _tstring& strFile, XSprDat* _pSprDat, const XE::xHSL& hsl, bool bBatch, bool bUseAtlas )
		: m_idDat(XE::GenerateID())
		, m_strKey( strKey )
		, m_strLoadFile( strFile )
		, m_pSprDat( _pSprDat )
		, m_HSL( hsl )
		, m_bBatch( bBatch )
		, m_bUseAtlas( bUseAtlas ) { 
		XBREAK( m_bBatch && bUseAtlas == false );
	}
	~xDat();
	inline ID getid();
	bool IsOverTime() const;
	LPCTSTR GetszFilename() const;
	const XActDat* GetAction( ID idAct ) const;
	int GetnNumActions() const;
	const XActDat* GetActionIndex( int index ) const;
	const XSprite* GetSprite( int nSpr ) const;
	XSprite* GetSpriteMutable( int nSpr );
};
//
XE_NAMESPACE_END; // XE

//////////////////////////////////////////////////////////////////////////
class XSprMng {
public:
//	static int s_sizeTotalVM;
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
		bool m_bRestore = FALSE;
		bool m_bUseAtlas = false;
		bool m_bBatch = false;			// 배치모드로 서피스 생성
		XSprDat* m_pSprDat = nullptr;		// 비동기로딩이 완료되었을때 그 sprdat
		XSPAtlasMng m_spAtlasMng;
		xAsync();
	};
	std::map<_tstring, xSpr::XSPDat> m_mapDat;		// 로딩된 모든 spr
//	std::map<_tstring, xSpr::XSPDat> m_mapCache;	// 현재 참조되고 있지는 않지만 캐시에 남아있는 자원.
//	XList4<xSpr::xDat*> m_listAll;			// 로딩했던 모든 spr을 가진다. 여기서 가비지 컬렉팅을 한다.
	XList4<xAsync> m_listAsync;		// 비동기로 로딩이 예약된 스프라이트들
	XList4<xAsync> m_listAsyncComplete;		// 비동기 로딩이 완료된 스프라이트
	XSPLock m_spLock;
	XVector<std::shared_ptr<std::thread>> m_aryThread;
	XSprDat* m_pNowLoad = nullptr;			// 현재 읽고있는 파일의 임시변수
	int m_cnt = 0;
	void Init( void ) {	}
	void Destroy( void );
	void DestroyThread();
	void CreateThreadx();
public:
	XSprMng();
	~XSprMng() {
		XTRACE( "destroy sprmng" ); Destroy();
	}
	GET_ACCESSOR( XSPLock, spLock );
	void WorkThread();
	void OnCreate();
	void Process();
	void UpdateUV( ID idTex, const XE::POINT& sizePrev, const XE::POINT& sizeNew );
	xSpr::XSPDat Load( LPCTSTR szFilename,
										 const XE::xHSL& hsl/* = XE::xHSL()*/,
										 bool bUseAtlas,
										 BOOL bSrcKeep, // = FALSE,
										 bool bAsyncLoad, // = false,
										 bool bBatch,
										 ID* pOutidAsync ); // = nullptr );
	void RestoreDevice();
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
	xSpr::XSPDat AddNew( const _tstring& strKey, const _tstring& strFile, XSprDat *pSprDat, const XE::xHSL& hsl, bool bUseAtlas, bool bBatch );
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
	void DestroyOverTimeFile();
	xSpr::XSPDat DoFindExist( const _tstring& strFileKey, ID* pOutidAsync );
	xSpr::XSPDat LoadAsync( LPCTSTR szFileKey, 
													const _tstring& strFile, 
													const XE::xHSL& hsl, 
													bool bUseAtlas, 
													BOOL bSrcKeep, 
													bool bBatch, 
													bool bRestore,
													XSprDat* pSprDat,
													ID* pOutidAsync );
	void LoadAsync( xSpr::XSPDat spDat, bool bRestore );
	xSpr::XSPDat LoadSync( LPCTSTR szFileKey, const _tstring& strFile, const XE::xHSL& hsl, bool bUseAtlas, BOOL bSrcKeep, bool bBatch, bool bRestore );
	//	void DeleteAsyncComplete( ID idAsync );
	void Release( const xSpr::XSPDat& spDatRelease );
	void ReleaseByID( ID snDat );
};


extern XSprMng *SPRMNG;

#endif // _XASYNC_SPR
