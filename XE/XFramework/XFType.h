/********************************************************************
	@date:	2016/07/22 11:02
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\XFramework\XFType.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

class XEBaseWorldObj;

typedef std::shared_ptr<XEBaseWorldObj> WorldObjPtr;
typedef std::shared_ptr<const XEBaseWorldObj> XSPWorldObjConst;

#define XSPWorldObj WorldObjPtr
