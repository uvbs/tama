#include "stdafx.h"
#include "XPropBgObj.h"
#include "XArchive.h"
#include "XWorld.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xnBgObj;
using namespace XGAME;

#define PROP_LOG( F, ... )	\
	CONSOLE( "%s(%s):" _T(F), GetstrFilename().c_str(), szTag, ##__VA_ARGS__ ); \
	m_bError = true;


XE_NAMESPACE_START( xnBgObj )
//
const char* GetstrEnum( XGAME::xtBgObj type )
{
	switch( type )	{
	case XGAME::xBOT_WATER_SHINE:	return "xBOT_WATER_SHINE";
	case XGAME::xBOT_WATER_FALL:	return "xBOT_WATER_FALL";
	case XGAME::xBOT_WATER_STEAM: return "xBOT_WATER_STEAM";
	case XGAME::xBOT_LAVA_BOIL:		return "xBOT_LAVA_BOIL";
	case XGAME::xBOT_LAVA_STEAM:	return "xBOT_LAVA_STEAM";
	case XGAME::xBOT_LAVA_STREAM:	return "xBOT_LAVA_STREAM";
	case XGAME::xBOT_JEWEL_SHINE:	return "xBOT_JEWEL_SHINE";
//	case XGAME::xBOT_SOUND: return "xBOT_SOUND";
	default:
		XBREAK( 1 );
		break;
	}
	return "";
}
#ifdef _xIN_TOOL
LPCTSTR GetSprByBgObjType( XGAME::xtBgObj type )
{
	switch( type )
	{
	case XGAME::xBOT_WATER_SHINE:
	case XGAME::xBOT_WATER_FALL:
	case XGAME::xBOT_WATER_STEAM:
	case XGAME::xBOT_LAVA_BOIL:
	case XGAME::xBOT_LAVA_STEAM:
	case XGAME::xBOT_LAVA_STREAM:
		return _T("obj_bg.spr");
	case XGAME::xBOT_JEWEL_SHINE:
		return _T("ui_jewel_shine.spr");
//	case XGAME::xBOT_SOUND:
		return _T("");
	default:
		XBREAK(1);
		return _T("obj_bg.spr");
		break;
	}
	return _T( "obj_bg.spr" );
}

//
void xProp::SetNew( XGAME::xtBgObj type
									, const _tstring& strSpr
									, ID idAct
									, const XE::VEC2& vwPos ) 
{
	m_idObj = 0;		// id는 Add할때 자동생성된다.
	m_Type = type;
	m_strType = GetstrEnum( type );
	m_typeSub = 0;
	m_strSpr = strSpr;
	m_idAct = idAct;
	m_vwPos = vwPos;
}
#endif	// xIN_TOOL
XE_NAMESPACE_END; // xnBgObj

#ifdef _xIN_TOOL
ID XPropBgObj::s_idGlobal = 0;
#endif // _xIN_TOOL
std::shared_ptr<XPropBgObj> XPropBgObj::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XPropBgObj>& XPropBgObj::sGet() {	if( s_spInstance == nullptr )		s_spInstance = std::shared_ptr<XPropBgObj>( new XPropBgObj );	return s_spInstance;}
void XPropBgObj::sDestroyInstance() {
	s_spInstance.reset();
}

////////////////////////////////////////////////////////////////
XPropBgObj::XPropBgObj()
{
	Init();
}

void XPropBgObj::Destroy()
{
	CONSOLE("Destroy PropBgObj");
	AUTO_DESTROY( m_listBgObjs );
	m_mapBgObjsID.clear();
}

bool XPropBgObj::LoadProp( LPCTSTR szXml )
{
	return Load( XE::MakePath( DIR_PROP, szXml ) );
}

bool XPropBgObj::OnDidFinishLoad()
{
	LPCTSTR szTag = _T("root");
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
		const std::string strNodeName = nodeChild.GetcstrName();
		const _tstring strtNodeName = C2SZ( strNodeName );
		auto type = (XGAME::xtBgObj)CONSTANT->GetValue( strtNodeName );
		if( type > XGAME::xBOT_NONE && type < XGAME::xBOT_MAX ) {
			LoadObj( nodeChild, type, strtNodeName.c_str() );
		} else {
			PROP_LOG( "%s: 알수없는 노드", nodeChild.GetstrName().c_str() );
		}
		//
		nodeChild = nodeChild.GetNext();
	}
	CONSOLE("propBgObj loaded....num=%d", m_listBgObjs.size() );
	if( m_bError ) {
		XALERT("%s: 로딩중 에러 발견.", GetstrFilename().c_str() );
	}
	return true;
}

bool XPropBgObj::LoadObj( XEXmlNode& nodeObj, XGAME::xtBgObj type, LPCTSTR szTag )
{
// 	LPCTSTR szTag = XFORMAT("type=%d", type );
	XEXmlNode nodeEach = nodeObj.GetFirst();
	bool bOk = true;
	while( !nodeEach.IsEmpty() ) {
		const std::string strcName = nodeEach.GetcstrName();
		if( strcName == "each" ) {
			const _tstring strTag = C2SZ(strcName);
			auto pProp = LoadEach( nodeEach, type, strTag.c_str() );
			if( pProp ) {
				pProp->m_strSpr = nodeObj.GetTString( "spr" );
//				if( type != xBOT_SOUND ) {
					if( XBREAK(pProp->m_strSpr.empty()) ) {
						PROP_LOG("error: spr is empty");
					}
//				}
				AddBgObj( pProp );
			}
		} else {
			PROP_LOG( "%s: 알수없는 노드", C2SZ(strcName.c_str()) )
		}
		//
		nodeEach = nodeEach.GetNext();
	}
	return bOk;
}

xnBgObj::xProp* XPropBgObj::LoadEach( XEXmlNode& nodeEach, xtBgObj type, LPCTSTR szTag )
{
	auto pProp = new xnBgObj::xProp();
	pProp->m_idObj = (ID)nodeEach.GetInt( "id" );
	if( XBREAK(pProp->m_idObj == 0) ) {
		SAFE_DELETE( pProp );
		return nullptr;
	}
#ifdef _xIN_TOOL
	s_idGlobal = std::max( pProp->m_idObj, s_idGlobal );
#endif // _xIN_TOOL
	pProp->m_vwPos = nodeEach.GetVec2();
	if( pProp->m_vwPos.IsInvalid() ) {
		PROP_LOG("warning: invalid pos");
	}
	pProp->m_Type = type;
	pProp->m_typeSub = 0;
//	if( type == xBOT_SOUND ) {
//		pProp->m_strSnd = nodeEach.GetTString( "file" );
//	} else {
		pProp->m_idAct = nodeEach.GetInt( "id_act" );
		if( XBREAK( pProp->m_idAct == 0 ) ) {
			PROP_LOG( "error: id_act is 0" );
		}
//	}
	pProp->m_strSnd = nodeEach.GetTString("sound");
	const _tstring strExt = XE::GetFileExt( pProp->m_strSnd );
	if( !strExt.empty() ) {
		pProp->m_strSnd = XE::GetFileTitle( pProp->m_strSnd );
	}
	return pProp;
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
	// 타입별로 분류한다.
	XVector<XVector<xProp*>> aryTypes( xBOT_MAX );
	for( int i = 1; i < xBOT_MAX; ++i ) {
		GetPropToAry( &aryTypes[i], (xtBgObj)i );
	}
	XXMLDoc xml;
	xml.SetDeclaration();
 	XEXmlNode nodeRoot = xml.AddNode("root");
	// 
	int nType = 0;
	for( auto& aryBgObj : aryTypes ) {
		const auto type = (xtBgObj)nType++;
		if( type == xBOT_NONE )
			continue;
		if( aryBgObj.size() > 0 ) {
			auto pPropFirst = aryBgObj[ 0 ];
			XEXmlNode nodeBgObj = nodeRoot.AddNode( pPropFirst->m_strType );
			nodeBgObj.AddAttribute( "spr", pPropFirst->m_strSpr );
			for( auto pProp : aryBgObj ) {
				XEXmlNode nodeEach = nodeBgObj.AddNode( "each" );
				nodeEach.AddAttribute( pProp->m_vwPos );
				nodeEach.AddAttribute( "id", pProp->m_idObj );
				nodeEach.AddAttribute( "id_act", pProp->m_idAct );
				nodeEach.AddAttribute( "sound", pProp->m_strSnd );
			}
		}
	}
	CONSOLE("save propBgObj");
	//
	return xml.Save( XE::MakePackageFullPath( DIR_PROP, szXml ) ) != FALSE;
}
#endif // xIN_TOOL

void XPropBgObj::AddBgObj( xProp *pProp )
{
	if( XBREAK( !pProp->m_Type ) )
		return;
#ifdef _xIN_TOOL
	if( pProp->m_idObj == 0 )
		pProp->m_idObj = sGenerateID();
	if( pProp->m_strType.empty() )
		pProp->m_strType = GetstrEnum( pProp->m_Type );
	pProp->m_strSpr = (pProp->m_strSpr.empty())? xnBgObj::GetSprByBgObjType(pProp->m_Type) : pProp->m_strSpr;
#else //_xIN_TOOL
	XBREAK( pProp->m_idObj == 0 );
	XBREAK( pProp->m_strSpr.empty() );
#endif // not _xIN_TOOL
//	_tstring strTag = C2SZ(pProp->m_strType);
	_tstring strTag = C2SZ( xnBgObj::GetstrEnum( pProp->m_Type ) );
	LPCTSTR szTag = strTag.c_str();
	XBREAK( pProp == nullptr );
	auto pExist = GetSameProp( pProp );
	if( XBREAK(pExist) ) {
		// 중복.
		return;
	}
	m_listBgObjs.Add( pProp );
	auto itor = m_mapBgObjsID.find( pProp->m_idObj );
	if( XASSERT(itor == m_mapBgObjsID.end()) ) {
		m_mapBgObjsID[ pProp->m_idObj ] = pProp;
	} else {
		PROP_LOG( "이미 같은 아이디(%d)의 BgObj가 있습니다.", pProp->m_idObj );
	}
}

/**
 @brief 모든 값이 같은 잘못된 객체가 있는지 검사.
*/
xnBgObj::xProp* XPropBgObj::GetSameProp( const xnBgObj::xProp* pPropSrc )
{
	for( auto pProp : m_listBgObjs ) {
		if( (int)pProp->m_vwPos.x == (int)pPropSrc->m_vwPos.x
			&& (int)pProp->m_vwPos.y == (int)pPropSrc->m_vwPos.y ) {
			if( pProp->m_strSpr == pPropSrc->m_strSpr ) {
				if( pProp->m_idAct == pPropSrc->m_idAct ) {
					if( pProp->m_Type == pPropSrc->m_Type 
						&& pProp->m_typeSub == pPropSrc->m_typeSub )
						return pProp;
				}
			}
		}
	}
	return false;
}

xnBgObj::xProp* XPropBgObj::CreateNewProp()
{
	auto pProp = new xProp();
	return pProp;
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
	m_listBgObjs.DelByID( idBgObj );
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
int XPropBgObj::GetPropToAry( XVector<xProp*> *pOutAry, xtBgObj type ) 
{
	XBREAK( pOutAry == nullptr );
	for( auto pProp : m_listBgObjs ) {
		if( !type || type == pProp->m_Type )
			pOutAry->push_back( pProp );
	}
	return pOutAry->size();
}
/**
 @brief 모든 지역(구름)의 지역ID를 어레이에 담아준다.
*/
int XPropBgObj::GetPropToAry( XVector<ID> *pOutAry, xtBgObj type ) 
{
	XBREAK( pOutAry == nullptr );
	for( auto pProp : m_listBgObjs ) {
		if( !type || type == pProp->m_Type )
			pOutAry->push_back( pProp->m_idObj );
	}
	return pOutAry->size();
}
