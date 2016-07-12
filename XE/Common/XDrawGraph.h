#pragma once

class XSurface;
namespace XUTIL 
{
	void DrawProgressBar( const XE::VEC2& vPos, float lerp, XE::VEC2 vSize, XCOLOR col=XCOLOR_GREEN, XCOLOR _colBack=0);
	inline void DrawProgressBar( const XE::VEC2& vPos, float current, float max, XE::VEC2 vSize, XCOLOR col=XCOLOR_GREEN ) {
		DrawProgressBar( vPos, current / max, vSize, col );
	}
	template<typename T>
	void DrawBarHoriz( const XE::VEC2& vPos, T current, T max, XSurface *pSurface ) {
		DrawBarHoriz( vPos, (float)current / max, pSurface );
	}

	void DrawBarHoriz( const XE::VEC2& vPos, float lerp, XSurface *pSurface, bool bReverse = false );
	void DrawBarHoriz2( const XE::VEC2& vPos, float lerp, XSurface *pSurface, const XE::VEC2& vScale, bool bReverse = false );

	template<typename T>
	void DrawBarVert( const XE::VEC2& vPos, T current, T max, XSurface *pSurface ) {
		DrawBarVert( vPos, (float)current / max, pSurface );
	}
	void DrawBarVert( const XE::VEC2& vPos, float lerp, XSurface *pSurface, bool bReverse = false );

};



class XProgressBar
{
	BOOL m_bHoriz;		// 가로형
	XE::VEC2 m_vPos;
	XE::VEC2 m_vSize;
	XSurface *m_pSurface;
	XSurface *m_psfcBg;
	XE::VEC2 m_vAdjBar;	// 앞에 바의 찍히는 위치 보정.
	bool m_bReverse = false;
	void Init() {
		m_bHoriz = TRUE;
		m_pSurface = NULL;
		m_psfcBg = NULL;
	}
	void Destroy();
public:
	XProgressBar( LPCTSTR szImg, LPCTSTR szImgBg=nullptr, BOOL bHoriz=TRUE );
	XProgressBar( const XE::VEC2& vPos, LPCTSTR szImg, LPCTSTR szImgBg=nullptr, BOOL bHoriz=TRUE );
	XProgressBar( const XE::VEC2& vPos, const XE::VEC2& vSize, BOOL bHoriz=TRUE );
	virtual ~XProgressBar() { Destroy(); }
	//
	GET_ACCESSOR( XSurface*, pSurface );
	GET_ACCESSOR( XSurface*, psfcBg );
	GET_SET_ACCESSOR( const XE::VEC2&, vAdjBar );
	GET_SET_ACCESSOR( bool, bReverse );
	void Draw( float lerp );
	void Draw( const XE::VEC2& vPos, float lerp );
	void Draw( const XE::VEC2& vPos, const XE::VEC2& vSize, float lerp );
};