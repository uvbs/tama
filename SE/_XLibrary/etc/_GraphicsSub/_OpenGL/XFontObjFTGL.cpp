#include "XFontObjFTGL.h"
#include "XFontMng.h"
#ifdef _VER_IPHONE
#undef min
#undef max
#endif
#include "xUtil.h"

XFontObjFTGL::XFontObjFTGL( XBaseFontDat *pDat ) 
: XBaseFontObj( pDat ) 
{
	Init(); 
	XFontDatFTGL *pFontDat = m_pFTGLDat = SafeCast<XFontDatFTGL*, XBaseFontDat*>( pDat );
	SetLineLength( 480 );
	m_Layout.SetFont( pFontDat->Getfont() );
}

XFontObjFTGL::XFontObjFTGL( const char *cFont, float fontSize, float outset )
: XBaseFontObj( FONTMNG->Load( Convert_char_To_TCHAR( cFont ), fontSize ) ) 
{
	Init(); 
	XBREAK( GetpDat() == NULL );
	XFontDatFTGL *pFontDat = m_pFTGLDat = SafeCast<XFontDatFTGL*, XBaseFontDat*>( GetpDat() );
	SetLineLength( 480 );
	m_Layout.SetFont( pFontDat->Getfont() );
}

// xALIGN_RIGHT, CENTER등을 쓰려면 미리 STATIC화 시켜놔야 한다.
// SetStaticText()로 찍을 텍스트를 미리 알려주고 SetAlign()을 불러야 한다.
void XFontObjFTGL::SetAlign( xAlign align ) 
{ 
	XBaseFontObj::SetAlign( align );
	// FTGL전용 코드
/*    switch ( align ) 
    {
        case xALIGN_LEFT:
            m_vAdjust.Set(0, 0);
            break;
        case xALIGN_RIGHT:
            m_vAdjust.Set( -m_sizeLayout.w, 0.f );
            break;
    }
    if( align & xALIGN_HCENTER )
        m_vAdjust.x = -(m_sizeLayout.w/2.f);
    if( align & xALIGN_VCENTER )
        m_vAdjust.y = -(m_sizeLayout.h/2.f);
 */
	switch( align )
	{
		case xALIGN_LEFT:	m_Layout.SetAlignment( FTGL::ALIGN_LEFT );	break;
		case xALIGN_RIGHT:	m_Layout.SetAlignment( FTGL::ALIGN_RIGHT );	break;
		case xALIGN_CENTER:	m_Layout.SetAlignment( FTGL::ALIGN_CENTER );	break;
	}
 
}
void XFontObjFTGL::SetLineLength( float screenw ) 
{
	XBaseFontObj::SetLineLength( screenw );
	// FTGL전용 코드
	screenw *= (1024.0f / GRAPHICS->GetScreenWidth());	// 480좌표계를 1024좌표계로 변환
	m_Layout.SetLineLength( screenw );
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
	FTBBox box = m_Layout.BBox( szString );             // 게임실행중 이걸 호출하니까 화면이 껌뻑거린다? 내부에 버그있나?
	vSize.x = box.Upper().X() - box.Lower().X();		// 더 큰사이즈로...
	vSize.y = box.Upper().Y() - box.Lower().Y();		
	vSize.y += GetFontHeight() * 1.1f;	
	// 480x320좌표계로 변환
	XFontDatFTGL::ToScreenCood( &vSize );
	return vSize;
//    return XE::VEC2(135,95);
}

float XFontObjFTGL::_RenderText( float x, float y, LPCTSTR szString )
{
	if( XE::IsEmpty( szString ) )
		return 0;
	y += GetFontHeight();		// FTGL은 좌표계가 글자의 아랫부분이므로 편의상 글자위 좌상귀가 좌표가 되도록 바꿈
//    x += m_vAdjust.x;
//    y += m_vAdjust.y;
	// 프리타입 폰트는 독자적인 좌표계를 씀. 아래쪽이 0임
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0.0f, 1024, 0.0f, 768, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	XE::VEC2 vc( x, y );	// virtual coodinate
	// 480x320좌표계를 1024x768좌표계로 바꿈
	// 그리고 y좌표를 뒤집음
	XFontDatFTGL::To1024Cood( &vc );
    if( m_Style == xFONT::xSTYLE_SHADOW )
    {
        glPushMatrix();
        glTranslatef(vc.x+1.0f, vc.y+1.0f, 0.0f);
        if( m_R == 0 && m_G == 0 && m_B == 0 )
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        else
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        
        m_Layout.Render(szString, -1, FTPoint(), FTGL::RENDER_FRONT );
        glPopMatrix();
    }
	glPushMatrix();
	glColor4f( m_R, m_G, m_B, m_A );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// SprObj쪽에서 이걸 썼을것이므로 해제해줘야 함
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glTranslatef(vc.x, vc.y, 0.0f);

	m_Layout.Render(szString, -1, FTPoint(), FTGL::RENDER_FRONT );

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
