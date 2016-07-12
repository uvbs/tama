#include "stdafx.h"
#include "XFramework/XEToolTip.h"
#ifdef WIN32
#include "_DirectX/XGraphicsD3DTool.h"
#endif
#include "XFont.h"
//#include "XFactory.h"
//#include "XSceneGame.h"
//#include "XChar.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XEToolTip *TOOLTIP = NULL;

XEToolTip::XEToolTip( LPCTSTR _szToolTip, float w, XBaseFontDat *pDat )
{
	Init();
//	TCHAR szToolTip[ 1024 ];
//	XBREAK( _szToolTip == NULL );
//	_tcscpy_s( szToolTip, _szToolTip );
	m_pFontBasic = pDat->CreateFontObj();
	m_Size.w = w;
	m_pFontBasic->SetLineLength( m_Size.w );	// 폰트객체의 가로 라인길이를 지정
	m_pFontBasic->SetStyle( xFONT::xSTYLE_SHADOW );
	if( _szToolTip )
		SetText( _szToolTip );
}

// 실시간으로 사용하지 말것 열라 느림
/**
 * 툴팁이 깨지는 이유
 유독 툴팁에서만 글자가 깨지며 다운된다.
 문제는 동적할당한 텍스트를 FTGL에 넘겨준게 문제였다.
 FTGL은 내부적으로 캐시를 쓰고 있는데 바뀌지 않은 글자가 캐시에 남아있을때
 이전에 가지고 있던 포인터(이미 파괴되어버린)를 재사용 해서 깨진것이다.
 FTGL의 구조적 문제인걸로 보인다. 가급적 동적할당한 같은텍스트는 ftgl에 넘기지 말아야 한다.
 파라메터를 지역버퍼로 넘겨주는 것도 문제가 된다. 캐시가 지역버퍼의 포인터를 캐시로 저장하기 때문이다.
 */
void XEToolTip::SetText( LPCTSTR szToolTip, BOOL bUpdateSize )
{
	if( XE::IsHave( szToolTip ) )	// 텍스트가 있을때만 실행함
	{
		// FTGL버그때문에 이전과 텍스트가 같으면 다시 보내지 않는다.
		// 정적메모리일땐 문제가 없지만 같은 텍스트를 다시 보낼필요는 없다.
		if( XE::IsSame(m_strText.c_str(), szToolTip) == FALSE )
		{
			int len = _tcslen(szToolTip);
			m_strText = szToolTip;
//			m_len = len;
			//
			CONSOLE( "(%d)%s", len, m_strText.c_str() );
			if( bUpdateSize  )
			{
				XE::VEC2 v = m_pFontBasic->GetLayoutSize( m_strText.c_str() );
				m_Size.h = v.y * 1.2f;
			}
		}
	}
}
void XEToolTip::DrawToolTipText( float x, float y, LPCTSTR szText, XCOLOR col )
{
	m_pFontBasic->SetColor( col );
	m_pFontBasic->DrawString( x, y, szText );
}

void XEToolTip::Draw( void )
{
	float x = m_vPos.x;
	float y = m_vPos.y;	
	if( x + m_Size.w > XE::GetGameWidth() )
		x = XE::GetGameWidth() - m_Size.w;
	if( x < 0 )
		x = 0;
	if( y < 0 )
	{
		y = 0;
		x += 32.f;
	}
	else
	if( y + m_Size.h > XE::GetGameHeight() )
		y = XE::GetGameHeight() - m_Size.h;
	// 조정된 좌표로 다시 맞춤
	m_vPos.Set( x, y );
	// 외곽 박스 그림
	GRAPHICS->SetLineWidth(1.0f);
	GRAPHICS->FillRect( x, y, m_Size.w, m_Size.h, XCOLOR_RGBA( 0, 0, 0, 192 ) );
	GRAPHICS->DrawRect( x, y, m_Size.w, m_Size.h, XCOLOR_WHITE );
	XE::VEC2 vLT = m_vPos;
	vLT.x += 5.f;
	vLT.y += 3.f;
	DrawText( vLT );		// virtual

}

void XEToolTip::DrawText( const XE::VEC2& vLT )
{
	if( m_strText.empty() == false )
	{
		DrawToolTipText( vLT.x, vLT.y, m_strText.c_str(), XCOLOR_WHITE );
	}
}





