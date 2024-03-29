﻿#include "stdafx.h"
#include "Sprite.h"
#include "SprDat.h"
#include "XResObj.h"
#include "XResMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

int XSprite::s_sizeTotalMem = 0;
// float XSprite::s_H = 0.f;
// float XSprite::s_S = 0.f;
// float XSprite::s_L = 0.f;		
// XE::VEC3 XSprite::s_HSL;
// XE::VEC2 XSprite::s_Range1;
// XE::VEC2 XSprite::s_Range2;
XE::xHSL XSprite::s_HSL;
/**
 @brief 
*/
XSprite::XSprite( int idx ) {
	Init();
	m_idxSprite = idx;
	// 이제 highreso가 false인 스프라이트는 없는걸로. 있으면 m_bHighReso다시 살릴것.
	m_pSurface = GRAPHICS->CreateSurface();
}
// XSprite::XSprite( BOOL bHighReso, int idx ) {
// 	Init();
// 	m_idxSprite = idx;
// 	m_pSurface = GRAPHICS->CreateSurface( bHighReso );
// }

void XSprite::Destroy( void )
{
	SAFE_DELETE( m_pSurface );
}
void XSprite::Load( XSprDat *pSprDat, XBaseRes *pRes, BOOL bSrcKeep, BOOL bRestore )
{
	if( bRestore ) {
		m_pSurface->ClearDevice();
	}
	DWORD *pImg = NULL;
	auto formatSurface = XE::xPF_ARGB4444;		// 디폴트 서피스 포맷
	{
		int strSize;
		TCHAR sztSrcFile[1024];
		pRes->Read( &strSize, 4 );
		pRes->Read( sztSrcFile, strSize );	
		DWORD dw1;
		if( pSprDat->IsUpperVersion(27) ) {
			BYTE b0;
			pRes->Read( &b0, 1 );		formatSurface = ( XE::xtPixelFormat )b0;
			if( XBREAK( formatSurface <= XE::xPF_NONE || formatSurface >= XE::xPF_MAX ) ) {
				formatSurface = XE::xPF_ARGB8888;
			}
			pRes->Read( &dw1, 3 );
		} else {
			pRes->Read( &dw1, 4 );
		}
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
		// 게임에선 소스이미지 정보 읽지 않음
	}
	float width, height, adjX, adjY;
	// 15버전 이후부터는 파일에 저장되는 사이즈 규격이 메모리크기에서 서피스 크기로 변경되었다.
	// 그러므로 15버전 이전파일들은 사이즈값을 줄여줘야 한다
	pRes->Read( &width, 4 );	
	pRes->Read( &height, 4 );	
	pRes->Read( &adjX, 4 );	
	pRes->Read( &adjY, 4 );
	XSPR_TRACE("SprDat: w,h(%d,%d)", (int)width, (int)height );
	const auto sizeSurface = XE::VEC2( width, height );
	const auto vSizeMem = sizeSurface * 2;
	const XE::POINT sizeMem( (int)vSizeMem.w, (int)vSizeMem.h );
	const int size = sizeMem.Size();
// 	int memw = m_pSurface->ConvertToMemSize( width );
// 	int memh = m_pSurface->ConvertToMemSize( height );
// 	int size = memw * memh;
	if( pSprDat->IsUpperVersion(25) ) {
		// 압축포맷
		int sizeComp, sizeUncomp;
		pRes->Read( &sizeComp, 4 );
		XSPR_TRACE("SprDat: compSize=%d", sizeComp );
		XBREAK( sizeComp < 0 );
		pRes->Read( &sizeUncomp, 4 );
		XSPR_TRACE("SprDat: UncompSize=%d", sizeUncomp );
		XBREAK( sizeUncomp < 0 );
		if( sizeComp > 0 ) {
			const _tstring strKey = XFORMAT( "%s%d", pSprDat->GetszFilename(), idxSpr );
			pImg = XSprMng::sGet()->GetImgFromCache( strKey );		// 이미 한번 로딩했던거면 캐시에서 찾음.
			if( pImg == nullptr ) {
				BYTE *pComp = new BYTE[sizeComp];
				pRes->Read( pComp, sizeComp );
				BYTE *pUncomp;
				if( XSYSTEM::UnCompressMem( &pUncomp, sizeUncomp, pComp, sizeComp ) ) {
					XBREAK( size * 4 != sizeUncomp );
					SAFE_DELETE_ARRAY( pComp );
					pImg = (DWORD*)pUncomp;
					XSprMng::sGet()->AddCache( strKey, pImg );
				} else {
					const _tstring strFile = XE::GetFileName( pSprDat->GetszFilename() );
					XBREAKF( 1, "spr uncompress failed:[%s] sizecomp=%d, sizeUncomp=%d", strFile.c_str(), sizeComp, sizeUncomp );
				}
			} else {
				pRes->Seek( sizeComp );
			}
		}
	} else {
		pImg = new DWORD[ size ];
		pRes->Read( pImg, 4 * size );			// RGBA8888포맷
	}

// #ifdef WIN32
#ifdef _VER_DX
	// BGR로 저장되어있는 데이타를 -> RGB로 변환
	ConvertMemBGR2RGB( pImg, size );
#endif // WIN32
	// HSL값이 있다면 변환
	if( !s_HSL.m_vHSL.IsZero() ) {
		const float h = D2R(s_HSL.m_vHSL.x);
		const float s = s_HSL.m_vHSL.y / 100.f;
		const float l = s_HSL.m_vHSL.z / 100.f;
		const XE::VEC2 range1 = D2R(s_HSL.m_vRange1);
		const XE::VEC2 range2 = D2R(s_HSL.m_vRange2);
		if( range1.v2 - range1.v1 == 0.f || range2.v2 - range2.v1 == 0.f)
			ApplyHSLNormal( pImg, size, h, s, l);
		else
			ApplyHSLRanged( pImg, size, h, s, l, range1, range2 );
	}
#ifdef _VER_OPENGL
	if( bRestore )
		// restore는 bKeepSrc를 무조건 false로 해야 중복해서 소스이미지가 만들어지지 않는다.
		m_pSurface->Create( XE::POINT(width, height)
											, XE::VEC2(adjX,adjY)
											, formatSurface
											, pImg
											, XE::xPF_ARGB8888		// src format
											, sizeMem // XE::POINT(memw,memh)
											, false
											, false );
//		m_pSurface->Create( width, height, adjX, adjY, xALPHA, pImg, sizeof(DWORD), 0, FALSE );
	else
		m_pSurface->Create( XE::POINT(width, height)
											, XE::VEC2(adjX,adjY)
											, formatSurface
											, pImg
											, XE::xPF_ARGB8888		// src format
											, sizeMem // XE::POINT(memw,memh)
											, bSrcKeep != FALSE
											, false );
//		m_pSurface->Create( width, height, adjX, adjY, xALPHA, pImg, sizeof(DWORD), 0, bSrcKeep );
	SAFE_DELETE_ARRAY( pImg );    // 뭐야 이거 -_-;;;  조낸 일관성 없네.
#else // opengl
	// d3d쪽은 restore를 아직 구현안했음. 윈도8포팅할때는 아래 Create()없애고 CreateFromImg()를 똑같이 일관되게 써서 구현할것.
	XBREAK( bRestore == TRUE );
	XSPR_TRACE("SprDat: CreateSurface" );
	if( bRestore )
		m_pSurface->Create( XE::POINT(width, height)
											, XE::VEC2(adjX,adjY)
											, formatSurface
											, pImg
											, XE::xPF_ARGB8888
											, sizeMem //XE::POINT(memw,memh)
											, false
											, false );
	else
		m_pSurface->Create( XE::POINT(width, height)
											, XE::VEC2(adjX,adjY)
											, formatSurface
											, pImg
											, XE::xPF_ARGB8888
											, sizeMem // XE::POINT(memw,memh)
											, bSrcKeep != FALSE
											, false );
    // d3d쪽도 Create()안에서 메모리를 삭제하는 방식은 피해야 할듯
	SAFE_DELETE_ARRAY( pImg );		// 15.12.07 추가
#endif // win32
}
/**
 @brief ABGR포맷으로 저장되어있는 pImg블럭의 픽셀데이타를 ARGB포맷으로 변환해서 pImg에 다시 넣는다.
*/
#ifdef WIN32
void XSprite::ConvertMemBGR2RGB( DWORD *pImg, int sizeBytes )
{
	// rgb순서를 바꿔 읽는다
	DWORD *_pTemp = pImg, dw1;
	BYTE a, r, g, b;
	for( int i = 0; i < sizeBytes; i ++ ) {
		dw1 = *_pTemp;
		a = XCOLOR_A_FROM_ABGR( dw1 );		// abgr로 저장되어있는 데이타를 argb로 바꾼다
		r = XCOLOR_R_FROM_ABGR( dw1 );
		g = XCOLOR_G_FROM_ABGR( dw1 );
		b = XCOLOR_B_FROM_ABGR( dw1 );
		// ARGB의 순서로 메모리를 재배열한다.
		*_pTemp++ = XCOLOR_TO_ARGB( a, r, g, b );
	}
}
#endif // WIN32

/**
 @brief ARGB포맷으로 저장되어있는 pImg의 데이타를 HSL값을 적용하여 변환한다.
 @param ah 0~2*PI(radian)
 @param as 0~1.0
 @param al 0~1.0
 포토샵 HSL변환시 변환컬러 범위가 지정되었을때.
 rRange1.v1----------rRange1.v2----------------rRange2.v1-----------rRange2.v2
            HSL보정 보간구간    HSL보정 100%적용구간       HSL보정 보간구간
*/
void XSprite::ApplyHSLRanged( DWORD *pImg, const int sizeBytes, const float ah, const float as, const float al
												, const XE::VEC2& rRange1
												, const XE::VEC2& rRange2 )
{
	const auto rDist1 = XE::GetDistRadian( rRange1.v1, rRange1.v2 );
	const auto rDist2 = XE::GetDistRadian( rRange2.v1, rRange2.v2 );
	const auto rDist3 = XE::GetDistRadian( rRange1.v2, rRange2.v1 );
	XBREAK( rDist1 <= 0 );
	XBREAK( rDist2 <= 0 );
	if( rDist1 == 0.f || rDist2 == 0.f ) {
		return ApplyHSLNormal( pImg, sizeBytes, ah, as, al );
	}
	// 변환시 색범위가 지정된 버전.
	// rgb순서를 바꿔 읽는다
	DWORD *_pTemp = pImg;
	const auto pi2 = 2 * 3.141592f;
	for( int i = 0; i < sizeBytes; i++ ) {
		XCOLOR colSrc = *_pTemp;
		BYTE a = XCOLOR_RGB_A( colSrc );
		double h, s, l;
		XE::RGB2HSL( colSrc, h, s, l );
		float hf = (float)h;
		float sf = (float)s;
		float lf = (float)l;
		// H
		float ratio = 0.f;
		const auto rDistHf1 = XE::GetDistRadian( rRange1.v1, hf );
		if( rDistHf1 < rDist1 ) {
			ratio = (hf - rRange1.v1) / rDist1;
		} else {
			const auto rDistHf2 = XE::GetDistRadian( rRange2.v1, hf );
			if( rDistHf2 < rDist2 ) {
				ratio = 1.0f - ( ( hf - rRange2.v1 ) / rDist2 );
			} else {
				const auto rDistHf3 = XE::GetDistRadian( rRange1.v2, hf );
				if( rDistHf3 < rDist3 ) {
					ratio = 1.f;
				}
			}
		}
		h += ah * ratio;
		if( h < 0 )
			h = pi2 + h;
		if( h > pi2 )
			h = h - pi2;
		// S
		s += as * ratio;
		if( s < 0 )
			s = 0;
		if( s > 1.0 )
			s = 1.0;
		l += al * ratio;
		if( l < 0 )
			l = 0;
		if( l > 1.0 )
			l = 1.0;
		XCOLOR colDst = XE::HSL2RGB( h, s, l );
		colDst &= ~( 0xff000000 );
		colDst |= ( (DWORD)a << 24 );
		*_pTemp++ = colDst;
	}
}

/**
 @brief 변환시 변환범위를 지정하지 않는 버전.
*/
void XSprite::ApplyHSLNormal( DWORD *pImg, const int sizeBytes, const float ah, const float as, const float al )
{
	// rgb순서를 바꿔 읽는다
	DWORD *_pTemp = pImg;
	const auto pi2 = 2 * 3.141592f;
	for( int i = 0; i < sizeBytes; i++ ) {
		XCOLOR colSrc = *_pTemp;
		const BYTE a = XCOLOR_RGB_A( colSrc );
		double h, s, l;
		XE::RGB2HSL( colSrc, h, s, l );
		// H
		h += ah;
		if( h < 0 )
			h = pi2 + h;
		if( h > pi2 )
			h = h - pi2;
		// S
		s += as;
		if( s < 0 )
			s = 0;
		if( s > 1.0 )
			s = 1.0;
		l += al;
		if( l < 0 )
			l = 0;
		if( l > 1.0 )
			l = 1.0;
		XCOLOR colDst = XE::HSL2RGB( h, s, l );
		colDst &= ~( 0xff000000 );
		colDst |= ( (DWORD)a << 24 );
		*_pTemp++ = colDst;
	}
}
MATRIX* XSprite::GetMatrix( MATRIX *pOut, float lx, float ly )
{
	lx += m_pSurface->GetfAdjustAxisX();		// 좌표축 보정
	ly += m_pSurface->GetfAdjustAxisY();
	MATRIX mTrans, mScale, mAxis;
	MatrixTranslation( mTrans, lx, ly, 0 );
	MATRIX mRx, mRy;
	MatrixRotationY( mRy, D2R(m_pSurface->GetfRotY()) );
	MatrixRotationX( mRx, D2R(m_pSurface->GetfRotX()) );
	MatrixMultiply( mRx, mRy, mRx );		// 순서가 XY인가 YX인가 ㅡ.ㅡ?
	MatrixScaling( mScale, m_pSurface->GetfScaleX(), m_pSurface->GetfScaleY(), 1.0f );
	MatrixTranslation( mAxis, -m_pSurface->GetfAdjustAxisX(), -m_pSurface->GetfAdjustAxisY(), 0 );
	MatrixMultiply( *pOut, mAxis, mScale );
	MatrixMultiply( *pOut, *pOut, mRx );
	MatrixMultiply( *pOut, *pOut, mTrans );
//	*pOut = mAxis * mScale * mRx * mTrans;
	return pOut;
}


