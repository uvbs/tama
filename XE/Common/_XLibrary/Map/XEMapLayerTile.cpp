#include "stdafx.h"
#include "XEMapLayerTile.h"
#include "XETilePack.h"
#include "XEMap.h"
#include "XETilePackMng.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// w,h: 타일단위 맵크기. w/hTile: 타일하나의 가로세로 크기
// 맵 가로,세로 크기(타일단위), szTilePack:타일묶음 파일
XEMapLayerTile::XEMapLayerTile( XEMap *pMap, int w, int h, XTilePack *pTilePack ) 
: XEMapLayer( pMap, xTYPE_TILE, 0, NULL )
{ 
	Init(); 
	// 타일 단위 맵 사이즈
//	m_sizeMapTile.w = (float)w;
//	m_sizeMapTile.h = (float)h;
	m_wMapTile = w;
	m_hMapTile = h;
	// 타일팩 로딩
	m_pTilePack = pTilePack;
	// 
	m_pMapDat = new short[ w * h ];
	memset( m_pMapDat, 0, sizeof(short) * w * h );		// 메모리 클리어
	//
	CreateVertexBuffer( pMap );
}

void XEMapLayerTile::CreateVertexBuffer( XEMap *pMap )
{
	XBREAK( m_pTilePack == NULL );
	int tw, th;
	m_pTilePack->GetTileSize( &tw, &th );
//	int num = (int)((XE::GetGameWidth() / tw) + 2) * (int)((XE::GetGameHeight() / th) + 2);		// 뷰포트사이즈에 들어가는 타일개수에 가로세로 2개씩 여분을 더 둠.
	int num = ((960 / tw) + 2) * ((640 / th) + 2);		// 화면이 축소가 되어 넓게 보여질수 있기때문에 크게 잡았다.
	#ifdef _VER_DX
		m_pVertexBuffer = new XVertexBufferD3D;
	#else
		m_pVertexBuffer = new XVertexBufferGL;
	#endif
	m_pVertexBuffer->Create( num );
}

void XEMapLayerTile::FrameMove( float dt )
{
	if( m_pMapDat == NULL )		return;
	//
}

void XEMapLayerTile::Draw( const XE::VEC2& vsLT )
{
	XBREAK( m_prefMap == NULL );
	if( m_pMapDat == NULL )		return;
	if( m_pTilePack == NULL )		return;
	// 
	if( m_pTilePack->GetNumPage() > 1 )		//최적화를 위해 페이지갯수에 따라 따로 불러준다.
		DrawForMultiPage();
	else
		DrawForOnePage();
}

// 타일페이지가 1개인 경우의 코드
void XEMapLayerTile::DrawForOnePage( void )
{
	// 타일페이지 vram에 올리기
	m_pTilePack->UploadTilePage( 0 );
	//
	float scale = m_prefMap->GetfCameraScale();
	int tw, th;
	GetTileSize( &tw, &th );
	XE::VEC2 vSizeTile( tw, th );
	// 현재 보이는화면에 속하는 맵을 돌면서 타일을 찍는다.
	XE::VEC2 vTSizeTile = vSizeTile * scale;
	if( GRAPHICS->GetPhyScreenWidth() == 480 )
	{
		vTSizeTile.x = (float)((int) vTSizeTile.x);
		vTSizeTile.y = (float)((int) vTSizeTile.y);
	} else
	{
#ifdef __NEW_RESO
#else
		vTSizeTile.x = ROUND_OFF2( vTSizeTile.x, 0.5f );
		vTSizeTile.y = ROUND_OFF2( vTSizeTile.y, 0.5f );
#endif
	}
	XE::VEC2 vPos = m_prefMap->GetvPosViewport();			// 뷰포트 좌상귀 픽셀좌표부터 시작
	vPos += (m_vDrawOffset * scale);
	XE::VEC2 vCurr = m_prefMap->GetvwLeftTop() / vSizeTile;		// 현재보이는곳의 좌상귀 맵좌표
	XE::VEC2 vNumDraw = m_prefMap->GetsizeViewport() / vTSizeTile;	// 화면에 보이는 가로세로 타일 개수.
	int xOfs = (int)m_prefMap->GetvwLeftTop().x % tw;
	int yOfs = (int)m_prefMap->GetvwLeftTop().y % th;
	XE::VEC2 vOfs = XE::VEC2( xOfs * scale, yOfs * scale );
	vPos -= vOfs;
	if( GRAPHICS->GetPhyScreenWidth() == 480 )
	{
		vPos.x = (float)((int) vPos.x);
		vPos.y = (float)((int) vPos.y);
	} else
	{
#ifdef __NEW_RESO
#else
		vPos.x = ROUND_OFF2( vPos.x, 0.5f );
		vPos.y = ROUND_OFF2( vPos.y, 0.5f );
#endif
	}
//	float xPos = vPos.x;
	float wView = m_prefMap->GetsizeViewport().w;
	float hView = m_prefMap->GetsizeViewport().h; 
	int i, k;
	float x, y;
	for( y = vPos.y, i = 0; y < hView; y += vTSizeTile.h, ++i )
	{
		int iy = i + (int)vCurr.y;
		if( iy >= 0 && iy < m_hMapTile )
		{
			for( x = vPos.x, k = 0; x < wView; x += vTSizeTile.w, ++k )
			{
				int ix = k + (int)vCurr.x;
				if( ix >= 0 && ix < m_wMapTile )
				{
					short idxTile = m_pMapDat[ iy * m_wMapTile + ix ];		// 맵에서 타일 인덱스 꺼냄
//					if( idxTile && i == 6 )
//					if( idxTile && k == 8 && i >= 7 && i <= 8 )
					if( idxTile )
					{
						// 타일을 하나씩 찍는 버전
						const XTilePack::TILE *pTile = m_pTilePack->GetpTile( idxTile );
						// 버텍스버퍼에 버텍스정보를 밀어넣는다.
						XBREAK( m_pVertexBuffer == NULL );
						m_pVertexBuffer->Add( x, y, vTSizeTile.w, vTSizeTile.h, pTile->u, pTile->v, pTile->u2, pTile->v2 );
					}
				}
			}
		}
	}
	XBREAK( m_pVertexBuffer == NULL );
	m_pVertexBuffer->Flush();
	m_pVertexBuffer->Draw( 0, 0 );
}


// 타일페이지가 2개 이상인 경우의 코드
void XEMapLayerTile::DrawForMultiPage( void )
{
	// 타일페이지 vram에 올리기
	// 
}

void XEMapLayerTile::Load( XResFile *pRes, LPCTSTR szPath )
{
	//
	DWORD w, h;
	pRes->Read( &w, 4 );
	pRes->Read( &h, 4 );
	XBREAK( w <= 0 );
	XBREAK( h <= 0 );
//	m_sizeMapTile.Set( w, h );
	m_wMapTile = w;
	m_hMapTile = h;
	// 
	int size;
	TCHAR szTPK[ 1024 ];
	UNICHAR szBuff[ 1024 ];
	pRes->Read( &size, 4 );
	XBREAK( size < 0 );
	XBREAK( size >= sizeof( szBuff ) );
	pRes->Read( szBuff, size );		// tpk filename
#ifdef WIN32
	_tcscpy_s( szTPK, szBuff );
#else
//	_tcscpy_s( szTPK, _ConvertUTF16ToUTF8( szBuff ) );
	_ConvertUTF16ToUTF8( szTPK, szBuff );
#endif
	// tpk load
	TCHAR szFullPath[ 1024 ];
	_tcscpy_s( szFullPath, XE::Format( _T("%s%s"), XE::GetFilePath( pRes->GetszResName() ), szTPK ) );
	XBREAK( TILEPACK_MNG == NULL );
#ifdef _XTOOL
	m_pTilePack = TILEPACK_MNG->Load( szFullPath, TRUE );
#else
	m_pTilePack = TILEPACK_MNG->Load( szFullPath, FALSE );
#endif
	CreateVertexBuffer( m_prefMap );
	DWORD reserved[4];
	pRes->Read( reserved, sizeof(DWORD), 4 );
	// 타일 맵 데이타 로드
	m_pMapDat = new short[ w * h ];
	pRes->Read( m_pMapDat, w * h * sizeof(short) );

}

// 월드좌표 wx, wy위치의 충돌값을 리턴한다. 
BYTE XEMapLayerTile::GetCollision( float wx, float wy )
{

	XTilePack *pTilePack = m_pTilePack;
	// 좌표의 타일을 구함.
	int tw = pTilePack->GetwTile();
	int th = pTilePack->GethTile();
	int tx = (int)wx / tw;
	int ty = (int)wy / th;
	int idxTile = GetMapData( tx, ty );
	// 타일의 로컬오프셋이 어딘지 계산
	int xOfs = (int)wx % tw;	
	int yOfs = (int)wy % th;
	BYTE coll = pTilePack->GetTileMask( idxTile, xOfs, yOfs );
	return coll;
}







//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _VER_DX
// 인수는 버텍스의 개수가 아니라 버텍스6개로 구성된 사각형의 최대개수다.
BOOL XVertexBufferD3D::Create( int numRect )
{
	// Create the vertex buffer.
	if( FAILED( GRAPHICS_D3D->GetDevice()->CreateVertexBuffer( numRect * 6 * sizeof(XSURFACE_VERTEX), 
															D3DUSAGE_DYNAMIC, D3DFVF_XSURFACE_VERTEX,
															D3DPOOL_DEFAULT, &m_pd3dVB, NULL ) ) )
	{
		XBREAK(1);
		return FALSE;
	}
	m_pList = new XSURFACE_VERTEX[ numRect * 6 ];
	m_nNumRect = 0;
	m_nMaxRect = numRect;
	return TRUE;
}
// 화면좌표 x, y - w, h영역의 버텍스를 버퍼에 추가한다.u,v는 좌상귀, u2,v2는 우하귀 텍스쳐 좌표
void XVertexBufferD3D::Add( float x, float y, float w, float h, float u, float v, float u2, float v2 )
{
//	int w = (int)_w;
//	int h = (int)_h;
	XBREAK( m_pList == NULL );
	XBREAK( m_pd3dVB == NULL );
	XBREAK( m_nNumRect >= m_nMaxRect );
	XSURFACE_VERTEX *pList = &m_pList[ m_nNumRect * 6 ];
#ifdef __NEW_RESO
//	float xx = 480.f / 1024.f / 2.f;
//	float yy = 320.f / 768.f / 2.f;;
//	float sx = (float)((int) x) - xx;
//	float sy = (float)((int) y) - yy;
//	float sx = ROUND_OFF2( x, 0.5f );
//	float sy = ROUND_OFF2( y, 0.5f );
	float sx = x;
	float sy = y;
#else
	float sx = ROUND_OFF2( x, 0.5f ) - 0.25f;
	float sy = ROUND_OFF2( y, 0.5f ) - 0.25f;
#endif
	pList[0].position.x = sx;		pList[0].position.y = sy;		// 좌상
	pList[0].position.z = 0;
	pList[1].position = pList[0].position + D3DXVECTOR3( 0, h, 0 );			// 좌하
	pList[2].position = pList[0].position + D3DXVECTOR3( w, h, 0 );		// 우하
	pList[3].position = pList[0].position;				// 좌상
	pList[4].position = pList[0].position + D3DXVECTOR3( w, h, 0 );		// 우하
	pList[5].position = pList[0].position + D3DXVECTOR3( w, 0, 0 );			// 우상
	
	pList[0].tu = u;	pList[0].tv = v;
	pList[1].tu = u;	pList[1].tv = v2;
	pList[2].tu = u2;	pList[2].tv = v2;
	pList[3].tu = u;	pList[3].tv = v;
	pList[4].tu = u2;	pList[4].tv = v2;
	pList[5].tu = u2;	pList[5].tv = v;
	++m_nNumRect;
}
// 메모리에 쌓인 버텍스버퍼를 디바이스버퍼로 카피한다.
BOOL XVertexBufferD3D::Flush( void )
{
	// lock
	XSURFACE_VERTEX* pVertices;
	if( FAILED( m_pd3dVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD ) ) )
		return FALSE;
	// 
	memcpy_s( pVertices, m_nMaxRect * 6 * sizeof(XSURFACE_VERTEX), m_pList, m_nNumRect * 6 * sizeof(XSURFACE_VERTEX) );
	// unlock
	m_pd3dVB->Unlock();
	return TRUE;
}
void XVertexBufferD3D::Draw( float x, float y )
{
	D3DXMATRIX mWorld;
	LPDIRECT3DDEVICE9  pd3dDevice = GRAPHICS_D3D->GetDevice();
	D3DXMatrixTranslation( &mWorld, x, y, 0 );
	pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );
	HRESULT hr;
	XV( XE::s_pEffect->SetMatrix( "mViewProj", &XE::s_mViewProj ) );
	XV( XE::s_pEffect->SetMatrix( "mWorld", &mWorld ) );
	XV( XE::s_pEffect->SetFloat( "fOpacity", 1.0f ) );
	XV( XE::s_pEffect->SetFloat( "col_r", 1.0f ) );
	XV( XE::s_pEffect->SetFloat( "col_g", 1.0f ) );
	XV( XE::s_pEffect->SetFloat( "col_b", 1.0f ) );
	// draw core
	XBREAK( m_pd3dVB == NULL );
	XV( XE::s_pEffect->SetTechnique( "RenderSprite" ) );
	UINT iPass, cPasses;
	XV( XE::s_pEffect->Begin( &cPasses, 0 ) );
	for( iPass = 0; iPass < cPasses; iPass++ )
	{
		XV( XE::s_pEffect->BeginPass( iPass ) );

		pd3dDevice->SetStreamSource( 0, m_pd3dVB, 0, sizeof(XSURFACE_VERTEX) );
		pd3dDevice->SetFVF( D3DFVF_XSURFACE_VERTEX );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nNumRect * 2 );

		XV( XE::s_pEffect->EndPass() );
	}
	XV( XE::s_pEffect->End() ); 
	m_nNumRect = 0;
}
#else // dx
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 인수는 버텍스의 개수가 아니라 버텍스6개로 구성된 사각형의 최대개수다.
BOOL XVertexBufferGL::Create( int numRect )
{
	glGenBuffers( 1, &m_glVB );
	if( XBREAK( m_glVB == 0 ) )
		return FALSE;
	//
	m_pList = new GL_VERTEX[ numRect * 6 ];
	m_nNumRect = 0;
	m_nMaxRect = numRect;
	return TRUE;
}
// 화면좌표 x, y - w, h영역의 버텍스를 버퍼에 추가한다.u,v는 좌상귀, u2,v2는 우하귀 텍스쳐 좌표
void XVertexBufferGL::Add( float x, float y, float w, float h, float u, float v, float u2, float v2 )
{
	XBREAK( m_pList == NULL );
	XBREAK( m_glVB == 0 );
	XBREAK( m_nNumRect >= m_nMaxRect );
	GL_VERTEX *pList = &m_pList[ m_nNumRect * 6 ];
	float sx, sy;
	if( GRAPHICS->GetPhyScreenWidth() == 480 )
	{
		sx = (float)((int) x);
		sy = (float)((int) y);
	} else
	{
		sx = ROUND_OFF2( x, 0.5f );
		sy = ROUND_OFF2( y, 0.5f );
	}
	pList[0].pos[0] = sx;		pList[0].pos[1] = sy;				// 좌상
	pList[1].pos[0] = pList[0].pos[0];	pList[1].pos[1] = pList[0].pos[1]+h;			// 좌하
	pList[2].pos[0] = pList[0].pos[0]+w;	pList[2].pos[1] = pList[0].pos[1]+h;		// 우하
	pList[3].pos[0] = pList[0].pos[0];	pList[3].pos[1] = pList[0].pos[1];				// 좌상
	pList[4].pos[0] = pList[0].pos[0]+w;	pList[4].pos[1] = pList[0].pos[1]+h;		// 우하
	pList[5].pos[0] = pList[0].pos[0]+w;	pList[5].pos[1] = pList[0].pos[1];			// 우상
	pList[0].tex[0] = u;	pList[0].tex[1] = v;
	pList[1].tex[0] = u;	pList[1].tex[1] = v2;
	pList[2].tex[0] = u2;	pList[2].tex[1] = v2;
	pList[3].tex[0] = u;	pList[3].tex[1] = v;
	pList[4].tex[0] = u2;	pList[4].tex[1] = v2;
	pList[5].tex[0] = u2;	pList[5].tex[1] = v;
	++m_nNumRect;
}
// 메모리에 쌓인 버텍스버퍼를 디바이스버퍼로 카피한다.
BOOL XVertexBufferGL::Flush( void )
{
	// 버텍스 버퍼에 버텍스값 전송.
	XBREAK( m_glVB == 0 );
	glBindBuffer(GL_ARRAY_BUFFER, m_glVB);
    // 스태틱이 쪼금더 빠르네. 왜그러지? 프레임당 glBufferData/layer한번은 '잦은'게 아닌가?
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_VERTEX) * m_nNumRect * 6, m_pList, GL_STATIC_DRAW);
    
	return TRUE;
}
void XVertexBufferGL::Draw( float x, float y )
{
#if 0
	// opengl 2.0으로 다시 만들것.
	glPushMatrix();
	{
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glTranslatef( (float)x, (float)y, 0);	
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glBindBuffer(GL_ARRAY_BUFFER, m_glVB);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(2, GL_FLOAT, sizeof(GL_VERTEX), (void*)offsetof(GL_VERTEX, pos));
		glTexCoordPointer(2, GL_FLOAT, sizeof(GL_VERTEX), (void*)offsetof(GL_VERTEX, tex));
		glDrawArrays(GL_TRIANGLES, 0, m_nNumRect * 6 );	

	}
	glPopMatrix();
	m_nNumRect = 0;
#endif // 0
}

#endif // not dx





