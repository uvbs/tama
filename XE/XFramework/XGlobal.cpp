#include "stdafx.h"
#include "XFramework/XGlobal.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XGlobalVal::XGlobalVal()
{
	Init();
	//
}

void XGlobalVal::Destroy()
{
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 루트키 이하의 모든 노드들의 변수들을 읽는다.
 억세스시엔 <노드이름>.<attr이름> 과같은식으로 접근한다.
*/
void XGlobalVal::LoadParams( const std::string& strKeyRoot )
{
	auto nodeParams = m_nodeRoot.FindNode( strKeyRoot );
	if( nodeParams.IsEmpty() )
		return;
	LoadNode( nodeParams, "" );
// 	auto node = nodeParams.GetFirst();
// 	while( !node.IsEmpty() ) {
// 		const std::string strKey = node.GetcstrName();
// 		if( strKey.at(0) != '_' ) {
// 			LoadNode( node, strKey );
// 		}
// 		node = node.GetNext();
// 	}
}

void XGlobalVal::LoadNode( XEXmlNode& nodeParent, const std::string& _strKeyParent )
{
	XEXmlAttr attr = nodeParent.GetFirstAttribute();
	while( !attr.IsEmpty() ) {
		// attr로 검색되는것들은 모두 최종 변수.
		const std::string strAttr = attr.GetcstrName();
		if( strAttr.at(0) != '_' ) {
			std::string strKey;
			if( strAttr == "val" ) {
				// attr이름이 val인것은 키 이름에 포함시키지 않는다.
				XBREAK( _strKeyParent.empty() );
				strKey = _strKeyParent;		// 부모키 이름만 사용함.
			} else {
				if( _strKeyParent.empty() ) {
					strKey = strAttr;
				} else {
					strKey = _strKeyParent;
					if( !_strKeyParent.empty() )
						strKey += ".";
					strKey += strAttr;
				}
			}
			const std::string strVal = attr.GetString2();		// 값을 스트링으로 읽음.
			m_Params.Set( strKey, strVal );		// 키/밸류 등록
		} // _
		attr = attr.GetNext();
	}
	///< 
	XEXmlNode nodeChild = nodeParent.GetFirst();
	while( !nodeChild.IsEmpty() ) {
		std::string strKey = _strKeyParent;
		if( !_strKeyParent.empty() )
			strKey +=+ ".";
		strKey += nodeChild.GetcstrName();		// aaa.bbb.ccc.val 이런식으로 연결되도록 계속 붙인다.
		if( strKey.at(0) != '_' ) {
			LoadNode( nodeChild, strKey );
		}
		nodeChild = nodeChild.GetNext();
	}
}

bool XGlobalVal::OnDidFinishLoad()
{
	CONSOLE( "loaded global.xml" );
	// 루트노드를 구해놓는다.
	m_pRoot = GetDoc().FirstChild( "global" );
	m_nodeRoot.SetpRoot( m_pRoot->ToElement() );
	XBREAKF( m_pRoot == NULL, "global node not found" );
	return true;
}

int XGlobalVal::GetAryTString( const char *cNodeName, std::vector<_tstring> *pOutAry ) 
{
	XBREAK( XE::IsEmpty( cNodeName ) );
	auto node = m_nodeRoot.FindNode( cNodeName );
	auto nodeChild = node.GetFirst();
	while( !nodeChild.IsEmpty() ) {
		auto pAttr = nodeChild.GetFirstAttribute();
		if( pAttr ) {
			_tstring strVal = C2SZ( pAttr->Value() );
			pOutAry->push_back( strVal );
		}
		nodeChild = nodeChild.GetNext();
	}
	return pOutAry->size();
}

/**
 @brief strNodeName elem밑에 "val" element들의 값을 int로 읽어 어레이에 담는다.
 @param strNodeName 전 xml에 걸쳐 고유한 이름
*/
int XGlobalVal::GetAryInt( const std::string& strNodeName, XVector<int>* pOutAry )
{
	int numItems = 0;
	if( m_nodeRoot.IsEmpty() )
		return 0;
	if( XASSERT( !strNodeName.empty() ) ) {
		// array가 될 루트 노드를 찾는다.
		auto nodeAry = m_nodeRoot.FindNodeRecursive( strNodeName.c_str() );
		if( nodeAry.IsEmpty() )
			return 0;
		// 루트노드 아래의 "val" element를 모두 찾음.
		auto nodeElem = nodeAry.GetFirst();
		while( nodeElem.IsHave() ) {
			int val = 0;
			nodeElem.GetInt2( &val );
// 			const int val = nodeElem.GetInt( "val" );
			pOutAry->Add( val );
			nodeElem = nodeElem.GetNext();
		}
	}
	return numItems;
}