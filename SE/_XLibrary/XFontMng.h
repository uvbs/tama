#pragma once
#include "XBaseFontDat.h"
#include "XList.h"

class XFontMng 
{
	XList<XBaseFontDat*> m_listObjs;

	void Init() {}
	void Destroy();
//	void DestroyAll();		// 이런거 외부에서 쓸수 있게 하지 말것. MAIN->GetpFont()이런 포인터 다 작살난다.
	void Add( XBaseFontDat *pFile ) { m_listObjs.Add( pFile ); }
	XBaseFontDat* Find( LPCTSTR szFilename, float sizeFont );
public:
	XFontMng() { Init(); }
	virtual ~XFontMng() { Destroy(); }	

	void Release( XBaseFontDat *pFontDat );
	XBaseFontDat* Load( LPCTSTR szFilename, float sizeFont, BOOL bAddRefCnt=TRUE );
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float sizeFont );
};

extern XFontMng *FONTMNG;
