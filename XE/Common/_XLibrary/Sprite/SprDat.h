#pragma once
#include "etc/xColor.h"
#include "Layer.h"
#include "Key.h"
#include "Sprite.h"
#include "XActDat.h"

#include <list>

class XSprDat;
class XActObj;
class XBaseRes;

/**
 ver32: KeyEvent의 idObj파라메터가 2바이트에서 4바이트로 변경
*/
class XSprDat
{
public:
	enum { SPR_VER = 33,
				MAX_ID = 100 };
	//////////////////////////////////////////////////////////////////////////
private:
	ID m_snDat = 0;
//	int m_nRefCnt;
	BOOL m_bKeepSrc;				// 이미지데이타를 메모리에 보존
	_tstring m_strFile;
	int m_indexFromID[ MAX_ID ];							// 실제 게임에서 로딩할때 액션아이디에 대응하는배열에 액션리스트의 인덱스가 들어있다
	int m_nVersion;
//	int m_nNumSprites;
//	XSprite **m_ppSprites;						
	XVector<XSprite*> m_arySprite;
	int m_nNumActions;
	XActDat **m_ppActions;					
	float m_fScaleFactor;						// 고해상도 이미지가 들어있으면 이것이 2가 된다.
	int m_nRepeat;								// 1회플레이, 루핑, 왔다갔다 등
	char *m_pcLuaAll;							// .spr파일 안에서 쓰는 모든 루아코드를 여기에 합친다
	int m_SizeByte;			///< 스프라이트 이미지들의 총용량
	bool m_bUseAtlas = false;
	XE::xHSL m_HSL;
//	bool m_bLoadComplete = false;			// spr파일로딩및 디바이스 텍스쳐생성까지 모두 끝남.
	void Init() {
//		m_nRefCnt = 0;
//		m_bHighReso = TRUE;
		m_bKeepSrc = FALSE;
		m_nVersion = 0;
		memset( m_indexFromID, -1, sizeof(m_indexFromID) );
		m_fScaleFactor = 1.0f;
		m_nRepeat = 0;
//		memset( m_szFilename, 0, sizeof(m_szFilename) );
//		m_ppSprites = nullptr;
		m_ppActions = nullptr;
		m_nNumActions = 0;
		m_pcLuaAll = nullptr;
		m_SizeByte = 0;
	}
	void Destroy();
	
	void AddAction( int idx, XActDat *pAction );
public:
	XSprDat() { 
		Init(); 
		m_snDat = XE::GenerateID();
	}
	~XSprDat() { Destroy(); }
	
	GET_ACCESSOR_CONST( ID, snDat );
//	GET_BOOL_ACCESSOR( bLoadComplete );
	inline LPCTSTR GetszFilename() const {
		return m_strFile.c_str();
	}
	GET_SET_ACCESSOR_CONST( const _tstring&, strFile );
	inline int GetnNumSprites() const {
		return m_arySprite.Size();
	}
	GET_ACCESSOR_CONST( int, nNumActions );
	GET_ACCESSOR( const char*, pcLuaAll );
	GET_ACCESSOR_CONST( BOOL, bKeepSrc );
	GET_ACCESSOR_CONST( int, SizeByte );
	inline bool IsUpperVersion( int nVersion ) const { 
		return (m_nVersion >= nVersion); 
	}	// 이 파일이 nVersion보다 같거나 높은가
	inline bool IsLowerVersion( int nVersion ) const { 
		return (m_nVersion < nVersion); 
	}	// 이 파일이 nVersion보다 버전이 낮은가

	// file
	BOOL Load( LPCTSTR szFilename, 
						 bool bUseAtlas, 
						 bool bAsyncLoad, 
						 const XE::xHSL& hsl,
						 BOOL bSrcKeep=FALSE, 
						 BOOL bRestore=FALSE );
	void CreateDevice();
	BOOL RestoreDevice() {
#ifdef _XASYNC_SPR
		return Load( nullptr, m_bUseAtlas, true, m_HSL, FALSE, TRUE );
#else
		return Load( nullptr, m_bUseAtlas, false, m_HSL, FALSE, TRUE );
#endif // _XASYNC_SPR
	}
	//key
	
	// action
	XActDat* GetActionIndex( int index ) { return m_ppActions[ index ]; }
	inline int GetActionIndexFromID( DWORD id ) const { return m_indexFromID[ id ]; }
	XActDat* GetAction( DWORD id ) {
		int idx = m_indexFromID[id];
#ifdef _XDEBUG
		if( XBREAKF( idx == -1, "%s에 %d ID의 액션이 없다", m_strFile.c_str(), id ) ) 
			idx = 0;		// 다운은 안되도록
#endif
		return m_ppActions[ idx ]; 
	}
	/**
	 @brief idAct액션이 있는지 검사
	*/
	inline BOOL IsHaveAction( ID idAct ) {
		return m_indexFromID[idAct] != -1;
	}	
	// sprite
	XSprite* GetSprite( int nSpr ) { 
		if( XBREAK( nSpr >= GetnNumSprites() || nSpr < 0 ) ) {
			return nullptr;
		}
		XBREAK( m_arySprite[ nSpr ] == nullptr );
		return m_arySprite[ nSpr ];
	}
#ifdef _XSPRITE2
	XSurface* GetSpriteSurface( int idxSpr ) { 
		if( XBREAK( idxSpr >= GetnNumSprites() || idxSpr < 0 ) ) {
			return nullptr;
		}
//		XSprite *pSpr = m_ppSprites[ nSpr ];
		auto pSpr = m_arySprite[ idxSpr ];
		XBREAK( pSpr == nullptr );
		if( pSpr )
			return pSpr->GetpSurface();
		return nullptr; 
	}
#endif
	void AddSprite( /*int idx, */XSprite *pSpr );
	XSprite* AddSprite( int nWidth, int nHeight, int nAdjustX, int nAdjustY, DWORD *pImg );
#ifdef WIN32
	void Reload();
#endif // WIN32
	void DestroyDevice();
	
};

typedef std::list<XSprDat *>				XSprDat_List;
typedef std::list<XSprDat *>::iterator	XSprDat_Itor;
