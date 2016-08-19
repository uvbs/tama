#ifndef __SPRMNG_H__
#define __SPRMNG_H__
#include "SprDat.h"
class XSprDat;
class XSprMng
{
	struct xDat {
		XSprDat* m_pSprDat = nullptr;
		std::vector<xLayerInfoByAction> m_aryLayerInfoAction;
	};
//	XSprDat_List m_listSprDat;
	XList4<xDat> m_listSprDat;
	void Init() {
	}
	void Destroy();
public:
	XSprMng() { Init(); XLOG("create sprmng"); }
	virtual ~XSprMng() { XLOG("destroy sprmng"); Destroy(); }

	void CheckRelease();
	XSprDat *Load( LPCTSTR szFilename, std::vector<xLayerInfoByAction> *pOutAryLayerInfo, BOOL bAddRefCnt=TRUE, BOOL bSrcKeep=FALSE );	
//	void Add( XSprDat *pSprDat ) { m_listSprDat.push_back( pSprDat ); }
	void Add( const xDat& spr ) {
		m_listSprDat.push_back( spr );
	}
	XSprMng::xDat Find( LPCTSTR szFilename, BOOL bSrcKeep );
	XSprDat *NewSprDat();
	void Release( XSprDat *pSprDat );
};

extern XSprMng *SPRMNG;
//inline XSprMng* GetSprMng() { return SPRMNG; }

#endif
