#pragma once

#include "xe.h"
#include "global.h"
#include "XTilePack.h"
#include "XList.h"
#include "XGraphics.h"
#include "XMapLayer.h"

#define VER_MAP	2

class XMapLayerTile;
class XMapLayerImage;
class XMap
{
	void Init() {
		m_fCameraScale = 1.0f;
//		XCLEAR_ARRAY( m_szName );
		m_Version = 0;
	}
	void Destroy();
protected:
	DWORD m_Version;
	XArrayLinear<XMapLayer*> m_listLayers;		// ���̾� ����Ʈ
	XE::VEC2 m_sizeWorld;		// ������ü �� ���μ��� ũ��	
	XE::VEC2 m_sizeViewport;	// ���� ���̴� ����Ʈ ũ��. ����Ʈ�� ��üȭ��
	XE::VEC2 m_vPosViewport;	// ����Ʈ�� �»�� �ȼ���ǥ
//	TCHAR m_szName[32];			// ���̸�
	//
	XE::VEC2 m_vwLeftTop;		// ���� ȭ�� �»���� ������ǥ
	XE::VEC2 m_vCameraPos;
	float m_fCameraScale;
public:
	XMap() { 
		Init(); 
		m_listLayers.Create( 5 );
	}
	// w,h: �� ���� ������
	XMap( int w, int h, LPCTSTR szName ) { 
		Init(); 
		m_listLayers.Create( 5 );
		m_sizeWorld.w = (float)w;
		m_sizeWorld.h = (float)h;
		// ����Ʈ
		m_sizeViewport = XE::VEC2( GRAPHICS->GetScreenWidth(), GRAPHICS->GetScreenHeight() );
//		if( szName )
//			_tcscpy_s( m_szName, szName );
	}
	virtual ~XMap() { Destroy(); }
	virtual XMapLayerTile* CreateMapLayerTile( ID idLayer, LPCTSTR szName=NULL );
	virtual XMapLayerImage* CreateMapLayerImage( ID idLayer, LPCTSTR szName=NULL );
	//
	void SetViewport( int x, int y, int w, int h ) {
		m_vPosViewport = XE::VEC2( x, y );
		m_sizeViewport = XE::VEC2( w, h );
	}
	BOOL IsUpperVersion( DWORD nVersion ) { return (m_Version >= nVersion)? TRUE : FALSE; }	// �� ������ nVersion���� ���ų� ������
	BOOL IsLowerVersion( DWORD nVersion ) { return (m_Version < nVersion)? TRUE : FALSE; }	// �� ������ nVersion���� ������ ������
	GET_ACCESSOR( const XE::VEC2&, sizeWorld );
//	GET_ACCESSOR( const XE::VEC2&, vwCurrent );
	GET_ACCESSOR( const XE::VEC2&, sizeViewport );
	GET_ACCESSOR( const XE::VEC2&, vPosViewport );
//	GET_ACCESSOR( LPCTSTR, szName );
	GET_ACCESSOR( const XE::VEC2&, vCameraPos );
	XE::VEC2 GetCenter( void ) { 
//		return m_vwLeftTop + m_sizeViewport / 2.f;
		return m_vCameraPos; 
	}
	GET_SET_ACCESSOR( float, fCameraScale );
	XE::VEC2 GetvwCurrent( void ) { return GetvwLeftTop(); }
	XE::VEC2 GetvwLeftTop( void ) { return m_vwLeftTop; }
	float AddCameraScale( float addScale ) { 
		m_fCameraScale += addScale;
		if( addScale > 0 && m_fCameraScale > 1.0f )
			m_fCameraScale = 1.0f;
		else 
		if( addScale < 0 && m_fCameraScale < 0.5f )
			m_fCameraScale = 0.5f;
			
		return  m_fCameraScale;
	}

	//
	XE::VEC2 LocalToWorld( const XE::VEC2& vPos ) {		// ��ũ����ǥ vPos�� ������ǥ�� ��ȯ
		XE::VEC2 vwStart = GetvwCurrent();
		vwStart += vPos - m_vPosViewport;
		return vwStart;
	}
	XE::VEC2 Projection( const XE::VEC2& vwPos ) {	// ������ǥ�� �Ѱ��ָ� draw�ؾ��� ȭ����ǥ�� �����Ѵ�
		return (vwPos - m_vwLeftTop) * m_fCameraScale;		// ȭ����ǥ�� ��ȯ
	}
	void Projection( const XE::VEC2& vwPos, float *pOutx, float *pOuty ) {
		*pOutx = (vwPos.x - m_vwLeftTop.x) * m_fCameraScale;		// ȭ����ǥ�� ��ȯ
		*pOuty = (vwPos.y - m_vwLeftTop.y) * m_fCameraScale;
	}
	XE::VEC2 UnProjection( const XE::VEC2& vsPos ) {		// ��ũ����ǥ�� ������ǥ�� ��ȯ
		return (vsPos - m_vPosViewport) / m_fCameraScale + m_vwLeftTop;
	}
	void UpdateForProj( void ) {		// Projection�� ������ �� ������ �̰��� ȣ���� �»�� ��ǥ�� �����ؾ��Ѵ�
		float fCameraScale = GetfCameraScale();
		XE::VEC2 vCameraPos = GetCenter();	// ȭ���߽�(ī�޶���ġ)
		XE::VEC2 sizeView = m_sizeViewport / fCameraScale;		// �������ϵ� ��ũ��
		m_vwLeftTop = vCameraPos - sizeView / 2.f;	// ����������� ���� ���� �»�� ������ǥ
	}
	//
	virtual void AddLayer( XMapLayer *pLayer ) {
		XBREAK( pLayer == NULL );
		m_listLayers.Add( pLayer );
	}
	XMapLayer* FindLayer( int idLayer );
	void Scroll( const XE::VEC2& vMove );
	void Scroll( float xMove, float yMove ) { Scroll( XE::VEC2( xMove, yMove ) ); }
	//
	virtual void FrameMove( float dt );
	virtual void Draw( int nGameLayer );
	virtual BOOL Load( LPCTSTR szMap );
	virtual BOOL LoadExtra( XResFile *pRes ) { return TRUE; }
	//
};

