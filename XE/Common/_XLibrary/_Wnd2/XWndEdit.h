#pragma once
//#include "XWindow.h"
#include "_Wnd2/XWnd.h"
//#include "sprite/SprObj.h"
#include "XFontMng.h"
#include "XFont.h"

class XWndMng;
class XSprObj;
class XWndEdit;

////////////////////////////////////////////////////////////////
class XDelegateWndEdit : public XBaseDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateWndEdit() { Init(); }
	virtual ~XDelegateWndEdit() { Destroy(); }
	//
	virtual void OnDelegateEnterEditBox( XWndEdit *pWndEdit, LPCTSTR szString, const _tstring& strOld ) {}
};
/////////////////////////////////////////////////////////////////////////////
class XWndEdit : public XWnd
{
	XDelegateWndEdit *m_pDelegate;
	XSprObj *m_psoField;
	TCHAR m_szString[256];
	_tstring m_strTextOld;		// 입력값이 바뀌기전의 텍스트
	XBaseFontDat *m_pFontDat;
	XBaseFontObj *m_pFont;
//	XE::VEC2 m_vAdjust;
	BOOL m_bPassword;
	XCOLOR m_colBg;
	void Init() {
		m_pDelegate = NULL;
		m_pFontDat = NULL;
		m_psoField = NULL;
		m_szString[0] = 0;
		m_pFont = NULL;
		m_bPassword = FALSE;
		m_colBg = 0;
	}
public:
	XWndEdit( const XE::VEC2& vPos
					, const XE::VEC2& vSize
					, LPCTSTR szFont = FONT_SYSTEM
					, float sizeFont = 20.f
					, XCOLOR colText=XCOLOR_BLACK );
	virtual ~XWndEdit();
	//
	GET_ACCESSOR( LPCTSTR, szString );
	GET_SET_ACCESSOR( BOOL, bPassword );
	void SetszString( LPCTSTR szStr ) {
		_tcscpy_s( m_szString, szStr );
		SetbUpdate( true );
	}
	inline void SetszString( const _tstring& strText ) {
		SetszString( strText.c_str() );
	}
	GET_ACCESSOR_CONST( const _tstring&, strTextOld );
	GET_ACCESSOR( XBaseFontObj*, pFont );
//	SET_ACCESSOR( const XE::VEC2&, vAdjust );
	GET_SET_ACCESSOR( XDelegateWndEdit*, pDelegate );
	void SetAlign( XE::xAlign align ) {
		if( m_pFont )
			m_pFont->SetAlign( align );
	}
	void SetAlignWidth( float width ) {
		if( m_pFont )
			m_pFont->SetLineLength( width );
	}

	void DrawField( void );
	//
	virtual void Draw( void );
	virtual void OnLButtonUp( float lx, float ly );
	void Update() override;
};

#define xSET_EDITBOX_HANDLER( ROOT, KEY, CALL ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_ENTER, this, CALL ); \
		else \
	XLOGXNA( "xSET_EDITBOX_HANDLER: %s not found", KEY ); \
}

#define xSET_EDITBOX_DELEGATER( ROOT, KEY, DELEGATER ) { \
	XWnd *_pWnd; \
	XBREAK( ROOT == NULL ); \
	_pWnd = ROOT->Find( KEY ); \
	if( _pWnd ) { \
		XWndEdit *_pEdit = SafeCast<XWndEdit*, XWnd*>( _pWnd ); \
		if( _pEdit ) \
			_pEdit->SetpDelegate( DELEGATER ); \
		} else \
		XLOGXNA( "xSET_EDITBOX_HANDLER: %s not found", KEY ); \
}


#ifdef _VER_ANDROID

enum KeyboardReturnType {
    kKeyboardReturnTypeDefault = 0,
    kKeyboardReturnTypeDone,
    kKeyboardReturnTypeSend,
    kKeyboardReturnTypeSearch,
    kKeyboardReturnTypeGo
};


/**
 * \brief The EditBoxInputMode defines the type of text that the user is allowed
 * to enter.
 */
enum EditBoxInputMode
{
    /**
     * The user is allowed to enter any text, including line breaks.
     */
    kEditBoxInputModeAny = 0,
    
    /**
     * The user is allowed to enter an e-mail address.
     */
    kEditBoxInputModeEmailAddr,
	
    /**
     * The user is allowed to enter an integer value.
     */
    kEditBoxInputModeNumeric,
	
    /**
     * The user is allowed to enter a phone number.
     */
    kEditBoxInputModePhoneNumber,
	
    /**
     * The user is allowed to enter a URL.
     */
    kEditBoxInputModeUrl,
	
    /**
     * The user is allowed to enter a real number value.
     * This extends kEditBoxInputModeNumeric by allowing a decimal point.
     */
    kEditBoxInputModeDecimal,
	
    /**
     * The user is allowed to enter any text, except for line breaks.
     */
    kEditBoxInputModeSingleLine
};

/**
 * \brief The EditBoxInputFlag defines how the input text is displayed/formatted.
 */
enum EditBoxInputFlag
{
    /**
     * Indicates that the text entered is confidential data that should be
     * obscured whenever possible. This implies EDIT_BOX_INPUT_FLAG_SENSITIVE.
     */
    kEditBoxInputFlagPassword = 0,
	
    /**
     * Indicates that the text entered is sensitive data that the
     * implementation must never store into a dictionary or table for use
     * in predictive, auto-completing, or other accelerated input schemes.
     * A credit card number is an example of sensitive data.
     */
    kEditBoxInputFlagSensitive,
	
    /**
     * This flag is a hint to the implementation that during text editing,
     * the initial letter of each word should be capitalized.
     */
    kEditBoxInputFlagInitialCapsWord,
	
    /**
     * This flag is a hint to the implementation that during text editing,
     * the initial letter of each sentence should be capitalized.
     */
    kEditBoxInputFlagInitialCapsSentence,
	
    /**
     * Capitalize all characters automatically.
     */
    kEditBoxInputFlagInitialCapsAllCharacters
	
};

namespace xANDROID {
	void DoModalEditBox( ID idEdit );
};

#endif // android
