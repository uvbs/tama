#pragma once
#include "SprDat.h"
class XSprDat;
class XSprMng
{
public:
	static int s_sizeTotalVM;
private:
	struct xDat {
		XSprDat *pSprDat;
		XE::VEC3 vHSL;
	};
//	XSprDat_List m_listSprDat;
	XList4<XSprDat*> m_listSprDat;
	XList4<XSprDat*> m_listPreLoad;
	void Init( void ) {
	}
	void Destroy( void );
public:
	XSprMng() { Init(); XTRACE("create sprmng"); }
	virtual ~XSprMng() { XTRACE("destroy sprmng"); Destroy(); }

	void CheckRelease( void );
	XSprDat *Load( LPCTSTR szFilename, BOOL bAddRefCnt=TRUE, BOOL bSrcKeep=FALSE );	
	void RestoreDevice();
	void Add( XSprDat *pSprDat ) { m_listSprDat.push_back( pSprDat ); }
	XSprDat *Find( LPCTSTR szFilename, BOOL bSrcKeep );
	XSprDat *New( void );
	void Release( XSprDat *pSprDat );
	void PreLoad( LPCTSTR szSpr, BOOL bSrcKeep=FALSE );
	void ReleasePreload( void );
};

extern XSprMng *SPRMNG;
//inline XSprMng* GetSprMng( void ) { return SPRMNG; }

