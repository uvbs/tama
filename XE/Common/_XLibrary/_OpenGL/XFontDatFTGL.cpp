#include "stdafx.h"
#include "XFontDatFTGL.h"
#include "XFontObjFTGL.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XBaseFontObj* XFontDatFTGL::CreateFontObj( void )
{
	return new XFontObjFTGL( this );
}

BOOL XFontDatFTGL::Create( LPCTSTR szFontFile, float fontSize, float outset ) 
{
	if( outset > 0 )	// 폰트를 얼마나 뚱뚱하게 할건가
		m_font = new FTPolygonFont( XE::MakePath( DIR_FONT, szFontFile ) );
	else
		m_font = new FTTextureFont( XE::MakePath( DIR_FONT, szFontFile ) );
	if( m_font->Error() )
	{
		FT_Error er = m_font->Error();
		XLOG_ALERT( "폰트파일 %s를 읽을 수 없습니다", szFontFile );
		SAFE_DELETE(m_font);
		return FALSE;
	}
	m_font->FaceSize( fontSize );
	if( outset > 0 )
		m_font->Outset( outset );		// 뚱뚱함의 굵기
	return TRUE;
}

// 텍스트 레이아웃 기능이 없는 단순 출력루틴
float XFontDatFTGL::DrawString( float x, float y, LPCTSTR szString, XCOLOR color, xFONT::xtStyle style )
{
	if( XBREAK(m_font == NULL) )
		return 0;
	if( XE::IsEmpty( szString ) )
		return 0;
	y += GetFontHeight();		// FTGL은 좌표계가 글자의 아랫부분이므로 편의상 글자위 좌상귀가 좌표가 되도록 바꿈
	// 프리타입 폰트는 독자적인 좌표계를 씀. 아래쪽이 0임
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if( GRAPHICS->GetScreenWidth() > GRAPHICS->GetScreenHeight() )
		glOrthof(0.0f, 1024, 0.0f, 768, -1.0f, 1.0f);
	else
		glOrthof(0.0f, 768, 0.0f, 1024, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	
	XE::VEC2 vc( x, y );	// virtual coodinate
	// 480x320좌표계를 1024x768좌표계로 바꿈
	// 그리고 y좌표를 뒤집음
	To1024Cood( &vc );
//	vc.x = x * (1024.0f / GRAPHICS->GetScreenWidth());	// ex: 1024/480
//	vc.y = 768.0f - (y * (768.0f / GRAPHICS->GetScreenHeight()));	// ex: 768/320
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// SprObj쪽에서 이걸 썼을것이므로 해제해줘야 함
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    if( style == xFONT::xSTYLE_SHADOW )
    {
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(vc.x+1.0f, vc.y+1.0f, 0.0f);
        if( color == XCOLOR_BLACK )
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        else
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

        m_font->Render(szString);
        glPopMatrix();
    }
	float r, g, b, a;
	r = XCOLOR_RGB_R(color) / 255.0f;
	g = XCOLOR_RGB_G(color) / 255.0f;
	b = XCOLOR_RGB_B(color) / 255.f;
	a = XCOLOR_RGB_A(color) / 255.f;
	glColor4f(r, g, b, a);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(vc.x, vc.y, 0.0f);
	m_font->Render(szString);
	glPopMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	// 좌표계를 원래대로 되돌림
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, GRAPHICS->GetScreenWidth(), GRAPHICS->GetScreenHeight(), 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return 0;
}


