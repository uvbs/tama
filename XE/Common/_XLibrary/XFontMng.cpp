
#include "stdafx.h"
#include "XFontMng.h"
//#include "XFactory.h"
#include "XRefRes.h"
//#include "XFont.h"
#include "etc/xLang.h"
#ifdef _VER_DX
#include "_DirectX/XFontObjDX.h"
#include "_DirectX/XFontDatDX.h"
#else
#include "OpenGL2/XFontObjFTGL.h"
#include "OpenGL2/XFontDatFTGL.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XFontMng *FONTMNG = NULL;

#ifdef _VER_DX
// 폰트파일명을 넘겨주면 폰트 이름을 돌려준다.
// win32 directx에서만 사용된다.
LPCTSTR GetFontFileToName( LPCTSTR szFontFile )
{
	if( XE::IsSame( szFontFile, FONT_NANUM_FILE ) )
		return _T("나눔고딕");
	else
	if( XE::IsSame( szFontFile, FONT_NANUM_BOLD_FILE ) )
		return _T("나눔고딕 ExtraBold");
// 	else if( XE::IsSame( szFontFile, _T("res_num.ttf")) )
// 		return _T("Alegreya Sans Black");
	else if( XE::IsSame( szFontFile, _T("res_num.ttf")) )
		return _T("Souses");
// 	else if( XE::IsSame( szFontFile, _T( "res_num.ttf" ) ) )
// 		return _T("나눔고딕");
	else if( XE::IsSame( szFontFile, _T( "damage.ttf" ) ) )
		return _T( "Mesmerize Eb" );
	else if( XE::IsSame( szFontFile, _T( "hand.ttf" ) ) ) {
		if( XE::LANG.GetstrFolder() == _T("en") )
			return _T( "AnglosaxObjlique" );
		return _T("나눔고딕");
	}
	// 변환되는 이름이 없다면 받은 그대로를 돌려준다.
	return _T("나눔고딕");
//	return szFontFile;
}
#endif // win32
/**
 @brief 폰트마다 기본크기가 다른문제때문에 폰트를 바꾸더라도 크기를 균일한 크기로 맞추기위해 폰트크기 필터를 거쳐서 크기를 다시 얻어낸다.
*/
float AdjFontSize( LPCTSTR szFontFile, float sizeOrig )
{
#ifdef _VER_OPENGL
 	if( XE::IsSame( szFontFile, _T( "res_num.ttf" ) ) )
 		return (float)((int)(sizeOrig * 0.95f));
#else
	if( XE::IsSame( szFontFile, _T( "res_num.ttf" ) ) )
		return sizeOrig;
#endif // 
	return sizeOrig;
}


//////////////////////////////////////////////////////////////////////////
void XFontMng::Destroy( void )
{
	// 폰트매니저가 최종 삭제될때까지 release되지 않은 객체가 있으면 경고알려줌
	for( auto pDat : m_listObjs ) {
		XALERT( "FontMng: 해제되지 않은 폰트 발견. %s", pDat->GetFilename().c_str() );
		SAFE_DELETE( pDat );
	}
	m_listObjs.clear();
//	DestroyAll();		이딴식으로 쓰지 말것
}

void XFontMng::Release( XBaseFontDat *pFontDat )
{
	if( pFontDat == NULL )
		return;
	for( auto itor = m_listObjs.begin(); itor != m_listObjs.end(); ) {
		XBaseFontDat *pDat = (*itor);
		if( pDat == pFontDat ) {
			pDat->DecRefCnt();			// 레퍼런스 카운트를 내린다
			if( pDat->GetnRefCnt() == 0 ) {		// 더이상 참조하는곳이 없다면
				SAFE_DELETE( pDat );		// 알맹이 지우고
				m_listObjs.erase( itor++ );
			}
			return;
		} else
			++itor;
	}
	
	XBREAK(1);		// 없으면 안됨.
}

XBaseFontDat* XFontMng::Find( LPCTSTR szFilename, float sizeFont ) 
{
	for( auto pObj : m_listObjs ) {
		// 폰트는 m_strRes에 파일명만 들어있음.(너무 일관성 없나?;;;)
		if( _tcsicmp( pObj->GetstrRes().c_str(), szFilename ) == 0	// 대소문자 무시하고 검색
			&& pObj->GetFontSize() == sizeFont )						// 폰트 사이즈도 같아야 함
			return pObj;
	}
	return nullptr;
}

XBaseFontDat* XFontMng::Load( LPCTSTR _szFilename, float _sizeFont, BOOL bAddRefCnt, BOOL bReload ) 
{
	float sizeFont = AdjFontSize( _szFilename, _sizeFont );
//	sizeFont *= FONT_SCALE_DEFAULT;
#ifdef _VER_DX
	if( XE::IsSame(_szFilename, FONT_NANUM )||
		XE::IsSame(_szFilename, FONT_NANUM_BOLD ) )
		sizeFont *= 1.1f;
#endif
#ifndef _VER_OPENGL
	LPCTSTR szFilename = GetFontFileToName( _szFilename );
#else
	LPCTSTR szFilename = _szFilename;
#endif
	if( XE::IsEmpty( szFilename ) )
		return NULL;
	// 리스트에 szFilename으로 생성된게 있는지 찾는다
	auto pObj = Find( szFilename, sizeFont );
	if( pObj )	{
		if( bAddRefCnt )
			pObj->IncRefCnt();		// 레퍼런스 카운트를 하나 올린다
		if( bReload )
			pObj->Reload();		// 폰트데이타를 강제로 다시 읽는다.
		return pObj;
	}
	// 리스트에 없는 파일이다
	pObj = CreateFontDat( szFilename, (float)sizeFont );		// virtual
	if( pObj ) {		// 파일을 로드 하고
		Add( pObj );							// 리스트에 추가한 후
		if( bAddRefCnt )
			pObj->IncRefCnt();				// 레퍼런스 카운트를 하나 올리고
		return pObj;							// 리턴 해준다
	}
	// failed
	SAFE_DELETE( pObj );
	return nullptr;
}

/**
 @brief pFontDat를 파괴하지 않은채 내부에서 폰트파일만 다시 읽는다.
*/
void XFontMng::ReloadAll()
{
	for( auto pFontdat : m_listObjs ) {
		pFontdat->Reload2();
	}
}

XBaseFontDat* XFontMng::CreateFontDat( LPCTSTR szFont, float sizeFont ) 
{ 
	XBaseFontDat *pFontDat = NULL;
	if( m_pDelegate )
		pFontDat = m_pDelegate->OnDelegateCreateFontDat( this, szFont, sizeFont );
	if( pFontDat == nullptr ) {
#ifdef _VER_DX
		return new XFontDatDX( szFont, sizeFont );
#else
		XFontDatFTGL *pFontDat = new XFontDatFTGL( szFont, sizeFont );
		if( pFontDat->Getfont() == NULL )
			SAFE_DELETE( pFontDat );
		return pFontDat;
#endif
	}
	return pFontDat;
}

XBaseFontObj* XFontMng::CreateFontObj( LPCTSTR szFont, float sizeFont )
{
#ifdef _VER_DX
	return new XFontObjDX( szFont, sizeFont );
#else
	return new XFontObjFTGL( szFont, sizeFont );
#endif
}
void XFontMng::RestoreDevice( void )
{
#ifndef _VER_DX
	xFTGL::Restore();
#endif // not _VER_DX
	for( auto pObj : m_listObjs ) {
		// 내부의 FTGL객체를 파괴하고 다시 할당한다.
		pObj->RestoreDevice();
	}
	
}

/**
 @brief 아무도 참조하고 있지는 않지만 캐쉬타임에 걸려 아직 파괴되지 않은 리스스들을 모두 날린다.
*/
void XFontMng::DoFlushCache()
{
	for( auto itor = m_listObjs.begin(); itor != m_listObjs.end(); ) {
		XBaseFontDat *pDat = (*itor);
		if( pDat->GetnRefCnt() == 0 ) {		// 더이상 참조하는곳이 없다면
			SAFE_DELETE( pDat );		// 알맹이 지우고
			m_listObjs.erase( itor++ );
		} else
			++itor;
	}
}