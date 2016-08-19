#include "stdafx.h"
#include "XTilePack.h"
#include "XSurfaceMem.h"
#ifdef WIN32
#include "XGraphicsD3D.h"
#else
#include "XGraphicsOpenGL.h"
#endif

DWORD* CreateImageFromMask( int w, int h, BYTE *pMask );

#ifdef WIN32
//////////////  나중에 딴데로 옮길것
// pSrc, w, h정보로 d3d텍스쳐를 만들어서 리턴
LPDIRECT3DTEXTURE9 CreateD3DTexture( DWORD *pSrc, int w, int h )
{
	XBREAK( pSrc == NULL );
	XBREAK( w < 0 || w > 16384 );
	XBREAK ( h < 0 || h > 16384 );
	XBREAK( GRAPHICS_D3D->GetDevice() == NULL );
	LPDIRECT3DTEXTURE9 pTexture;
	HRESULT hr = D3DXCreateTexture( GRAPHICS_D3D->GetDevice(),
												w, h, 
												1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
												&pTexture );
	if( FAILED( hr ) ) 
		XERROR( "D3DXCreateTexture failed. %dx%dx", w, h );
	//
	D3DLOCKED_RECT	d3dLockRect;
	memset( &d3dLockRect, 0, sizeof( D3DLOCKED_RECT ) );
	if( FAILED( pTexture->LockRect( 0, &d3dLockRect, NULL, 0 ) ) ) {
		XERROR( "텍스쳐 Lock실패" );
	}
	//
	DWORD *pDst = (DWORD *)d3dLockRect.pBits;
	int size = w * h;
	memcpy_s( pDst, size * sizeof(DWORD), pSrc, size * sizeof(DWORD) );
	//
	pTexture->UnlockRect( 0 );
	return pTexture;
}
LPDIRECT3DVERTEXBUFFER9 CreateD3DVertexBuffer( int tw, int th, float u, float v, float u2, float v2 )
{
	LPDIRECT3DVERTEXBUFFER9 pd3dVB;
	// Create the vertex buffer.
	if( FAILED( GRAPHICS_D3D->GetDevice()->CreateVertexBuffer( 4 * sizeof(XSURFACE_VERTEX), 
															0, D3DFVF_XSURFACE_VERTEX,
															D3DPOOL_DEFAULT, &pd3dVB, NULL ) ) )
	{
		return NULL;
	}
	// lock
	XSURFACE_VERTEX* pVertices;
	if( FAILED( pd3dVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
		return NULL;
	// 
	pVertices[0].position = D3DXVECTOR3( 0, (float)th, 0 );		// 좌하
	pVertices[0].tu	= u;	pVertices[0].tv	= v2;	
	pVertices[1].position = D3DXVECTOR3( (float)tw, (float)th, 0 );		//우하
	pVertices[1].tu	= u2;		pVertices[1].tv	= v2;
	pVertices[2].position = D3DXVECTOR3( 0, 0, 0 );		// 좌상
	pVertices[2].tu	= u;	pVertices[2].tv	= v;
	pVertices[3].position = D3DXVECTOR3( (float)tw, 0, 0 );	// 우상
	pVertices[3].tu	= u2;	pVertices[3].tv	= v;
	// unlock
	pd3dVB->Unlock();
	return pd3dVB;
}
#else	// win32
// pSrc, w, h정보로 d3d텍스쳐를 만들어서 리턴
GLuint CreateGLTexture( DWORD *pSrc, int w, int h )
{
	GLuint glTexture;
	glGenTextures( 1, &glTexture );
	glBindTexture(GL_TEXTURE_2D, glTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 
				0, 
				GL_RGBA, 
				w, 
				h, 
				0, 
				GL_RGBA, 
				GL_UNSIGNED_BYTE, 
				pSrc );
	return glTexture;
}
typedef struct tagSTRUCT_VERTEX_SURFACE{
	GLfloat x[2];//, y;
	GLfloat t[2];//u, tv;
} GL_VERTEX;

GLuint CreateGLVertexBuffer( int tw, int th, float u, float v, float u2, float v2 )
{
	GLuint glVB;
	glGenBuffers( 1, &glVB );
	const GL_VERTEX vertices[4] = 
	{
		0, th,  u, v2, 
		tw, th, u2, v2,
		0, 0,   u, v,
		tw, 0, u2, v
	};

	glBindBuffer(GL_ARRAY_BUFFER, glVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(GL_VERTEX), (void*)offsetof(GL_VERTEX,x));
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(GL_VERTEX), (void*)offsetof(GL_VERTEX,t));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    return glVB;

}
#endif  // not win32


BOOL XTilePack::Load( LPCTSTR szTilePack, BOOL bMakePage, BOOL bSrcKeep )
{
	// tpk파일을 읽는다.
	XResFile res, *pRes = &res;
	if( pRes->Open( szTilePack, XBaseRes::xREAD ) == 0 )
	{
		return FALSE;
	}
	//
	pRes->Read( &m_Version, 4 );	// 버전정보
	VER_CHECK( szTilePack, m_Version, VER_TILEPACK );
	int tw, th, ttw, tth;	
	pRes->Read( &tw, 4 );			// 타일 크기
	pRes->Read( &th, 4 );
	SetTileSize( tw, th );
	pRes->Read( &ttw, 4 );		// 타일텍스쳐 크기
	pRes->Read( &tth, 4 );
	SetTileTextureSize( ttw, tth );
	int numTiles;
	pRes->Read( &numTiles, 4 );		// 타일 총개수
	m_Tiles.Create( numTiles );
	DWORD reserved[16];
	pRes->Read( reserved, sizeof(DWORD), 16 );
	_tcscpy_s( m_szTPK, XE::GetFileName( szTilePack ) );

	// 타일데이터 배열을 채운다.
	for( int i = 0; i < numTiles; ++i )
	{
		TILE tile;
		pRes->Read( &tile.idTile, 4 );
		DWORD dw;
		pRes->Read( &dw, 4 );
		if( dw > 1 )	// 투명색이 섞인 타일이므로 마스크이미지를 로딩해야함.
		{
			tile.pMask = new BYTE[ ttw * tth ];
			pRes->Read( tile.pMask, ttw * tth * sizeof(BYTE) );		// mask이미지 로딩
		} else
		if( dw == 1 )		// 꽉찬 마스크이므로 굳이 이미지가 필요 없음.
			tile.pMask = (BYTE*)1;
		else
			tile.pMask = NULL;		// 빈타일.
		//
		m_Tiles.Add( tile );
	}
	// 타일이미지 읽음.
	for( int i = 1; i < numTiles; ++i )
	{
		DWORD *pMem = new DWORD[ ttw * tth ];
		pRes->Read( pMem, ttw * tth * sizeof(DWORD) );			// 타일데이타를 하나씩 읽는다.
#ifdef WIN32
		if( IsUpperVersion(3) )
			XE::ConvertBlockABGR_To_ARGB( pMem, ttw * tth );	
#endif
		m_Tiles[i].pImg = pMem;
	}
	// virtual load tpx
	{
		LoadTPXFromTPK( szTilePack );	// tpk이름으로부터 tpx이름을 만들어 로딩한다.
	}

	// 페이지 만들어 타일데이타를 옮김
	if( bMakePage )
		MakePage();
	/*
		// 타일 하나하나 일일히 glDraw를 호출하는 방법일경우
		모든 타일들에 대해서 버텍스4개짜리 버텍스버퍼를 미리 만들어 둔다. 버텍스들의 로컬좌표는 다 같지만 u,v값이 다르다.
		찍을땐 이 버텍스버퍼를 바로 이용해 찍는다.
		// strip으로 뭉텅으로 만들어서 찍을때
		맵클래스쪽에서 실시간으로 strip 버텍스버퍼를 만들어 다이나믹 버퍼에 밀어넣고 한번에 찍는다. 버텍스 약 300여개
		// 그냥 triangle로 뭉텅이로 찍을때.
		맵클래스쪽에서 실시간으로 triangle버텍스 버퍼를 만들어 다이나믹 버퍼에 밀어넣고 한번에 찍는다. 버텍스 약 900개
	*/
	// KeepSrc=false라면 원본이미지는 파괴함.
	if( bSrcKeep == FALSE )
	{
		for( int i = 0; i < GetnNum(); ++i )
			SAFE_DELETE_ARRAY( m_Tiles[i].pImg );
	}
	return TRUE;
}

// 타일텍스쳐 크기가 64일때.
// 256x256페이지에는 타일 16개
// 512페이지에는 타일 64개 <- 최소 페이지 크기.
// 1024페이지에는 타일 256개. 
// 타일개수를 넘겨주면 최적의 페이지 크기를 계산해서 돌려준다.
BOOL CalcPageSize( int maxTile, int *pOutW, int *pOutH, int ttw, int tth )
{
	*pOutW = *pOutH = 0;
	// 이 기기의 최대 페이지크기 알아냄
	int maxSurfaceWidth = XSurface::GetMaxSurfaceWidth();		// 하드웨어가 가능한 텍스쳐 최대너비
	// 페이지가 2048보다 커봐야 4096x4096하면 16메가나 되기때문에 iOS에서 다루기는 무리다.
	if( maxSurfaceWidth > 2048 )
		maxSurfaceWidth = 2048;
	// 최대 페이지크기에 들어갈 개수보다 타일이 더 많다면 최대크기로 돌려줌.
	if( maxTile > (maxSurfaceWidth / ttw) * (maxSurfaceWidth / tth) )
	{
		*pOutW = maxSurfaceWidth;
		*pOutH = maxSurfaceWidth;
		return TRUE;
	}
	int wNum = maxSurfaceWidth / ttw;		// ex) 2048 / 64 = 32. 한줄에 32개
	int numLine = maxTile / wNum;			// 타일전부를 넣으려면 몇줄이 필요한가. ex) maxTile(33) / 32 = 1.xxx
	if( maxTile % wNum )						// 짜투리가 남으면 한줄더 추가.
		++numLine;
	int hSize = numLine * tth;					// 필요한 페이지세로 크기
	hSize =  GRAPHICS->AlignPow2Height( hSize );		// 2의 승수로 정렬.
	XBREAK( hSize > maxSurfaceWidth );
	*pOutW = maxSurfaceWidth;
	*pOutH = hSize;
	return TRUE;
}

// 타일데이터를 페이지에 옮긴다.
void XTilePack::MakePage( void )
{
	int ttw, tth;
	GetTileTextureSize( &ttw, &tth );
	// 타일개수를 넘겨주면 최적의 페이지 크기를 계산해서 돌려준다.
	int wPage, hPage;
	CalcPageSize( GetnNum(), &wPage, &hPage, ttw, tth );	
	
	// 전부 몇페이지가 나올건지 계산
	int xNum = wPage / ttw;		// 페이지의 최대 크기를 타일사이즈로 나눠서 개수를 알아냄
	int yNum = hPage / tth;
	int numTilePerPage = xNum * yNum;		// 페이지당 들어갈수 있는 타일 최대 갯수
	int numPage = GetnNum() / numTilePerPage;		// 전부 몇페이지가 필요한지 계산.
	if( GetnNum() % numTilePerPage )
		++numPage;
	// 페이지에 차례대로 데이타를 옮긴다.
	m_NumPage = 0;
	int nNum = 1;
	int tw, th;
	GetTileSize( &tw, &th );
	// 메모리 서피스 생성
	for( int n = 0; n < numPage; ++n )
	{
#pragma message( "마지막 페이지의 짜투리부분크기만큼만 페이지 할당받도록 최적화 할것" )
		XSurfaceMem *pSurface = new XSurfaceMem( wPage, hPage );	
		for( int i = 0; i < yNum; ++i )
		{
			for( int k = 0; k < xNum; ++k )
			{
				// 타일1개당 한번씩 이곳에 들어옴
				TILE *pTile = &m_Tiles[ nNum ];
				// 페이지로 타일블럭 카피

				pSurface->CopyRect( k * ttw, i * tth, pTile->pImg, ttw, tth );		// page의 x,y위치에 메모리p의 ttw,tth사이즈 만큼의 데이타를 카피
				// u,v좌표 계산 
#if defined(__TILE_DRAW2) || defined(__TILE_DRAW3)
				float u = ( k * ttw ) / (float)wPage;		// 페이지 전체에서 좌표의 위치 0 ~ 1
				float v = ( i * tth ) / (float)hPage; 
				float u2 = ( k * ttw + ttw ) / (float)wPage;
				float v2 = ( i * tth + tth ) / (float)hPage;
				pTile->nPage = n;	// 페이지 번호
	#ifdef __TILE_DRAW2
        #ifdef WIN32
				pTile->pd3dVB = CreateD3DVertexBuffer( tw, th, u, v, u2, v2 );
        #else
                pTile->glVB = CreateGLVertexBuffer( tw, th, u, v, u2, v2 );
        #endif
	#endif
	#ifdef __TILE_DRAW3
				pTile->u = u;
				pTile->v = v;
				pTile->u2 = u2;
				pTile->v2 = v2;
	#endif
#endif // 
				if( ++nNum >= GetnNum() )		// 진행된 타일 개수
					goto LP1;
			}
		}
LP1:
		// 메모리상에 준비된 페이지를 이용, 텍스쳐 페이지를 생성
		int wSurface, hSurface;
		DWORD *pSurfaceMem = pSurface->GetSrcImg( &wSurface, &hSurface );
		XTilePage *pPage = new XTilePage( pSurfaceMem, wSurface, hSurface );		
		m_pPages[ m_NumPage++ ] = pPage;
		SAFE_DELETE( pSurface );
	}
}

// 디바이스 버퍼를 제외한 완전한 타일데이타를 갖춘 타일하나를 생성한다.
XTilePack::TILE XTilePack::CreateTile( DWORD *pTileMem )
{
	TILE tile;
	tile.idTile = XE::GenerateID();
	tile.pImg = pTileMem;
	tile.pMask = NULL;
	return tile;
}
/*
void XTilePack::CreateTileDeviceBuffer( TILE *pTile )
{
#ifdef WIN32
#if defined(__TILE_DRAW1) || (__TILE_DRAW2)
	pTile->pd3dVB = CreateD3DVertexBuffer( m_wTile, m_hTile, pTile->u, pTile->v );		// 논리적 타일사이즈로 버텍스 버퍼 생성
	XBREAK( pTile->pd3dVB == NULL );
#endif
#ifdef __TILE_DRAW1
	pTile->pd3dTexture = CreateD3DTexture( pTile->pImg, m_wTileTexture, m_hTileTexture );
	XBREAK( pTile->pd3dTexture == NULL );
	#ifdef _XTOOL
	{
		// 툴에서는 BYTE형 마스크 이미지를 DWORD형픽셀화 시켜서 텍스쳐를 저장한다.
		BYTE *pMask = pTile->pMask;
		if( pMask && pMask != (BYTE*)0x01 )
		{
			DWORD *pMaskImg = new DWORD[ m_wTileTexture * m_hTileTexture ];
			DWORD *p = pMaskImg;
			for( int i = 0; i < m_wTileTexture * m_hTileTexture; ++i )
			{
				*p++ = XCOLOR_RGBA( *pMask, *pMask, *pMask, *pMask );
				++pMask;
			}
			pTile->pd3dTextureMask = CreateD3DTexture( pMaskImg, m_wTileTexture, m_hTileTexture );
		}
	}
	#endif
#endif
#else // win32
	XBREAK(1);
#endif // iOS
}
*/
#ifdef __TILE_DRAW1
void XTilePack::CreateTileSurface( TILE *pTile, DWORD *pMaskMem )
{
	// 화면에 그릴용도의 서피스를 만든다.
	pTile->psfcImage = GRAPHICS->CreateSurface( m_wTile, m_hTile, m_wTileTexture, m_hTileTexture, 0, 0, pTile->pImg, TRUE );
	if( pMaskMem )
		pTile->psfcMask = GRAPHICS->CreateSurface( m_wTile, m_hTile, m_wTileTexture, m_hTileTexture, 0, 0, pMaskMem, TRUE );
}
#endif 
DWORD* CreateImageFromMask( int w, int h, BYTE *pMask )
{
	DWORD *pDstImg = new DWORD[ w * h ];
	DWORD *p = pDstImg;
	for( int i = 0; i < w * h; ++i )
	{
		*p++ = XCOLOR_RGBA( 0, 0, 0, *pMask );
		++pMask;
	}
	return pDstImg;
}

int XTilePack::AddTile( const TILE& tile )
{
	int idx = m_Tiles.Add( tile );
	return idx;
}


#ifdef WIN32
#if defined(__TILE_DRAW2) || defined(__TILE_DRAW3)
// page번호 타일페이지를 vram에 올린다.
void XTilePack::UploadTilePage( int page )
{
	LPDIRECT3DDEVICE9  pd3dDevice = GRAPHICS_D3D->GetDevice();
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );		
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	GRAPHICS_D3D->GetDevice()->SetTexture( 0, m_pPages[ page ]->GetpTexture() );
	HRESULT hr;
	XV( XE::s_pEffect->SetTexture( "Sprite", m_pPages[ page ]->GetpTexture() ) );	// base texture
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
}
#endif 

void XTilePack::DrawMask( float x, float y, int idx, float alpha )
{
	if( idx == 0 )
		return;
#ifdef __TILE_DRAW1
	if( m_Tiles[ idx ].psfcMask )
	{
		m_Tiles[ idx ].psfcMask->SetfAlpha( alpha );
		m_Tiles[ idx ].psfcMask->Draw( x, y );
	}
#endif
}
void XTilePack::Draw( float x, float y, int idx, float alpha )
{
	if( idx == 0 )
		return;
	TILE *pTile = &m_Tiles[ idx ];
#ifdef __TILE_DRAW1
	XBREAK( pTile->psfcImage == NULL );
	pTile->psfcImage->SetfAlpha( alpha );
	pTile->psfcImage->Draw( x, y );
#endif
#ifdef __TILE_DRAW2
	D3DXMATRIX mWorld;
	LPDIRECT3DDEVICE9  pd3dDevice = GRAPHICS_D3D->GetDevice();
	D3DXMatrixTranslation( &mWorld, (float)((int)x), (float)((int)y), 0 );
	pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );
	HRESULT hr;
	XV( XE::s_pEffect->SetMatrix( "mViewProj", &XE::s_mViewProj ) );
	XV( XE::s_pEffect->SetMatrix( "mWorld", &mWorld ) );
	XV( XE::s_pEffect->SetFloat( "fOpacity", alpha ) );
	XV( XE::s_pEffect->SetFloat( "col_r", 1.0f ) );
	XV( XE::s_pEffect->SetFloat( "col_g", 1.0f ) );
	XV( XE::s_pEffect->SetFloat( "col_b", 1.0f ) );
	// draw core
	XBREAK( pTile->pd3dVB == NULL );
	XV( XE::s_pEffect->SetTechnique( "RenderSprite" ) );
	UINT iPass, cPasses;
	XV( XE::s_pEffect->Begin( &cPasses, 0 ) );
	for( iPass = 0; iPass < cPasses; iPass++ )
	{
		XV( XE::s_pEffect->BeginPass( iPass ) );

		pd3dDevice->SetStreamSource( 0, pTile->pd3dVB, 0, sizeof(XSURFACE_VERTEX) );
		pd3dDevice->SetFVF( D3DFVF_XSURFACE_VERTEX );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		XV( XE::s_pEffect->EndPass() );
	}
	XV( XE::s_pEffect->End() );
#endif // __TILE_DRAW2	
}

#else // win32
#if defined(__TILE_DRAW2) || defined(__TILE_DRAW3)
// page번호 타일페이지를 vram에 올린다.
void XTilePack::UploadTilePage( int page )
{
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glBindTexture(GL_TEXTURE_2D, m_pPages[ page ]->GetglTexture() );
}
#endif 

void XTilePack::DrawMask( float x, float y, int idx, float alpha )
{
	if( idx == 0 )
		return;
#ifdef __TILE_DRAW1
	if( m_Tiles[ idx ].psfcMask )
	{
		m_Tiles[ idx ].psfcMask->SetfAlpha( alpha );
		m_Tiles[ idx ].psfcMask->Draw( x, y );
	}
#endif
}
void XTilePack::Draw( float x, float y, int idx, float alpha )
{
	if( idx == 0 )
		return;
	TILE *pTile = &m_Tiles[ idx ];
#ifdef __TILE_DRAW1
	XBREAK( pTile->psfcImage == NULL );
	pTile->psfcImage->SetfAlpha( alpha );
	pTile->psfcImage->Draw( x, y );
#endif
#ifdef __TILE_DRAW2
	glPushMatrix();
	{
		glTranslatef( (float)x, (float)y, 0);	
		glBindBuffer(GL_ARRAY_BUFFER, pTile->glVB);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, sizeof(GL_VERTEX), (void*)offsetof(GL_VERTEX, x));
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(GL_VERTEX), (void*)offsetof(GL_VERTEX, t));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );	
        
	}
	glPopMatrix();
#endif // __TILE_DRAW2	
}
#endif // GL

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tile Page. 타일 페이지
//
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
XTilePage::XTilePage( DWORD *pSrc, int w, int h )
{
	Init();
	m_pTexture = CreateD3DTexture( pSrc, w, h );
}
XTilePage::~XTilePage()
{
	SAFE_RELEASE( m_pTexture );
}
#else	// win32
XTilePage::XTilePage( DWORD *pSrc, int w, int h )
{
	Init();
	m_glTexture = CreateGLTexture( pSrc, w, h );
}
XTilePage::~XTilePage()
{
	glDeleteTextures( 1, &m_glTexture );
}
#endif		// GL