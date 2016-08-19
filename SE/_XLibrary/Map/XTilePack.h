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


// Ÿ�ϵ��� ����
// *.tpk 
// �ϵ����� �ִ������� ũ�Ⱑ �ٸ��Ƿ� tpk�󿡼��� �Ϸķ� �þ �޸𸮵���� ���� �ְ� �Ѵ�.
// �ε��ϸ鼭 �ִ� ������ ũ�⸸ŭ �޸𸮸� �Ҵ�������� �̰����� �ű�� �ؽ��ķ� �����.
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
		ID idTile;					// Ÿ�� �������̵�
		BYTE *pMask;		// �浹 ����ũ. �� ��巹���� 1�̸� ����ŷ�� ������ϳ��� ���� �����ִ� Ÿ��. 
		DWORD *pImg;		// ���� Ÿ�� �̹���
	#ifdef _XTOOL
		char cImg[ 32 ];	// Ÿ���̹����� �ִ� ���� �̹������ϸ�(�н��� ����)
		int xOffset, yOffset;	// Ÿ���̹����� �ִ� ��ġ.
	#endif
		// page�� �ʿ��Ѱ�
		int nPage;			// ���� ������ ��ȣ
		float u, v;				// Ÿ���� uv��ǥ
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
	int m_wTile, m_hTile;			// Ÿ�� ���� ���� ũ��
	int m_wTileTexture, m_hTileTexture;	// Ÿ���ؽ����� ũ��(iPhone4�� ��� �ؽ����� ũ��� 64x64�̰� m_Size�� 32x32�̴�).
	int m_nSizeTileTexture;				// Ÿ��ũ�� �̸� ���س�����.
	int m_NumPage;		// ������ ����
	XTilePage *m_pPages[ MAX_PAGE ];	// Ÿ�������� ����Ʈ
#ifdef _XTOOL
	XArrayList<TILE> m_Tiles;					
#else
	XArrayLinear<TILE> m_Tiles;					
#endif
//	XArrayList<DWORD> m_TilesImg;
	void SetTileSize( int tw, int th ) { m_wTile = tw; m_hTile = th; }
	void SetTileTextureSize( int ttw, int tth ) { m_wTileTexture = ttw; m_hTileTexture = tth; m_nSizeTileTexture = ttw * tth; }
public:
	// Ÿ�� ���μ��� ũ��. w/hTileTexture:Ÿ���ؽ����� ũ��
	XTilePack( int wTile, int hTile, int wTileTexture, int hTileTexture, LPCTSTR szName ) { 
		Init(); 
		m_Tiles.Create( MAX_TILE );
		TILE tileEmpty;
		m_Tiles.Add( tileEmpty );		// 0��Ÿ���� ����Ÿ���̴�.
		SetTileSize( wTile, hTile );
		SetTileTextureSize( wTileTexture, hTileTexture );
		m_nSizeTileTexture = wTileTexture * hTileTexture;
		if( szName && XE::IsHave( szName ) )
			_tcscpy_s( m_szTPK, XE::Format( _T("%s.tpk"), szName ) );
	}
	XTilePack() {	Init();	}
	// �׽�Ʈ�� ������
//	XTilePack( int wTile, int hTile, int wTileTexture, int hTileTexture, int num );
	virtual ~XTilePack() { Destroy(); }
	//
	BOOL IsUpperVersion( DWORD nVersion ) { return (m_Version >= nVersion)? TRUE : FALSE; }	// �� ������ nVersion���� ���ų� ������
	BOOL IsLowerVersion( DWORD nVersion ) { return (m_Version < nVersion)? TRUE : FALSE; }	// �� ������ nVersion���� ������ ������
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

