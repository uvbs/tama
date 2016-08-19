#pragma once

#include "xe.h"
#include "global.h"
#include "XSurface.h"
#include "XMapLayer.h"
#include "XList.h"

// �̹��� �ϳ��� ��¥��
class XMapLayerImage : public XMapLayer
{
public:
	enum xtType { xNONE, xNORMAL, xSTRETCH, xTILE };
private:
	void Init() {
		m_Type = XMapLayer::xTYPE_IMAGE;
		m_DrawType = xNONE;
		m_wSize = m_hSize = 0;
		m_wTexture = m_hTexture = 0;
		XCLEAR_ARRAY( m_szFilename );
	}
	void Destroy() {
		for( int i = 0; i < m_listImage.GetNum(); ++i )
		{
			XSurface *p = m_listImage[i]; 
			SAFE_DELETE( p );
		}
	}
protected:
	XArrayLinear<XSurface*> m_listImage;		// ��¥���� �̹��� ����Ʈ.
	int m_wTexture;		// ���� ��¥�̹����� ũ��.
	int m_hTexture;
	TCHAR m_szFilename[ 32 ];
	xtType m_DrawType;
	int m_wSize;		// ���̾��� ������� ������
	int m_hSize;
public:
	XMapLayerImage( XMap *pMap, ID idLayer=0, LPCTSTR szName=NULL ) : XMapLayer( pMap, xTYPE_IMAGE, idLayer, szName ) { Init(); }
	XMapLayerImage( XMap *pMap, LPCTSTR szImg, int wLayer, int hLayer );
	virtual ~XMapLayerImage() { Destroy(); }
	//
	GET_SET_ACCESSOR( xtType, DrawType );
	//
	BOOL LoadImg( LPCTSTR szImg );
	void DrawNormal( void );
	void DrawStretch( void );
	void DrawTile( void );
	//
	virtual void Load( XResFile *pRes, LPCTSTR szPath );
	virtual void Draw( void );
	virtual void DrawLayer( int y, int cw, int ch, BYTE alpha=255 ) {}
};

