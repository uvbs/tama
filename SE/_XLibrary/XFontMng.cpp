#include "stdafx.h"
#include "XFontMng.h"
#include "XFactory.h"
#include "XRefRes.h"
#include "XFont.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XFontMng *FONTMNG = nullptr;

void XFontMng::Destroy()
{
	// 폰트매니저가 최종 삭제될때까지 release되지 않은 객체가 있으면 경고알려줌
	XLIST_LOOP( m_listObjs, XBaseFontDat*, pObj )
	{
		XALERT( "FontMng: 해제되지 않은 폰트 발견. %s", pObj->GetszName() );
		SAFE_DELETE( pObj );
	} END_LOOP;
	m_listObjs.Clear();
//	DestroyAll();		이딴식으로 쓰지 말것
}

/*void XFontMng::DestroyAll()
{
	XLIST_LOOP( m_listObjs, XBaseFontDat*, pObj )
	{
		SAFE_DELETE( pObj );
	} END_LOOP;
	m_listObjs.Clear();

} */
void XFontMng::Release( XBaseFontDat *pFontDat )
{
	if( pFontDat == nullptr )
		return;
	XLIST_LOOP( m_listObjs, XBaseFontDat*, pObj )
	{
		if( pObj == pFontDat )
		{
			pObj->DecRefCnt();			// 레퍼런스 카운트를 내린다
			if( pObj->GetnRefCnt() == 0 )		// 더이상 참조하는곳이 없다면
			{
				m_listObjs.Del( pFontDat );		// 노드 부터 지우고
				SAFE_DELETE( pObj );		// 알맹이 지우고
			}
			break;
		}
	} END_LOOP;
}

XBaseFontDat* XFontMng::Find( LPCTSTR szFilename, float sizeFont ) 
{
	XLIST_LOOP( m_listObjs, XBaseFontDat*, pObj ) 
	{
		if( _tcsicmp( pObj->GetszName(), szFilename ) == 0 &&		// 대소문자 무시하고 검색
			pObj->GetFontSize() == sizeFont )						// 폰트 사이즈도 같아야 함
			return pObj;
	} END_LOOP;
	return nullptr;
}

XBaseFontDat* XFontMng::Load( LPCTSTR szFilename, float sizeFont, BOOL bAddRefCnt ) 
{
	if( szFilename == nullptr )
		return nullptr;
	// 리스트에 szFilename으로 생성된게 있는지 찾는다
	XBaseFontDat *pObj = Find( szFilename, sizeFont );
	//
	if( pObj )	{
		if( bAddRefCnt )
			pObj->IncRefCnt();		// 레퍼런스 카운트를 하나 올린다
		return pObj;
	}
	// 리스트에 없는 파일이다
//	pObj = FACTORY->CreateFontDat( szFilename, (float)sizeFont );
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


XBaseFontDat* XFontMng::CreateFontDat( LPCTSTR szFont, float sizeFont ) 
{ 
#ifdef WIN32
	return new XFontDatDX( szFont, sizeFont );
#else
	return new XFontDatFTGL( szFont, sizeFont );
#endif
}

