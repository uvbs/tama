#include "stdafx.h"
#ifdef _VER_OPENGL
#include "XFontObjFTGL.h"
#include "XFontMng.h"
#include "etc/xMath.h"
#include "XBatchRenderer.h"
#ifdef _VER_IOS
#undef min
#undef max
#endif
#include "etc/xUtil.h"
#include "XFramework/client/XClientMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


XFontObjFTGL::XFontObjFTGL( XBaseFontDat *pDat ) 
: XBaseFontObj( pDat ) 
{
	Init(); 
	XFontDatFTGL *pFontDat = m_pFTGLDat = SafeCast<XFontDatFTGL*, XBaseFontDat*>( pDat );
	m_idCurrFTGLDat = pFontDat->GetidFTGLFont();
// 	SetLineLength( XE::GetGameWidth() );
	SetLineLength( 0 );		// 이제 0이 기본값임
	GetLayout().SetFont( pFontDat->Getfont() );
    float linespace = GetLayout().GetLineSpacing();
    GetLayout().SetLineSpacing( linespace * 1.2f );
}

XFontObjFTGL::XFontObjFTGL( LPCTSTR szFont, float fontSize, float outset )
//: XBaseFontObj( FONTMNG->Load( szFont, fontSize ) ) 
: XBaseFontObj( szFont, fontSize )
{
	Init(); 
	XBREAK( GetpDat() == NULL );
	XFontDatFTGL *pFontDat = m_pFTGLDat = SafeCast<XFontDatFTGL*, XBaseFontDat*>( GetpDat() );
	m_idCurrFTGLDat = pFontDat->GetidFTGLFont();
// 	SetLineLength( XE::GetGameWidth() );
	SetLineLength( 0 );
	GetLayout().SetFont( pFontDat->Getfont() );
    float linespace = GetLayout().GetLineSpacing();
    GetLayout().SetLineSpacing( linespace * 1.2f );
}

void XFontObjFTGL::SetAlign( XE::xAlign align )
{ 
	XBaseFontObj::SetAlign( align );
	if( align == XE::xALIGN_LEFT )
		GetLayout().SetAlignment( FTGL::ALIGN_LEFT );
	else if( align == XE::xALIGN_RIGHT )
		GetLayout().SetAlignment( FTGL::ALIGN_RIGHT );
 	else if( align & XE::xALIGN_HCENTER )
 		GetLayout().SetAlignment( FTGL::ALIGN_CENTER );
	else if( align & XE::xALIGN_VCENTER ) {
		XBREAK(1);		// 이걸로 여기까지 호출하면 안됨. 상위에서 처리 다끝내고 hcenter까지만 와야함.
	}
	// FTGL엔 VCENTER기능이 없으므로 여기서 처리하지 않는다.
}
void XFontObjFTGL::SetLineLength( float screenw ) 
{
	XBaseFontObj::SetLineLength( screenw );
	// FTGL전용 코드
	if( screenw == 0 )
		screenw = XE::GetGameWidth();		// 자동으로 최대사이즈로 입력한다.
	screenw *= 2.f;
	GetLayout().SetLineLength( screenw );
}

void XFontObjFTGL::SetColor( XCOLOR color )
{
	XBaseFontObj::SetColor( color );
	// FTGL전용 코드
	m_R = XCOLOR_RGB_R(color) / 255.0f;
	m_G = XCOLOR_RGB_G(color) / 255.0f;
	m_B = XCOLOR_RGB_B(color) / 255.f;
	m_A = XCOLOR_RGB_A(color) / 255.f;
}

float XFontObjFTGL::GetLengthPixel( LPCTSTR str )
{
	XBREAK(1);
	return 0;
}
// FontObj를 static화 시킨다. 
//void XFontObjFTGL::SetStaticText( LPCTSTR str )
//{
//   m_sizeLayout = GetLayoutSize( str );    // 미리 레이아웃 크기를 계산함
//}

// szString을 지역변수버퍼로 넘기니까 에러가 난다 왜그러지?
// szString이 utf-8이 아니면 에러나더라..
XE::VEC2 XFontObjFTGL::GetLayoutSize( LPCTSTR szString )
{
	if( XE::IsEmpty( szString ) )
		return XE::VEC2();
	XE::VEC2 vSize;
//	XTRACE("bbox1");
	FTBBox box = GetLayout().BBox( szString );             // 게임실행중 이걸 호출하니까 화면이 껌뻑거린다? 내부에 버그있나?
//	XTRACE("bbox2");
	vSize.x = (float)(box.Upper().X() - box.Lower().X());		// 더 큰사이즈로...
	vSize.y = (float)(box.Upper().Y() - box.Lower().Y());
//	vSize.y += GetFontHeight() * 1.1f;
	if( XE::IsHave(szString) && vSize.IsZero() )
		vSize.Set( 1 );		// 영문폰트를 쓸때 한글출력이 들어온다면 0이나와서 assert창이 너무 많이 떠서 1로 바꿈.
	// 480x320좌표계로 변환
	XFontDatFTGL::ToScreenCood( &vSize );
	return vSize;
//    return XE::VEC2(135,95);
}
XE::VEC2 XFontObjFTGL::GetLayoutPos( LPCTSTR szString )
{
	if( XE::IsEmpty( szString ) )
		return XE::VEC2();
	XE::VEC2 vPos;
	FTBBox box = GetLayout().BBox( szString );             // 게임실행중 이걸 호출하니까 화면이 껌뻑거린다? 내부에 버그있나?
  vPos.x = (float)box.Upper().X();
  vPos.y = (float)box.Upper().Y();
	// 480x320좌표계로 변환
	XFontDatFTGL::ToScreenCood( &vPos );
	return vPos;
}
/**
 @brief 어쩌면 필요없을지도 모른다. BBox함수가 내부에서 캐싱을 하고 있는것 같으므로.
*/
// void XFontObjFTGL::GetLayoutPosAndSize( XE::VEC2* pOutPos, XE::VEC2* pOutSize )
// {
// 	// 한꺼번에 두가지를 모두 구하는 함수. 최적화용
// }

float XFontObjFTGL::_RenderText( float x, float y, LPCTSTR szString )
{
	if( m_bBatch ) {
		auto pRenderer = XBatchRenderer::sGetpCurrRenderer();
		if( XASSERT(pRenderer) )
			RenderTextBatch( pRenderer, XE::VEC2(x,y), szString );
		return 0.f;
	}
	if( XE::IsEmpty( szString ) )
		return 0;
	const float hFont = GetFontHeight();
	const float hFontAdj = (hFont * 0.8f);		// 폰트 출력시 기준이 되는 y값이 폰트의 아래쪽이 되어야 하는데 위쪽으로 사용해서 생기는 근본적인 문제. 폰트의 좌표계를 바꾸지 않는한 이렇게 하드코딩으로 보정해야한다.
	y += hFontAdj * m_vScale.y;		// FTGL은 좌표계가 글자의 아랫부분이므로 편의상 글자위 좌상귀가 좌표가 되도록 바꿈
																//    x += m_vAdjust.x;
																//    y += m_vAdjust.y;
																// 프리타입 폰트는 독자적인 좌표계를 씀. 아래쪽이 0임
	MATRIX mVP;
	XE::VEC2 sizeViewport = GRAPHICS->GetViewportSize();
	MatrixMakeOrtho( mVP, 0, sizeViewport.w*2.f, 0, sizeViewport.h*2.f, -1.0f, 1.0f );

	XE::VEC2 vc( x, y );	// virtual coodinate
												// 480x320좌표계를 1024x768좌표계로 바꿈
												// 그리고 y좌표를 뒤집음
	vc *= 2.f;
	vc.y = sizeViewport.h * 2.f - vc.y;

	float adjStyle = 1.f * m_vScale.x;
	float alpha = GetAlpha() * m_A;
	if( m_Style == xFONT::xSTYLE_SHADOW ) {
		MATRIX mModel, mMVP, mWorld;
		MatrixIdentity( mWorld );
		MatrixScaling( mModel, m_vScale.x, m_vScale.y, 1.0f );
		MatrixMultiply( mWorld, mWorld, mModel );
		MatrixTranslation( mModel, vc.x + adjStyle, vc.y - adjStyle, 0 );
		MatrixMultiply( mWorld, mWorld, mModel );
		MatrixMultiply( mMVP, mWorld, mVP );
		xFTGL::SetMatrixModelViewProjection( GetMatrixPtr( mMVP ) );
		if( m_R == 0 && m_G == 0 && m_B == 0 )
			xFTGL::SetColor( 1.0f, 1.0f, 1.0f, alpha );
		else
			xFTGL::SetColor( 0, 0, 0, alpha );
#ifdef _XPROFILE
		if( !(XGraphics::s_dwDraw & XE::xeBitNoFont) )
#endif // _XPROFILE
			GetLayout().Render( szString, -1, FTPoint(), FTGL::RENDER_FRONT );
	} else
	if( m_Style == xFONT::xSTYLE_STROKE ) {
		_RenderStroke( vc.x - adjStyle, vc.y, szString, mVP );
		_RenderStroke( vc.x + adjStyle, vc.y, szString, mVP );
		_RenderStroke( vc.x, vc.y - adjStyle, szString, mVP );
		_RenderStroke( vc.x, vc.y + adjStyle, szString, mVP );
	}
	// 알파라 따로 지정되어 있으면 알파값을우선으로 쓴다.
	xFTGL::SetColor( m_R, m_G, m_B, alpha );
	// 글자 확축을 하려면 전체 글자레이아웃에서 중심을 기준으로 확축이 되어야하는데 현재는 좌측하단을 기준으로 확축이 되고 있음. 1.0스케일에서의 레이아웃 크기를 먼저 계산한후 축소된 글자를 찍을땐 x,y좌표를 보정해주는 방법이 있음.
	MATRIX mModel, mMVP, mWorld;
	MatrixIdentity( mWorld );
	auto vScale = m_vScale;
	MatrixScaling( mModel, vScale.x, vScale.y, 1.0f );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixTranslation( mModel, vc.x, vc.y, 0 );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixMultiply( mMVP, mWorld, mVP );
	xFTGL::SetMatrixModelViewProjection( GetMatrixPtr( mMVP ) );

#ifdef _XPROFILE
	if( !(XGraphics::s_dwDraw & XE::xeBitNoFont) )
#endif // _XPROFILE
		GetLayout().Render( szString, -1, FTPoint(), FTGL::RENDER_FRONT );

	return 0;
}

void XFontObjFTGL::_RenderStroke( float x, float y, LPCTSTR szStr, const MATRIX& mVP )
{
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoFont) 
		return;
#endif // _XPROFILE
	float alpha = GetAlpha() * m_A;
  xFTGL::SetColor( 0, 0, 0, alpha );
	const auto vScale = m_vScale;
	MATRIX mModel, mMVP, mWorld;
	MatrixIdentity( mWorld );
	MatrixScaling( mModel, vScale.x, vScale.y, 1.0f );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixTranslation( mModel, x, y, 0 );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixMultiply( mMVP, mWorld, mVP );
    xFTGL::SetMatrixModelViewProjection( GetMatrixPtr(mMVP) );
	
	GetLayout().Render(szStr, -1, FTPoint(), FTGL::RENDER_FRONT );
}

void XFontObjFTGL::RenderTextBatch( XBatchRenderer* pRenderer,
																		const XE::VEC2& _vPos, 
																		LPCTSTR szString )
{
	if( XE::IsEmpty(szString) )
		return;
	XE::VEC2 vc = _vPos;
	const float hFont = GetFontHeight();
	const float hFontAdj = (hFont * 0.8f);		// 폰트 출력시 기준이 되는 y값이 폰트의 아래쪽이 되어야 하는데 위쪽으로 사용해서 생기는 근본적인 문제. 폰트의 좌표계를 바꾸지 않는한 이렇게 하드코딩으로 보정해야한다.
	vc.y += hFontAdj * m_vScale.y;		// FTGL은 좌표계가 글자의 아랫부분이므로 편의상 글자위 좌상귀가 좌표가 되도록 바꿈
	// 프리타입 폰트는 독자적인 좌표계를 씀. 아래쪽이 0임
	MATRIX mVP;
	const XE::VEC2 sizeViewport = GRAPHICS->GetViewportSize();
	MatrixMakeOrtho( mVP, 0, sizeViewport.w*2.f, 0, sizeViewport.h*2.f, -1.0f, 1.0f );

	// 480x320좌표계를 1024x768좌표계로 바꿈
	vc *= 2.f;
	// 그리고 y좌표를 뒤집음
	vc.y = sizeViewport.h * 2.f - vc.y;

	const float adjStyle = 1.f * m_vScale.x;
	const float alpha = GetAlpha() * m_A;
	// 알파라 따로 지정되어 있으면 알파값을우선으로 쓴다.
	// 글자 확축을 하려면 전체 글자레이아웃에서 중심을 기준으로 확축이 되어야하는데 현재는 좌측하단을 기준으로 확축이 되고 있음. 1.0스케일에서의 레이아웃 크기를 먼저 계산한후 축소된 글자를 찍을땐 x,y좌표를 보정해주는 방법이 있음.
	MATRIX mModel, mMVP, mWorld;
	MatrixIdentity( mWorld );
	auto vScale = m_vScale;
	MatrixScaling( mModel, vScale.x, vScale.y, 1.0f );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixTranslation( mModel, vc.x, vc.y, 0 );
	MatrixMultiply( mWorld, mWorld, mModel );
	MatrixMultiply( mMVP, mWorld, mVP );

#ifdef _XPROFILE
	if( !(XGraphics::s_dwDraw & XE::xeBitNoFont) )
#endif // _XPROFILE
	{
		static ID s_idFont = 0;
		xRenderCmd::xCmd cmd;
		cmd.m_v4Color = XE::VEC4( m_R, m_G, m_B, alpha );
		cmd.m_Font.m_idFont = ++s_idFont;
		cmd.m_Font.m_vPos = vc;
		cmd.m_Font.m_vScale = vScale;
		cmd.m_Font.m_adjStyle = adjStyle;
		cmd.m_Font.m_mVP = mVP;
		cmd.m_Font.m_Style = m_Style;
		cmd.m_Font.m_pFont = nullptr;
		cmd.m_Font.m_pLayer = &_m_Layout;
		cmd.m_Font.m_szString = szString;
		pRenderer->PushCmd( cmd );
	}
} // RenderTextBatch

#endif // gl