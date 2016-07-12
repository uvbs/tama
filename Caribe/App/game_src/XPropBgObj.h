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
	static void sDestroyInstance();
public:
	XPropBgObj();
	virtual ~XPropBgObj() { Destroy(); }
	//
	bool LoadProp( LPCTSTR szXml );
	bool OnDidFinishLoad() override;
#ifdef _xIN_TOOL
	bool SaveProp( LPCTSTR szXml );
#endif
	GET_ACCESSOR_CONST( const XList4<xnBgObj::xProp*>&, listBgObjs );
	bool LoadObj( XEXmlNode& nodeObj, XGAME::xtBgObj type, LPCTSTR szTag );
	xnBgObj::xProp* LoadEach( XEXmlNode& nodeEach, XGAME::xtBgObj type, LPCTSTR szTag );
	int GetPropToAry( XVector<xnBgObj::xProp*> *pOutAry, XGAME::xtBgObj type = XGAME::xBOT_NONE );
	int GetPropToAry( XVector<ID> *pOutAry, XGAME::xtBgObj type = XGAME::xBOT_NONE );
	void AddBgObj( xnBgObj::xProp *pProp );
	xnBgObj::xProp* GetpProp( ID idBgObj );
	void DestroyBgObj( ID idBgObj );
	xnBgObj::xProp* CreateNewProp();
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
	XList4<xnBgObj::xProp*> m_listBgObjs;	// 툴때문에 리스트로 함.
	std::map<ID, xnBgObj::xProp*> m_mapBgObjsID;
	bool m_bError = false;
private:
	void Init() {}
	void Destroy();
	xnBgObj::xProp* GetSameProp( const xnBgObj::xProp* pPropSrc );
}; // class XPropBgObj

