#pragma once
#include "XBaseFontDat.h"
#include "XList.h"

#define FONT_SYSTEM		_T("normal.ttf")
#define FONT_SIZE_DEFAULT	20.f

#define BASE_FONT XE::GetGame()->GetpfdSystem()
#define BASE_FONT_SMALL XE::GetGame()->GetpfdSystemSmall()

// #define FONT_NANUM_FILE			_T("normal.ttf")
// #define FONT_NANUM_BOLD_FILE	_T("bold.ttf")
// #define FONT_MNLS_FILE			_T("normal.ttf")
// #define FONT_NANUM_FILE				XFontMng::s_aryFonts[0].c_str()
// #define FONT_NANUM_BOLD_FILE	XFontMng::s_aryFonts[1].c_str()
// #define FONT_MNLS_FILE				XFontMng::s_aryFonts[2].c_str()

#define FONT_NANUM				XFontMng::s_aryFonts[0].c_str()
#define FONT_NANUM_BOLD		XFontMng::s_aryFonts[1].c_str()
#define FONT_MNLS					XFontMng::s_aryFonts[2].c_str()

#ifdef WIN32
// 폰트파일명을 넘겨주면 폰트 이름을 돌려준다.
// win32 directx에서만 사용된다.
LPCTSTR GetFontFileToName( LPCTSTR szFontFile );
#endif // win32


////////////////////////////////////////////////////////////////
class XFontMng;
class XDelegateFontMng
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateFontMng() { Init(); }
	virtual ~XDelegateFontMng() { Destroy(); }
	//
	virtual XBaseFontDat* OnDelegateCreateFontDat( XFontMng *pFontMng, 
											LPCTSTR szFont,
											float sizeFont ) { return NULL; }
};
////////////////////////////////////////////////////////////////

class XFontMng 
{
	XDelegateFontMng *m_pDelegate;
	XList4<XBaseFontDat*> m_listObjs;

	void Init( void ) {
		m_pDelegate = NULL;
	}
	void Destroy( void );
//	void DestroyAll( void );		// 이런거 외부에서 쓸수 있게 하지 말것. MAIN->GetpFont()이런 포인터 다 작살난다.
	void Add( XBaseFontDat *pFile ) { m_listObjs.Add( pFile ); }
	XBaseFontDat* Find( LPCTSTR szFilename, float sizeFont );
public:
	/**
	0: 본문용
	1: 제목용
	2: 유니코드용
	*/
	static std::vector<_tstring> s_aryFonts;		/// 폰트파일들
	XFontMng() {
		Init(); 
	}
	virtual ~XFontMng() { Destroy(); }	
	//
	GET_SET_ACCESSOR( XDelegateFontMng*, pDelegate );
	void Release( XBaseFontDat *pFontDat );
	XBaseFontDat* Load( LPCTSTR szFilename, float sizeFont, BOOL bAddRefCnt=TRUE, BOOL bReload=FALSE );
	void RestoreDevice( void );
	virtual XBaseFontObj* CreateFontObj( LPCTSTR szFont, float sizeFont );
	void DoFlushCache();
	void ReloadAll();
private:
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float sizeFont );
};

extern XFontMng *FONTMNG;

