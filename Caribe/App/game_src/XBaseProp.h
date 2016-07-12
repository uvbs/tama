#pragma once

#include "XE.h"
#include "etc/Token.h"
class CToken;

class XBaseProp
{
	void Init() {}
	void Destroy() {}
public:
	XBaseProp() { Init(); }	// 여기에 Load를 넣지말것. Load안에서 버추얼함수 불러야 하는데 생성자라서 안불러짐
	virtual ~XBaseProp() { Destroy(); }

	virtual BOOL LoadBlock( CToken& token, void *pBlockObj, LPCTSTR szBlockName, ID idBlock=0 );

	virtual BOOL Load( LPCTSTR szFilename, XE::xtENCODING encode );
	virtual void* CreateNewBlock( LPCTSTR szBlockName, ID idBlock ) { return NULL; }
	virtual void AddNewBlock( void *pBlockObj ) {}
	virtual BOOL OnParse( CToken& token, void *pBlockObj, LPCTSTR szBlockName ) = 0;
};
