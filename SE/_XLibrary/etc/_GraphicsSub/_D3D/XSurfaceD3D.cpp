#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <dxerr.h>
#include "XGraphics.h"
#include "XSurfaceD3D.h"
#include "Alert.h"
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//#define _X05		// x - 0.5 버전

#include "XGraphicsD3D.h"
#define	DEVICE		XSurfaceD3D::s_pd3dDevice
LPDIRECT3DDEVICE9 XSurfaceD3D::s_pd3dDevice = nullptr;

// namespace XE
namespace XE
{
	D3DXMATRIX s_mViewProj;
	LPD3DXEFFECT s_pEffect;
}

#pragma comment( lib, "winmm" )		// winmm.lib 추가

BOOL XE::Init( LPDIRECT3DDEVICE9 pd3dDevice, int nScrWidth, int nScrHeight )
{
	BOOL bRet = TRUE;
	SetWorkDir();		// 워킹폴더를 세팅
	XSurfaceD3D::SetDevice( pd3dDevice );
	bRet = LoadShader( _T("2dsprite.fx") );
	SetProjection( nScrWidth, nScrHeight );
	return bRet;
}
BOOL XE::Init( LPDIRECT3DDEVICE9 pd3dDevice, int nScrWidth, int nScrHeight, LPD3DXEFFECT pEffect ) 
{
	SetWorkDir();		// 워킹폴더를 세팅
	XSurfaceD3D::SetDevice( pd3dDevice );
	SetEffect( pEffect );
	SetProjection( nScrWidth, nScrHeight );
	return TRUE;
}

void XE::Destroy()
{
	SAFE_RELEASE( s_pEffect );
}

BOOL XE::LoadShader( LPCTSTR szFilename )
{
	LPD3DXBUFFER pCompileError;
	LPD3DXEFFECT pEffect;
	HRESULT hr = D3DXCreateEffectFromFile( DEVICE, szFilename, nullptr, nullptr, 0, nullptr, &pEffect, &pCompileError );
    if (FAILED(hr))
    {
		if( pCompileError ) 
		{
			char *str = (char *)pCompileError->GetBufferPointer();
			XERROR( "Failed to load effect file\r\n%s", str );
		} else {
			XERROR( "%s File not found", szFilename );
			exit(1);
		}
		return FALSE;
    } 
	SetEffect( pEffect );
	return TRUE;
}
/*
float XE::GetDT()
{
	static DWORD dwPrevTime = timeGetTime();
	float dt = (timeGetTime() - dwPrevTime) / (1000.0f / XFPS);
	dwPrevTime = timeGetTime();
	return dt;
}*/
///////////////////////////////////////////////////////////////////////////////////////
DWORD XSurfaceD3D::s_dwTextureCaps = 0;


void XSurfaceD3D::Destroy() 
{
	s_sizeTotalVMem -= (GetAlignedWidth() * GetAlignedHeight() * sizeof(DWORD));
	if( m_pTexture != nullptr )
		m_pTexture->Release();

	if( m_pVB != nullptr )        
		m_pVB->Release();
	if( m_pVBSub != nullptr )        
		m_pVBSub->Release();

}
// _adjxy 는 서피스크기 기준의 adj다. 외부에 노출된 adj값들은 모두 서피스 기준
XE::xRESULT XSurfaceD3D::ChangeAdjust( float _adjx, float _adjy )
{
	float adjx = _adjx;
	float adjy = _adjy;
	SetAdjust( adjx, adjy );
	int nAlignW = m_nAlignedWidth;
	int nAlignH = m_nAlignedHeight;
	int memw = GetMemWidth();
	int memh = GetMemHeight();
	XSURFACE_VERTEX* pVertices;
	XSURFACE_VERTEX* pVerticesSub;
	if( FAILED( m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
		return XE::xFAIL;
	if( FAILED( m_pVBSub->Lock( 0, 0, (void**)&pVerticesSub, 0 ) ) )
		return XE::xFAIL;

	pVertices[0].position = D3DXVECTOR3( 0+adjx, GetHeight()+adjy, 0 );
	pVertices[0].tu	= 0;	pVertices[0].tv	= (float)memh / nAlignH;	// 이 값은 바뀌지 않지만 Lock으로 tu,tv가 안돌아올수도 있을듯?
	pVertices[1].position = D3DXVECTOR3( GetWidth()+adjx, GetHeight()+adjy, 0 );
	pVertices[1].tu	= (float)memw / nAlignW;	pVertices[1].tv	= (float)memh / nAlignH;
	pVertices[2].position = D3DXVECTOR3( 0+adjx, 0+adjy, 0 );
	pVertices[2].tu	= 0;	pVertices[2].tv	= 0;
	pVertices[3].position = D3DXVECTOR3( GetWidth()+adjx, adjy, 0 );
	pVertices[3].tu	= (float)memw / nAlignW;	pVertices[3].tv	= 0;
#ifdef _X05
	pVertices[0].position.x -= 0.5f;		pVertices[0].position.y -= 0.5f;
	pVertices[1].position.x -= 0.5f;		pVertices[1].position.y -= 0.5f;
	pVertices[2].position.x -= 0.5f;		pVertices[2].position.y -= 0.5f;
	pVertices[3].position.x -= 0.5f;		pVertices[3].position.y -= 0.5f;
#endif
	pVerticesSub[0].position = pVertices[0].position;
	pVerticesSub[0].tu = pVertices[0].tu;
	pVerticesSub[0].tv = pVertices[0].tv;
	pVerticesSub[1].position = pVertices[1].position;
	pVerticesSub[1].tu = pVertices[1].tu;
	pVerticesSub[1].tv = pVertices[1].tv;
	pVerticesSub[2].position = pVertices[2].position;
	pVerticesSub[2].tu = pVertices[2].tu;
	pVerticesSub[2].tv = pVertices[2].tv;
	pVerticesSub[3].position = pVertices[3].position;
	pVerticesSub[3].tu = pVertices[3].tu;
	pVerticesSub[3].tv = pVertices[3].tv;

	m_pVB->Unlock();
	m_pVBSub->Unlock();
	return XE::xSUCCESS;
}
// this의 이미지정보를 pNewImg로 교체한다. 텍스쳐도 새로 할당한다
BOOL XSurfaceD3D::SetImage( const float width, const float height, const float adjx, const float adjy, DWORD *pNewImg )
{
	// 기존데이타를 지움
	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pVBSub );
	DestroySrcImg();
	// 재생성
	Create( width, height, adjx, adjy, xNO_ALPHA, pNewImg, sizeof(DWORD), 0, TRUE );
	return TRUE;
}
/*
버텍스버퍼도 gscale신경쓰지 않고 화면에 보이는크기 그대로 만들어야 하지 않을까?
그렇다면 굳이 draw에서 축소시킬필요도 없을듯. 단지 텍스쳐 크기만 커지면 되니까. 어차피 텍스쳐는 uv좌표를 사용하므로 크기가 몇이든지 상관이 없다
단지 aligned크기라는것은 필요하다. 그것은 실제 물리적인 텍스쳐의 크기의 지정에 필요하다
aligned도 gscale적용된 값으로 버텍스버퍼가 만들어지면 될듯
AdjustXY값도 결국 사라져야 한다. 이게 살아있으면 adj * gscale 이런코드가 계속 들어가게 되므로 프로그래밍이 난해해진다
현재의 AdjustAxis와 통합을 시켜야 할듯
당장은 그것까지 고치면 너무 많아지니까 adj * gscale코드로 당분간 때우자
결국 adjust값이란건 run타임중엔 사용되지 않고 초기 버텍스버퍼 생성시에만 로컬축을 어디로 쓸지에 대한 문제이다
HP같은거 그릴땐 adjust값이 필요한데 이것도 뭔가 깔끔한 다른방법을 찾아야 한다
하여간 확축회전이 가능해지면서 부터는 도트단위로 뭔갈 처리해야하는 일은 없어야 한다
*/

// _pSrcImg의 일부분만 잘라서 서피스로 만드는 버전
// srcx, srcy : srcw, srch영역에서 잘라낼 시작위치
// srcw,srch : _pSrcImg의 메모리 크기
// dstw, dsth : 잘라서 만들 서피스 크기
// adjxy: 서피스크기 기준 adj값
XE::xRESULT		XSurfaceD3D::Create( const int memSrcx, const int memSrcy, const int memSrcw, const int memSrch, 
											const float surfaceDstw, const float surfaceDsth, 
										const float _adjx, const float _adjy, void *_pSrcImg, 
										const int bpp, BOOL bSrcKeep )
{

	XBREAK_ERROR( bpp != sizeof(DWORD), "bytePerPixel=4 이외의 포맷은 아직 지원하지 않음" );
	const float adjx = _adjx;	
	const float adjy = _adjy;
	const int memDstw = ConvertToMemSize( surfaceDstw );
	const int memDsth = ConvertToMemSize( surfaceDsth );
//	GetWidth() = dstw;
//	GetHeight() = dsth;
	SetAdjust( adjx, adjy );
	m_nAlignedWidth = memDstw;		// 고해상도데이타면 서피스 크기에 *2(/0.5)가 되어야 한다
	m_nAlignedHeight = memDsth;
	GRAPHICS->AlignPowSize( &m_nAlignedWidth, &m_nAlignedHeight );		// 2^ 로 변환
	const int nAlignW = m_nAlignedWidth;
	const int nAlignH = m_nAlignedHeight;

	// Create the vertex buffer.
	if( FAILED( DEVICE->CreateVertexBuffer( 4 * sizeof(XSURFACE_VERTEX), 
															0, D3DFVF_XSURFACE_VERTEX,
															m_VBPool, &m_pVB, nullptr ) ) )
	{
		return XE::xFAIL;
	}
	// Create the vertex buffer.
	if( FAILED( DEVICE->CreateVertexBuffer( 4 * sizeof(XSURFACE_VERTEX), 
															0, D3DFVF_XSURFACE_VERTEX,
															m_VBPool, &m_pVBSub, nullptr ) ) )
	{
		return XE::xFAIL;
	}

	// Fill the vertex buffer. We are setting the tu and tv texture
	// coordinates, which range from 0.0 to 1.0
	XSURFACE_VERTEX* pVertices;
	XSURFACE_VERTEX* pVerticesSub;
	if( FAILED( m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
		return XE::xFAIL;
	if( FAILED( m_pVBSub->Lock( 0, 0, (void**)&pVerticesSub, 0 ) ) )
		return XE::xFAIL;

	// 기존방식은 200사이즈 그림이라도 256으로 텍스쳐를 생성하고 남은부분은 알파0색으로 채워놓고 버텍스버퍼도 256크기로 잡는방식이었는데
	// 텍스쳐는 2^반드시 만들어야 하지만 버텍스 버퍼는 그럴필요가 없다. 그래서 버텍스 버퍼는 200사이즈로 잡고 uv좌표를 계산해서 조정해주는
	// 방식으로 바꿨다. 남는 알파부분을 아예 찍지 않기때문에 속도면에서도 더 유리할듯 하다
	pVertices[0].position = D3DXVECTOR3( 0+adjx, surfaceDsth+adjy, 0 );		// 좌하
	pVertices[0].tu	= 0;	pVertices[0].tv	= (float)memDsth / nAlignH;	// uv좌표 계산
	pVertices[1].position = D3DXVECTOR3( surfaceDstw+adjx, surfaceDsth+adjy, 0 );		//우하
	pVertices[1].tu	= (float)memDstw / nAlignW;		pVertices[1].tv	= (float)memDsth / nAlignH;
	pVertices[2].position = D3DXVECTOR3( 0+adjx, 0+adjy, 0 );		// 좌상
	pVertices[2].tu	= 0;	pVertices[2].tv	= 0;
	pVertices[3].position = D3DXVECTOR3( surfaceDstw+adjx, adjy, 0 );	// 우상
	pVertices[3].tu	= (float)memDstw / nAlignW;	pVertices[3].tv	= 0;
/*
	pVertices[0].position = D3DXVECTOR3( 0+adjx, nAlignH+adjy, 0 );
	pVertices[0].tu	= 0;	pVertices[0].tv	= 1;
	pVertices[1].position = D3DXVECTOR3( nAlignW+adjx, nAlignH+adjy, 0 );
	pVertices[1].tu	= 1;	pVertices[1].tv	= 1;
	pVertices[2].position = D3DXVECTOR3( 0+adjx, 0+adjy, 0 );
	pVertices[2].tu	= 0;	pVertices[2].tv	= 0;
	pVertices[3].position = D3DXVECTOR3( nAlignW+adjx, adjy, 0 );
	pVertices[3].tu	= 1;	pVertices[3].tv	= 0;
*/
#ifdef _X05
	pVertices[0].position.x -= 0.5f;		pVertices[0].position.y -= 0.5f;
	pVertices[1].position.x -= 0.5f;		pVertices[1].position.y -= 0.5f;
	pVertices[2].position.x -= 0.5f;		pVertices[2].position.y -= 0.5f;
	pVertices[3].position.x -= 0.5f;		pVertices[3].position.y -= 0.5f;
#endif

	pVerticesSub[0].position = pVertices[0].position;
	pVerticesSub[0].tu = pVertices[0].tu;
	pVerticesSub[0].tv = pVertices[0].tv;
	pVerticesSub[1].position = pVertices[1].position;
	pVerticesSub[1].tu = pVertices[1].tu;
	pVerticesSub[1].tv = pVertices[1].tv;
	pVerticesSub[2].position = pVertices[2].position;
	pVerticesSub[2].tu = pVertices[2].tu;
	pVerticesSub[2].tv = pVertices[2].tv;
	pVerticesSub[3].position = pVertices[3].position;
	pVerticesSub[3].tu = pVertices[3].tu;
	pVerticesSub[3].tv = pVertices[3].tv;

	m_pVB->Unlock();
	m_pVBSub->Unlock();

	if( _pSrcImg )
	{
		// 소스이미지보관
		{
			// 타겟크기만큼만 타이트하게 할당받아서 옮긴다
			DWORD *_dst;
			DWORD *dst = _dst = new DWORD[ memDstw * memDsth ];
			DWORD *src = (DWORD *)_pSrcImg;
			src += (memSrcy * memSrcw + memSrcx);
			memset( dst, 0, sizeof(DWORD) * memDstw * memDsth );
			for( int i = 0; i < memDsth; i ++ )
			{
				for( int j = 0; j < memDstw; j ++ )
					*_dst++ = *src++;
				src += ( memSrcw - memDstw );
			}
			SetSrcImg( dst, memDstw, memDsth );	// 메모리상의 데이타
		}
		// 텍스쳐 생성
		s_sizeTotalVMem += (nAlignW * nAlignH * sizeof(DWORD));
		HRESULT hr = D3DXCreateTexture( DEVICE, nAlignW, nAlignH, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture );
		if( FAILED( hr ) ) {
			XERROR( "D3DXCreateTexture failed %dx%d", nAlignW, nAlignH );
			return XE::xFAIL;
		}
		D3DLOCKED_RECT	d3dLockRect;
		memset( &d3dLockRect, 0, sizeof( D3DLOCKED_RECT ) );
		if( FAILED( m_pTexture->LockRect( 0, &d3dLockRect, nullptr, 0 ) ) ) {
			XERROR( "텍스쳐 Lock실패" );
			return XE::xFAIL;
		}
		// 텍스쳐로 이미지 옮김
//		if( s_dwTextureCaps & D3DPTEXTURECAPS_POW2 || 1 )
		{
			int srcw, srch;
			DWORD *dst = (DWORD *)d3dLockRect.pBits;	// aligned된 이미지
			DWORD *src = GetSrcImg( &srcw, &srch );		// aligned안된 이미지
			memset( dst, 0, sizeof(DWORD) * nAlignW * nAlignH );
			for( int i = 0; i < srch; i ++ )
			{
				for( int j = 0; j < srcw; j ++ )
					*dst++ = *src++;
				dst += (nAlignW - srcw);
			}
		}

		m_pTexture->UnlockRect( 0 );
		if( bSrcKeep == FALSE )	// 소스보관 안하는건 
			DestroySrcImg();			// 이미지 지운다
	}

	return XE::xSUCCESS;
}

// 미리 m_bHighReso를 결정해놓고 호출되어야 한다. surfaceWH는 만들어질 서피스의 크기이지 메모리의 크기가 아니다
XE::xRESULT		XSurfaceD3D::Create( const float surfaceW, const float surfaceH, const float _adjx, const float _adjy, xAlpha alpha, 
														const void *pImg, int bytePerPixel, XCOLOR dwKeyColor, BOOL bSrcKeep )
{
	const float adjx = (float)_adjx;
	const float adjy = (float)_adjy;
	const int memSrcW = ConvertToMemSize( surfaceW );		// 실제 메모리 크기
	const int memSrcH = ConvertToMemSize( surfaceH );
	SetAdjust( adjx, adjy );
	if( bytePerPixel == sizeof(DWORD) )
		SetSrcImg( (DWORD *)pImg, memSrcW, memSrcH );
	else  {
		XERROR( "bytePerPixel=4 이외의 포맷은 아직 지원하지 않음" );
		return XE::xFAIL;
	}
	m_nAlignedWidth = memSrcW;
	m_nAlignedHeight = memSrcH;
	GRAPHICS->AlignPowSize( &m_nAlignedWidth, &m_nAlignedHeight );		// 2^ 로 변환
	int nAlignW = m_nAlignedWidth;
	int nAlignH = m_nAlignedHeight;

	// Create the vertex buffer.
	if( FAILED( DEVICE->CreateVertexBuffer( 4 * sizeof(XSURFACE_VERTEX), 
															0, D3DFVF_XSURFACE_VERTEX,
															m_VBPool, &m_pVB, nullptr ) ) )
	{
		return XE::xFAIL;
	}
	// Create the vertex buffer.
	if( FAILED( DEVICE->CreateVertexBuffer( 4 * sizeof(XSURFACE_VERTEX), 
															0, D3DFVF_XSURFACE_VERTEX,
															m_VBPool, &m_pVBSub, nullptr ) ) )
	{
		return XE::xFAIL;
	}

	// Fill the vertex buffer. We are setting the tu and tv texture
	// coordinates, which range from 0.0 to 1.0
	XSURFACE_VERTEX* pVertices;
	XSURFACE_VERTEX* pVerticesSub;
	if( FAILED( m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
		return XE::xFAIL;
	if( FAILED( m_pVBSub->Lock( 0, 0, (void**)&pVerticesSub, 0 ) ) )
		return XE::xFAIL;

	pVertices[0].position = D3DXVECTOR3( 0+adjx, surfaceH+adjy, 0 );
	pVertices[0].tu	= 0;	pVertices[0].tv	= (float)memSrcH / nAlignH;
	pVertices[1].position = D3DXVECTOR3( surfaceW+adjx, surfaceH+adjy, 0 );
	pVertices[1].tu	= (float)memSrcW / nAlignW;	pVertices[1].tv	= (float)memSrcH / nAlignH;
	pVertices[2].position = D3DXVECTOR3( 0+adjx, 0+adjy, 0 );
	pVertices[2].tu	= 0;	pVertices[2].tv	= 0;
	pVertices[3].position = D3DXVECTOR3( surfaceW+adjx, adjy, 0 );
	pVertices[3].tu	= (float)memSrcW / nAlignW;	pVertices[3].tv	= 0;

#ifdef _X05
	pVertices[0].position.x -= 0.5f;		pVertices[0].position.y -= 0.5f;
	pVertices[1].position.x -= 0.5f;		pVertices[1].position.y -= 0.5f;
	pVertices[2].position.x -= 0.5f;		pVertices[2].position.y -= 0.5f;
	pVertices[3].position.x -= 0.5f;		pVertices[3].position.y -= 0.5f;
#endif 
	pVerticesSub[0].position = pVertices[0].position;
	pVerticesSub[0].tu = pVertices[0].tu;
	pVerticesSub[0].tv = pVertices[0].tv;
	pVerticesSub[1].position = pVertices[1].position;
	pVerticesSub[1].tu = pVertices[1].tu;
	pVerticesSub[1].tv = pVertices[1].tv;
	pVerticesSub[2].position = pVertices[2].position;
	pVerticesSub[2].tu = pVertices[2].tu;
	pVerticesSub[2].tv = pVertices[2].tv;
	pVerticesSub[3].position = pVertices[3].position;
	pVerticesSub[3].tu = pVertices[3].tu;
	pVerticesSub[3].tv = pVertices[3].tv;

	m_pVB->Unlock();
	m_pVBSub->Unlock();

	if( pImg )
	{
		s_sizeTotalVMem += (nAlignW * nAlignH * sizeof(DWORD));
		HRESULT hr = D3DXCreateTexture( DEVICE, nAlignW, nAlignH, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture );
		if( FAILED( hr ) ) {
			XALERT_OKCANCEL( "D3DXCreateTexture failed" );
			return XE::xFAIL;
		}
		D3DLOCKED_RECT	d3dLockRect;
		memset( &d3dLockRect, 0, sizeof( D3DLOCKED_RECT ) );
		if( FAILED( m_pTexture->LockRect( 0, &d3dLockRect, nullptr, 0 ) ) ) {
			XLOG_ALERT( "텍스쳐 Lock실패" );
			return XE::xFAIL;
		}
//		if( s_dwTextureCaps & D3DPTEXTURECAPS_POW2 || 1 )
		{
			DWORD *dst = (DWORD *)d3dLockRect.pBits;
			int memw, memh;
			DWORD *src = GetSrcImg( &memw, &memh );
			memset( dst, 0, sizeof(DWORD) * nAlignW * nAlignH );
			for( int i = 0; i < memh; i ++ )
			{
				for( int j = 0; j < memw; j ++ )
					*dst++ = *src++;
				dst += (nAlignW - memw);
			}
		}// else
		//	memcpy( d3dLockRect.pBits, GetSrcImg(), sizeof(DWORD) * surfaceW * surfaceH );
		m_pTexture->UnlockRect( 0 );
		if( bSrcKeep == FALSE )
			DestroySrcImg();	
	}
	
	return XE::xSUCCESS;
}

// pTexture의 내용을 읽어 메모리를 할당받아 리턴
DWORD* D3DUTLoadImg( LPDIRECT3DTEXTURE9 pTexture, int tw, int th )
{
	D3DLOCKED_RECT	d3dLockRect;
	memset( &d3dLockRect, 0, sizeof( D3DLOCKED_RECT ) );
	if( FAILED( pTexture->LockRect( 0, &d3dLockRect, nullptr, 0 ) ) ) {
		XERROR( "텍스쳐 Lock실패" );
		return nullptr;
	}
	//
	DWORD *pDst = new DWORD[ tw * th ];
	DWORD *pSrc = (DWORD*)d3dLockRect.pBits;
	memcpy_s( pDst, tw*th*sizeof(DWORD), pSrc, tw*th*sizeof(DWORD) ); 
	//
	pTexture->UnlockRect(0);
	return pDst;
}

XE::xRESULT		XSurfaceD3D::LoadImg( LPCTSTR szFilename, BOOL bKeepSrc )
{
	D3DXIMAGE_INFO		srcInfo;

	// 이건 락 안됨
	if( FAILED( D3DXCreateTextureFromFileEx( DEVICE, 
		szFilename, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, 
		D3DFMT_UNKNOWN, m_TexPool, D3DX_FILTER_POINT, D3DX_FILTER_POINT, 
		D3DCOLOR_XRGB(0,0,255), &srcInfo, nullptr, &m_pTexture ) ) )
	{
		return XE::xFAIL;
	}        
	// 서피스를 생성
	int w = srcInfo.Width;
	int h = srcInfo.Height;
	Create( (float)w, (float)h, 0, 0, xNO_ALPHA, nullptr, sizeof(DWORD) );		// srcInfo.Width가 unsigned라 계산이 이상해질때가 있어서 signed로 바꿨음
	// 로딩한 텍스쳐의 실제이미지를 메모리로 뜬다.
/*	락이 안되는 텍스쳐므로 이렇게 하면 안됨.
	if( bKeepSrc )
	{
		DWORD *pMem = D3DUTLoadImg( m_pTexture, w, h );
		if( pMem )
			SetSrcImg( pMem, w, h );
		else {
			XBREAKF( pMem == nullptr, "D3DUTCreateImg실패, %dx%d", w, h );
			return XE::xFAIL;
		}
	} */

	return XE::xSUCCESS;
}

void XSurfaceD3D::CopySurface( XSurface *src )
{
}
	
void XSurfaceD3D::DrawCore( int nAlpha )
{
	LPDIRECT3DDEVICE9  pd3dDevice = DEVICE;

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );		
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	if( m_DrawMode == xDM_NORMAL )	{
		pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA  );
///		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA );
	} else
	if( m_DrawMode == xDM_ADD )	{
		pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	} else
	if( m_DrawMode == xDM_MULTIPLY ) {
		pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
	} else
	if( m_DrawMode == xDM_SUBTRACT ) {
		pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	} else
	if( m_DrawMode == xDM_LIGHTEN ) {
		pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_MAX );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	} else
	if( m_DrawMode == xDM_DARKEN ) {
		pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_MIN );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	} else
	if( m_DrawMode == xDM_SCREEN ) {
		pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_MIN );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	} 
	pd3dDevice->SetTexture( 0, m_pTexture );
//	pd3dDevice->SetTexture( 0, nullptr );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	if( nAlpha < 255 )
	{
	} else {}
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

//    pd3dDevice->SetVertexShader( nullptr );
	HRESULT hr;
	XV( XE::s_pEffect->SetTexture( "Sprite", m_pTexture ) );	// base texture
	XV( XE::s_pEffect->SetTechnique( "RenderSprite" ) );
    UINT iPass, cPasses;
	XV( XE::s_pEffect->Begin( &cPasses, 0 ) );
	for( iPass = 0; iPass < cPasses; iPass++ )
	{
		XV( XE::s_pEffect->BeginPass( iPass ) );

		pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(XSURFACE_VERTEX) );
		pd3dDevice->SetFVF( D3DFVF_XSURFACE_VERTEX );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		XV( XE::s_pEffect->EndPass() );
	}
	XV( XE::s_pEffect->End() );
//	if( nAlpha < 255 )
//	{
//		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	}
	pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
}

void XSurfaceD3D::DrawCoreSub()
{
	LPDIRECT3DDEVICE9  pd3dDevice = DEVICE;

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );		
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	if( m_DrawMode == xDM_NORMAL )
	{
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
	} else
	if( m_DrawMode == xDM_ADD )
	{
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}
	pd3dDevice->SetTexture( 0, m_pTexture );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	HRESULT hr;
	XV( XE::s_pEffect->SetTexture( "Sprite", m_pTexture ) );	// base texture
	XV( XE::s_pEffect->SetTechnique( "RenderSprite" ) );
    UINT iPass, cPasses;
	XV( XE::s_pEffect->Begin( &cPasses, 0 ) );
	for( iPass = 0; iPass < cPasses; iPass++ )
	{
		XV( XE::s_pEffect->BeginPass( iPass ) );

		pd3dDevice->SetStreamSource( 0, m_pVBSub, 0, sizeof(XSURFACE_VERTEX) );
		pd3dDevice->SetFVF( D3DFVF_XSURFACE_VERTEX );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		XV( XE::s_pEffect->EndPass() );
	}
	XV( XE::s_pEffect->End() );
//	pd3dDevice->SetStreamSource( 0, m_pVBSub, 0, sizeof(XSURFACE_VERTEX) );
//	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

void XSurfaceD3D::DrawLocal( float x, float y, float lx, float ly, const D3DXMATRIX &mParent )
{
	D3DXMATRIX mTrans, mScale, mRot, mWorld, mAxis;
	if( m_DrawMode != xDM_NONE )		// 아무것도 그리지 않는 모드
	{
		LPDIRECT3DDEVICE9  pd3dDevice = DEVICE;
//		float gscale = __fResoScale;
//		D3DXMatrixScaling( &mScale, gscale, gscale, 1.0f );	// gscale
//		mWorld = mScale;
		XE::VEC2 vAdjAxis( m_fAdjustAxisX, m_fAdjustAxisY);
//		lx *= gscale;	ly *= gscale;
		D3DXMatrixTranslation( &mTrans, (float)((int)lx), (float)((int)ly), 0 );
		D3DXMatrixTranslation( &mAxis, -vAdjAxis.x, -vAdjAxis.y, 0 );	// 변환축을 옮겨놓는다
		mWorld = mTrans * mAxis;		// 필요에따라 Scale, Rotate매트릭스도 곱해줘야함
		D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
		D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(m_fRotY), D3DXToRadian(m_fRotX), D3DXToRadian(m_fRotZ) );
	//	D3DXMatrixTranslation( &mTrans, 0, 0, 0 );		// 생략
		mWorld *= mScale * mRot/* * mTrans*/;
		D3DXMatrixTranslation( &mAxis, vAdjAxis.x, vAdjAxis.y, 0 );		// 옮겨놨던 변환축을 복구
		mWorld *= mAxis;
		D3DXMatrixTranslation( &mTrans, (float)((int)x), (float)((int)y), 0 );	// 최종 스크린좌표
		mWorld *= mTrans * mParent;
		pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );
		HRESULT hr;
		XV( XE::s_pEffect->SetMatrix( "mViewProj", &XE::s_mViewProj ) );
		XV( XE::s_pEffect->SetMatrix( "mWorld", &mWorld ) );
		XV( XE::s_pEffect->SetFloat( "fOpacity", m_fAlpha ) );

		DrawCore();
	}
	// 그리고 난후 속성을 원래대로 돌려놓는다
	m_fAlpha = 1.0f;
	m_fScaleX = m_fScaleY = 1.0f;
	m_fRotX = m_fRotY = m_fRotZ = 0;
	m_fAdjustAxisX = m_fAdjustAxisY = 0;
	m_DrawMode = xDM_NORMAL;
}

void XSurfaceD3D::Draw( float x, float y, const D3DXMATRIX &mParent )
{
	D3DXMATRIX mTrans, mScale, mRot, mWorld, mAxis, mReverseAxis;
/*	
	툴에선 보통 창크기변동에 따라 뷰포트크기를 바꿔주지 얂기때문에 아래 클리핑이 먹지 않아서 일단 뺌. 새버전에선 제대로 해야함.
	if( x > GRAPHICS->GetViewportRight() )
		goto LP1;
	if( y > GRAPHICS->GetViewportBottom() )
		goto LP1;
	if( x + GetWidth() < 0 )
		goto LP1;
	if( y + GetHeight() < 0 )
		goto LP1;
*/
	if( m_DrawMode != xDM_NONE )		// 아무것도 그리지 않는 모드
	{
//		float gscale = __fResoScale;
//		D3DXMatrixScaling( &mScale, gscale, gscale, 1.0f );	// gscale
//		mWorld = mScale;
		LPDIRECT3DDEVICE9  pd3dDevice = DEVICE;
		D3DXMatrixTranslation( &mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
		D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
		D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(m_fRotY), D3DXToRadian(m_fRotX), D3DXToRadian(m_fRotZ) );
		D3DXMatrixTranslation( &mReverseAxis, m_fAdjustAxisX, m_fAdjustAxisY, 0 );
//		D3DXMatrixTranslation( &mTrans, (float)((int)x), (float)((int)y), 0 );
		D3DXMatrixTranslation( &mTrans, x, y, 0 );
		mWorld = mAxis * mScale * mRot * mReverseAxis * mTrans * mParent;	// 좌표축 옮겨놓고 * 축소하고 * 회전하고 * 다시 좌표축 돌려놓고 * 원하는 스크린위치에다 옮김 * 마지막으로 부모곱함
		pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );
		HRESULT hr;
		XV( XE::s_pEffect->SetMatrix( "mViewProj", &XE::s_mViewProj ) );
		XV( XE::s_pEffect->SetMatrix( "mWorld", &mWorld ) );
		XV( XE::s_pEffect->SetFloat( "fOpacity", m_fAlpha ) );
		XV( XE::s_pEffect->SetFloat( "col_r", m_ColorR ) );
		XV( XE::s_pEffect->SetFloat( "col_g", m_ColorG ) );
		XV( XE::s_pEffect->SetFloat( "col_b", m_ColorB ) );

		DrawCore();
	}
	// 그리고 난후 속성을 원래대로 돌려놓는다
	m_fAlpha = 1.0f;
	m_ColorR = m_ColorG = m_ColorB = 1.0f;
	m_fScaleX = m_fScaleY = 1.0f;
	m_fRotX = m_fRotY = m_fRotZ = 0;
	m_fAdjustAxisX = m_fAdjustAxisY = 0;
	m_DrawMode = xDM_NORMAL;
}

void XSurfaceD3D::DrawSub( float x, float y, const RECT *src )
{
	XSURFACE_VERTEX* pVertices;
	if( FAILED( m_pVBSub->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
		return;
	const int nAlignW = m_nAlignedWidth / ((GetbHighReso())? 2 : 1);
	const int nAlignH = m_nAlignedHeight / ((GetbHighReso())? 2 : 1);
	pVertices[0].position.x = 0;			// 좌하
	pVertices[0].position.y = (float)(src->bottom - src->top);
	pVertices[0].tu = (float)src->left / nAlignW;	
	pVertices[0].tv = (float)src->bottom / nAlignH;
	pVertices[1].position.x = (float)(src->right - src->left);			// 우하
	pVertices[1].position.y = (float)(src->bottom - src->top);
	pVertices[1].tu = (float)src->right / nAlignW;	
	pVertices[1].tv = (float)src->bottom / nAlignH;
	pVertices[2].position.x = 0;			// 좌하
	pVertices[2].position.y = 0;
	pVertices[2].tu = (float)src->left / nAlignW;			// 좌상
	pVertices[2].tv = (float)src->top / nAlignH;
	pVertices[3].position.x = (float)(src->right - src->left);									// 우상
	pVertices[3].position.y = 0;
	pVertices[3].tu = (float)src->right / nAlignW;	
	pVertices[3].tv = (float)src->top / nAlignH;
	m_pVBSub->Unlock();

	D3DXMATRIX mTrans, mScale, mRot, mWorld, mAxis, mReverseAxis;
	if( m_DrawMode != xDM_NONE )		// 아무것도 그리지 않는 모드
	{
		LPDIRECT3DDEVICE9  pd3dDevice = DEVICE;
		D3DXMatrixTranslation( &mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
		D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
		D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(m_fRotY), D3DXToRadian(m_fRotX), D3DXToRadian(m_fRotZ) );
		D3DXMatrixTranslation( &mReverseAxis, m_fAdjustAxisX, m_fAdjustAxisY, 0 );
		D3DXMatrixTranslation( &mTrans, x, y, 0 );
		mWorld = mAxis * mScale * mRot * mReverseAxis * mTrans;// * mParent;	// 좌표축 옮겨놓고 * 축소하고 * 회전하고 * 다시 좌표축 돌려놓고 * 원하는 스크린위치에다 옮김 * 마지막으로 부모곱함
		pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );
		HRESULT hr;
		XV( XE::s_pEffect->SetMatrix( "mViewProj", &XE::s_mViewProj ) );
		XV( XE::s_pEffect->SetMatrix( "mWorld", &mWorld ) );
		XV( XE::s_pEffect->SetFloat( "fOpacity", m_fAlpha ) );
		XV( XE::s_pEffect->SetFloat( "col_r", m_ColorR ) );
		XV( XE::s_pEffect->SetFloat( "col_g", m_ColorG ) );
		XV( XE::s_pEffect->SetFloat( "col_b", m_ColorB ) );

		DrawCoreSub();
	}
	// 그리고 난후 속성을 원래대로 돌려놓는다
	m_fAlpha = 1.0f;
	m_ColorR = m_ColorG = m_ColorB = 1.0f;
	m_fScaleX = m_fScaleY = 1.0f;
	m_fRotX = m_fRotY = m_fRotZ = 0;
	m_fAdjustAxisX = m_fAdjustAxisY = 0;
	m_DrawMode = xDM_NORMAL;

}

void XSurfaceD3D::Fill( DWORD col )
{
}

// 테스트용으로 만듬
void XSurfaceD3D::RenderTexture( float x, float y, float w, float h, LPDIRECT3DTEXTURE9 pd3dTexture )
{
	LPDIRECT3DDEVICE9       pd3dDevice = DEVICE;
//	pd3dTexture = s_pTexture;
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
//	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );		
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	float vertex2D[4][4+2] = 
	{
		{   x,   y,0,1, 0,0 },
		{ x+w, y,0,1, 1,0 },
		{ x+w, y+h,0,1, 1,1 },
		{    x, y+h,0,1, 0,1 },
	};
	pd3dDevice->SetTexture( 0, m_pTexture );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	
	pd3dDevice->SetVertexShader( nullptr );
	pd3dDevice->SetVertexDeclaration( nullptr );
	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertex2D, sizeof(float)*(4+2) );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}
