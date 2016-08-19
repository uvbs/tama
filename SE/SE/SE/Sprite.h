#pragma once

#include "XSurface.h"
#include "Select.h"
#include "XSurfaceD3D.h"
#define EFF_FLIP_HORIZ		0x01		// 좌우 플립
#define EFF_FLIP_VERT			0x02		// 상하 플립

class XSprDat;

class XSprite : public XBaseElem, public XSurfaceD3D
{
	ID m_idSpr;
	float m_fPriority;					// 소트시킬때 기준이 되는 변수
	int m_nIdx;
	DWORD m_dwDrawFlag;		// EFF_****
	TCHAR m_szSrcFile[1024];		// 스프라이트의 원본 png파일명을 저장함
	RECT m_rectSrc;					// 원본 png에서 영역을 표시함. 전부0이면 전체를 의미함
	XE::xtPixelFormat m_formatSurface = XE::xPF_ARGB4444;		// 디바이스 텍스쳐를 생성할때 써야하는 서피스 포맷. 게임에서만 사용.
	void Init()	{
		m_idSpr = XE::GenerateID();
		m_fPriority = 0;
		m_nIdx = 0;
		m_dwDrawFlag = 0;
		memset( m_szSrcFile, 0, sizeof(m_szSrcFile) );
		memset( &m_rectSrc, 0, sizeof(m_rectSrc) );
	}
	void Destroy();
public:
	virtual void ValueAssign( const XSurface *pSrc ) {
//		XSurfaceD3D::ValueAssign( pSrc );
		*this = *(dynamic_cast<const XSprite *>(pSrc));
	}
	XSprite( BOOL bHighReso ) : XSurfaceD3D( bHighReso ) { Init(); }
	XSprite( BOOL bHighReso, float width, float height, float adjustX, float adjustY, DWORD *pImg );
	virtual ~XSprite() { Destroy(); }

	GET_SET_ACCESSOR( ID, idSpr );
	GET_SET_ACCESSOR( float, fPriority );
	GET_SET_ACCESSOR( int, nIdx );
	GET_ACCESSOR( LPCTSTR, szSrcFile );
	GET_ACCESSOR( const RECT&, rectSrc );
	GET_SET_ACCESSOR_CONST( XE::xtPixelFormat, formatSurface );
	void SetFlipHoriz( BOOL bFlag ) { ( bFlag ) ? m_dwDrawFlag |= EFF_FLIP_HORIZ : m_dwDrawFlag &= ~EFF_FLIP_HORIZ; }
	void SetFlipVert( BOOL bFlag ) { ( bFlag ) ? m_dwDrawFlag |= EFF_FLIP_VERT : m_dwDrawFlag &= ~EFF_FLIP_VERT; }
	void SetSrcInfo( LPCTSTR szSrcFile, const RECT &rect ) {
		if( szSrcFile )
			_tcscpy_s( m_szSrcFile, szSrcFile );
//		memcpy( &m_rectSrc, &rect, sizeof(RECT) );
		m_rectSrc = rect;
	}
	BOOL IsSameImage( DWORD *pImg );
	
	void Draw( const XE::VEC2& vPos ) { Draw( vPos.x, vPos.y ); }
	void Draw( const XE::VEC2& vPos, const D3DXMATRIX &m ) { Draw( vPos.x, vPos.y, m ); }
	void Draw( float x, float y ) { 
		D3DXMATRIX m; 
		D3DXMatrixIdentity( &m ); 
		Draw( x, y, m ); 
	}
	void Draw( float x, float y, const D3DXMATRIX &m );
	void DrawNoAdjust( float x, float y );
	void Save( XResFile *pRes );
	void Load( XResFile *pRes, int ver );
//	DWORD GetPixel( float lx, float ly, BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL  );	// 이것은 장차 XSurfaceD3D로 가야한다
	D3DXMATRIX* GetMatrix( D3DXMATRIX *pOut, float lx, float ly );

	virtual XSprite* Copy( BOOL bPropOnly=FALSE ) {
		XSprite *pSprite = new XSprite( GetbHighReso() );
		pSprite->ValueAssign( this );
		return pSprite;
	}

	BOOL ConvertLowReso();
};
