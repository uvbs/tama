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
	XArrayLinear<XMapLayer*> m_listLayers;		// 레이어 리스트
	XE::VEC2 m_sizeWorld;		// 월드전체 맵 가로세로 크기	
	XE::VEC2 m_sizeViewport;	// 맵이 보이는 뷰포트 크기. 디폴트는 전체화면
	XE::VEC2 m_vPosViewport;	// 뷰포트의 좌상귀 픽셀좌표
//	TCHAR m_szName[32];			// 맵이름
	//
	XE::VEC2 m_vwLeftTop;		// 현재 화면 좌상귀의 월드좌표
	XE::VEC2 m_vCameraPos;
	float m_fCameraScale;
public:
	XMap() { 
		Init(); 
		m_listLayers.Create( 5 );
	}
	// w,h: 맵 월드 사이즈
	XMap( int w, int h, LPCTSTR szName ) { 
		Init(); 
		m_listLayers.Create( 5 );
		m_sizeWorld.w = (float)w;
		m_sizeWorld.h = (float)h;
		// 뷰포트
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
	BOOL IsUpperVersion( DWORD nVersion ) { return (m_Version >= nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 같거나 높은가
	BOOL IsLowerVersion( DWORD nVersion ) { return (m_Version < nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 버전이 낮은가
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
	XE::VEC2 LocalToWorld( const XE::VEC2& vPos ) {		// 스크린좌표 vPos를 월드좌표로 변환
		XE::VEC2 vwStart = GetvwCurrent();
		vwStart += vPos - m_vPosViewport;
		return vwStart;
	}
	XE::VEC2 Projection( const XE::VEC2& vwPos ) {	// 월드좌표를 넘겨주면 draw해야할 화면좌표를 리턴한다
		return (vwPos - m_vwLeftTop) * m_fCameraScale;		// 화면좌표로 변환
	}
	void Projection( const XE::VEC2& vwPos, float *pOutx, float *pOuty ) {
		*pOutx = (vwPos.x - m_vwLeftTop.x) * m_fCameraScale;		// 화면좌표로 변환
		*pOuty = (vwPos.y - m_vwLeftTop.y) * m_fCameraScale;
	}
	XE::VEC2 UnProjection( const XE::VEC2& vsPos ) {		// 스크린좌표를 월드좌표로 변환
		return (vsPos - m_vPosViewport) / m_fCameraScale + m_vwLeftTop;
	}
	void UpdateForProj( void ) {		// Projection을 쓰려면 매 프레임 이것을 호출해 좌상귀 좌표를 갱신해야한다
		float fCameraScale = GetfCameraScale();
		XE::VEC2 vCameraPos = GetCenter();	// 화면중심(카메라위치)
		XE::VEC2 sizeView = m_sizeViewport / fCameraScale;		// 역스케일된 뷰크기
		m_vwLeftTop = vCameraPos - sizeView / 2.f;	// 스케일적용된 현재 뷰의 좌상귀 월드좌표
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

