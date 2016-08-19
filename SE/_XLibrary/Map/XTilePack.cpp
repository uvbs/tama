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
//////////////  ���߿� ������ �ű��
// pSrc, w, h������ d3d�ؽ��ĸ� ���� ����
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
		XERROR( "�ؽ��� Lock����" );
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
	pVertices[0].position = D3DXVECTOR3( 0, (float)th, 0 );		// ����
	pVertices[0].tu	= u;	pVertices[0].tv	= v2;	
	pVertices[1].position = D3DXVECTOR3( (float)tw, (float)th, 0 );		//����
	pVertices[1].tu	= u2;		pVertices[1].tv	= v2;
	pVertices[2].position = D3DXVECTOR3( 0, 0, 0 );		// �»�
	pVertices[2].tu	= u;	pVertices[2].tv	= v;
	pVertices[3].position = D3DXVECTOR3( (float)tw, 0, 0 );	// ���
	pVertices[3].tu	= u2;	pVertices[3].tv	= v;
	// unlock
	pd3dVB->Unlock();
	return pd3dVB;
}
#else	// win32
// pSrc, w, h������ d3d�ؽ��ĸ� ���� ����
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
	// tpk������ �д´�.
	XResFile res, *pRes = &res;
	if( pRes->Open( szTilePack, XBaseRes::xREAD ) == 0 )
	{
		return FALSE;
	}
	//
	pRes->Read( &m_Version, 4 );	// ��������
	VER_CHECK( szTilePack, m_Version, VER_TILEPACK );
	int tw, th, ttw, tth;	
	pRes->Read( &tw, 4 );			// Ÿ�� ũ��
	pRes->Read( &th, 4 );
	SetTileSize( tw, th );
	pRes->Read( &ttw, 4 );		// Ÿ���ؽ��� ũ��
	pRes->Read( &tth, 4 );
	SetTileTextureSize( ttw, tth );
	int numTiles;
	pRes->Read( &numTiles, 4 );		// Ÿ�� �Ѱ���
	m_Tiles.Create( numTiles );
	DWORD reserved[16];
	pRes->Read( reserved, sizeof(DWORD), 16 );
	_tcscpy_s( m_szTPK, XE::GetFileName( szTilePack ) );

	// Ÿ�ϵ����� �迭�� ä���.
	for( int i = 0; i < numTiles; ++i )
	{
		TILE tile;
		pRes->Read( &tile.idTile, 4 );
		DWORD dw;
		pRes->Read( &dw, 4 );
		if( dw > 1 )	// ������� ���� Ÿ���̹Ƿ� ����ũ�̹����� �ε��ؾ���.
		{
			tile.pMask = new BYTE[ ttw * tth ];
			pRes->Read( tile.pMask, ttw * tth * sizeof(BYTE) );		// mask�̹��� �ε�
		} else
		if( dw == 1 )		// ���� ����ũ�̹Ƿ� ���� �̹����� �ʿ� ����.
			tile.pMask = (BYTE*)1;
		else
			tile.pMask = NULL;		// ��Ÿ��.
		//
		m_Tiles.Add( tile );
	}
	// Ÿ���̹��� ����.
	for( int i = 1; i < numTiles; ++i )
	{
		DWORD *pMem = new DWORD[ ttw * tth ];
		pRes->Read( pMem, ttw * tth * sizeof(DWORD) );			// Ÿ�ϵ���Ÿ�� �ϳ��� �д´�.
#ifdef WIN32
		if( IsUpperVersion(3) )
			XE::ConvertBlockABGR_To_ARGB( pMem, ttw * tth );	
#endif
		m_Tiles[i].pImg = pMem;
	}
	// virtual load tpx
	{
		LoadTPXFromTPK( szTilePack );	// tpk�̸����κ��� tpx�̸��� ����� �ε��Ѵ�.
	}

	// ������ ����� Ÿ�ϵ���Ÿ�� �ű�
	if( bMakePage )
		MakePage();
	/*
		// Ÿ�� �ϳ��ϳ� ������ glDraw�� ȣ���ϴ� ����ϰ��
		��� Ÿ�ϵ鿡 ���ؼ� ���ؽ�4��¥�� ���ؽ����۸� �̸� ����� �д�. ���ؽ����� ������ǥ�� �� ������ u,v���� �ٸ���.
		������ �� ���ؽ����۸� �ٷ� �̿��� ��´�.
		// strip���� �������� ���� ������
		��Ŭ�����ʿ��� �ǽð����� strip ���ؽ����۸� ����� ���̳��� ���ۿ� �о�ְ� �ѹ��� ��´�. ���ؽ� �� 300����
		// �׳� triangle�� �����̷� ������.
		��Ŭ�����ʿ��� �ǽð����� triangle���ؽ� ���۸� ����� ���̳��� ���ۿ� �о�ְ� �ѹ��� ��´�. ���ؽ� �� 900��
	*/
	// KeepSrc=false��� �����̹����� �ı���.
	if( bSrcKeep == FALSE )
	{
		for( int i = 0; i < GetnNum(); ++i )
			SAFE_DELETE_ARRAY( m_Tiles[i].pImg );
	}
	return TRUE;
}

// Ÿ���ؽ��� ũ�Ⱑ 64�϶�.
// 256x256���������� Ÿ�� 16��
// 512���������� Ÿ�� 64�� <- �ּ� ������ ũ��.
// 1024���������� Ÿ�� 256��. 
// Ÿ�ϰ����� �Ѱ��ָ� ������ ������ ũ�⸦ ����ؼ� �����ش�.
BOOL CalcPageSize( int maxTile, int *pOutW, int *pOutH, int ttw, int tth )
{
	*pOutW = *pOutH = 0;
	// �� ����� �ִ� ������ũ�� �˾Ƴ�
	int maxSurfaceWidth = XSurface::GetMaxSurfaceWidth();		// �ϵ��� ������ �ؽ��� �ִ�ʺ�
	// �������� 2048���� Ŀ���� 4096x4096�ϸ� 16�ް��� �Ǳ⶧���� iOS���� �ٷ��� ������.
	if( maxSurfaceWidth > 2048 )
		maxSurfaceWidth = 2048;
	// �ִ� ������ũ�⿡ �� �������� Ÿ���� �� ���ٸ� �ִ�ũ��� ������.
	if( maxTile > (maxSurfaceWidth / ttw) * (maxSurfaceWidth / tth) )
	{
		*pOutW = maxSurfaceWidth;
		*pOutH = maxSurfaceWidth;
		return TRUE;
	}
	int wNum = maxSurfaceWidth / ttw;		// ex) 2048 / 64 = 32. ���ٿ� 32��
	int numLine = maxTile / wNum;			// Ÿ�����θ� �������� ������ �ʿ��Ѱ�. ex) maxTile(33) / 32 = 1.xxx
	if( maxTile % wNum )						// ¥������ ������ ���ٴ� �߰�.
		++numLine;
	int hSize = numLine * tth;					// �ʿ��� ���������� ũ��
	hSize =  GRAPHICS->AlignPow2Height( hSize );		// 2�� �¼��� ����.
	XBREAK( hSize > maxSurfaceWidth );
	*pOutW = maxSurfaceWidth;
	*pOutH = hSize;
	return TRUE;
}

// Ÿ�ϵ����͸� �������� �ű��.
void XTilePack::MakePage( void )
{
	int ttw, tth;
	GetTileTextureSize( &ttw, &tth );
	// Ÿ�ϰ����� �Ѱ��ָ� ������ ������ ũ�⸦ ����ؼ� �����ش�.
	int wPage, hPage;
	CalcPageSize( GetnNum(), &wPage, &hPage, ttw, tth );	
	
	// ���� ���������� ���ð��� ���
	int xNum = wPage / ttw;		// �������� �ִ� ũ�⸦ Ÿ�ϻ������ ������ ������ �˾Ƴ�
	int yNum = hPage / tth;
	int numTilePerPage = xNum * yNum;		// �������� ���� �ִ� Ÿ�� �ִ� ����
	int numPage = GetnNum() / numTilePerPage;		// ���� ���������� �ʿ����� ���.
	if( GetnNum() % numTilePerPage )
		++numPage;
	// �������� ���ʴ�� ����Ÿ�� �ű��.
	m_NumPage = 0;
	int nNum = 1;
	int tw, th;
	GetTileSize( &tw, &th );
	// �޸� ���ǽ� ����
	for( int n = 0; n < numPage; ++n )
	{
#pragma message( "������ �������� ¥�����κ�ũ�⸸ŭ�� ������ �Ҵ�޵��� ����ȭ �Ұ�" )
		XSurfaceMem *pSurface = new XSurfaceMem( wPage, hPage );	
		for( int i = 0; i < yNum; ++i )
		{
			for( int k = 0; k < xNum; ++k )
			{
				// Ÿ��1���� �ѹ��� �̰��� ����
				TILE *pTile = &m_Tiles[ nNum ];
				// �������� Ÿ�Ϻ� ī��

				pSurface->CopyRect( k * ttw, i * tth, pTile->pImg, ttw, tth );		// page�� x,y��ġ�� �޸�p�� ttw,tth������ ��ŭ�� ����Ÿ�� ī��
				// u,v��ǥ ��� 
#if defined(__TILE_DRAW2) || defined(__TILE_DRAW3)
				float u = ( k * ttw ) / (float)wPage;		// ������ ��ü���� ��ǥ�� ��ġ 0 ~ 1
				float v = ( i * tth ) / (float)hPage; 
				float u2 = ( k * ttw + ttw ) / (float)wPage;
				float v2 = ( i * tth + tth ) / (float)hPage;
				pTile->nPage = n;	// ������ ��ȣ
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
				if( ++nNum >= GetnNum() )		// ����� Ÿ�� ����
					goto LP1;
			}
		}
LP1:
		// �޸𸮻� �غ�� �������� �̿�, �ؽ��� �������� ����
		int wSurface, hSurface;
		DWORD *pSurfaceMem = pSurface->GetSrcImg( &wSurface, &hSurface );
		XTilePage *pPage = new XTilePage( pSurfaceMem, wSurface, hSurface );		
		m_pPages[ m_NumPage++ ] = pPage;
		SAFE_DELETE( pSurface );
	}
}

// ����̽� ���۸� ������ ������ Ÿ�ϵ���Ÿ�� ���� Ÿ���ϳ��� �����Ѵ�.
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
	pTile->pd3dVB = CreateD3DVertexBuffer( m_wTile, m_hTile, pTile->u, pTile->v );		// ���� Ÿ�ϻ������ ���ؽ� ���� ����
	XBREAK( pTile->pd3dVB == NULL );
#endif
#ifdef __TILE_DRAW1
	pTile->pd3dTexture = CreateD3DTexture( pTile->pImg, m_wTileTexture, m_hTileTexture );
	XBREAK( pTile->pd3dTexture == NULL );
	#ifdef _XTOOL
	{
		// �������� BYTE�� ����ũ �̹����� DWORD���ȼ�ȭ ���Ѽ� �ؽ��ĸ� �����Ѵ�.
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
	// ȭ�鿡 �׸��뵵�� ���ǽ��� �����.
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
// page��ȣ Ÿ���������� vram�� �ø���.
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
// page��ȣ Ÿ���������� vram�� �ø���.
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
// Tile Page. Ÿ�� ������
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