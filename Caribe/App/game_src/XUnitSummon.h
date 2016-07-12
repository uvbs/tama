/********************************************************************
	@date:	2015/02/06 18:38
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XUnitSummon.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#if 0
#include "XFramework/Game/XEBaseWorldObj.h"
#include "XFramework/Game/XEComponents.h"


/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/02/06 18:38
*****************************************************************/
class XUnitSummon : public XEBaseWorldObj
{
	void Init() {}
	void Destroy() {}
public:
	XUnitSummon() { Init(); }
	virtual ~XUnitSummon() { Destroy(); }
	//
}; // class XUnitSummon
////////////////////////////////////////////////////////////////
XUnitSummon::XUnitSummon()
{
	Init();
}

void XUnitSummon::Destroy()
{
}
#endif // 0