#include "stdafx.h"
#include "XWndEdit.h"
#include "XWndMng.h"
#include "SprObj.h"
#ifdef WIN32
#include "DlgEnterName.h"
#else
#include "civmm.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

///////////////////////////////////////////////////////////////////////////
void XWndEdit::Draw( void )
{
	DrawField();
	XE::VEC2 vPos = GetPosFinal();
	vPos.x += 16.f;
	vPos.y += 2.f;
	m_pFont->DrawString( vPos, m_szString );
}

// LButtonDown이벤트로 하지 말것. 멀티터치관련해서 버그가 있어서 잘못된 좌표가 넘어온다
ID XWndEdit::OnLButtonUp( float lx, float ly )
{    
#ifdef WIN32
	CDlgEnterName dlg;		// 이거 CIV전용이니까 일반화 시킬것
	if( dlg.DoModal() == IDOK )
	{
		_tcscpy_s( m_szString, dlg.m_strName );
	}
#else
    XE::VEC2 vPos = GetPosFinal();
    XE::VEC2 vSize = GetSizeFinal();
//	DoEditBox( vPos.x+4.f, vPos.y+4.f, vSize.w-2.f, vSize.h-2.f );	// iOS의 EditBox를 띄운다
    DoModalEditBox();
#endif
	return GetID();
}


