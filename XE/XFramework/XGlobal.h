#pragma once
#include "XXMLDoc.h"
#include "XList.h"
#include <vector>
#include "XFramework/XParamObj.h"

////////////////////////////////////////////////////////////////
// xml을 읽어서 상수들을 읽은 후 전역변수에 모두 세팅하고 객체는 다시 삭제한다.
class XGlobalVal : public XXMLDoc
{
	TiXmlNode *m_pRoot;
	XEXmlNode m_nodeRoot;
	XParamObj2 m_Params;
	void Init() {
		m_pRoot = NULL;
	}
	void Destroy();
public:
	XGlobalVal();
	virtual ~XGlobalVal() { Destroy(); }
	//
	void LoadParams();
	void LoadNode( XEXmlNode& node, const std::string& strKey );
	bool OnDidFinishLoad() override;
	template<typename RET>
	RET GetVal( LPCTSTR szVal, LPCTSTR szNodeName=NULL ) {
		char utf8[ 2048 ];
		XBREAK( szVal == NULL );
		TiXmlElement *elem = m_pRoot->ToElement();
		// 지정된 노드이름이 있으면 그걸찾아서 루트로 쓴다. 없으면 디폴트 루트가 기본이 된다.
		if( XE::IsHave( szNodeName ) ) {
			Convert_TCHAR_To_utf8( szNodeName, utf8 );
//			ConvertUTF16toUTF8( szNodeName, utf8 );
			elem = m_pRoot->FirstChildElement( utf8 );
			utf8[0] = 0;
		}
		Convert_TCHAR_To_utf8( szVal, utf8 );
//		ConvertUTF16toUTF8( szVal, utf8 );
		const char *str = elem->Attribute( utf8 );
//		XBREAKF( str == NULL, "%s: \"%s\" not found.", GetresXml().c_str(), szVal );
		RET v;
		elem->Attribute( utf8, &v );
		return v;
	}
	// cNodeName의 이름을 가진 Element를 xml전체에서 리커시브로 찾아 스트링형태로 된 "val"값을 읽는다.
	template<typename RET>
	RET GetVal2ByString( const char *cNodeName = nullptr ) {
		auto pElem = GetDoc().FirstChildElement();
		while( pElem ) {
			auto pElemFind = XEXmlNode::sFindElemRecursive( pElem, cNodeName );
			if( pElemFind ) {
				RET v = pElemFind->Attribute( "val" );
				return v;
			}
			pElem = pElem->NextSiblingElement();
		}
		return RET();
	}
	template<typename RET>
	RET GetVal2( const char *cNodeName = nullptr ) {
		auto pElem = GetDoc().FirstChildElement();
		while( pElem ) {
			auto pElemFind = XEXmlNode::sFindElemRecursive( pElem, cNodeName );
			if( pElemFind ) {
				RET v;
				pElemFind->Attribute( "val", &v );
				return v;
			}
			pElem = pElem->NextSiblingElement();
		}
		return RET();
	}
	/// szNodeName밑에 있는 모든 노드를 어레이로 읽는다.
	int GetAryTString( const char *cNodeName, std::vector<_tstring> *pOutAry );
	int GetAryInt( const std::string& strNodeName, XVector<int>* pOutAry );
	float GetFloat( const char* cVal, const char* cNodeName ) {
		_tstring strVal = C2SZ(cVal);
		_tstring strNodeName = C2SZ(cNodeName);
		return (float)GetVal<double>( strVal.c_str(), strNodeName.c_str() );
	}
	int GetInt( const char* cVal, const char* cNodeName ) {
		_tstring strVal = C2SZ( cVal );
		_tstring strNodeName = C2SZ( cNodeName );
		return GetVal<int>( strVal.c_str(), strNodeName.c_str() );
	}
	// 소스컨트롤에 사용하기 좋게 만든 버전.
	int GetInt( const char* cNodeName ) {
 		return GetVal2<int>( cNodeName );
	}
	float GetFloat( const char* cNodeName ) {
		return (float)(GetVal2<double>( cNodeName ));
	}
	LPCTSTR GetString( const char *cKey, const char *cRootNode ) {
		TiXmlElement *elem = m_pRoot->ToElement();
		// 지정된 노드이름이 있으면 그걸찾아서 루트로 쓴다. 없으면 디폴트 루트가 기본이 된다.
		if( XE::IsHave( cRootNode ) ) {
			elem = m_pRoot->FirstChildElement( cRootNode );
		}
		const char *str = elem->Attribute( cKey );
		if( str == NULL ) {
			XLOG( "%s key not found", C2SZ(cKey) );
			return _T("");
		}
		return C2SZ(str);
	}
	/**
	 @brief element이름 cKey를 갖는 노드를 찾아 "val" attribute의 값을 스트링으로 읽는다.
	 검색은 모든 자식들을 검사한다.
	*/
	const char* GetString( const char *cKey ) {
		return GetVal2ByString<const char*>( cKey );
	}
	LPCTSTR GetTString( const char *cKey ) {
		auto cStr = GetString( cKey );
		if( cStr )
			return U82SZ( cStr );
		return _T("");
	}
};
