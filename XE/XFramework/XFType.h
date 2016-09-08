/********************************************************************
	@date:	2016/07/22 11:02
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\XFramework\XFType.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

class XEBaseWorldObj;
class XSprDat;

typedef std::shared_ptr<XEBaseWorldObj> XSPWorldObj;
typedef std::weak_ptr<XEBaseWorldObj> XSPWorldObjW;
typedef std::shared_ptr<const XEBaseWorldObj> XSPWorldObjConst;
// typedef std::shared_ptr<XSprDat> XSPSprDat;
// typedef std::shared_ptr<const XSprDat> XSPSprDatConst;

//#define XSPWorldObj WorldObjPtr
