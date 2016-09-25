/********************************************************************
	@date:	2016/07/22 11:02
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\XFramework\XFType.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

class XEBaseWorldObj;
class XEWorld;

typedef std::shared_ptr<XEBaseWorldObj> XSPWorldObj;
typedef std::weak_ptr<XEBaseWorldObj> XSPWorldObjW;
typedef std::shared_ptr<const XEBaseWorldObj> XSPWorldObjConst;
namespace xSpr {
struct xDat;
typedef std::shared_ptr<xDat> XSPDat;
typedef std::shared_ptr<const xDat> XSPDatConst;
}
typedef std::shared_ptr<XEWorld> XSPWorld;
typedef std::shared_ptr<const XEWorld> XSPWorldConst;
//#define XSPWorldObj WorldObjPtr

XE_NAMESPACE_START( xnTexAtlas )
struct xAtlas;
XE_NAMESPACE_END; // xnTexAtlas
class XTextureAtlas;

typedef std::shared_ptr<xnTexAtlas::xAtlas> XSPAtlas;
typedef std::shared_ptr<const xnTexAtlas::xAtlas> XSPAtlasConst;
typedef std::shared_ptr<XTextureAtlas> XSPAtlasMng;
typedef std::shared_ptr<const XTextureAtlas> XSPAtlasMngConst;
