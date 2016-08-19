#ifndef __SPRMNG_H__
#define __SPRMNG_H__
#include "SprDat.h"
class XSprDat;
class XSprMng
{
	XSprDat_List m_listSprDat;
	void Init() {
	}
	void Destroy();
public:
	XSprMng() { Init(); XLOG("create sprmng"); }
	virtual ~XSprMng() { XLOG("destroy sprmng"); Destroy(); }

	void CheckRelease();
	XSprDat *Load( LPCTSTR szFilename, BOOL bAddRefCnt=TRUE, BOOL bSrcKeep=FALSE );	
	void Add( XSprDat *pSprDat ) { m_listSprDat.push_back( pSprDat ); }
	XSprDat *Find( LPCTSTR szFilename, BOOL bSrcKeep );
	XSprDat *New();
	void Release( XSprDat *pSprDat );
};

extern XSprMng *SPRMNG;
//inline XSprMng* GetSprMng() { return SPRMNG; }

#endif
