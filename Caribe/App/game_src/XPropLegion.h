/********************************************************************
	@date:	2015/12/23 15:18
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XPropLegion.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

#include "XXMLDoc.h"
#include "XPropLegionH.h"
#include "XStruct.h"

const int VER_PROP_LEGION = 1;

////////////////////////////////////////////////////////////////
class XPropLegion : public XXMLDoc
{
	static ID s_idGlobal;
public:
	static std::shared_ptr<XPropLegion>& sGet();
	//
public:
	XPropLegion();
	virtual ~XPropLegion() { Destroy(); }
	//
	GET_ACCESSOR_CONST( const XVector<XGAME::xLegion*>&, aryLegions );
	void DestroyAll();
	bool Load( LPCTSTR szXml );
	XGAME::xLegion* CreateProp( const char* cIdentifier );
	inline XGAME::xLegion* CreateProp( const std::string& strIdentifier ) {
		return CreateProp( strIdentifier.c_str() );
	}
	/**
	 @brief 리스트의 프로퍼티들을 어레이에 담아준다.
	*/
	template<typename T>
	int GetpPropToAry( XVector<T> *pAryOut ) {
		XBREAK( pAryOut->size() > 0 );	// 받을 어레이는 비어있어야 함.
		*pAryOut = m_aryLegions;
		return pAryOut->size();
	}
	XGAME::xLegion* GetpProp( const ID idProp );
	XGAME::xLegion* GetpProp( const char *cIdentifier );
	inline XGAME::xLegion* GetpProp( const std::string& strIdentifier ) {
		return GetpProp( strIdentifier.c_str() );
	}
	XGAME::xLegion* LoadLegionEach( const std::string& idsNode );
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
#ifdef _xIN_TOOL
	bool Save( LPCTSTR szXml );
#endif // _xIN_TOOL
private:
	static std::shared_ptr<XPropLegion> s_spInstance;
private:
	bool m_bError = false;
	XVector<XGAME::xLegion*> m_aryLegions;
	// 노드가 128개 이하의 경우는 map이 빠르고 그이상은 unordered_map이 빠르다고 함.
	// 키를 문자열로 했을경우 1024개 까지는 map이 빠르다함.
	std::map<ID, XGAME::xLegion*> m_mapLegionsByID;
	std::map<std::string, XGAME::xLegion*> m_mapLegionsByIds;
	void Init() {}
	void Destroy();
	void AddProp( XGAME::xLegion* pLegion );
	bool OnDidFinishLoad() override;
	XGAME::xLegion* LoadLegionNode( XEXmlNode& node );
}; // XPropLegion


