#pragma once

#ifdef _GAME
#include "SprObj.h"
#endif
#include "XLua.h"
#include "XFont.h"
//#include "XTilePack.h"		// 프레임워크 개념이므로 SE에서도 이걸 링크하도록 하자.
//#include "XMapLayerTile.h"
//#include "XMap.h"

class XFactory;
class XBaseObj;
extern XFactory *FACTORY;
class XFactory
{
public:
	XFactory() {
		XBREAK( FACTORY != nullptr );
		FACTORY = this;
	}
	virtual ~XFactory() {
		FACTORY = nullptr;
	}

#ifdef _GAME
	virtual XSprObj* CreateSprObj( LPCTSTR szSpr, XBaseObj *pParent=nullptr );
#endif
	virtual XLua* CreateScript( const char *cLua=nullptr );
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float fontSize );
	virtual XBaseFontObj* CreateFontObj( XBaseFontDat *pDat );
	// 이런식으로 팩토리에다 다 꾸겨넣으니까 게임이고 툴이고 죄다 타일팩모듈을 링크시켜야해서 안좋은거 같아 뺌.
//	virtual XTilePack* CreateTilePack( LPCTSTR szTPK, BOOL bMakePage, BOOL bSrcKeep );	
//	virtual XMapLayer* CreateMapLayerTile( XMap *pMap, int w, int h, XTilePack *pTilePack );

//	virtual XObjSkillRecv* CreateObjSkillRecv()=0;
//	virtual XObjSkillUse*  CreateObjSkillUse()=0;

};

