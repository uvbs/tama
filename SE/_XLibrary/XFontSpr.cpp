#include "stdafx.h"
#include "XFontSpr.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////
XBaseFontObj* XFontDatSpr::CreateFontObj( void )
{
	return new XFontObjSpr( this );
}
float XFontDatSpr::GetFontWidth( void )
{
	XSprite *pSpr = m_pSprDat->GetSprite( 0 );
	XBREAK( pSpr == NULL );
	return pSpr->GetWidth() + pSpr->GetAdjustX();
}
float XFontDatSpr::GetFontHeight( void )
{
	XSprite *pSpr = m_pSprDat->GetSprite( 0 );
	XBREAK( pSpr == NULL );
	return pSpr->GetHeight();
}
// 문자tc의 가로길이를 돌려준다
float XFontDatSpr::GetCharWidth( TCHAR tc )
{
	int idx = GetCharToIdx( tc );
	if( idx < 0 )
		return 0;
	XSprite *pSpr = m_pSprDat->GetSprite( idx );
	XBREAK( pSpr == NULL );
	return pSpr->GetWidth();
}

int XFontDatSpr::GetCharToIdx( TCHAR c )
{
	int idx = -1;
	if( c >= _T('!') && c <= _T('~') )
		idx = c - _T('!');
	else 
		idx = -1;	// *
	return idx; 
}

float XFontDatSpr::DrawString( float x, float y, LPCTSTR str, XCOLOR color, xFONT::xtStyle )
{
	const TCHAR *p;
	TCHAR c;
	p = str;
	float w = 0;
	float cw = 0;
	XBREAK( m_pSprDat == NULL );
	p = str;
	while(c = *p++ )
	{
		if( c == _T(' ') )		// 스페이스일때는 좌표만 이동하고 다음으로 넘어감
		{
			cw = m_pSprDat->GetSprite(0)->GetWidth() * m_vScale.w; 
			w += cw;
			x += cw;
			continue;
		}
		int idx = GetCharToIdx(c);
		if( idx < 0 )
			continue;
		XSprite *pSpr = m_pSprDat->GetSprite( idx );
		pSpr->SetfAlpha( m_Alpha );
		pSpr->SetScale( m_vScale );
		pSpr->Draw(x, y);
		cw = (pSpr->GetWidth() * m_vScale.x) + 1.0f + m_fDist;	
		w += cw;
		x += cw;
	}
	m_Alpha = 1.0f;
	m_vScale.Set( 1.0f );		// XFontSpr은 객체인스턴스가 아니기땜에 이런 변수값은 모두 1회성으로 간주하고 바로 삭제
	return w;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
float XFontObjSpr::GetLengthPixel( LPCTSTR str )
{
	const TCHAR *p = str;
	TCHAR c;
	float w = 0;
	while( c = *p++ )
	{
		if( c == _T(' ') )		// 스페이스일때는 좌표만 이동하고 다음으로 넘어감
			w += m_pDatSpr->GetFontWidth() * m_vScale.x;
		else
			w += m_pDatSpr->GetCharWidth( c ) * m_vScale.x + m_fDist;
	}
	return w;
}

// return: draw한 스트링의 pixel단위 길이를 돌려준다
float XFontObjSpr::_RenderText( float x, float y, LPCTSTR str )
{
	float w = GetLengthPixel( str );
	if( w == 0 )				// sprDat가 없는 상태면 스트링길이 읽어서 적당한 수 곱해서 걍 돌려줌
		return (float)_tcslen( str ) * (32.0f * m_vScale.x);
	if( m_Align & xALIGN_HCENTER ) {
		x -= w * 0.5f;
		x += 1.0f;		// 왼쪽으로 너무 붙어서 +2넣어줌
	}
	else if( m_Align & xALIGN_RIGHT )
		x -= w;
	
	if( m_Align & xALIGN_VCENTER )
		y -= GetFontHeight() * 0.5f;

	m_pDatSpr->SetAlpha( m_Alpha );
	m_pDatSpr->SetvScale( m_vScale );
	m_pDatSpr->SetfDist( m_fDist );
	m_pDatSpr->DrawString( x, y, str, m_Color, xFONT::xSTYLE_NORMAL );
	return w;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

