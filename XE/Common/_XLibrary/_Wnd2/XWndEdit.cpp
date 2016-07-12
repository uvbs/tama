#include "stdafx.h"
#include "XWndEdit.h"
#include "XWndText.h"
#include "sprite/SprObj.h"
#include "XFramework/client/XClientMain.h"
#include "XFramework/client/XEContent.h"
#ifdef WIN32
#ifndef _LIB
#include "../_Tool/DlgEnterName.h"
#endif
#endif
#ifdef _VER_IOS
#include "objc/xe_ios.h"
#endif
#ifdef _VER_ANDROID
#include "XFramework/android/com_mtricks_xe_Cocos2dxHelper.h"
#include "XFramework/android/JniHelper.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

///////////////////////////////////////////////////////////////////////////
// XWndEdit::XWndEdit( float x, float y, float w, float h, LPCTSTR szSpr, ID idAct, XBaseFontDat *pFontDat )
// 	: XWnd( x, y, w, h ) 
// {
// 	Init();
// 	if( szSpr ) {
// 		m_psoField = new XSprObj( szSpr );
// 		m_psoField->SetAction( idAct );
// 	}
// 	m_pFont = pFontDat->CreateFontObj();
// 	m_pFont->SetColor( XCOLOR_WHITE );
// 	XCLEAR_ARRAY( m_szString );
// 	//		_tcscpy_s( m_szString, _T("") );
// }
XWndEdit::XWndEdit( const XE::VEC2& vPos
									, const XE::VEC2& vSize
									, LPCTSTR szFont /*= FONT_SYSTEM*/
									, float sizeFont /*= 20.f*/
									, XCOLOR colText/*=XCOLOR_BLACK*/ )
	: XWnd( vPos, vSize )	
{
	Init();
	auto pText = new XWndTextString(_T(""), szFont, sizeFont, colText );
	pText->SetstrIdentifier("__text.edit");
	Add( pText );
	m_pFontDat = FONTMNG->Load( szFont, sizeFont );
	if( m_pFontDat ) {
		m_pFont = m_pFontDat->CreateFontObj();
		m_pFont->SetColor( colText );
		m_pFont->SetLineLength( vSize.w );
		m_pFont->SetAlign( XE::xALIGN_HCENTER );
	}
}

XWndEdit::~XWndEdit() 
{
	SAFE_RELEASE2( FONTMNG, m_pFontDat );
	SAFE_DELETE( m_psoField );
	SAFE_DELETE( m_pFont );
}

void XWndEdit::Update()
{
	auto pText = xGET_TEXT_CTRL( this, "__text.edit" );
	if( pText ) {
		const auto sizeThis = GetSizeLocalNoTrans();
		pText->SetSizeLocal( sizeThis );
		pText->SetText( m_szString );
		pText->SetAlign( XE::xALIGN_CENTER );
		pText->SetbPassword( m_bPassword != FALSE );
	}
	XWnd::Update();
}

void XWndEdit::DrawField( void ) 
{
	if( m_psoField )
		m_psoField->Draw( GetPosFinal() );		// 바탕 입력필드를 그린다.
	else
	{
		XE::VEC2 v = GetPosFinal();
		XE::VEC2 size = GetSizeFinal();
		if( m_colBg )
			GRAPHICS->FillRectSize( v, size, m_colBg );
		GRAPHICS->DrawRectSize( v, size, XCOLOR_BLACK );
	}
}
void XWndEdit::Draw( void )
{
	XWnd::Draw();
//	DrawField();
// 	XE::VEC2 vPos = GetPosFinal();
// 	const auto sizeEdit = GetSizeLocal();
// 	const auto sizeText = m_pFont->GetLayoutSize( m_szString );
// 	if( m_pFont->GetAlign() & XE::xALIGN_VCENTER ) {
// 		vPos.h += (sizeEdit.h * 0.5f) - (sizeText.h * 0.5f);
// 	}
// 	vPos += GetvAdjDrawFinal();
// 	if( m_bPassword )	{
// 		TCHAR szString[ 256 ];
// 		_tcscpy_s( szString, m_szString );
// 		int len = _tcslen( szString );
// 		for( int i = 0; i < len; ++i )
// 			szString[ i ] = _T('*');
// 		m_pFont->DrawString( vPos, szString );
// 	} else
// 		m_pFont->DrawString( vPos, m_szString );
}

#ifdef _VER_ANDROID
static void editBoxCallbackFunc(const char* pText, void* ctx)
{
	AXLOGXN("editbox: %s, ctx=0x%08x", pText, (DWORD)ctx );
	XWndEdit *pEdit = (XWndEdit*) ctx;
	pEdit->SetszString( pText );
	pEdit->SetbUpdate( true );
	pEdit->CallEventHandler( XWM_ENTER, (DWORD)pText );
	if( pEdit->GetpDelegate() )
		pEdit->GetpDelegate()->OnDelegateEnterEditBox( pEdit, pEdit->GetszString(), pEdit->GetstrTextOld() );
}


#endif // android


// LButtonDown이벤트로 하지 말것. 멀티터치관련해서 버그가 있어서 잘못된 좌표가 넘어온다
void XWndEdit::OnLButtonUp( float lx, float ly )
{    
	m_strTextOld = m_szString;
#ifdef WIN32
	if( XE::GetMain()->DoEditBox( m_szString ) ) {
		SetbUpdate( true );
		CallEventHandler( XWM_ENTER, (DWORD)m_szString );
		if( m_pDelegate )
			m_pDelegate->OnDelegateEnterEditBox( this, m_szString, m_strTextOld );
	}
/*
#ifndef _LIB
	// 라이브러리에 포함시키기가 애매함. 방법이 없나? 일단 델리게이트 형태로 위임시키는 형태로 하자.
	CDlgEnterName dlg;		// 이거 CIV전용이니까 일반화 시킬것
	if( dlg.DoModal() == IDOK )
	{
		_tcscpy_s( m_szString, dlg.m_strName );
		CallEventHandler( XWM_ENTER, (DWORD)m_szString );
	}
#endif
*/
#endif
#ifdef _VER_IOS
    XE::VEC2 vPos = GetPosFinal();
    XE::VEC2 vSize = GetSizeFinal();
    IOS::DoEditBox( vPos.x+4.f, vPos.y+4.f, vSize.w-8.f, vSize.h-8.f, this );	// iOS의 EditBox를 띄운다
#endif
#ifdef _VER_ANDROID
    showEditTextDialogJNI(  "Name:",
						  m_szString,
						  kEditBoxInputModeSingleLine,
						  -1,
						  kKeyboardReturnTypeDefault,
						  100,
						  editBoxCallbackFunc,
						  (void*)this  );
#endif
}


#ifdef _VER_ANDROID
namespace xANDROID
{
	ID x_idEditField = 0;
	TCHAR x_szModalString[ 256 ] = { 0, };
	static void ModalEditBoxCallbackFunc(const char* pText, void* ctx)
	{
		AXLOGXN("modal editbox: %s, ctx=0x%08x", pText, (DWORD)ctx );
		XE::GetMain()->GetpGame()->OnEndEditBox( x_idEditField, pText );
		
	}
	void DoModalEditBox( ID idEdit )
	{
		x_idEditField = idEdit;
		showEditTextDialogJNI(  "Input:",
							  x_szModalString,
							  kEditBoxInputModeSingleLine,
							  -1,
							  kKeyboardReturnTypeDefault,
							  100,
							  ModalEditBoxCallbackFunc,
							  (void*)0  );
		
	}
};
#endif // ANDROID

