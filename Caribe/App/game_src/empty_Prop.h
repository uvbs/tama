/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XEBaseProp.h"
#include "empty_PropH.h"

class XPropSample : public XEBaseProp
{
public:
	static std::shared_ptr<XPropSample>& sGet() {
		if( s_spInstance == nullptr )
			s_spInstance = std::shared_ptr<XPropSample>( new XPropSample );
		return s_spInstance;
	}
public:
public:
	XPropSample();
	virtual ~XPropSample() { Destroy(); }

	GET_ACCESSOR( XArrayLinear<xPropSample::xPROP*>&, aryProp );

	BOOL ReadProp( CToken& token, DWORD dwParam ) override;					///< txt 파싱

	xPropSample::xPROP* GetpProp( LPCTSTR szIdentifier );
	/** strIdentifier는 모두 소문자여야 함. */
	xPropSample::xPROP* GetpProp( _tstring& strIdentifier ) {
		XBREAK( strIdentifier.empty() == TRUE );
		return GetpProp( strIdentifier.c_str() );
	}
	xPropSample::xPROP* GetpProp( ID idType );
	void Add( _tstring& strIdentifier, xPropSample::xPROP *pProp );
	int GetSize( void ) override { return m_mapData.size(); }
	LPCTSTR GetszSpr( ID idProp ) {
		auto pProp = GetpProp( idProp );
		if( pProp == nullptr )
			return nullptr;
		return pProp->strSpr.c_str();
	}

	void OnDidFinishReadProp() override;

	xPropSample::xPROP* GetpPropFromName( LPCTSTR szName );
	xPropSample::xPROP* GetPropRandom( void ) {
		return m_aryProp.GetFromRandom();
	}

private:
	static std::shared_ptr<XPropUser> s_spInstance;
	std::map<_tstring, xPropSample::xPROP*> m_mapData;			///< 프로퍼티 맵(식별자)
	std::map<ID, xPropSample::xPROP*> m_mapID;					///< 프로퍼티 맵(ID)
	XArrayLinear<xPropSample::xPROP*> m_aryProp;				///< 전체 프로퍼티 배열

	void Init() {}
	void Destroy();


};
