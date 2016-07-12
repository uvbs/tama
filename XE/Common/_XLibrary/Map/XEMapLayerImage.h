#pragma once
#ifdef _XEMAP

#include "etc/XSurface.h"
#include "XEMapLayer.h"

// 이미지 하나의 통짜맵
class XEMapLayerImage : public XEMapLayer
{
public:
	enum xtType { xNONE, xNORMAL, xSTRETCH, xTILE, xLOOP };
private:
	void Init() {
		m_Type = XEMapLayer::xTYPE_IMAGE;
		m_DrawType = xNONE;
		m_wSize = m_hSize = 0;
	}
	void Destroy();
protected:
	XArrayLinear<XSurface*> m_listImage;		// 통짜맵의 이미지 리스트.
	XE::VEC2 m_sizeTexture;		// 통짜이미지의 텍스쳐 크기
	XE::VEC2 m_sizeSurface;		// 통짜이미지의 서피스 크기

//	TCHAR m_szFilename[ 32 ];
	_tstring m_strResPath;		// 이미지의 리소스패스
	xtType m_DrawType;
	float m_wSize;		// 레이어의 월드단위 사이즈
	float m_hSize;
public:
	XEMapLayerImage( XEMap *pMap, ID idLayer=0, LPCTSTR resImg=NULL );
	XEMapLayerImage( XEMap *pMap, LPCTSTR resImg, int wLayer, int hLayer );
	virtual ~XEMapLayerImage() { Destroy(); }
	//
	GET_SET_ACCESSOR( xtType, DrawType );
	GET_ACCESSOR( const XE::VEC2&, sizeSurface );
	//
	BOOL LoadImg( LPCTSTR szImg );
	void DrawNormal( const XE::VEC2& vsLT );
	void DrawStretch( const XE::VEC2& vsLT );
	void DrawTile( const XE::VEC2& vsLT );
	void DrawLoop( const XE::VEC2& vsLT );
	//
	void DrawTexture( float x, float y );
	//
	virtual void Load( XResFile *pRes, LPCTSTR szPath );
	virtual void Draw( const XE::VEC2& vsLT );
	virtual void DrawLayer( int y, int cw, int ch, BYTE alpha=255 ) {}
	virtual BOOL RestoreDevice( void );
};

#endif // _XEMAP
