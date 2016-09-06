#pragma once
#include "etc/xColor.h"
#include "Layer.h"
#include "Key.h"
#include "Sprite.h"
#include "XActObj.h"

#include <list>

class XSprDat;
class XObjAct;
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
	int m_nRefCnt;
//	BOOL m_bHighReso;			// 고해상도 스프라이트(아이폰 전용)
	BOOL m_bKeepSrc;				// 이미지데이타를 메모리에 보존
	TCHAR m_szFilename[ 256 ];
	int m_indexFromID[ MAX_ID ];							// 실제 게임에서 로딩할때 액션아이디에 대응하는배열에 액션리스트의 인덱스가 들어있다
	int m_nVersion;
	int m_nNumSprites;
	XSprite **m_ppSprites;						
	int m_nNumActions;
	XAniAction **m_ppActions;					
	float m_fScaleFactor;						// 고해상도 이미지가 들어있으면 이것이 2가 된다.
	int m_nRepeat;								// 1회플레이, 루핑, 왔다갔다 등
	char *m_pcLuaAll;							// .spr파일 안에서 쓰는 모든 루아코드를 여기에 합친다
	int m_SizeByte;			///< 스프라이트 이미지들의 총용량
	bool m_bUseAtlas = false;
	void Init( void ) {
		m_nRefCnt = 0;
//		m_bHighReso = TRUE;
		m_bKeepSrc = FALSE;
		m_nVersion = 0;
		memset( m_indexFromID, -1, sizeof(m_indexFromID) );
		m_fScaleFactor = 1.0f;
		m_nRepeat = 0;
		memset( m_szFilename, 0, sizeof(m_szFilename) );
		m_ppSprites = NULL;
		m_ppActions = NULL;
		m_nNumActions = m_nNumSprites = 0;
		m_pcLuaAll = NULL;
		m_SizeByte = 0;
	}
	void Destroy( void );
	
	void AddAction( int idx, XAniAction *pAction );
public:
	XSprDat() { 
		Init(); 
		m_snDat = XE::GenerateID();
	}
	~XSprDat() { Destroy(); }
	
	GET_ACCESSOR( ID, snDat );
//	GET_ACCESSOR( BOOL, bHighReso );
	GET_ACCESSOR( LPCTSTR, szFilename );
	GET_ACCESSOR( int, nNumSprites );
	GET_ACCESSOR( int, nNumActions );
	GET_ACCESSOR( int, nRefCnt );
	GET_ACCESSOR( const char*, pcLuaAll );
	GET_ACCESSOR( BOOL, bKeepSrc );
	GET_ACCESSOR( int, SizeByte );
	BOOL IsUpperVersion( int nVersion ) { return (m_nVersion >= nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 같거나 높은가
	BOOL IsLowerVersion( int nVersion ) { return (m_nVersion < nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 버전이 낮은가
	
	void AddRefCnt( void ) { ++m_nRefCnt; }
	void DecRefCnt( void ) { --m_nRefCnt; }
	// file
	BOOL Load( LPCTSTR szFilename, bool bUseAtlas, BOOL bSrcKeep=FALSE, BOOL bRestore=FALSE );
	BOOL RestoreDevice( void ) {
		return Load( NULL, FALSE, TRUE );
	}
	//key
	
	// action
	XAniAction* GetActionIndex( int index ) { return m_ppActions[ index ]; }
	int GetActionIndexFromID( DWORD id ) { return m_indexFromID[ id ]; }
	XAniAction* GetAction( DWORD id ) {
		int idx = m_indexFromID[id];
#ifdef _XDEBUG
		if( XBREAKF( idx == -1, "%s에 %d ID의 액션이 없다", m_szFilename, id ) ) 
			idx = 0;		// 다운은 안되도록
#endif
		return m_ppActions[ idx ]; 
	}
	/**
	 @brief idAct액션이 있는지 검사
	*/
	BOOL IsHaveAction( ID idAct ) {
		return m_indexFromID[idAct] != -1;
	}	
	// sprite
	XSprite* GetSprite( int nSpr ) { 
		if( XBREAK( nSpr >= m_nNumSprites || nSpr < 0 ) ) {
			return NULL;
		}
		XBREAK( m_ppSprites[ nSpr ] == NULL );
		return m_ppSprites[ nSpr ]; 
	}
#ifdef _XSPRITE2
	XSurface* GetSpriteSurface( int nSpr ) { 
		if( XBREAK( nSpr >= m_nNumSprites || nSpr < 0 ) ) {
			return NULL;
		}
		XSprite *pSpr = m_ppSprites[ nSpr ];
		XBREAK( pSpr == NULL );
		if( pSpr )
			return pSpr->GetpSurface();
		return NULL; 
	}
#endif
	void AddSprite( int idx, XSprite *pSpr );
	XSprite* AddSprite( int nWidth, int nHeight, int nAdjustX, int nAdjustY, DWORD *pImg );
#ifdef WIN32
//	void DestroyDeviceForWin32();
	void Reload();
#endif // WIN32
	void DestroyDevice();
	
};

typedef std::list<XSprDat *>				XSprDat_List;
typedef std::list<XSprDat *>::iterator	XSprDat_Itor;
