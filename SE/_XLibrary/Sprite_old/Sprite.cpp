#include "stdafx.h"
#include "Sprite.h"
#include "SprDat.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

int XSprite::s_sizeTotalMem = 0;

void XSprite::Destroy( void )
{
}
void XSprite::Load( XSprDat *pSprDat, XBaseRes *pRes, BOOL bSrcKeep )
{
	DWORD *pImg;
	if( pSprDat->IsUpperVersion(10) )
	{
		int strSize;
		TCHAR sztSrcFile[1024];
		pRes->Read( &strSize, 4 );
		pRes->Read( sztSrcFile, strSize );	
		DWORD dw1;
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
		// 게임에선 소스이미지 정보 읽지 않음
	}
	float width, height, adjX, adjY;
	// 15버전 이후부터는 파일에 저장되는 사이즈 규격이 메모리크기에서 서피스 크기로 변경되었다.
	// 그러므로 15버전 이전파일들은 사이즈값을 줄여줘야 한다
	if( pSprDat->IsLowerVersion( 15 ) )	// 0 ~ 14
	{
		int nWidth, nHeight, nAdjustX, nAdjustY;
		pRes->Read( &nWidth, 4 );	
		pRes->Read( &nHeight, 4 );	
		pRes->Read( &nAdjustX, 4 );	
		pRes->Read( &nAdjustY, 4 );
		width = ConvertToSurfaceSize( nWidth );
		height = ConvertToSurfaceSize( nHeight );
		adjX = ConvertToSurfaceSize( nAdjustX );
		adjY = ConvertToSurfaceSize( nAdjustY );
	} else
	{
		pRes->Read( &width, 4 );	
		pRes->Read( &height, 4 );	
		pRes->Read( &adjX, 4 );	
		pRes->Read( &adjY, 4 );
	}
	int memw = ConvertToMemSize( width );
	int memh = ConvertToMemSize( height );
	int size = memw * memh;
	pImg = new DWORD[ size ];
	pRes->Read( pImg, 4 * size );			// RGBA8888포맷
#ifdef _VER_OPENGL
	XSurfaceOpenGL::Create( width, height, adjX, adjY, xALPHA, pImg, sizeof(DWORD), 0, bSrcKeep );
	SAFE_DELETE( pImg );    // 뭐야 이거 -_-;;;  조낸 일관성 없네.
#else
	// rgb순서를 바꿔 읽는다
	DWORD *_pTemp = pImg, dw1;
	BYTE a, r, g, b;
	for( int i = 0; i < size; i ++ )
	{
		dw1 = *_pTemp;
		a = XCOLOR_BGR_A( dw1 );		// abgr로 저장되어있는 데이타를 argb로 바꾼다
		r = XCOLOR_BGR_R( dw1 );
		g = XCOLOR_BGR_G( dw1 );
		b = XCOLOR_BGR_B( dw1 );
		*_pTemp++ = XCOLOR_RGBA( r, g, b, a );
	}

	XSurfaceD3D::Create( width, height, adjX, adjY, xNO_ALPHA, pImg, 4, 0, bSrcKeep );
    // d3d쪽도 Create()안에서 메모리를 삭제하는 방식은 피해야 할듯
#endif
}

MATRIX* XSprite::GetMatrix( MATRIX *pOut, float lx, float ly )
{
	lx += m_fAdjustAxisX;		// 좌표축 보정
	ly += m_fAdjustAxisY;
	MATRIX mTrans, mScale, mAxis;
	MatrixTranslation( mTrans, lx, ly, 0 );
	MATRIX mRx, mRy;
	MatrixRotationY( mRy, D2R(m_fRotY) );
	MatrixRotationX( mRx, D2R(m_fRotX) );
	MatrixMultiply( mRx, mRy, mRx );		// 순서가 XY인가 YX인가 ㅡ.ㅡ?
	MatrixScaling( mScale, m_fScaleX, m_fScaleY, 1.0f );
	MatrixTranslation( mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
	MatrixMultiply( *pOut, mAxis, mScale );
	MatrixMultiply( *pOut, *pOut, mRx );
	MatrixMultiply( *pOut, *pOut, mTrans );
//	*pOut = mAxis * mScale * mRx * mTrans;
	return pOut;
}
