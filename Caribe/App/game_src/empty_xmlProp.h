#pragma once
/********************************************************************
	@date:	2016/01/24 13:42
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XPropBgObj.h
	@author:	xuzhu
	
	@brief:	배경오브젝트 프로퍼티
*********************************************************************/
#include "XXMLDoc.h"
#include "XPropBgObjH.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/24 13:46
*****************************************************************/
class XPropBgObj : public XXMLDoc
{
public:
	static std::shared_ptr<XPropBgObj>& sGet();
public:
	XPropBgObj();
	virtual ~XPropBgObj() { Destroy(); }
	//
	bool LoadProp( LPCTSTR szXml );
	bool OnDidFinishLoad() override;
#ifdef _xIN_TOOL
	bool SaveProp( LPCTSTR szXml );
#endif
	bool LoadObj( XEXmlNode& nodeObj );
	int GetPropToAry( XVector<xnBgObj::xProp*> *pOutAry );
	int GetPropToAry( XVector<ID> *pOutAry );
	void AddBgObj( xnBgObj::xProp *pProp );
	xnBgObj::xProp* GetpProp( ID idBgObj );
	void DestroyBgObj( ID idBgObj );
private:
#ifdef _xIN_TOOL
public:
	static ID sGenerateID( void ) {
		return ++s_idGlobal;
	}
	static void sSetidGlobal( ID idGlobal ) {
		s_idGlobal = idGlobal;
	}
private:
	static ID s_idGlobal;
#endif // _xIN_TOOL
	static std::shared_ptr<XPropBgObj> s_spInstance;
	XList4<xnBgObj::xProp*> m_BgObjs;	// 툴때문에 리스트로 함.
	std::map<ID, xnBgObj::xProp*> m_mapBgObjsID;
private:
	void Init() {}
	void Destroy();
}; // class XPropBgObj

