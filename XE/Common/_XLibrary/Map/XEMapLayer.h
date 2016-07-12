#pragma once
#ifdef _XEMAP

class XEMap;
// 맵레이어 기본형
#ifdef _XTOOL
#include "Select.h"
class XEMapLayer : public XBaseElem
#else
class XEMapLayer
#endif
{
public:
	enum xtType { xTYPE_NONE=0, xTYPE_TILE, xTYPE_IMAGE };
private:
	void Init() {
		m_idLayer = 0;
		m_Type = xTYPE_NONE;
		m_prefMap = NULL;
		m_bVisible = TRUE;
		m_Alpha = 1.0f;
		m_bShowMask = TRUE;
//		XCLEAR_ARRAY( m_szName );
	}
	void Destroy() {}
protected:
	ID m_idLayer;
	xtType m_Type;
//	TCHAR m_szName[ 32 ];
	XEMap *m_prefMap;		// 이 레이어를 소유하고 있는 맵의 포인터
	BOOL m_bVisible;		// 레이어 보임/안보임.
	BOOL m_bShowMask;	// 충돌마스크 보임 안보임.
	float m_Alpha;			
	XE::VEC2 m_vDrawOffset;		// 약간씩 비껴찍을 수 있게 함.
public:
	XEMapLayer( XEMap *pMap, xtType type, ID idLayer=0, LPCTSTR szName=NULL ) { 
		Init(); 
		if( idLayer == 0 )		// 지정한 번호가 없으면 번호생성.
			m_idLayer = XE::GenerateID();
		else
			m_idLayer = idLayer;
//		if( szName )
//			_tcscpy_s( m_szName, szName );
		m_Type = type;
		m_prefMap = pMap;
	}
	virtual ~XEMapLayer() { Destroy(); }
	//
//	GET_ACCESSOR( LPCTSTR, szName );
//	void SetszName( LPCTSTR szName ) {
//		if( szName )
//			_tcscpy_s( m_szName, szName );
//	}
	GET_ACCESSOR( xtType, Type );
	GET_ACCESSOR( ID, idLayer );
	GET_SET_ACCESSOR( BOOL, bVisible );
	GET_SET_ACCESSOR( float, Alpha );
	GET_SET_ACCESSOR( XE::VEC2&, vDrawOffset );
	GET_SET_ACCESSOR( BOOL, bShowMask );
	//
	virtual void FrameMove( float dt ) {}
	virtual void Draw( const XE::VEC2& vsLT ) {}
	virtual void Load( XBaseRes *pRes, LPCTSTR szPath );
	virtual BYTE GetCollision( float wx, float wy ) { return 0; }
	// tool 관련. 나중에 다른 클래스로 빠질듯.
	virtual void DrawLayer( int y, int cw, int ch, BYTE alpha=255 ) {}
#ifdef _VER_IOS
	virtual BOOL RestoreDevice( void ) { return TRUE; }
#else
	// 로딩부에서 XMapLayer만 단독으로 쓰는게 있어서 pure로 못씀.
	virtual BOOL RestoreDevice( void ) {XBREAKF(1, "not implement restore"); return FALSE;}
#endif
//	virtual void Save( XResFile *pRes );
//	virtual void PaintBrush( int wx, int wy, XTileBrush *pBrush );
//	virtual void EraseBrush( int wx, int wy, XTileBrush *pBrush );
	friend class XEMap;
};
#endif // _XEMAP
