
#include "stdafx.h"
#ifdef _XEMAP
#include "XEMap.h"
#include "XEMapLayer.h"
//#include "XEMapLayerTile.h"
#include "XEMapLayerImage.h"
#include "XFactory.h"
//#include "XSceneGame.h"	// 이거 이렇게 하면 안된다 civ에 의존하지 않게 다시 만들것

/*
XEMapLayerTile* XEMap::CreateMapLayerTile( ID idLayer, LPCTSTR resImg ) 
{ 
	return new XEMapLayerTile( this, idLayer, resImg );
}
*/
XEMapLayerImage* XEMap::CreateMapLayerImage( ID idLayer, LPCTSTR resImg ) 
{
	return new XEMapLayerImage( this, idLayer, resImg );
}

void XEMap::Destroy() 
{
	for( int i = 0; i < m_listLayers.GetNum(); ++i )
	{
		XEMapLayer *pLayer = m_listLayers[ i ];
		SAFE_DELETE( pLayer ); 
	}
}

// vsLT: 현재 사용하지 않음.
void XEMap::Draw( const XE::VEC2& vsLT, int nGameLayer )
{
	// 좌상귀 월드좌표 갱신
	UpdateForProj();
	//
	int num = m_listLayers.GetNum();
	for( int i = num-1; i >= 0; --i )		// 원경부터 찍어야 함
	{
		XEMapLayer *pLayer = m_listLayers[i];
		if( pLayer->GetbVisible() == FALSE )
			pLayer->SetAlpha( 0.5f );	
		else
			pLayer->SetAlpha( 1.0f );	
		pLayer->Draw( m_vPosViewport );
		if( i == nGameLayer )	
		{
			float sx, sy;
			sx = sy = 0;
			if( m_pDelegate )
				m_pDelegate->OnDrawObjLayer( nGameLayer, sx, sy );	// 유닛레이어를 호출해서 유닛을 그림
		}
	}
}

void XEMap::Scroll( const XE::VEC2& vMove )
{
	XBREAK( (int)m_sizeViewport.w == 0 );
	XBREAK( (int)m_sizeViewport.h == 0 );
	if( vMove.x == 0 && vMove.y == 0 )
		return;
	m_vCameraPos += vMove;
	// 스크롤 제한
	XE::VEC2 vsizeView = m_sizeViewport / m_fCameraScale;;		// 스케일된 뷰크기
	XE::VEC2 vLT = m_vCameraPos - ( vsizeView / 2.f );	// 좌상귀 월드 좌표
	if( vLT.x < 0 )
		m_vCameraPos.x -= vLT.x;
	if( vLT.y < 0 )
		m_vCameraPos.y -= vLT.y;
	XE::VEC2 vRB = m_vCameraPos + ( vsizeView / 2.0f );		// 우하귀 월드 좌표
	if( m_sizeWorld.w != -1 && vRB.x > m_sizeWorld.w )
		m_vCameraPos.x -= (vRB.x - m_sizeWorld.w);
	if( m_sizeWorld.h != -1 && vRB.y > m_sizeWorld.h )
		m_vCameraPos.y -= (vRB.y - m_sizeWorld.h);
}


XEMapLayer* XEMap::FindLayer( int idLayer ) 
{
	int num = m_listLayers.GetNum();
	for( int i = 0; i < num; ++i)
		if( m_listLayers[ i ]->GetidLayer() == idLayer )
			return m_listLayers[ i ];
	return NULL;
}

BOOL XEMap::Load( LPCTSTR szMap )
{
	XResFile xfile, *pRes = &xfile;
	if( pRes->Open( szMap, XBaseRes::xREAD ) == 0 )
	{
		XALERT( "%s 파일 열기 실패", szMap );
		return FALSE;
	}
	int ver;
	pRes->Read( &ver, 4 );
	// 버전 체크
	if( ver < VER_MAP )				// 버전이 다르면
	{
		CONSOLE( "경고: %s 파일의 버전(%d)이 최신버전인 %d보다 낮다.", szMap, ver, VER_MAP );
	}
	if( ver > VER_MAP )		// 이런 경우는 무조건 로드 불가.
	{
		XALERT( "에러: %s 파일의 버전(%d)이 최신버전인 %d보다 높아 열수가 없습니다.", szMap, ver, VER_MAP );
		return FALSE;
	}
	m_Version = ver;
	//
	DWORD w, h;
	pRes->Read( &w, 4 );		// 맵 월드 사이즈
	pRes->Read( &h, 4 );
	m_sizeWorld.Set( w, h );
	if( IsUpperVersion(2) )
	{
		int size;
		UNICHAR szName[32];
		pRes->Read( &size, 4 );
		pRes->Read( szName, size );	// 사용안함.
	}
	DWORD reserved[16];
	pRes->Read( reserved, sizeof(DWORD), 16 );
	// 레이어 로드
	int num;
	pRes->Read( &num, 4 );		// 레이어 수
	XEMapLayer baseLayer( this, XEMapLayer::xTYPE_NONE, 0, NULL );
	for( int i = 0; i < num; ++i )
	{
		baseLayer.Load( pRes, NULL );		// 레이어 기본형의 데이타 읽음.
		switch( baseLayer.GetType() )
		{
/*
		case XEMapLayer::xTYPE_TILE:
			{
				XEMapLayerTile *pLayer = CreateMapLayerTile( baseLayer.GetidLayer() );	// virtual
				pLayer->Load( pRes, XE::GetFilePath( szMap ) );
				pLayer->SetvDrawOffset( baseLayer.GetvDrawOffset() );
				pLayer->SetbVisible( baseLayer.GetbVisible() );
				pLayer->SetbShowMask( baseLayer.GetbShowMask() );
				AddLayer( pLayer );
			}
			break;
*/
		case XEMapLayer::xTYPE_IMAGE:
			{
				XEMapLayerImage *pLayer = CreateMapLayerImage( baseLayer.GetidLayer() );	// virtual
				pLayer->Load( pRes, XE::GetFilePath( szMap ) );
				pLayer->SetvDrawOffset( baseLayer.GetvDrawOffset() );
				pLayer->SetbVisible( baseLayer.GetbVisible() );
				pLayer->SetbShowMask( baseLayer.GetbShowMask() );
				AddLayer( pLayer );
			}
			break;
		default:
			XALERT( "%s읽던중. 맵레이어의 타입이 이상함. %d", szMap, baseLayer.GetType() );
			return FALSE;
		} 
	}
	//
	LoadExtra( pRes );
	//
	SetViewport( 0, 0, XE::GetGameWidth(), XE::GetGameHeight() );	// 디폴트로 풀스크린
	return TRUE;
}

void XEMap::FrameMove( float dt )
{
	// 좌상귀 월드좌표 갱신
//	UpdateForProj();
}

BOOL XEMap::RestoreDevice( void )
{
	XARRAYLINEAR_LOOP( m_listLayers, XEMapLayer*, pLayer )
	{
		if( pLayer->RestoreDevice() == FALSE )
			return FALSE;
	} END_LOOP;
	return TRUE;
}

#endif // _XEMAP
