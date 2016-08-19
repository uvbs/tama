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
// ����tc�� ���α��̸� �����ش�
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
		if( c == _T(' ') )		// �����̽��϶��� ��ǥ�� �̵��ϰ� �������� �Ѿ
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
	m_vScale.Set( 1.0f );		// XFontSpr�� ��ü�ν��Ͻ��� �ƴϱⶫ�� �̷� �������� ��� 1ȸ������ �����ϰ� �ٷ� ����
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
		if( c == _T(' ') )		// �����̽��϶��� ��ǥ�� �̵��ϰ� �������� �Ѿ
			w += m_pDatSpr->GetFontWidth() * m_vScale.x;
		else
			w += m_pDatSpr->GetCharWidth( c ) * m_vScale.x + m_fDist;
	}
	return w;
}

// return: draw�� ��Ʈ���� pixel���� ���̸� �����ش�
float XFontObjSpr::_RenderText( float x, float y, LPCTSTR str )
{
	float w = GetLengthPixel( str );
	if( w == 0 )				// sprDat�� ���� ���¸� ��Ʈ������ �о ������ �� ���ؼ� �� ������
		return (float)_tcslen( str ) * (32.0f * m_vScale.x);
	if( m_Align & xALIGN_HCENTER ) {
		x -= w * 0.5f;
		x += 1.0f;		// �������� �ʹ� �پ +2�־���
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

