#include "stdafx.h"
#include "XXMLDoc.h"
#include "XResMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//#define _XTRACE			XTRACE
#define _XTRACE(...)		(0)

/*
ex)
BOOL MWPropWorld::Load( LPCTSTR szXml )
{
	XXMLDoc xmlWorld;
	if( xmlWorld.Load( XE::MakePath( DIR_PROP, szXml ) ) == FALSE )
		return FALSE;

	// 루트 노드를 찾는다.
	XEXmlNode node = xmlWorld.FindNode("world");
	if( XBREAKF(node.IsEmpty() == TRUE, "not found world node") )
		return FALSE;
	// 첫번째 자식 Land노드를 찾는다.
	XEXmlNode nodeLand = node.GetFirst();
	XBREAKF( nodeLand.IsEmpty(), "not found land" );
	XArrayLinearN<MWPropLand*, 128> aryLands;
	while( !nodeLand.IsEmpty() )
	{
		MWPropLand *pNewLand = new MWPropLand;
		pNewLand->LoadLand( nodeLand, this );
		aryLands.Add( pNewLand );
		// 다음 자식 Land노드를 찾는다.
		nodeLand = nodeLand.GetNext();
	}
	m_aryLands = aryLands;
	return TRUE;
}
*/
////////////////////////////////////////////////////////////////
XXMLDoc::XXMLDoc()
{
	Init();
}

void XXMLDoc::Destroy()
{
}

void XXMLDoc::Clear()
{
	m_Doc.Clear();
	m_bLoad = FALSE;
	m_resXml.clear();
	m_strFullpath.clear();

}

bool XXMLDoc::Load( LPCTSTR resXml )
{
	Clear();
	TCHAR szFullpath[ 1024 ];
	// resXml의 풀패스를 얻어낸다.
	XBREAK( XE::IsEmpty( resXml ) );
	XE::SetReadyRes( szFullpath, resXml );
	if( XE::IsEmpty( szFullpath ) == TRUE )
		return false;
	m_strFullpath = SZ2C( szFullpath );
	//
	if( m_Doc.LoadFile( m_strFullpath.c_str() ) == false ) {
		XLOGXNA("failed open xml: %s\n%s", m_strFullpath.c_str(), m_Doc.ErrorDesc() );
		return false;
	}
	CONSOLE("%s loaded...", C2SZ(m_strFullpath) );
	m_bLoad = TRUE;
	m_resXml = resXml;
	//
// 	auto pRoot = m_Doc.FirstChild( "global" );
// 	auto pNode = pRoot->FirstChild();
// 	while( pNode ) {
// 		const _tstring strVal = U82SZ(pNode->Value());
// 		TRACE("[%s](%d)\n", strVal.c_str(), pNode->Type() );
// 		pNode = pNode->NextSibling();
// 	}
	bool bOk = OnDidFinishLoad();
	return bOk;
}

BOOL XXMLDoc::Reload()
{
	const auto resXml = m_resXml;
	m_Doc.Clear();
	return Load( resXml.c_str() );
}

XEXmlNode XXMLDoc::FindNode( const char *cNodeName ) const
{
	auto pNode = m_Doc.FirstChild( cNodeName );
	if( pNode )	{
		return XEXmlNode( pNode->ToElement() );
	}
	return XEXmlNode();
}

XEXmlNode XXMLDoc::FindNode2( const char *cNodeName ) const
{
	auto pNode = m_Doc.FirstChild();
	while( pNode ) {
		auto pFindNode = FindNode( pNode, cNodeName );
		if( pFindNode )
			return XEXmlNode( pFindNode->ToElement() );
		pNode = pNode->NextSibling();
	}
	return XEXmlNode();
}

const TiXmlNode* XXMLDoc::FindNode( const TiXmlNode *pRoot, const char* cNodeFind ) const
{
	auto pChild = pRoot->FirstChild();
	// 바로 아래단계 자식부터 검사한다.
	while( pChild ) {
		if( XE::IsSame( pChild->Value(), cNodeFind ) )
			return pChild;
		pChild = pChild->NextSibling();
	}
	// 없으면 자식들의 자식들을 검사한다.
	pChild = pRoot->FirstChild();
	while( pChild ) {
		auto pFind = FindNode( pChild, cNodeFind );
		if( pFind )
			return pFind;
		pChild = pChild->NextSibling();
	}
	return nullptr;
}

BOOL XXMLDoc::Save( LPCTSTR szFullpath )
{
	bool bRet = m_Doc.SaveFile( SZ2C(szFullpath) );
	return (BOOL)bRet;
}

/**
 노드(엘리먼트,폴더)를 추가한다.
*/
XEXmlNode XXMLDoc::AddNode( const char *cNodeName )
{
	TiXmlElement *nodeElem = m_Doc.FirstChildElement();
	TiXmlElement *elem = new TiXmlElement( cNodeName );
	if( nodeElem )
	{
		nodeElem->LinkEndChild( elem );
	} else
	{
		// 루트 노드가 없으면 루트노드로 만든다.
		m_Doc.LinkEndChild( elem );
	}
	return XEXmlNode( elem );
}

XEXmlNode XXMLDoc::AddRoot( const char *cNodeName )
{
	TiXmlElement *elem = new TiXmlElement( cNodeName );
	m_Doc.LinkEndChild( elem );
	return XEXmlNode( elem );
}


/**
 속성을 추가한다.
*/
void XXMLDoc::AddAttribute( const char *cAttrName )
{
//	elem->SetAttribute( "xuzhu", 11 );
}

void XXMLDoc::SetDeclaration()
{
	TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	m_Doc.LinkEndChild( decl );
}

////////////////////////////////////////////////////////////////
void XEXmlNode::Destroy()
{
}



XEXmlNode XEXmlNode::GetFirst()
{
	XBREAK( m_pRoot == NULL );
	TiXmlElement *pElem = m_pRoot->FirstChildElement();
	while( pElem ) {
		// 노드이름 첫글자가 언더바(_)면 스킵한다.
		// 범용이 아니라 엔진의 영역이므로 이부분은 엔진에 포함시켜 자동화 시켰다.
		const char *cName = pElem->Value();
		if( cName[0] != '_' )
			break;
		pElem = pElem->NextSiblingElement();	
	}
	return XEXmlNode( pElem );
}

XEXmlNode XEXmlNode::GetNext()
{
	XBREAK( m_pRoot == NULL );
	TiXmlElement *pElem = m_pRoot->NextSiblingElement();
	while( pElem )	{
		// 노드이름 첫글자가 언더바(_)면 스킵한다.
		const char *cName = pElem->Value();
		if( cName[0] != '_' )
			break;
		pElem = pElem->NextSiblingElement();
	} 
	return XEXmlNode( pElem );
}

const char* XEXmlNode::GetString( const char *cKey )
{
	m_bNotFound = false;
	const char *cStr = m_pRoot->Attribute(cKey);
	if( cStr == nullptr ) {
		m_bNotFound = true;
		return "";
	} else
		m_bNotFound = false;
	return cStr;
}

LPCTSTR XEXmlNode::GetTString( const char *cKey )
{
	m_bNotFound = false;
	const char *cStr = m_pRoot->Attribute( cKey );
	if( cStr == nullptr ) {
		m_bNotFound = true;
		return _T("");
	} else
		m_bNotFound = false;
	return U82SZ(cStr);
}

/**
 @brief 콤마로 구분된 스트링배열을 얻는다.
*/
int XEXmlNode::GetStringAry( const char *cKey, std::vector<std::string> *pOutAry )
{
	m_bNotFound = false;
	const char *cStr = m_pRoot->Attribute( cKey );
	if( XE::IsEmpty( cStr ) ) {
		m_bNotFound = true;
		return 0;
	}
	_tstring strSrc = C2SZ(cStr);	// 콤마로연결된 문자열
	_tstring strToken;
	std::string cstr;
	CToken token;
	if( token.LoadStr( strSrc.c_str() ) ) {
		int num = 0;
		while(1) {
			strToken = token.GetTokenByComma();
			if( strToken.empty() )
				break;
			cstr = SZ2C(strToken);
			pOutAry->push_back( cstr );
			++num;
		}
	}
	return pOutAry->size();
}

int XEXmlNode::GetInt( const char *cKey )
{
	m_bNotFound = false;
	int val = 0x7fffffff;
	m_pRoot->Attribute( cKey, &val );
	if( val == 0x7fffffff ) {
		val = 0;
		m_bNotFound = true;
	} else
		m_bNotFound = false;
	return val;
}

/**
 @brief element밑에 attribute가 없이 바로 값을 읽는버전(#text값)
*/
bool XEXmlNode::GetInt2( int* pOut )
{
	auto pElem = m_pRoot->FirstChild();
	if( !pElem )
		return false;
//	*pOut = std::stoi( pElem->ValueStr() );
	*pOut = ::atoi( pElem->ValueStr().c_str() );
	return true;
}

float XEXmlNode::GetFloat( const char *cKey )
{
	m_bNotFound = false;
	double val = 99999.0;
	m_pRoot->Attribute( cKey, &val );
	if( val == 99999.0 ) {
		m_bNotFound = true;
		val = 0;
	} else
		m_bNotFound = false;
	return (float)val;
}

bool XEXmlNode::GetBool( const char *cKey )
{
	std::string strVal = GetString( cKey );
	if( strVal.empty() )
		return false;
	if( strVal == "on" || strVal == "yes" || strVal == "true" || strVal == "1" )
		return true;
	if( strVal == "off" || strVal == "no" || strVal == "false" || strVal == "0" )
		return false;
	XBREAKF( 1, "%s:알수없는 토큰", C2SZ(strVal.c_str()) );
	return false;
}

XE::VEC2 XEXmlNode::GetVec2()
{
	XE::VEC2 v;
	v.x = GetFloat( "posx" );
	v.y = GetFloat( "posy" );
	return v;
}

_tstring XEXmlNode::GetstrName() const
{
	const char *cNodeName = m_pRoot->Value();
	XBREAK( cNodeName == NULL );	// 노드네임이 없는경우는 없을거 같아서 해놓음. 만약 있다면 이부분 없앨것.
	if( cNodeName )
		return C2SZ(cNodeName);
	return _T("");
}

std::string XEXmlNode::GetcstrName() const
{
	const char *cNodeName = m_pRoot->Value();
	XBREAK( cNodeName == NULL );	// 노드네임이 없는경우는 없을거 같아서 해놓음. 만약 있다면 이부분 없앨것.
	if( cNodeName )
		return cNodeName;
	return "";
}


XEXmlNode XEXmlNode::FindNode( const char *cNodeName )
{
	TiXmlElement *pElem = m_pRoot->FirstChildElement( cNodeName );
	return XEXmlNode( pElem );
}

/**
 @brief 재귀검색으로 모든 노드를 다 검사한다.
*/
XEXmlNode XEXmlNode::FindNodeRecursive( const char *cNodeName )
{
	auto node = GetFirst();
	if( node.IsEmpty() ) {
		return node;
	} else {
		while( !node.IsEmpty() ) {
			_XTRACE("%s ", node.GetcstrName().c_str() );
			if( node.GetcstrName() == cNodeName )
				return node;
			node = node.GetNext();
		}
		node = GetFirst();
		while( !node.IsEmpty() ) {
			_XTRACE( "%s ", node.GetcstrName().c_str() );
			auto nodeFind = node.FindNodeRecursive( cNodeName );
			if( !nodeFind.IsEmpty() )
				return nodeFind;
			node = node.GetNext();
		}
	}
	return XEXmlNode();
}

TiXmlElement* XEXmlNode::sFindElemRecursive( TiXmlElement *pRoot, const char *cNodeName )
{
	auto pElem = pRoot->FirstChildElement();
	while( pElem ) {
		auto cValue = pElem->Value();
		if( cValue[0] != '_' ) {
			if( XE::IsSame( cValue, cNodeName ) )
				return pElem;
		}
		pElem = pElem = pElem->NextSiblingElement();
	}
	pElem = pRoot->FirstChildElement();
	while( pElem ) {
		_XTRACE( "%s ", pElem->Value() );
		auto cValue = pElem->Value();
		if( cValue[0] != '_' ) {
			auto pElemFind = XEXmlNode::sFindElemRecursive( pElem, cNodeName );
			if( pElemFind )
				return pElemFind;
		}
		pElem = pElem = pElem->NextSiblingElement();
	}
	return nullptr;
}

/**
 노드(엘리먼트,폴더)를 추가한다.
*/
XEXmlNode XEXmlNode::AddNode( const char *cNodeName )
{
	TiXmlElement *elem = new TiXmlElement( cNodeName );
	m_pRoot->LinkEndChild( elem );
	return XEXmlNode( elem );
}
XEXmlNode XEXmlNode::AddNode( const std::string& strNodeName )
{
	return AddNode( strNodeName.c_str() );
}

/**
 속성을 추가한다.
*/
void XEXmlNode::AddAttribute( const char *cAttrName, int value )
{
	m_pRoot->SetAttribute( cAttrName, value );
}
void XEXmlNode::AddAttribute( const char *cAttrName, bool bFlag )
{
	m_pRoot->SetAttribute( cAttrName, (bFlag)? "yes" : "no" );
}

void XEXmlNode::AddAttribute( const char *cAttrName, float value )
{
	m_pRoot->SetDoubleAttribute( cAttrName, (double)value );
}
#ifdef WIN32
void XEXmlNode::AddAttribute( const char *cAttrName, LPCTSTR szStr )
{
	if( XE::IsEmpty( szStr ) )
		return;
	char cBuff[ 1024 ];
	cBuff[0] = 0;
	ConvertUTF16toUTF8( szStr, cBuff );
	m_pRoot->SetAttribute( cAttrName, cBuff );
}
#endif // WIN32

void XEXmlNode::AddAttribute( const char *cAttrName, const char* cStr )
{
	if( XE::IsEmpty(cStr) )
		return;
	m_pRoot->SetAttribute( cAttrName, cStr );
}

void XEXmlNode::AddAttribute( const XE::VEC2& vPos )
{
	AddAttribute( "posx", vPos.x );
	AddAttribute( "posy", vPos.y );
}



TiXmlAttribute* XEXmlNode::GetFirstAttribute()
{
	if( XBREAK( m_pRoot == NULL ) )
		return nullptr;
	TiXmlAttribute *pAttr = m_pRoot->FirstAttribute();
	while( pAttr )
	{
		// 노드이름 첫글자가 언더바(_)면 스킵한다.
		// 범용이 아니라 엔진의 영역이므로 이부분은 엔진에 포함시켜 자동화 시켰다.
		const char *cName = pAttr->Name();
		int val = pAttr->IntValue();
		if( cName[ 0 ] != '_' )
			break;
		pAttr = pAttr->Next();
	}
	return pAttr;
}

// XEXmlNode XEXmlNode::GetNext()
// {
// 	XBREAK( m_pRoot == NULL );
// 	TiXmlElement *pElem = m_pRoot->NextSiblingElement();
// 	while( pElem )
// 	{
// 		// 노드이름 첫글자가 언더바(_)면 스킵한다.
// 		const char *cName = pElem->Value();
// 		if( cName[ 0 ] != '_' )
// 			break;
// 		pElem = pElem->NextSiblingElement();
// 	}
// 	return XEXmlNode( pElem );
// }
// 

////////////////////////////////////////////////////////////////
void XEXmlAttr::Destroy()
{
}

const char* XEXmlAttr::GetString()
{
	return m_pAttr->Value();
}

LPCTSTR XEXmlAttr::GetTString()
{
	return C2SZ(m_pAttr->Value());
}

int XEXmlAttr::GetInt()
{
	return m_pAttr->IntValue();
}

float XEXmlAttr::GetFloat()
{
	return (float)m_pAttr->DoubleValue();
}

bool XEXmlAttr::GetBool()
{
	std::string strVal = GetString();
	if( strVal.empty() )
		return false;
	if( strVal == "on" || strVal == "yes" || strVal == "true" || strVal == "1" )
		return true;
	if( strVal == "off" || strVal == "no" || strVal == "false" || strVal == "0" )
		return false;
	XBREAKF( 1, "%s:알수없는 토큰", C2SZ( strVal.c_str() ) );
	return false;
}

_tstring XEXmlAttr::GetstrName()
{
	const char *cAttrName = m_pAttr->Name();
	XBREAK( cAttrName == NULL );	// 노드네임이 없는경우는 없을거 같아서 해놓음. 만약 있다면 이부분 없앨것.
	if( cAttrName )
		return C2SZ(cAttrName);
	return _T("");
}

std::string XEXmlAttr::GetcstrName()
{
	const char *cAttrName = m_pAttr->Name();
	XBREAK( cAttrName == NULL );	// 노드네임이 없는경우는 없을거 같아서 해놓음. 만약 있다면 이부분 없앨것.
	if( cAttrName )
		return cAttrName;
	return "";
}

XEXmlAttr XEXmlAttr::GetNext()
{
	XBREAK( m_pAttr == NULL );
	TiXmlAttribute *pAttr = m_pAttr->Next();
	while( pAttr )
	{
		// 노드이름 첫글자가 언더바(_)면 스킵한다.
		const char *cName = pAttr->Value();
		if( cName[ 0 ] != '_' )
			break;
		pAttr = pAttr->Next();
	}
	return XEXmlAttr( pAttr );
}

