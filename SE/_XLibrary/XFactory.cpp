#include "stdafx.h"
#include "XFactory.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XFactory *FACTORY = nullptr;

#ifdef _GAME	// 땜빵
XSprObj* XFactory::CreateSprObj( LPCTSTR szSpr, XBaseObj *pParent ) 
{ 
	return new XSprObj( szSpr, pParent ); 
}
#endif 
XLua* XFactory::CreateScript( const char *cLua ) 
{
	if( cLua )
		return new XLua( cLua );
	return new XLua;
}
XBaseFontDat* XFactory::CreateFontDat( LPCTSTR szFont, float fontSize ) 
{
#ifdef WIN32
	return new XFontDatDX( szFont, fontSize );
#else
	return new XFontDatFTGL( szFont, fontSize );
#endif
}
XBaseFontObj* XFactory::CreateFontObj( XBaseFontDat *pDat ) 
{
	XBREAK( pDat == nullptr );
	return pDat->CreateFontObj();
/*#ifdef WIN32
	return new XFontObjDX( pDat );
#else
	return new XFontObjFTGL( pDat );
#endif */
}
/*
XTilePack* XFactory::CreateTilePack( LPCTSTR szTPK, BOOL bMakePage, BOOL bSrcKeep )
{
	XTilePack *pPack = new XTilePack;
	if( pPack->Load( szTPK, bMakePage, bSrcKeep ) )
		return pPack;
	SAFE_DELETE( pPack );
	return nullptr;
}

// 타일레이어를 만든다. w, h는 타일단위 월드크기.
XMapLayer* XFactory::CreateMapLayerTile( XMap *pMap, int w, int h, XTilePack *pTilePack )
{
	XBREAK( pMap == nullptr );
	XBREAK( pTilePack == nullptr );
	XMapLayer *spLayer = new XMapLayerTile( pMap, w, h, pTilePack );
	return spLayer;
}
*/