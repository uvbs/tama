#pragma once

#include "xe.h"
#include "global.h"
#include "XList.h"
#include "Alert.h"
#include "XTilePack.h"


class XTilePackMng;
extern XTilePackMng *TILEPACK_MNG;
class XTilePackMng 
{
	XList<XTilePack*> m_listTilePack;
	void Init() {}
	void Destroy();
public:
	XTilePackMng() { Init(); }
	virtual ~XTilePackMng() { Destroy(); }

	XTilePack* Load( LPCTSTR szTPK, BOOL bSrcKeep=FALSE );
	XTilePack* Find( LPCTSTR szTPK );
	void Add( XTilePack* pTilePack );
	XTilePack* GetFromIndex( int idx ) {
		if( m_listTilePack.size() > 0 )
			return m_listTilePack.GetFromIndex(0);
		return NULL;
	}
	virtual XTilePack* CreateTilePack( LPCTSTR szTPK, BOOL bMakePage, BOOL bSrcKeep );
};
