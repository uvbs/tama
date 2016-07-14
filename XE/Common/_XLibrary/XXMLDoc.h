#pragma once
#include <string>
#include "XE.h"
#include "etc/Types.h"
#include "etc/Global.h"
#include "tinyxml/tinyxml.h"

class XEXmlNode;
////////////////////////////////////////////////////////////////
class XXMLDoc
{
	std::string m_strFullpath;
	_tstring m_resXml;
	TiXmlDocument m_Doc;
	BOOL m_bLoad;
	ID m_idKey = 0;     // 생성될때마다 새로운 키값을 갖는다.
	void Init() {
		m_bLoad = FALSE;
		m_idKey = XE::GenerateID();
	}
	void Destroy();
public:
	XXMLDoc();
	virtual ~XXMLDoc() { Destroy(); }
	//
	GET_ACCESSOR( ID, idKey );
	GET_ACCESSOR( const std::string&, strFullpath );
	const _tstring& GetstrFilename() {
		return m_resXml;
	}
	GET_ACCESSOR( TiXmlDocument&, Doc );
	GET_ACCESSOR( const _tstring&, resXml );
	BOOL IsError() const {
		return m_bLoad == FALSE;
	}
	bool IsLoaded() const {
		return m_bLoad != FALSE;
	}
	bool Load( LPCTSTR resXml );
	BOOL Save( LPCTSTR szFullpath );
	BOOL Reload();
	XEXmlNode FindNode( const char *cNodeName ) const;
	XEXmlNode FindNode2( const char *cNodeName ) const;
	const TiXmlNode* FindNode( const TiXmlNode *pRoot, const char* cNodeFind ) const;
	// xml로딩이 다 끝난후 호출된다.
	virtual bool OnDidFinishLoad() { return true; }
	XEXmlNode AddNode( const char *cNodeName );
	XEXmlNode AddRoot( const char *cNodeName );
	void AddAttribute( const char *cAttrName );
	void SetDeclaration();
	void Clear();
protected:
	SET_ACCESSOR( BOOL, bLoad );
};

////////////////////////////////////////////////////////////////
class XEXmlNode
{
	TiXmlElement *m_pRoot;
	bool m_bNotFound;
	void Init() {
		m_pRoot = NULL;
		m_bNotFound = false;
	}
	void Destroy();
public:
	XEXmlNode( TiXmlElement *pElem ) {
		Init();
		m_pRoot = pElem;
	}
	XEXmlNode( const TiXmlElement *pElem ) {
		Init();
		m_pRoot = const_cast<TiXmlElement*>( pElem );
	}
	XEXmlNode() { Init(); }

	virtual ~XEXmlNode() { Destroy(); }
	//
	SET_ACCESSOR( TiXmlElement*, pRoot );
	BOOL IsEmpty() {
		return m_pRoot == nullptr;
	}
	BOOL IsHave() {
		return m_pRoot != nullptr;
	}
	/// GetInt()류의 attr읽기후 attr이 없을때 true를 리턴한다.
	bool IsNotFound() {
		return m_bNotFound;
	}
	XEXmlNode GetFirst();
	XEXmlNode GetNext();
	XEXmlNode FindNode( const char *cNodeName );
	XEXmlNode FindNode( const std::string& strNodeName ) {
		return FindNode( strNodeName.c_str() );
	}
	XEXmlNode FindNodeRecursive( const char *cNodeName );
	_tstring GetstrName() const;
	std::string GetcstrName() const;
	const char* GetString( const char *cKey );
	LPCTSTR GetTString( const char *cKey );
	int GetStringAry( const char *cKey, std::vector<std::string> *pOutAry );
	int GetInt( const char *cKey );
	bool GetInt2( int* pOut );
	float GetFloat( const char *cKey );
	bool GetBool( const char *cKey );
	XE::VEC2 GetVec2();
	//
	XEXmlNode AddNode( const char *cNodeName );
	XEXmlNode AddNode( const std::string& strNodeName );
	void AddAttribute( const char *cAttrName, int value );
	inline void AddAttribute( const char *cAttrName, DWORD value ) {
		AddAttribute( cAttrName, (int)value );
	}
	void AddAttribute( const char *cAttrName, float value );
#ifdef WIN32
	void AddAttribute( const char *cAttrName, LPCTSTR szStr );
	void AddAttribute( const char *cAttrName, const _tstring& str ) {
		AddAttribute( cAttrName, str.c_str() );
	}
#endif // WIN32
	void AddAttribute( const char *cAttrName, const char* cStr );
	void AddAttribute( const char *cAttrName, const std::string& str ) {
		AddAttribute( cAttrName, str.c_str() );
	}
	void AddAttribute( const char *cAttrName, bool bFlag );
	void AddAttribute( const XE::VEC2& vPos );
// 	template<typename T>
// 	void AddAttribute( const _tstring& strNodeName, T& v ) {
// 		const std::string& strcName = SZ2C(strNodeName);
// 		AddAttribute( strcName.c_str(), v );
// 	}
// 	template<typename T>
// 	void AddAttribute( const std::string& strcNodeName, T& v ) {
///< ""를 넘기면  std::string이 암시적 형변환으로 ""로 바껴서 위에 있는 함수와 모호해져서 에러남
// 		AddAttribute( strcNodeName.c_str(), v );
// 	}
	TiXmlAttribute* GetFirstAttribute();
	static TiXmlElement* sFindElemRecursive( TiXmlElement *pRoot, const char *cNodeName );
	//
	template<typename T>
	int GetAry( const char *cKey, std::vector<T> *pOutAry ) {
		m_bNotFound = false;
		// 콤마로 나뉘어진 값들의 스트링
		const char *cStr = m_pRoot->Attribute( cKey );
		if( XE::IsEmpty( cStr ) ) {
			m_bNotFound = true;
			return 0;
		}
		_tstring strSrc = C2SZ(cStr);	// 콤마로연결된 문자열
		_tstring strToken;
		std::string cstr;
		CToken token;
		if( token.LoadString( strSrc.c_str() ) ) {
			while(1) {
				T val;
				token.GetToken( &val );
				pOutAry->push_back( val );
				if( token.IsEof() )
					break;
				token.GetToken();		// ,
				XBREAK( token.m_Token[ 0 ] != ',' );
			}
		}
		return pOutAry->size();
	}
};

class XEXmlAttr
{
	TiXmlAttribute *m_pAttr;
	void Init() {
		m_pAttr = NULL;
	}
	void Destroy();
public:
	XEXmlAttr( TiXmlAttribute *pAttr ) {
		Init();
		m_pAttr = pAttr;
	}

	virtual ~XEXmlAttr() { Destroy(); }
	//
	BOOL IsEmpty() {
		return m_pAttr == nullptr;
	}
	BOOL IsHave() {
		return m_pAttr != nullptr;
	}
	_tstring GetstrName();		///< 속성 이름을 얻는다.
	std::string GetcstrName();
	//
	int GetInt();		// 속성의 값을 int로 읽는다.
	float GetFloat();
	const char* GetString();
	LPCTSTR GetTString();
	//
	XEXmlAttr GetNext();
	bool GetBool();
};
