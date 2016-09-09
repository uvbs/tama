﻿/********************************************************************
	@date:	2016/07/22 11:02
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\XFramework\XFType.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

class XEBaseWorldObj;

typedef std::shared_ptr<XEBaseWorldObj> XSPWorldObj;
typedef std::weak_ptr<XEBaseWorldObj> XSPWorldObjW;
typedef std::shared_ptr<const XEBaseWorldObj> XSPWorldObjConst;
namespace xSpr {
struct xDat;
typedef std::shared_ptr<xDat> XSPDat;
typedef std::shared_ptr<const xDat> XSPDatConst;
}

//#define XSPWorldObj WorldObjPtr
