#include "stdafx.h"
#include "XPropBgObj.h"
#include "XArchive.h"
#include "XWorld.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xnBgObj;

#define PROP_LOG( F, ... )	\
	CONSOLE( "%s:"_T(F), GetstrFilename().c_str(), ##__VA_ARGS__ )

#ifdef _xIN_TOOL
ID XPropBgObj::s_idGlobal = 0;
#endif
std::shared_ptr<XPropBgObj> XPropBgObj::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XPropBgObj>& XPropBgObj::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XPropBgObj>( new XPropBgObj );
	return s_spInstance;
}
////////////////////////////////////////////////////////////////
XPropBgObj::XPropBgObj()
{
	Init();
}

void XPropBgObj::Destroy()
{
	AUTO_DESTROY( m_BgObjs );
}

bool XPropBgObj::LoadProp( LPCTSTR szXml )
{
	return Load( XE::MakePath( DIR_PROP, szXml ) );
}

bool XPropBgObj::OnDidFinishLoad()
{
	XEXmlNode nodeRoot = FindNode( "root" );
	if( nodeRoot.IsEmpty() ) {
		XALERT("%s: not found root node", GetstrFilename().c_str() );
		return false;
	}
	XEXmlNode nodeChild = nodeRoot.GetFirst();
	if( nodeChild.IsEmpty() ) {
		return false;
	}
	while( !nodeChild.IsEmpty() ) {
		if( nodeChild.GetcstrName() == "obj" ) {
			LoadObj( nodeChild );
		} else {
			PROP_LOG( "%s: 알수없는 노드", nodeChild.GetstrName().c_str() );
		}
		//
		nodeChild = nodeRoot.GetNext();
	}
	CONSOLE("propBgObj loaded....numArea=%d", m_BgObjs.size() );
	return true;
}

bool XPropBgObj::LoadObj( XEXmlNode& nodeObj )
{
	XEXmlNode nodeEach = nodeObj.GetFirst();
	bool bOk = true;
	while( !nodeObj.IsEmpty() ) {
		//
		nodeEach = nodeEach.GetNext();
	}
	return bOk;
}

/**
 PropBgObj의 데이타를 xml로 쓴다.
*/
#ifdef _xIN_TOOL
bool XPropBgObj::SaveProp( LPCTSTR szXml )
{
#ifdef _XSINGLE
	return TRUE;
#endif
	// 자동백업
	_tstring strSrc = XE::MakePackageFullPath( DIR_PROP, szXml );
	TCHAR szSystemPath[ 4096 ];
	XE::MakeHighPath( szSystemPath, XE::MakePackageFullPath( _T( "" ), _T( "" ) ) );
	_tstring strDstPath = szSystemPath;
	strDstPath += _T( "backup/" );
	_tstring strDst = strDstPath;
	int h, m, s;
	XSYSTEM::GetHourMinSec( &h, &m, &s );
	strDst += XE::Format( _T( "propBgObj_%02d%02d%02d.xml" ), h, m, s );
	XSYSTEM::MakeDir( strDstPath.c_str() );
	XSYSTEM::CopyFileX( strSrc.c_str(), strDst.c_str() );
	//
	_tstring strPath = XE::MakePackageFullPath( DIR_PROP, szXml);
	XE::xtERROR err = XSYSTEM::IsReadOnly( strPath.c_str() );
	if( err == XE::ERR_READONLY || err == XE::ERR_PERMISSION_DENIED ) {
		XALERT("파일을 check out하십시오.\n%s", XE::GetFileName(strPath.c_str()) );
		return FALSE;
	}
	XXMLDoc xml;
	xml.SetDeclaration();
// 	XEXmlNode nodeRoot = xml.AddNode("clouds");
// 
// 	for( auto pBgObj : m_BgObjs ) {
// 		XEXmlNode nodeBgObj = nodeRoot.AddNode( "cloud" );
// 		nodeBgObj.AddAttribute( "id", (int)pBgObj->idBgObj );
// 		nodeBgObj.AddAttribute( "ids", pBgObj->strIdentifier.c_str() );
// 		nodeBgObj.AddAttribute( "level", (int)pBgObj->lvArea );
// 		if( pBgObj->lvOpenable2 > 0 )
// 			nodeBgObj.AddAttribute( "lv_open", (int)pBgObj->lvOpenable2 );
// 		nodeBgObj.AddAttribute( "cost", (int)pBgObj->cost );
// 		if( pBgObj->vAdjust.x != 0 )
// 			nodeBgObj.AddAttribute( "cost_pos_x", (int)pBgObj->vAdjust.x );
// 		if( pBgObj->vAdjust.y != 0 )
// 			nodeBgObj.AddAttribute( "cost_pos_y", (int)pBgObj->vAdjust.y );
// 		if( pBgObj->idsPrecedeArea.empty() )
// 			nodeBgObj.AddAttribute( "ids_precede", "none" );
// 		else
// 			nodeBgObj.AddAttribute( "ids_precede", pBgObj->idsPrecedeArea.c_str() );
// 		nodeBgObj.AddAttribute( "ids_quest", pBgObj->idsQuest.c_str() );
// 		if( pBgObj->idName )
// 			nodeBgObj.AddAttribute( "name", (int)pBgObj->idName );
// 		nodeBgObj.AddAttribute( "key_item", pBgObj->idsItem.c_str() );
// 		//
// 		{
// 			XArrayLinearN<ID, 512> ary;
// 			pBgObj->GetSpotsToAry( ary );
// 			XARRAYLINEARN_LOOP( ary, ID, idSpot ) {
// 				XEXmlNode nodeSpot = nodeBgObj.AddNode( "spot" );
// 				nodeSpot.AddAttribute( "id", (int)idSpot );
// 			} END_LOOP;
// 		}
// 	}
	CONSOLE("save propBgObj");
	//
	return xml.Save( XE::MakePackageFullPath( DIR_PROP, szXml ) ) != FALSE;
}
#endif

void XPropBgObj::AddBgObj( xProp *pProp )
{
	XBREAK( pProp == nullptr );
	m_BgObjs.Add( pProp );
	auto itor = m_mapBgObjsID.find( pProp->m_idObj );
	if( XASSERT(itor == m_mapBgObjsID.end()) ) {
		m_mapBgObjsID[ pProp->m_idObj ] = pProp;
	} else {
		PROP_LOG( "이미 같은 아이디(%d)의 BgObj가 있습니다.", pProp->m_idObj );
	}
}

xProp* XPropBgObj::GetpProp( ID idBgObj )
{
	if( idBgObj == 0 )
		return nullptr;
	auto itor = m_mapBgObjsID.find( idBgObj );
	if( itor == m_mapBgObjsID.end() )
		return nullptr;
	auto pBgObj = itor->second;
	XBREAK( pBgObj == nullptr );
	return pBgObj;
}

void XPropBgObj::DestroyBgObj( ID idBgObj )
{
#ifdef _xIN_TOOL
	xProp *pProp = GetpProp( idBgObj );
	if( pProp == nullptr )
		return;
	m_BgObjs.DelByID( idBgObj );
	{
		auto itor = m_mapBgObjsID.find( idBgObj );
		if( itor != m_mapBgObjsID.end() ) {
			m_mapBgObjsID.erase( itor++ );
		}
	}
	SAFE_DELETE( pProp );
#else
	XBREAK(1);		// 툴모드에서만 쓸수 있는 기능임. 게임에서는 삭제할일이 없음.
#endif
}

/**
 @brief 모든 지역(구름) 프로퍼티를 어레이에 담아준다.
*/
int XPropBgObj::GetPropToAry( XVector<xProp*> *pOutAry ) 
{
	XBREAK( pOutAry == nullptr );
	for( auto pBgObj : m_BgObjs ) {
		pOutAry->push_back( pBgObj );
	}
	return pOutAry->size();
}
/**
 @brief 모든 지역(구름)의 지역ID를 어레이에 담아준다.
*/
int XPropBgObj::GetPropToAry( XVector<ID> *pOutAry ) 
{
	XBREAK( pOutAry == nullptr );
	for( auto pBgObj : m_BgObjs ) {
		pOutAry->push_back( pBgObj->m_idObj );
	}
	return pOutAry->size();
}
