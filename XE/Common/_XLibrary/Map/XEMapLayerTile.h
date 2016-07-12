#pragma once

#include "XETilePack.h"
#include "etc/XGraphics.h"
#include "XEMapLayer.h"

class XVertexBuffer
{
	void Init() {
		m_nNumRect = 0;
		m_nMaxRect = 0;
	}
	void Destroy() {}
protected:
	int m_nNumRect;		// 현재추가된 사각형 개수. 
	int m_nMaxRect;			// 사각형 최대 개수
public:
	XVertexBuffer() { Init(); }
	virtual ~XVertexBuffer() { Destroy(); }

	virtual BOOL Create( int max )=0;
	virtual void Add( float x, float y, float w, float h, float u, float v, float u2, float v2 ) {}
	virtual BOOL Flush( void ) { return FALSE; }
	virtual void Draw( float x, float y ) {}
};

#ifdef _VER_DX
#include "_DirectX/XGraphicsD3D.h"
class XVertexBufferD3D : public XVertexBuffer
{
	LPDIRECT3DVERTEXBUFFER9 m_pd3dVB;
	XSURFACE_VERTEX *m_pList;
	void Init() {
		m_pd3dVB = NULL;
		m_pList = NULL;
	}
	void Destroy() {
		SAFE_RELEASE( m_pd3dVB );
		SAFE_DELETE_ARRAY( m_pList );
	}
public:
	XVertexBufferD3D() { Init(); }
	virtual ~XVertexBufferD3D() { Destroy(); }
	//
	virtual BOOL Create( int numRect );
	virtual void Add( float x, float y, float w, float h, float u, float v, float u2, float v2 );
	virtual BOOL Flush( void );
	virtual void Draw( float x, float y );
};
#else // dx
class XVertexBufferGL : public XVertexBuffer
{
	struct GL_VERTEX {
		GLfloat pos[2];
		GLfloat tex[2];
	};
	GLuint m_glVB;
	GL_VERTEX *m_pList;
	void Init() {
		m_glVB = 0;
		m_pList = NULL;
	}
	void Destroy() {
		glDeleteBuffers( 1, &m_glVB );
		SAFE_DELETE_ARRAY( m_pList );
	}
public:
	XVertexBufferGL() { Init(); }
	virtual ~XVertexBufferGL() { Destroy(); }
	//
	virtual BOOL Create( int numRect );
	virtual void Add( float x, float y, float w, float h, float u, float v, float u2, float v2 );
//	virtual void Add( int x, int y, int w, int h, float u, float v, float u2, float v2 );
	virtual BOOL Flush( void );
	virtual void Draw( float x, float y );
};
#endif // not dx

// 타일방식의 레이어
class XEMap;
class XEMapLayerTile : public XEMapLayer
{
	void Init() {
		m_Type = XEMapLayer::xTYPE_TILE;
		m_pTilePack = NULL;
		m_pMapDat = NULL;
		m_pVertexBuffer = NULL;
	}
	void Destroy() {
		SAFE_DELETE( m_pMapDat );
		SAFE_DELETE( m_pVertexBuffer );
	}
protected:
	short *m_pMapDat;		// 타일인덱스 2바이트짜리 맵데이타.
	int m_wMapTile;			// 타일단위 맵 크기
	int m_hMapTile;
	XTilePack *m_pTilePack;		// 타일 묶음 참조
	XVertexBuffer *m_pVertexBuffer;
public:
	// w,h: 타일단위 맵크기. w/hTile: 타일하나의 가로세로 크기
//	XMapLayerTile( XMap *pMap, int w, int h, int wTile, int hTile, int wTileTexture, int hTileTexture );
	// 맵 가로,세로 크기(타일단위), szTilePack:타일묶음 파일
	XEMapLayerTile( XEMap *pMap, ID idLayer=0, LPCTSTR szName=NULL )
		: XEMapLayer( pMap, xTYPE_TILE, idLayer, szName ) { 
			Init(); 
	}
	XEMapLayerTile( XEMap *pMap, int w, int h, XTilePack *pTilePack );
	// 
	virtual ~XEMapLayerTile() { Destroy(); }
	//
	GET_ACCESSOR( int, wMapTile );
	GET_ACCESSOR( int, hMapTile );
	GET_ACCESSOR( XTilePack*, pTilePack );
	void GetTileSize( int *pOutW, int *pOutH ) { 
		m_pTilePack->GetTileSize( pOutW, pOutH );
	}
	void SetMapData( int mx, int my, short idxTile ) { 
//		int w = (int)m_sizeMapTile.x;
		int w = m_wMapTile;
		m_pMapDat[ my * 	w + mx ] = idxTile;
	}
	int GetMapData( int mx, int my ) {
//		int w = (int)m_sizeMapTile.x;
		int w = m_wMapTile;
		return m_pMapDat[ my * 	w + mx ];
	}

	//
	void CreateVertexBuffer( XEMap *pMap );
	void DrawForOnePage( void );
	void DrawForMultiPage( void );
	//
	virtual void FrameMove( float dt );
	virtual void Draw( const XE::VEC2& vsLT );
	virtual void Load( XResFile *pRes, LPCTSTR szPath );
	virtual void DrawLayer( int y, int cw, int ch, BYTE alpha=255 ) {}
	virtual void OnDrawPerTile( const XE::VEC2& vPos, int idxTile ) {}
	virtual BYTE GetCollision( float wx, float wy );
	virtual BOOL RestoreDevice( void ) {XBREAKF(1, "not implement restore"); return FALSE;}	// 구현할것. 
};

