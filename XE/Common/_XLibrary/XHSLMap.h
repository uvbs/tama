#pragma once
#include "etc/XGraphicsDef.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/06/07 15:23
*****************************************************************/
class XHSLMap
{
public:
	static std::shared_ptr<XHSLMap>& sGet();
	static void sDestroyInstance();
public:
	XHSLMap() { Init(); }
	~XHSLMap() { Destroy(); }
	//
	bool Load( LPCTSTR szFile );
	const XE::xHSLFile* GetInfo( const _tstring& strKey ) const;
	inline const XE::xHSLFile* GetInfo( LPCTSTR szKey ) const {
		return GetInfo( _tstring(szKey) );
	}
private:
	static std::shared_ptr<XHSLMap> s_spInstance;
	std::map<_tstring, XE::xHSLFile> m_mapTable;
	void Init() {}
	void Destroy() {}
}; // class XHSLMap

