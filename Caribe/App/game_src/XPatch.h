/********************************************************************
	@date:	2015/02/27 8:58
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XPatch.h
	@author:	xuzhu
	
	@brief:	패치 관련 함수.
*********************************************************************/
#pragma once

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/02/27 8:59
*****************************************************************/
class XPatch
{
public:
	struct XRES_INFO : public XSYSTEM::XFILE_INFO
	{
		XUINT64 llChecksum = 0;
	};
private:
	void Init() {}
	void Destroy() {}
public:
	XPatch() { Init(); }
	virtual ~XPatch() { Destroy(); }
	//
	static bool sLoadFullList( LPCTSTR szFullpath, XList4<XRES_INFO> *pOutList );
}; // class XPatch
