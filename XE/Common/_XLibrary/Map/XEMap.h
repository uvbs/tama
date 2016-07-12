#pragma once
#ifdef _XEMAP

//#include "XETilePack.h"	// 타일 시스템 다시만들려고 엔진에서 뺌
#include "etc/XGraphics.h"
#include "XEMapLayerImage.h"

#define VER_MAP	2

class XEMapLayerTile;
class XEMapLayerImage;

class XDelegateMap
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateMap() { Init(); }
	virtual ~XDelegateMap() { Destroy(); }
	//
	virtual void OnDrawObjLayer( int nLayer, float sx, float sy ) {}
};



class XEMap
{
	void Init() {
		m_fCameraScale = 1.0f;
//		XCLEAR_ARRAY( m_szName );
		m_Version = 0;
		m_pDelegate = NULL;
	}
	void Destroy();
protected:
	DWORD m_Version;
	XArrayLinear<XEMapLayer*> m_listLayers;		// 레이어 리스트
	XE::VEC2 m_sizeWorld;		// 월드전체 맵 가로세로 크기	
	XE::VEC2 m_sizeViewport;	// 맵이 보이는 뷰포트 크기. 디폴트는 전체화면
	XE::VEC2 m_vPosViewport;	// 뷰포트의 좌상귀 픽셀좌표
//	TCHAR m_szName[32];			// 맵이름
	//
	XE::VEC2 m_vwLeftTop;		// 현재 화면 좌상귀의 월드좌표
	XE::VEC2 m_vCameraPos;
	float m_fCameraScale;
	XDelegateMap *m_pDelegate;
public:
	XEMap() { 
		Init(); 
//		m_listLayers.Create( 5 );
	}
	/**
		w,h: 맵 월드 사이즈 
		-1이면 무한대크기
	*/
	XEMap( int w, int h, int maxLayer, LPCTSTR szName=NULL ) { 
		Init(); 
		m_listLayers.Create( maxLayer );
		m_sizeWorld.w = (float)w;
		m_sizeWorld.h = (float)h;
		// 뷰포트
		m_sizeViewport = XE::VEC2( XE::GetGameWidth(), XE::GetGameHeight() );
//		if( szName )
//			_tcscpy_s( m_szName, szName );
	}
	virtual ~XEMap() { Destroy(); }
//	virtual XEMapLayerTile* CreateMapLayerTile( ID idLayer, LPCTSTR resImg=NULL );
	virtual XEMapLayerImage* CreateMapLayerImage( ID idLayer, LPCTSTR resImg=NULL );
	//
	void SetViewport( float x, float y, float w, float h ) {
		m_vPosViewport = XE::VEC2( x, y );
		m_sizeViewport = XE::VEC2( w, h );
	}
	void SetViewportLT( const XE::VEC2& vsLT ) {
		m_vPosViewport = vsLT;
	}

	BOOL IsUpperVersion( DWORD nVersion ) { return (m_Version >= nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 같거나 높은가
	BOOL IsLowerVersion( DWORD nVersion ) { return (m_Version < nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 버전이 낮은가
	GET_ACCESSOR( const XE::VEC2&, sizeWorld );
//	GET_ACCESSOR( const XE::VEC2&, vwCurrent );
	GET_ACCESSOR( const XE::VEC2&, sizeViewport );
	GET_ACCESSOR( const XE::VEC2&, vPosViewport );
//	GET_ACCESSOR( LPCTSTR, szName );
	GET_ACCESSOR( const XE::VEC2&, vCameraPos );
	GET_SET_ACCESSOR( XDelegateMap*, pDelegate );
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
	// 월드좌표 vwPos를 스크린좌표로 변환
	XE::VEC2 WorldToLocal( const XE::VEC2& vwPos ) {
		return Projection( vwPos );
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
		if( m_vwLeftTop.x < 0 ) {
			m_vCameraPos.x -= m_vwLeftTop.x;
			m_vwLeftTop.x = 0;
		}
		if( m_vwLeftTop.y < 0 ) {
			m_vCameraPos.y -= m_vwLeftTop.y;
			m_vwLeftTop.y = 0;
		}
	}
	void SetCameraPos( const XE::VEC2& vwPos ) {
		SetCameraPos( vwPos.x, vwPos.y );
	}
	void SetCameraPos( float wx, float wy ) {
		float fCameraScale = GetfCameraScale();
		m_vCameraPos = XE::VEC2( wx, wy );
		XE::VEC2 sizeView = m_sizeViewport / fCameraScale;		// 역스케일된 뷰크기
		XE::VEC2 vwLeftTop = m_vCameraPos - sizeView / 2.f;	// 스케일적용된 현재 뷰의 좌상귀 월드좌표
		if( vwLeftTop.x < 0 )
			m_vCameraPos.x -= vwLeftTop.x;
		if( vwLeftTop.y < 0 ) 
			m_vCameraPos.y -= vwLeftTop.y;
	}
	//
	virtual void AddLayer( XEMapLayer *pLayer ) {
		XBREAK( pLayer == NULL );
		m_listLayers.Add( pLayer );
	}
	XEMapLayer* FindLayer( int idLayer );
	void Scroll( const XE::VEC2& vMove );
	void Scroll( float xMove, float yMove ) { Scroll( XE::VEC2( xMove, yMove ) ); }
	BOOL RestoreDevice( void );
	//
	virtual void FrameMove( float dt );
	virtual void Draw( const XE::VEC2& vsLT, int nGameLayer );
	virtual BOOL Load( LPCTSTR szMap );
	virtual BOOL LoadExtra( XResFile *pRes ) { return TRUE; }
	//
};

#endif // _XEMAP
