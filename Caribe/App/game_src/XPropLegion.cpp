#include "stdafx.h"
#include "XPropLegion.h"
#include "XSystem.h"
#include "XPropLegionH.h"
#ifdef _XSINGLE
#include "XAccount.h"
#include "XHero.h"
#include "XLegion.h"
#endif // _XSINGLE

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

std::shared_ptr<XPropLegion> XPropLegion::s_spInstance;

ID XPropLegion::s_idGlobal = 0;
//////////////////////////////////////////////////////////////////////////
std::shared_ptr<XPropLegion>& XPropLegion::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XPropLegion>( new XPropLegion );
	return s_spInstance;
}
////////////////////////////////////////////////////////////////
XPropLegion::XPropLegion()
{
	Init();
}

void XPropLegion::Destroy()
{
	DestroyAll();
}

void XPropLegion::DestroyAll()
{
// 	Destroy();
	XVECTOR_DESTROY( m_aryLegions );
	m_mapLegionsByIds.clear();
	m_mapLegionsByID.clear();
}

bool XPropLegion::OnDidFinishLoad( void )
{
	XEXmlNode nodeRoot = FindNode( "root" );
	if( nodeRoot.IsEmpty() )
		return false;
	///< 
	XEXmlNode nodeLegion = nodeRoot.GetFirst();
	if( XBREAK( nodeLegion.IsEmpty() ) )
		return false;
	// "legion.xxx"블럭들을 읽음.
	while( !nodeLegion.IsEmpty() ) {
		auto pNewProp = LoadLegionNode( nodeLegion );
		if( pNewProp )
			AddProp( pNewProp );
		// 다음 노드로
		nodeLegion = nodeLegion.GetNext();
	}
	if( m_bError ) {
		XALERT( "%s:프로퍼티 읽던 중 에러. 콘솔창을 참고하시오.", GetstrFilename().c_str() );
		return false;
	}
	return true;
}

bool XPropLegion::Load( LPCTSTR szXml )
{
	return XXMLDoc::Load( XE::MakePath( DIR_PROP, szXml ) );
}

/**
 @brief idsSeq만 읽어서 메모리에 올린다.
*/
xLegion* XPropLegion::LoadLegionEach( const std::string& idsNode )
{
	if( !IsLoaded() ) {
		_tstring strFullpath = C2SZ( GetstrFullpath() );
		if( !XXMLDoc::Load( strFullpath.c_str() ) )
			return nullptr;
	}
	XEXmlNode nodeRoot = FindNode( "root" );
	if( nodeRoot.IsEmpty() )
		return nullptr;
	// "legion.xxx"블럭들을 읽음.
	auto nodeProp = nodeRoot.FindNode( idsNode.c_str() );
	if( !nodeProp.IsEmpty() ) {
		auto pNewProp = LoadLegionNode( nodeProp );
		if( pNewProp ) {
			AddProp( pNewProp );
			return pNewProp;
		}
		if( m_bError ) {
			XALERT( "%s:프로퍼티 읽던 중 에러. 콘솔창을 참고하시오.", GetstrFilename().c_str() );
		}
	}
	return nullptr;
}

void XPropLegion::AddProp( xLegion* pLegion )
{
	XBREAK( pLegion->strIds.empty() );
	m_aryLegions.Add( pLegion );
	m_mapLegionsByIds[ pLegion->strIds ] = pLegion;
	// ids를 기반으로 idProp을 자동생성한다.
	XUINT64 idProp = XE::GetCheckSum( pLegion->strIds );
	XBREAK( idProp > 0xffffffff );	// 32비트만 사용한다. 넘어가는 경우가 생기면 아이디 만드는 다른방법을 찾아야함.
	pLegion->_SetidProp( (ID)idProp );
	m_mapLegionsByID[ (ID)idProp ] = pLegion;
}

xLegion* XPropLegion::GetpProp( const char *cIdentifier )
{
	if( XE::IsEmpty(cIdentifier) )
		return nullptr;
	auto itor = m_mapLegionsByIds.find( std::string(cIdentifier) );
	if( itor == m_mapLegionsByIds.end() ) {
		// 없으면 해당 elem만 lazy로딩한다.
		auto pPropSeq = LoadLegionEach( std::string(cIdentifier) );
		return pPropSeq;
	}
	auto pProp = itor->second;
	XBREAK( pProp == nullptr );
	return pProp;
}

xLegion* XPropLegion::GetpProp( const ID idProp )
{
	if( idProp == 0 )
		return nullptr;
	auto itor = m_mapLegionsByID.find( idProp );
	if( itor == m_mapLegionsByID.end() ) {
		return nullptr;
	}
	auto pProp = itor->second;
	XBREAK( pProp == nullptr );
	return pProp;
}

/**
 @brief nodeLegion노드를 읽어 프로퍼티 객체를 생성하여 돌려준다.
 프로퍼티의 식별자는 nodeLegion의 이름이 된다.
*/
xLegion* XPropLegion::LoadLegionNode( XEXmlNode& nodeLegion )
{
	auto pProp = CreateProp( nodeLegion.GetcstrName() );
	bool bOk = pProp->LoadFromXML( nodeLegion, nodeLegion.GetstrName() );
	if( !bOk ) {
		SAFE_DELETE( pProp );
	}
	return pProp;
}

XGAME::xLegion* XPropLegion::CreateProp( const char* cIdentifier )
{
	XBREAK( cIdentifier == nullptr );
	return new XGAME::xLegion( cIdentifier );
}

#ifdef _xIN_TOOL
bool XPropLegion::Save( LPCTSTR szXml )
{
	// 자동 백업.
	_tstring strSrc = XE::MakePackageFullPath( DIR_PROP, szXml );
	TCHAR szSystemPath[ 4096 ];
	XE::MakeHighPath( szSystemPath, XE::MakePackageFullPath( _T( "" ), _T( "" ) ) );
	_tstring strDstPath = szSystemPath;
	strDstPath += _T( "backup/" );
	_tstring strDst = strDstPath;
	int h, m, s;
	XSYSTEM::GetHourMinSec( &h, &m, &s );
	strDst += XE::Format( _T( "propLegion_%02d%02d%02d.xml" ), h, m, s );
	XSYSTEM::MakeDir( strDstPath.c_str() );
	XSYSTEM::CopyFileX( strSrc.c_str(), strDst.c_str() );

	_tstring strPath = XE::MakePackageFullPath( DIR_PROP, szXml );
	XE::xtERROR err = XSYSTEM::IsReadOnly( strPath.c_str() );
	if( err == XE::ERR_READONLY || err == XE::ERR_PERMISSION_DENIED ) {
		XALERT( "파일을 check out하십시오.\n%s", XE::GetFileName( strPath.c_str() ) );
		return false;
	}
	XXMLDoc xml;
	xml.SetDeclaration();
	XEXmlNode nodeRoot = xml.AddNode( "root" );
	for( auto pProp : m_aryLegions ) {
		pProp->SaveXML( nodeRoot );
	}
	BOOL bRet = xml.Save( XE::MakePackageFullPath( DIR_PROP, szXml ) );
	return ( bRet ) ? true : false;
}

/**
 @brief idsLegion prop을 찾아서 spAcc의 영웅정보를 바탕으로 갱신한다.
*/
#ifdef _XSINGLE
void XPropLegion::UpdatePropWithAcc( const std::string& idsLegion, XSPAcc spAcc )
{
	auto pPropLegion = GetpProp( idsLegion );
	if( pPropLegion ) {
		for( auto& squad : pPropLegion->arySquads ) {
			const int idxPos = squad.idxPos;
			auto spLegion = spAcc->GetCurrLegion();
			auto pHero = spLegion->GetpHeroByIdxPos( idxPos );
			if( XASSERT( pHero ) ) {
				squad.m_listAbil.clear();
				for( auto i = 1; i < XGAME::xUNIT_MAX; ++i ) {
					const auto unit = (XGAME::xtUnit)i;
					const auto& mapAbil = pHero->GetmapAbil( unit );
					// 영웅이 보유한 모든 특성을 갱신한다.
					for( auto& itorAbil : mapAbil ) {
						const ID idAbil = itorAbil.first;
						const XGAME::xAbil& abil = itorAbil.second;
						if( abil.point > 0 ) {
							auto pPropAbil = XPropTech::sGet()->GetpNode( unit, idAbil );
							if( pPropAbil ) {
								XGAME::xAbil2 abil2;
								abil2.m_idsAbil = pPropAbil->strSkill;
								abil2.point = abil.point;
								squad.m_listAbil.Add( abil2 );
							}
						}
					}
				}
			}
		}
	}
}
#endif // _XSINGLE
/**
 @brief spAcc계정의 영웅들이 보유한 특성들을 propLegion_s.xml에 갱신하고 저장한다.
 이것은 싱글전용이다.
*/
// bool XPropLegion::SaveWithAcc( LPCTSTR szXml, XSPAcc spAcc )
// {
// 	// propLegion에 원래 있던 군단프로퍼티를 순회하며 해당 부대 영웅의 특성트리를 갱신한다.
// 	for( auto pPropLegion : m_aryLegions ) {
// 	}
// 	return Save( szXml );
// }
#endif // _xIN_TOOL


void XPropLegion::Serialize( XArchive& ar ) const
{
	ar << VER_PROP_LEGION;
	ar << (int)m_aryLegions.size();
	for( auto pProp : m_aryLegions ) {
		ar << (*pProp);
	}
}
void XPropLegion::DeSerialize( XArchive& ar, int )
{
	DestroyAll();
	int ver, num;
	ar >> ver;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pProp = new XGAME::xLegion();
		ar >> (*pProp);
		AddProp( pProp );
	}
}


