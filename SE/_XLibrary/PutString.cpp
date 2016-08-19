/*
 *  PutString.cpp
 *  ShakeMP
 *
 *  Created by xuzhu on 10. 6. 22..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "XFontSpr.h"
#include "xString.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#if 0 
float XPutString::DrawNumber( float x, float y, int num, xAlign align )
{
	TCHAR szBuff[64];
	_stprintf_s( szBuff, _T("%d"), num );
	return DrawString( x, y, szBuff, align );
}
float XPutString::DrawFloat( float x, float y, float num, xAlign align )
{
	TCHAR szBuff[64];
	_stprintf_s( szBuff, _T("%.2f"), num );
	return DrawString( x, y, szBuff );
}
float XPutString::DrawString( float x, float y, xAlign align, LPCTSTR format, ... )
{
    TCHAR szBuff[1024];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, format);
    _vstprintf_s(szBuff, format, vl);
	float len = DrawString(x, y, szBuff, align );
    va_end(vl);
	return len;
}
// char c의 스프라이트 인덱스를 돌려준다.
int XPutString::GetCharToIdx( TCHAR c )
{
	int idx = -1;
	if( c >= _T('!') && c <= _T('~') )
		idx = c - _T('!');
	else 
		idx = -1;	// *
	return idx; 
}
float XPutString::GetLengthPixel( LPCTSTR str )
{
	const TCHAR *p = str;
	TCHAR c;
	int idx;
	float w = 0;
	if( m_pSprDat == NULL )				// sprDat가 없는 상태면 스트링길이 읽어서 적당한 수 곱해서 걍 돌려줌
		return 0;
	while( c = *p++ )
	{
		if( c == _T(' ') )		// 스페이스일때는 좌표만 이동하고 다음으로 넘어감
		{
			w += m_pSprDat->GetSprite(0)->GetWidth() * m_fScaleX;
			continue;
		}
		idx = GetCharToIdx(c);
		if( idx < 0 )
			continue;
		XSprite *pSpr = m_pSprDat->GetSprite( idx );
		w += ((pSpr->GetWidth()+pSpr->GetAdjustX()) * m_fScaleX) + m_fDist;
	}
	return w;
}
float XPutString::GetFontHeight( void )
{
	XSprite *pSpr = m_pSprDat->GetSprite( 0 );
	return pSpr->GetHeight() * m_fScaleY;
}
// w, h크기에 들어맞게 축소해서 찍는다
float XPutString::PutStringFit(  float x, float y, float wFit, float hFit, LPCTSTR str, xAlign align )
{
	PushScale();
	SetScale( 1.0f, 1.0f );
	float origw = GetLengthPixel(str );		// 스케일링 1.0일때 스트링 길이를 구함x GScale만 반영됨
	float r = wFit / (origw * 1.1f);
	SetScale( r, r );
	float ret = DrawString( x, y, str, align );
	PopScale();
	return ret;
}
// return: draw한 스트링의 pixel단위 길이를 돌려준다
float XPutString::DrawString( float x, float y, LPCTSTR str, xAlign align )
{
	const TCHAR *p;
	TCHAR c;
	p = str;
	int idx;
	float w = GetLengthPixel(str );
	if( w == 0 )				// sprDat가 없는 상태면 스트링길이 읽어서 적당한 수 곱해서 걍 돌려줌
		return (float)_tcslen( str ) * (32.0f * m_fScaleX);
	if( align & xALIGN_HCENTER ) {
		x -= w / 2.0f;	
		x += 1;		// 왼쪽으로 너무 붙어서 +2넣어줌
	}
	else if( align & xALIGN_RIGHT )
		x -= w;
	
	if( align & xALIGN_VCENTER )
		y -= GetFontHeight() / 2.0f;

	
	p = str;
	while(c = *p++ )
	{
		if( c == _T(' ') )		// 스페이스일때는 좌표만 이동하고 다음으로 넘어감
		{
			x += m_pSprDat->GetSprite(0)->GetWidth() * m_fScaleX;
			continue;
		}
		idx = GetCharToIdx(c);
		if( idx < 0 )
			continue;
		XSprite *pSpr = m_pSprDat->GetSprite( idx );
		pSpr->SetfAlpha( m_Alpha );
		pSpr->SetScale( m_fScaleX, m_fScaleY );
		pSpr->Draw(x, y);
		x += (pSpr->GetWidth() * m_fScaleX) + m_fDist;				
	}
	m_Alpha = 1.0f;
	return w;
}
///////////////////////////////////////////////////////////////////////////
// char c의 스프라이트 인덱스를 돌려준다.
int XPutStringNumber::GetCharToIdx( TCHAR c )
{
	int idx = -1;
	if( c >= _T('0') && c <= _T('9') )
		idx = c - _T('0');
	else 
		idx = -1;	// *
	return idx; 
}
///////////////////////////////////////////////////////////////////////////
// char c의 스프라이트 인덱스를 돌려준다.
int XPutStringTime::GetCharToIdx( TCHAR c )
{
	int idx = -1;
	if( c >= _T('0') && c <= _T('9') )
		idx = c - _T('0');
	else if( c == ':' )
		idx = 10;
	else if( c == 'm' )
		idx = 11;
	else if( c == 's' )
		idx = 12;
	else
		idx = -1;	// 
	return idx; 
}

///////////////////////////////////////////////////////////////////////////
// char c의 스프라이트 인덱스를 돌려준다.
int XPutStringAll::GetCharToIdx( TCHAR c )
{
	int idx = 0;
	if( c >= _T('!') && c <= _T('~') )
	{
		idx = c - _T('!');
	}
	return idx; 
}
#endif // 0 