#pragma once
#include "XBaseFontDat.h"
#include "XList.h"

#define FONT_SYSTEM		_T("normal.ttf")
#define FONT_SIZE_DEFAULT	20.f
//#define FONT_SCALE_DEFAULT	0.9f

#define BASE_FONT XE::GetGame()->GetpfdSystem()
#define BASE_FONT_SMALL XE::GetGame()->GetpfdSystemSmall()

#define FONT_NANUM_FILE			_T("normal.ttf")
#define FONT_NANUM_BOLD_FILE	_T("bold.ttf")
/*
#define FONT_NANUM_FILE		_T("NanumGothicBold.ttf")
#define FONT_COOPER_FILE		_T("CooperBlackStd.otf")
#define FONT_NANUM_BOLD_FILE		_T("NanumGothicExtraBold.ttf")
#define FONT_THAI_FILE		_T("kunlasatri.ttf")
*/

#ifdef WIN32
//#define FONT_NANUM		_T("서울남산체 B")
//#define FONT_NANUM		_T("2002L")
//#define FONT_NANUM		_T("나눔고딕")
//#define FONT_NANUM		_T("Noto Sans CJK KR Regular")
//#define FONT_NANUM_BOLD		_T("서울남산체 EB")
//#define FONT_NANUM_BOLD		_T("-2002")
//#define FONT_NANUM_BOLD		_T("나눔고딕 ExtraBold")
#define FONT_NANUM		FONT_NANUM_FILE
#define FONT_NANUM_BOLD		FONT_NANUM_BOLD_FILE
#define FONT_COOPER		_T("Cooper Std Black")
#define FONT_THAI		_T("Kunlasatri 보통")
#define FONT_BADABOOM	_T("BadaBoom BB")
//#define FONT_MOK		_T("-우리목각M")
#else
#define FONT_NANUM		FONT_NANUM_FILE
#define FONT_NANUM_BOLD		FONT_NANUM_BOLD_FILE
#define FONT_COOPER		FONT_NANUM_FILE
#define FONT_THAI		FONT_NANUM_FILE
#define FONT_BADABOOM	_T( "damage.ttf" )
#define FONT_MOK		_T("mok.TTF")
#endif

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

