#pragma once

#include "xe.h"
#include "global.h"
#include "XList.h"
#include "ResMng.h"
#include "Alert.h"
#ifdef WIN32
#include "XGraphicsD3D.h"
#endif

#define VER_TILEPACK		3

#ifdef WIN32
class XTilePage
{
	LPDIRECT3DTEXTURE9 m_pTexture; 
	void Init() {
		m_pTexture = NULL;
	}
public:
	XTilePage( DWORD *pSrc, int w, int h );
	virtual ~XTilePage();
	//
	GET_ACCESSOR( LPDIRECT3DTEXTURE9, pTexture );
};
#else // win32
class XTilePage
{
	GLuint m_glTexture;
	void Init() {
		m_glTexture = 0;
	}
public:
	XTilePage( DWORD *pSrc, int w, int h );
	virtual ~XTilePage();
	//
	GET_ACCESSOR( GLuint, glTexture );
};
#endif // GL


// 타일들의 묶음
// *.tpk 
// 하드웨어별로 최대페이지 크기가 다르므로 tpk상에서는 일렬로 늘어선 메모리덩어리를 갖고 있게 한다.
// 로딩하면서 최대 페이지 크기만큼 메모리를 할당받은다음 이곳으로 옮기고 텍스쳐로 만든다.
class XTilePack
{
	int m_nRefCnt;
	void Init() {
		m_Version = 0;
		m_nSizeTileTexture = 0;
		m_nRefCnt = 0;
		m_NumPage = 0;
		XCLEAR_ARRAY( m_pPages );
		XCLEAR_ARRAY( m_szTPK );
	}
	void Destroy() {
		for( int i = 0; i < m_Tiles.GetNum(); ++i )
			m_Tiles[i].Destroy();
		for( int i = 0; i < MAX_PAGE; ++i )
			SAFE_DELETE( m_pPages[i] );
	}
public:
	enum { MAX_PAGE=5, MAX_TILE=4096 };
	struct TILE {
		ID idTile;					// 타일 고유아이디
		BYTE *pMask;		// 충돌 마스크. 이 어드레스가 1이면 마스킹이 투명색하나도 없이 꽉차있는 타일. 
		DWORD *pImg;		// 실제 타일 이미지
	#ifdef _XTOOL
		char cImg[ 32 ];	// 타일이미지가 있던 원본 이미지파일명(패스는 없음)
		int xOffset, yOffset;	// 타일이미지가 있던 위치.
	#endif
		// page에 필요한것
		int nPage;			// 속한 페이지 번호
		float u, v;				// 타일의 uv좌표
		float u2, v2;
		TILE() {
			idTile = 0;
			nPage = 0;
			u = v = 1.0f;
			u2 = v2 = 1.0f;
			pMask = NULL;
			pImg = NULL;
	#ifdef _XTOOL
			XCLEAR_ARRAY( cImg );
			xOffset = yOffset = 0;
	#endif
		}
		void Destroy() {
			SAFE_DELETE_ARRAY( pImg );
			if( (int)pMask != 1 )
				SAFE_DELETE_ARRAY( pMask );
		}
	};
protected:
	DWORD m_Version;
	TCHAR m_szTPK[ 32 ];
	int m_wTile, m_hTile;			// 타일 가로 세로 크기
	int m_wTileTexture, m_hTileTexture;	// 타일텍스쳐의 크기(iPhone4의 경우 텍스쳐의 크기는 64x64이고 m_Size는 32x32이다).
	int m_nSizeTileTexture;				// 타일크기 미리 곱해놓은거.
	int m_NumPage;		// 페이지 개수
	XTilePage *m_pPages[ MAX_PAGE ];	// 타일페이지 리스트
#ifdef _XTOOL
	XArrayList<TILE> m_Tiles;					
#else
	XArrayLinear<TILE> m_Tiles;					
#endif
//	XArrayList<DWORD> m_TilesImg;
	void SetTileSize( int tw, int th ) { m_wTile = tw; m_hTile = th; }
	void SetTileTextureSize( int ttw, int tth ) { m_wTileTexture = ttw; m_hTileTexture = tth; m_nSizeTileTexture = ttw * tth; }
public:
	// 타일 가로세로 크기. w/hTileTexture:타일텍스쳐의 크기
	XTilePack( int wTile, int hTile, int wTileTexture, int hTileTexture, LPCTSTR szName ) { 
		Init(); 
		m_Tiles.Create( MAX_TILE );
		TILE tileEmpty;
		m_Tiles.Add( tileEmpty );		// 0번타일은 투명타일이다.
		SetTileSize( wTile, hTile );
		SetTileTextureSize( wTileTexture, hTileTexture );
		m_nSizeTileTexture = wTileTexture * hTileTexture;
		if( szName && XE::IsHave( szName ) )
			_tcscpy_s( m_szTPK, XE::Format( _T("%s.tpk"), szName ) );
	}
	XTilePack() {	Init();	}
	// 테스트용 생성자
//	XTilePack( int wTile, int hTile, int wTileTexture, int hTileTexture, int num );
	virtual ~XTilePack() { Destroy(); }
	//
	BOOL IsUpperVersion( DWORD nVersion ) { return (m_Version >= nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 같거나 높은가
	BOOL IsLowerVersion( DWORD nVersion ) { return (m_Version < nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 버전이 낮은가
	GET_ACCESSOR( int, nRefCnt );
	int IncRefCnt( void ) { return ++m_nRefCnt; }
	int DecRefCnt( void ) { return --m_nRefCnt; }
	const TILE& GetTile( int idx ) { return m_Tiles[ idx ]; }
	const TILE* GetpTile( int idx ) { return &m_Tiles[ idx ]; }
	DWORD* GetTileImage( int idx ) { return m_Tiles[ idx ].pImg; }
	void GetTileSize( int *pOutW, int *pOutH ) { 
		*pOutW = m_wTile;
		*pOutH = m_hTile;
	}
	void GetTileTextureSize( int *pOutW, int *pOutH ) { 
		*pOutW = m_wTileTexture;
		*pOutH = m_hTileTexture;
	}
	XE::VEC2 GetvTileSize( void ) { return XE::VEC2( m_wTile, m_hTile ); }
	XE::VEC2 GetvTileTextureSize( void ) { return XE::VEC2( m_wTileTexture, m_hTileTexture ); }
	GET_ACCESSOR( LPCTSTR, szTPK );
	void SetszTPK( LPCTSTR szTPK ) { _tcscpy_s( m_szTPK, szTPK ); }
	int GetnNum( void ) { return m_Tiles.GetNum(); }
	int GetMax( void ) { return m_Tiles.GetMaxSize(); }
	GET_ACCESSOR( int, NumPage );
	GET_ACCESSOR( int, wTile );
	GET_ACCESSOR( int, hTile );
	GET_ACCESSOR( int, wTileTexture );
	GET_ACCESSOR( int, hTileTexture );
	BYTE GetTileMask( int idxTile, int xOfs, int yOfs ) {
		const TILE *pTile = GetpTile( idxTile );
		if( pTile->pMask == NULL )
			return 0;
		else if( pTile->pMask == (BYTE*)0x01 )
			return 255;
		else
			return pTile->pMask[ (yOfs*2) * m_wTileTexture + (xOfs*2) ];
	}
	//
	BOOL Load( LPCTSTR szTilePack, BOOL bMakePage=FALSE, BOOL bSrcKeep=FALSE );
	void MakePage( void );
	void Draw( float x, float y, int idx, float alpha=1.0f );
	void DrawMask( float x, float y, int idx, float alpha );
	void Draw( const XE::VEC2& vPos, int idx, float alpha ) { Draw( vPos.x, vPos.y, idx, alpha ); }
	void DrawMask( const XE::VEC2& vPos, int idx, float alpha ) { DrawMask( vPos.x, vPos.y, idx, alpha ); }
	int AddTile( const TILE& tile );
	TILE CreateTile( DWORD *pTileMem );
//	void CreateTileDeviceBuffer( TILE *pTile );
#ifdef __TILE_DRAW1
	void CreateTileSurface( TILE *pTile, DWORD *pMaskMem );
#endif
	
//	void _CreateEmptyTiles( int num );
	//
#if defined(__TILE_DRAW2) || defined(__TILE_DRAW3)
	virtual void UploadTilePage( int page );
#endif
	//
	//
	virtual BOOL LoadTPXFromTPK( LPCTSTR szTPK ) { return TRUE; }
};
/*
class XTilePackDX : public XTilePack
{
	XSurface *m_psfcPage;
	void Init() {
	}
	void Destroy() {
	}
public:
	XTilePackDX( LPCTSTR szTilePack ) { 
		Init(); 
		Load( szTilePack );
	}
	virtual ~XTilePackDX() { Destroy(); }
};
*/

