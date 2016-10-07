#include "stdafx.h"
#include "XPropWorld.h"
#include "XPropCloud.h"
#include "XQuestProp.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XPropWorld *PROP_WORLD = nullptr;

ID XPropWorld::s_idGlobal = 0;
float XPropWorld::s_ratioProduce = 1.f;		// 생산양 조절용 비율.
float XPropWorld::s_ratioRegen = 1.f;		// 리젠시간 조절용 비율.
float XPropWorld::s_ratioCastleProduce = 1.f;		// 생산양 조절용 비율.

void XPropWorld::xBASESPOT::Serialize( XArchive& ar ) const {
	XBREAK( level < 0 || level > 255 );
	ar << strIdentifier << strWhen << strWhenDel << strSpr;
#ifdef _DEV
	ar << strCode;
#endif
	ar << idWhen << idWhenDel << idSpot;
	ar << vWorld;
	ar << (BYTE)type;
	ar << (BYTE)level;
	ar << (BYTE)xboolToByte(m_bitAttr.bOpened);
	ar << (BYTE)idAct;
	ar << idArea << idCode << idName;
}
void XPropWorld::xBASESPOT::DeSerialize( XArchive& ar, int ) {
	BYTE b0;
	ar >> strIdentifier >> strWhen >> strWhenDel >> strSpr;
#ifdef _DEV
	ar >> strCode;
#endif
	ar >> idWhen >> idWhenDel >> idSpot;
	ar >> vWorld;
	ar >> b0;		type = (xtSpot)b0;
	ar >> b0;		level = b0;
	ar >> b0;		m_bitAttr.bOpened = xbyteToBool(b0);
	ar >> b0;		idAct = b0;
	ar >> idArea >> idCode >> idName;
}

void XPropWorld::xCASTLE::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
#if _DEV_LEVEL <= DLV_DEV_CREW
	ar << m_aryProduceOrig;
#endif
	ar << m_aryProduce;
}
void XPropWorld::xCASTLE::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
#if _DEV_LEVEL <= DLV_DEV_CREW
#pragma message("======================임시 삭제=====================")
	ar >> m_aryProduceOrig;
#endif
	ar >> m_aryProduce;
}

void XPropWorld::xJEWEL::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << idx;
	ar << produce;
}
void XPropWorld::xJEWEL::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> idx;
	ar >> produce;
}

void XPropWorld::xSULFUR::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << produce;
	ar << secRegenNpc;
}
void XPropWorld::xSULFUR::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> produce;
	ar >> secRegenNpc;
}

void XPropWorld::xMANDRAKE::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << idx;
	ar << produce;
}
void XPropWorld::xMANDRAKE::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> idx;
	ar >> produce;
}

void XPropWorld::xNPC::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << secRegen;
	ar << m_idsLegion;
}
void XPropWorld::xNPC::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> secRegen;
	_tstring idstLegion;
	ar >> idstLegion;
	m_idsLegion = SZ2C( idstLegion );
}

void XPropWorld::xDaily::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << dummy << m_v1 << m_v2 << m_v3;
	ar << m_aryDays;
}
void XPropWorld::xDaily::DeSerialize( XArchive& ar, int ver ) {
	m_aryDays.clear();
	xBASESPOT::DeSerialize( ar, ver );
	ar >> dummy >> m_v1 >> m_v2 >> m_v3;
	ar >> m_aryDays;
}

void XPropWorld::xCampaign::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << strCamp;
	ar << idCamp;
}
void XPropWorld::xCampaign::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> strCamp;
	ar >> idCamp;
}

void XPropWorld::xVisit::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << idDialog;
	ar << idsSeq;
}
void XPropWorld::xVisit::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> idDialog;
	_tstring idstSeq;
	ar >> idstSeq;
	idsSeq = SZ2C( idstSeq );
}

void XPropWorld::xCash::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << secRegen << produceMin << produceMax;
}
void XPropWorld::xCash::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> secRegen >> produceMin >> produceMax;
}

void XPropWorld::xGuildRaid::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << dummy;
}
void XPropWorld::xGuildRaid::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> dummy;
}

void XPropWorld::xPrivateRaid::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << m_idsLegion;
}
void XPropWorld::xPrivateRaid::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> m_idsLegion;
}

void XPropWorld::xCommon::Serialize( XArchive& ar ) const {
	xBASESPOT::Serialize( ar );
	ar << strType << strParam;
	for( int i = 0; i < 4; ++i )
		ar << nParam[i];
}
void XPropWorld::xCommon::DeSerialize( XArchive& ar, int ver ) {
	xBASESPOT::DeSerialize( ar, ver );
	ar >> strType >> strParam;
	for( int i = 0; i < 4; ++i )
		ar >> nParam[i];
}


////////////////////////////////////////////////////////////////
XPropWorld::XPropWorld( LPCTSTR szXml )
{
	Init();
	Load( XE::MakePath( DIR_PROP, szXml ) );
}

void XPropWorld::Destroy()
{
	XLIST4_DESTROY( _m_listSpots );
	m_mapSpots.clear();
	m_mapSpotsIds.clear();
	m_mapSpotsByWhen.clear();
	m_mapSpotsByWhenDel.clear();
	m_mapSpotsByCode.clear();
	XCLEAR_ARRAY( m_numSpot );
}

bool XPropWorld::OnDidFinishLoad()
{
	XEXmlNode rootNode = FindNode( "world" );
	m_strIdentifier = C2SZ( rootNode.GetString( "identifier" ) );
	m_strImg = C2SZ( rootNode.GetString( "file" ) );
	s_ratioProduce = rootNode.GetFloat("ratio_produce");
	if( s_ratioProduce == 0 )
		s_ratioProduce = 1.f;
	s_ratioCastleProduce = rootNode.GetFloat( "ratio_castle_produce" );
	if( s_ratioCastleProduce == 0 )
		s_ratioCastleProduce = 1.f;
	s_ratioRegen = rootNode.GetFloat( "ratio_regen" );
	if( s_ratioRegen == 0 )
		s_ratioRegen = 1.f;
	XBREAK( m_strIdentifier == _T("nullptr") );
	XBREAK( m_strImg == _T( "nullptr" ) );

	// 자원지/유저 스팟 리스트
	{
		XEXmlNode resNode = rootNode.FindNode( "spots" );
		LoadSpots( resNode );
	}
	return true;
}

LPCTSTR XPropWorld::MakeIds( ID idSpot )
{
	return XE::Format( _T( "spot.%d" ), idSpot );
}

void XPropWorld::GetBaseSpotAttr( xBASESPOT *pSpot, XEXmlNode& childNode )
{
	ID idSpot = (ID)childNode.GetInt( "id" );
	XBREAK( idSpot == 0 );
	XBREAK( idSpot > 0xffff );	// word로 serialize하는곳이 있음.
	auto pExist = GetpProp( idSpot );
	if( pExist ) {
		XBREAKF( 1, "같은 id(%d) 스팟 발견:%s", idSpot, pExist->strIdentifier.c_str() );
	}
	pSpot->idSpot = idSpot;
	if( pSpot->idSpot > s_idGlobal )
		s_idGlobal = pSpot->idSpot;
	pSpot->vWorld.x = (float)childNode.GetInt( "x" );
	pSpot->vWorld.y = (float)childNode.GetInt( "y" );
	_tstring strIdentifier = C2SZ( childNode.GetString( "identifier" ) );
	if( strIdentifier.empty() )
		strIdentifier = C2SZ( childNode.GetString( "ids" ) );
	// 직접 지정한 식별자가 있을때만 넣는다.
	if( strIdentifier.empty() == false )
		pSpot->strIdentifier = strIdentifier;
 	if( strIdentifier.empty() )
 		pSpot->strIdentifier = MakeIds( pSpot->idSpot );
 	else
 		pSpot->strIdentifier = strIdentifier;
	pSpot->strWhen = C2SZ( childNode.GetString("when") );
	pSpot->idName = childNode.GetInt("name");
	std::string strWhen = childNode.GetString( "when" );
	pSpot->strWhen = C2SZ( strWhen.c_str() );	// 디버깅용.
	if( strWhen.empty() == false )
		pSpot->idWhen = XQuestProp::sGetIdsToId( strWhen );
	std::string strCode = childNode.GetString( "code" );
	if( strCode.empty() == false ) {
		pSpot->strCode = C2SZ( strCode.c_str() );
		pSpot->idCode = XQuestProp::sGetIdsToId( strCode );
	}
	std::string strWhenDel = childNode.GetString( "when_del" );
	pSpot->strWhenDel = C2SZ( strWhenDel.c_str() );	// 디버깅용.
	if( strWhenDel.empty() == false )
		pSpot->idWhenDel = XQuestProp::sGetIdsToId( strWhenDel );
	pSpot->level = childNode.GetInt("level");
//	pSpot->adjLevel = childNode.GetInt("adj_level");
	pSpot->strSpr = childNode.GetTString( "spr" );
	int nAct = childNode.GetInt("act");
	if( nAct > 0 )
		pSpot->idAct = (ID)nAct;
	int _size = sizeof(pSpot->m_bitAttr);
	pSpot->m_bitAttr.bOpened = childNode.GetBool( "opened" );
}

/**
자원지/유저스팟 루트노드 
*/
BOOL XPropWorld::LoadSpots( XEXmlNode& node )
{
	XBREAK( s_ratioProduce == 0.f );
	XEXmlNode childNode = node.GetFirst();
//	XArrayLinearN<xBASESPOT*,1024> m_listSpots;
	while( !childNode.IsEmpty() ) {
		xBASESPOT *pBaseProp = nullptr;
		// 식별자가 있는 경우 읽는다.
		if( childNode.GetstrName() == _T("castle") ) {
			pBaseProp = LoadCastle( node, childNode );
		} else
		if( childNode.GetstrName() == _T("jewel") ) {
			pBaseProp = LoadJewel( node, childNode );
		} else
		if( childNode.GetstrName() == _T("sulfur") ) {
			pBaseProp = LoadSulfur( node, childNode );
		} else
		if( childNode.GetstrName() == _T("mandrake") ) {
			pBaseProp = LoadMandrake( node, childNode );
		} else
		if( childNode.GetstrName() == _T("npc") || childNode.GetstrName() == _T("event") ) {
			pBaseProp = LoadNpc( node, childNode );
		} else
		if( childNode.GetstrName() == _T("daily") ) {
			pBaseProp = LoadDaily( node, childNode );
// 			xDaily *spot = new xDaily;
// 			pBaseSpot = spot;
// 			GetBaseSpotAttr( spot, childNode );
		} else
// 		if( childNode.GetstrName() == _T( "special" ) ) {
// 			xSpecial *spot = new xSpecial;
// 			pBaseSpot = spot;
// 			GetBaseSpotAttr( spot, childNode );
// 		} else
		if( childNode.GetstrName() == _T("camp") ) {
			xCampaign *spot = new xCampaign;
			pBaseProp = spot;
			GetBaseSpotAttr( spot, childNode );
			std::string strCamp = childNode.GetString( "camp" );
			spot->strCamp = C2SZ( strCamp.c_str() );	// 디버깅용.
			if( strCamp.empty() == false )
				spot->idCamp = XQuestProp::sGetIdsToId( strCamp );
		} else
		if( childNode.GetstrName() == _T("visit") ) {
			xVisit *spot = new xVisit;
			pBaseProp = spot;
			GetBaseSpotAttr( spot, childNode );
			spot->idDialog = (ID)childNode.GetInt( "dialog" );
			spot->idsSeq = childNode.GetString( "seq" );
		} else
		if( childNode.GetstrName() == _T("cash") ) {
			xCash *spot = new xCash;
			pBaseProp = spot;
			GetBaseSpotAttr( spot, childNode );
			spot->secRegen = (float)childNode.GetInt( "secRegen" );
			spot->produceMin = childNode.GetFloat( "numMin" );
			spot->produceMax = childNode.GetFloat( "numMax" );
		} else
		if( childNode.GetstrName() == _T("private_raid") ) {
			auto* pProp = new xPrivateRaid;
			pBaseProp = pProp;
			GetBaseSpotAttr( pProp, childNode );
			pProp->m_idsLegion = childNode.GetString( "legion" );
		} else
		if( childNode.GetstrName() == _T("common") ) {
			xCommon *spot = new xCommon;
			pBaseProp = spot;
			GetBaseSpotAttr( spot, childNode );
			spot->strType = childNode.GetTString( "type" );
			spot->strParam = childNode.GetTString( "str" );
#ifdef _XCAMP_HERO2
			if( spot->strParam == _T("camp.hero") )
				spot->strParam = _T("camp.hero2");

#endif // _XCAMP_HERO2
			XBREAK( spot->strType == _T("fixed_camp") && spot->strParam.empty() );
			spot->nParam[ 0 ] = childNode.GetInt( "p1" );
			spot->nParam[ 1 ] = childNode.GetInt( "p2" );
			spot->nParam[ 2 ] = childNode.GetInt( "p3" );
			spot->nParam[ 3 ] = childNode.GetInt( "p4" );
		}
		// 
		//
		if( XASSERT(pBaseProp) ) {
			AddSpot( pBaseProp );
		}
		childNode = childNode.GetNext();
	}
	return TRUE;
}
///< "castle"블럭을 읽음.
XPropWorld::xBASESPOT* XPropWorld::LoadCastle( XEXmlNode& node, XEXmlNode& childNode )
{
	xCASTLE *spot = new xCASTLE;
	GetBaseSpotAttr( spot, childNode );
	// childNode내의 "produce"블럭을 ResXML타입으로 읽어서 배열에 쌓는다.
	_tstring strTag = XFORMAT("%s:node=%s", GetstrFilename().c_str(), childNode.GetstrName().c_str() );
	std::vector<XGAME::xRES_NUM> aryRes;
	XGAME::LoadResFromXMLToAry( childNode, _T("produce"), strTag.c_str(), &aryRes );
	if( aryRes.size() > 0 ) {
		spot->SetAryProduce( aryRes );
	} else {
		// 모든 성은 목재/철은 기본 생산
		XGAME::xRES_NUM res1( XGAME::xRES_WOOD, 60 );
		aryRes.push_back( res1 );
		XGAME::xRES_NUM res2( XGAME::xRES_IRON, 60 );
		aryRes.push_back( res2 );
		spot->SetAryProduce( aryRes );
	}
	return spot;
}

XPropWorld::xJEWEL* XPropWorld::LoadJewel( XEXmlNode& node, XEXmlNode& childNode )
{
	xJEWEL *spot = new xJEWEL;
	GetBaseSpotAttr( spot, childNode );
	spot->idx = childNode.GetInt( "idx" );
	spot->SetProduce( childNode.GetFloat( "produce" ) );
	if( spot->_GetProduce() == 0 )
		spot->SetProduce( 180.f );
	return spot;
}

XPropWorld::xSULFUR* XPropWorld::LoadSulfur( XEXmlNode& node, XEXmlNode& childNode )
{
	xSULFUR *spot = new xSULFUR;
	GetBaseSpotAttr( spot, childNode );
	auto produce = childNode.GetFloat( "produce" );
	if( produce == 0 )
		produce = 1;
	spot->SetProduce( produce );
	spot->secRegenNpc = (float)childNode.GetInt( "secRegenNpc" );
	if( spot->secRegenNpc == 0 )
		spot->secRegenNpc = 300;
	return spot;
}

XPropWorld::xMANDRAKE* XPropWorld::LoadMandrake( XEXmlNode& node, XEXmlNode& childNode )
{
	xMANDRAKE *spot = new xMANDRAKE;
	GetBaseSpotAttr( spot, childNode );
	spot->idx = childNode.GetInt( "idx" );
	spot->SetProduce( childNode.GetFloat( "produce" ) );
	return spot;
}

XPropWorld::xNPC* XPropWorld::LoadNpc( XEXmlNode& node, XEXmlNode& childNode )
{
	xNPC *spot = new xNPC;
	GetBaseSpotAttr( spot, childNode );
	spot->secRegen = 180.f;	// 코드에서 관리하기위해 일괄값을 넣음.
	// 구버전 방식의 하위호환 맞춰야 함.
	spot->m_idsLegion = childNode.GetString( "legion" );
	return spot;
}

XPropWorld::xDaily* XPropWorld::LoadDaily( XEXmlNode& node, XEXmlNode& childNode )
{
	auto pProp = new xDaily;
	GetBaseSpotAttr( pProp, childNode );
	auto nodeReward = childNode.FindNode( "reward" );
	if( !nodeReward.IsEmpty() ) {
		pProp->m_v1 = nodeReward.GetFloat( "v1" );
		pProp->m_v2 = nodeReward.GetFloat( "v2" );
		pProp->m_v3 = nodeReward.GetFloat( "v3" );
		for( int i = 0; i < 7; ++i ) {
			const std::string strNode = XE::Format("day%d", i+1);
			auto nodeDay = nodeReward.FindNode( strNode );
			if( !nodeDay.IsEmpty() ) {
				auto& aryRes = pProp->m_aryDays[i];
				XGAME::xReward::sLoadResFromXMLToAry( nodeDay, _T("reward"), _T("daily"), &aryRes );
			}
		}
	}
	return pProp;
}


//////////////////////////////////////////////////////////////////////////
XPropWorld::xJEWEL* XPropWorld::GetpPropJewel( LPCTSTR szIdentifier )
{
	auto pProp = GetpProp( szIdentifier );
	if( pProp ) {
		if( XASSERT(pProp->GetType() == XGAME::xSPOT_JEWEL) )
			return static_cast<xJEWEL*>( pProp );
	}
	return nullptr;
}

/**
 광산인덱스 번호로 찾기: 0:말뚝광산 1,2가 공유광산
*/
XPropWorld::xJEWEL* XPropWorld::GetpPropJewelByIdx( int idx )
{
	for( auto pProp : _m_listSpots ) {
		if( pProp->type == XGAME::xSPOT_JEWEL ) {
			auto pJewel = static_cast<xJEWEL*>( pProp );
			if( pJewel->idx == idx )
				return pJewel;
		}
	}
	return nullptr;
}

XPropWorld::xMANDRAKE* XPropWorld::GetpPropMandrakeByIdx( int idx )
{
	for( auto pProp : _m_listSpots ) {
		if( pProp->type == XGAME::xSPOT_MANDRAKE ) {
			auto pMandrake = static_cast<xMANDRAKE*>( pProp );
			if( pMandrake->idx == idx )
				return pMandrake;
		}
	}
	return nullptr;
}


XPropWorld::xNPC* XPropWorld::GetpPropNpc( LPCTSTR szIdentifier )
{
	auto pProp = GetpProp( szIdentifier );
	if( pProp ) {
		if( XASSERT( pProp->GetType() == XGAME::xSPOT_NPC ) )
			return static_cast<xNPC*>( pProp );
	}
// 	for( auto pProp : m_listSpots )
// 	{
// 		if( pProp->strIdentifier == szIdentifier )
// 		{
// 			if( pProp->type == XGAME::xSPOT_NPC )
// 				return static_cast<xNPC*>( pProp );
// 		}
// 	}
	return nullptr;
}

/**
 @brief 구름 프로퍼티를 읽은 후 각 스팟들은 어느 구름지역에 속해있는지 아이디를 세팅한다.
*/
void XPropWorld::SetAreaToSpots()
{
	for( auto pProp : _m_listSpots )	{
		pProp->idArea = PROP_CLOUD->GetidAreaHaveSpot( pProp->idSpot );
		if( pProp->IsInvalidSpot() ) {
			XBREAKF( pProp->IsInvalidSpot(),
				"idspot=%d pos:%d,%d: 구름영역에 속하지 않은 스팟발견. 스팟좌표 10픽셀정도 수정할것.", pProp->idSpot, (int)pProp->vWorld.x, (int)pProp->vWorld.y );
			// hexa반지름을 조금더 키워서 이제 사각지대가 없을걸로 생각된다.
			// 있으면 반지름을 좀더 키우든가 해야함.
		}
	}
}

/**
 @brief strWhenKey를 키로하여 pSpot을 맵에 넣는다.
*/
void XPropWorld::AddWhenSpot( const _tstring& strWhenKey, xBASESPOT *pSpot )
{
	if( strWhenKey.empty() )
		return;
	XList4<xBASESPOT*> *pList = GetpPropWhen( strWhenKey );
		m_mapSpotsByWhen[ strWhenKey ].Add( pSpot );
}

/**
 @brief 
*/
void XPropWorld::AddWhenDelSpot( const _tstring& strWhenDelKey, xBASESPOT *pSpot )
{
	if( strWhenDelKey.empty() )
		return;
	XList4<xBASESPOT*> *pList = GetpPropWhenDel( strWhenDelKey );
	m_mapSpotsByWhenDel[ strWhenDelKey ].Add( pSpot );
}


bool XPropWorld::Save( LPCTSTR szXml )
{
#ifdef _xIN_TOOL
	// 자동 백업.
	_tstring strSrc = XE::MakePackageFullPath( DIR_PROP, szXml );
	TCHAR szSystemPath[4096];
	XE::MakeHighPath( szSystemPath, XE::MakePackageFullPath( _T(""), _T("") ) );
	_tstring strDstPath = szSystemPath;
	strDstPath += _T("backup/");
	_tstring strDst = strDstPath;
		//XE::MakePackageFullPath( _T("../../backup/"), XE::GetFileTitle(szXml) );
	int h, m, s;
	XSYSTEM::GetHourMinSec( &h, &m, &s );
	strDst += XE::Format(_T("propWorld_%02d%02d%02d.xml"), h, m, s );
	XSYSTEM::MakeDir( strDstPath.c_str() );
	XSYSTEM::CopyFileX( strSrc.c_str(), strDst.c_str() );

	_tstring strPath = XE::MakePackageFullPath( DIR_PROP, szXml );
	XE::xtERROR err = XSYSTEM::IsReadOnly( strPath.c_str() );
	if( err == XE::ERR_READONLY || err == XE::ERR_PERMISSION_DENIED )
	{
		XALERT( "파일을 check out하십시오.\n%s", XE::GetFileName( strPath.c_str() ) );
		return false;
	}
	XXMLDoc xml;
	xml.SetDeclaration();
	XEXmlNode nodeRoot = xml.AddNode( "world" );
	XEXmlNode nodeSpots = nodeRoot.AddNode( "spots" );

	nodeRoot.AddAttribute( "ids", m_strIdentifier.c_str() );
	nodeRoot.AddAttribute( "file", m_strImg.c_str() );
	nodeRoot.AddAttribute( "ratio_produce", s_ratioProduce );
	nodeRoot.AddAttribute( "ratio_castle_produce", s_ratioCastleProduce );
	nodeRoot.AddAttribute( "ratio_regen", s_ratioRegen );
	//
	for( auto pBaseProp : _m_listSpots ) {
		switch( pBaseProp->type ) {
		case XGAME::xSPOT_CASTLE: {
			auto pProp = static_cast<xCASTLE*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "castle" );
			AddBaseAttribute( pBaseProp, node );
#if _DEV_LEVEL <= DLV_DEV_CREW
			for( auto& res : pProp->_GetAryProduceOrig() ) {
				auto nodeProduce = node.AddNode( "produce" );
				if( XASSERT(nodeProduce.IsHave()) ) {
					nodeProduce.AddAttribute( "res", XGAME::GetEnumResource( res.type ) );
					nodeProduce.AddAttribute( "num", res.num );
				}
			}
#endif
		} break;
		case XGAME::xSPOT_JEWEL: {
			auto pProp = static_cast<xJEWEL*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "jewel" );
			AddBaseAttribute( pBaseProp, node );
			node.AddAttribute( "idx", pProp->idx );
			node.AddAttribute( "produce", pProp->_GetProduce() );
		} break;
		case XGAME::xSPOT_SULFUR: {
			auto pProp = static_cast<xSULFUR*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "sulfur" );
			AddBaseAttribute( pBaseProp, node );
			node.AddAttribute( "produce", pProp->_GetProduce() );
			node.AddAttribute( "secRegenNpc", pProp->secRegenNpc );
		} break;
		case XGAME::xSPOT_MANDRAKE: {
			auto pProp = static_cast<xMANDRAKE*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "mandrake" );
			AddBaseAttribute( pBaseProp, node );
			node.AddAttribute( "idx", pProp->idx );
			node.AddAttribute( "produce", pProp->_GetProduce() );
		} break;
		case XGAME::xSPOT_NPC: {
			auto pProp = static_cast<xNPC*>( pBaseProp );
			XEXmlNode node;
			if( pProp->idWhen )
				node = nodeSpots.AddNode( "event" );
			else
				node = nodeSpots.AddNode( "npc" );
			AddBaseAttribute( pBaseProp, node );
			node.AddAttribute( "secRegen", pProp->secRegen );
			node.AddAttribute( "legion", pProp->m_idsLegion );
		} break;
		case XGAME::xSPOT_DAILY: {
			auto pProp = static_cast<xDaily*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "daily" );
			AddBaseAttribute( pBaseProp, node );
			SaveDaily( pProp, node );
		} break;
		case XGAME::xSPOT_CAMPAIGN: {
			auto pProp = static_cast<xCampaign*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "camp" );
			AddBaseAttribute( pBaseProp, node );
//			XBREAK( pProp->strCamp.empty() );
			node.AddAttribute( "camp", pProp->strCamp.c_str() );
		} break;
		case XGAME::xSPOT_VISIT: {
			auto pProp = static_cast<xVisit*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "visit" );
			AddBaseAttribute( pBaseProp, node );
			node.AddAttribute( "dialog", pProp->idDialog );
			node.AddAttribute( "seq", pProp->idsSeq.c_str() );
		} break;
		case XGAME::xSPOT_CASH: {
			auto pProp = static_cast<xCash*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "cash" );
			AddBaseAttribute( pBaseProp, node );
			node.AddAttribute( "secRegen", pProp->secRegen );
			node.AddAttribute( "numMin", pProp->produceMin );
			node.AddAttribute( "numMax", pProp->produceMax );
		} break;
    case XGAME::xSPOT_COMMON: {
			auto pProp = static_cast<xCommon*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "common" );
			AddBaseAttribute( pBaseProp, node );
			node.AddAttribute( "type", pProp->strType.c_str() );
			if( !pProp->strParam.empty() )
				node.AddAttribute( "str", pProp->strParam.c_str() );
			if( pProp->nParam[ 0 ] )
				node.AddAttribute( "p1", pProp->nParam[ 0 ] );
			if( pProp->nParam[ 1 ] )
				node.AddAttribute( "p2", pProp->nParam[ 1 ] );
			if( pProp->nParam[ 2 ] )
				node.AddAttribute( "p3", pProp->nParam[ 2 ] );
			if( pProp->nParam[ 3 ] )
				node.AddAttribute( "p4", pProp->nParam[ 3 ] );
    } break;
		case XGAME::xSPOT_PRIVATE_RAID: {
			auto pProp = static_cast<xPrivateRaid*>( pBaseProp );
			XEXmlNode node = nodeSpots.AddNode( "private_raid" );
			AddBaseAttribute( pBaseProp, node );
			node.AddAttribute( "legion", pProp->m_idsLegion );
		} break;
		default:
			XBREAK(1);
			break;
		}
	} // for

	CONSOLE( "save propWorld" );
	//
	BOOL bRet = xml.Save( XE::MakePackageFullPath( DIR_PROP, szXml ) );
	return (bRet)? true : false;
#else
	return TRUE;
#endif // _xIN_TOOL
}

bool XPropWorld::SaveDaily( xDaily* pProp, XEXmlNode& nodeDaily )
{
	auto nodeReward = nodeDaily.AddNode( "reward" );
	if( XASSERT(!nodeReward.IsEmpty()) ) {
		nodeReward.AddAttribute( "v1", pProp->m_v1 );
		nodeReward.AddAttribute( "v2", pProp->m_v2 );
		nodeReward.AddAttribute( "v3", pProp->m_v3 );
		XBREAK( pProp->m_aryDays.size() != 7 );
		int idx = 1;
		for( auto& aryReward : pProp->m_aryDays ) {
			const std::string& strKey = XE::Format("day%d", idx++ );
			auto nodeDay = nodeReward.AddNode( strKey );
			if( XASSERT(nodeDay.IsHave()) ) {
				for( auto& reward : aryReward ) {
					if( !reward.SaveXML( nodeDay, strKey.c_str(), "reward" ) )
						return false;
				}
			}
		}
	}
	return true;
}

void XPropWorld::AddBaseAttribute( xBASESPOT *pProp, XEXmlNode& node )
{
	node.AddAttribute( "id", pProp->idSpot );
	node.AddAttribute( "x", (int)pProp->vWorld.x );
	node.AddAttribute( "y", (int)pProp->vWorld.y );
	if( pProp->idName )
		node.AddAttribute( "name", pProp->idName );
	if( pProp->strIdentifier.empty() == false )
		node.AddAttribute( "ids", pProp->strIdentifier.c_str() );
	if( pProp->strWhen.empty() == false )
		node.AddAttribute( "when", pProp->strWhen.c_str() );
	if( pProp->strWhenDel.empty() == false )
		node.AddAttribute( "when_del", pProp->strWhenDel.c_str() );
	if( pProp->level )
		node.AddAttribute( "level", pProp->level );
	if( !pProp->strSpr.empty() )
		node.AddAttribute( "spr", pProp->strSpr.c_str() );
// 	if( pProp->adjLevel )
// 		node.AddAttribute( "level_grade", pProp->adjLevel );
	if( pProp->m_bitAttr.bOpened )
		node.AddAttribute( "opened", true );
#ifdef _DEV
	if( pProp->strCode.empty() == false )
		node.AddAttribute( "code", pProp->strCode.c_str() );
#endif // _DEV
}

/**
 @brief empty의 스팟프로퍼티를 하나 만들어서 리턴한다.
*/
XPropWorld::xBASESPOT* XPropWorld::CreateSpot( XGAME::xtSpot type )
{
	ID idSpot = ++s_idGlobal;
	_tstring strIdentifier = MakeIds( idSpot );
	xBASESPOT *pBaseSpot = sCreateSpot( type );

	if( pBaseSpot )	{
		pBaseSpot->idSpot = idSpot;
		pBaseSpot->strIdentifier = strIdentifier;
	}
	return pBaseSpot;
}

XPropWorld::xBASESPOT* XPropWorld::sCreateSpot( xtSpot type )
{
	xBASESPOT *pBaseSpot = nullptr;
	switch( type ) {
	case XGAME::xSPOT_CASTLE: {
		pBaseSpot = new xCASTLE;
	} break;
	case XGAME::xSPOT_JEWEL:
		pBaseSpot = new xJEWEL;
		break;
	case XGAME::xSPOT_SULFUR:
		pBaseSpot = new xSULFUR;
		break;
	case XGAME::xSPOT_MANDRAKE:
		pBaseSpot = new xMANDRAKE;
		break;
	case XGAME::xSPOT_NPC:
		pBaseSpot = new xNPC;
		break;
	case XGAME::xSPOT_DAILY:
		pBaseSpot = new xDaily;
		break;
// 	case XGAME::xSPOT_SPECIAL:
// 		pBaseSpot = new xSpecial;
// 		break;
	case XGAME::xSPOT_CAMPAIGN:
		pBaseSpot = new xCampaign;
		break;
	case XGAME::xSPOT_VISIT:
		pBaseSpot = new xVisit;
		break;
	case XGAME::xSPOT_CASH:
		pBaseSpot = new xCash;
		break;
	case XGAME::xSPOT_PRIVATE_RAID:
		pBaseSpot = new xPrivateRaid;
		break;
	case XGAME::xSPOT_COMMON:
		pBaseSpot = new xCommon;
		break;
	default:
		XBREAKF( 1, "unknown spot type:type=%d", type );
		break;
	}
	return pBaseSpot;
}

void XPropWorld::AddSpot( xBASESPOT *pBaseSpot )
{
	XBREAK( pBaseSpot == nullptr );
	XBREAK( pBaseSpot->type == XGAME::xSPOT_NONE );
	_m_listSpots.Add( pBaseSpot );
	m_mapSpots[ pBaseSpot->idSpot ] = pBaseSpot;
	++m_numSpot[ pBaseSpot->type ];
	if( pBaseSpot->strIdentifier.empty() == false ) {
		if( GetpProp( pBaseSpot->strIdentifier ) )
			XALERT("propWorld: 중복된 스팟 식별자=%s", pBaseSpot->strIdentifier.c_str() );
		m_mapSpotsIds[ pBaseSpot->strIdentifier ] = pBaseSpot;
	}
	if( pBaseSpot->strWhen.empty() == false ) {
		AddWhenSpot( pBaseSpot->strWhen, pBaseSpot );
		// 캠페인 스팟은 when_del을 사용하지 않는다.
		if( pBaseSpot->GetType() != XGAME::xSPOT_CAMPAIGN ) {
			// strWhenDel퀘가 삭제될때 이스팟도 삭제됨.
			if( pBaseSpot->strWhenDel.empty() ) 		// when_del이 정의되지 않았으면 when을 삭제시점으로 함.
				AddWhenDelSpot( pBaseSpot->strWhen, pBaseSpot );
			else
				AddWhenDelSpot( pBaseSpot->strWhenDel, pBaseSpot );
		}
	}
	if( pBaseSpot->idCode ) {
		m_mapSpotsByCode[ pBaseSpot->idCode ].Add( pBaseSpot );
	}
}

#ifdef _xIN_TOOL
void XPropWorld::DestroySpot( ID idSpot )
{
	auto pPropSpot = GetpProp( idSpot );
	if( XASSERT(pPropSpot) ) {
		--m_numSpot[ pPropSpot->type ];
		// 우선 맵에 있는걸 모두 지운다.
		m_mapSpots.erase( idSpot );
		m_mapSpotsIds.erase( pPropSpot->strIdentifier );
		DestroySpotInMap( m_mapSpotsByWhen, idSpot );
		DestroySpotInMap( m_mapSpotsByWhenDel, idSpot );
		DestroySpotInMap( m_mapSpotsByCode, idSpot );
		bool bDeleted = false;
		// 리스트에서도 지우고 실제로 파괴.
		for( auto itor = _m_listSpots.begin(); itor != _m_listSpots.end(); ) {
			xBASESPOT *pProp = (*itor);
			if( pProp->idSpot == idSpot ) {
				XBREAK( bDeleted == true );		// 이미 같은 아이디를 삭제했는데 같은아이디가 또나옴.
				_m_listSpots.erase( itor++ );
				bDeleted = true;
				SAFE_DELETE( pProp );
	#ifndef _DEBUG
				break;
	#endif // not _DEBUG
			} else
				++itor;
		}
	}
}
#endif // _XIN_TOOL
/**
 @brief mapSpots에서 idSpot이 들어간 요소를 지운다.
*/
#ifdef _xIN_TOOL
void XPropWorld::DestroySpotInMap( 
										std::unordered_map<_tstring, XList4<xBASESPOT*>>& mapSpots
									, ID idSpot )
{
	bool bDeleted = false;
	// 모든맵에 들어있는 리스트를 다 검색해서 idSpot을 찾아낸다.
	for( auto itor : m_mapSpotsByWhen ) {
		XList4<xBASESPOT*>& rlist = itor.second;
		for( auto itor = rlist.begin(); itor != rlist.end(); ) {
			xBASESPOT *pProp = (*itor);
			if( pProp->idSpot == idSpot ) {
				XBREAK( bDeleted == true );	// 같은게 리스트에 두개 있었다는 뜻임.
				bDeleted = true;
				rlist.erase( itor++ );
				// 이함수는 보통 툴에서 사용되므로 안정성을 위해서 그냥 모든 맵을 다 검사해보자.
			} else
				++itor;
		}
	}
}
#endif // _xIN_TOOL
/**
 @brief 타입이 다른 버전.
*/
#ifdef _xIN_TOOL
void XPropWorld::DestroySpotInMap(
													std::unordered_map<ID, XList4<xBASESPOT*>>& mapSpots
												, ID idSpot )
{
	bool bDeleted = false;
	// 모든맵에 들어있는 리스트를 다 검색해서 idSpot을 찾아낸다.
	for( auto itor : m_mapSpotsByWhen ) {
		auto& rlist = itor.second;
		for( auto itor = rlist.begin(); itor != rlist.end(); ) {
			xBASESPOT *pProp = ( *itor );
			if( pProp->idSpot == idSpot ) {
				XBREAK( bDeleted == true );	// 같은게 리스트에 두개 있었다는 뜻임.
				bDeleted = true;
				rlist.erase( itor++ );
				// 이함수는 보통 툴에서 사용되므로 안정성을 위해서 그냥 모든 맵을 다 검사해보자.
			}
			else
				++itor;
		}
	}
}
#endif // _xIN_TOOL

/**
 @brief 리스트에서만 제거하고 메모리를 파괴하지 않는다.
*/
#ifdef _xIN_TOOL
XPropWorld::xBASESPOT* XPropWorld::DelSpot( ID idSpot )
{
	auto pPropSpot = GetpProp( idSpot );
	if( XASSERT( pPropSpot ) ) {
		// 우선 맵에 있는걸 모두 지운다.
		m_mapSpots.erase( idSpot );
		m_mapSpotsIds.erase( pPropSpot->strIdentifier );
		DestroySpotInMap( m_mapSpotsByWhen, idSpot );
		DestroySpotInMap( m_mapSpotsByWhenDel, idSpot );
		DestroySpotInMap( m_mapSpotsByCode, idSpot );
		for( auto itor = _m_listSpots.begin(); itor != _m_listSpots.end(); ) {
			xBASESPOT *pProp = ( *itor );
			if( pProp->idSpot == idSpot ) {
				_m_listSpots.erase( itor++ );
				return pProp;
			} else
				++itor;
		}
	}
	return nullptr;
}
#endif // _xIN_TOOL

#ifdef _xIN_TOOL
void XPropWorld::CopyProp( xBASESPOT *pBasePropDst, xBASESPOT *pBasePropSrc ) 
{
	auto type = pBasePropSrc->type;
	switch( type ) {
	case XGAME::xSPOT_CASTLE:
		CopyPropTemplate<xCASTLE*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_JEWEL:
		CopyPropTemplate<xJEWEL*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_SULFUR:
		CopyPropTemplate<xSULFUR*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_MANDRAKE:
		CopyPropTemplate<xMANDRAKE*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_NPC:
		CopyPropTemplate<xNPC*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_DAILY:
		CopyPropTemplate<xDaily*>( pBasePropDst, pBasePropSrc );
		break;
// 	case XGAME::xSPOT_SPECIAL:
// 		CopyPropTemplate<xSpecial*>( pBasePropDst, pBasePropSrc );
// 		break;
	case XGAME::xSPOT_CAMPAIGN:
		CopyPropTemplate<xCampaign*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_VISIT:
		CopyPropTemplate<xVisit*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_CASH:
		CopyPropTemplate<xCash*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_GUILD_RAID:
		CopyPropTemplate<xGuildRaid*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_PRIVATE_RAID:
		CopyPropTemplate<xPrivateRaid*>( pBasePropDst, pBasePropSrc );
		break;
	case XGAME::xSPOT_COMMON:
		CopyPropTemplate<xCommon*>( pBasePropDst, pBasePropSrc );
		break;
	default:
		XBREAK(1);
		break;
	}
}
#endif // _xIN_TOOL

XPropWorld::xBASESPOT* XPropWorld::GetpProp( ID idProp ) 
{
	auto itor = m_mapSpots.find( idProp );
	if( itor == m_mapSpots.end() )
		return nullptr;
	auto pProp = itor->second;
	XBREAK( pProp == nullptr );
	return pProp;
//	 	for( auto pProp : m_listSpots ) {
// 		if( pProp->idSpot == idProp )
// 			return pProp;
// 	}
}
XPropWorld::xBASESPOT* XPropWorld::GetpProp( const _tstring& strIdentifier ) 
{
	auto itor = m_mapSpotsIds.find( strIdentifier );
	if( itor == m_mapSpotsIds.end() )
		return nullptr;
	xBASESPOT *pProp = itor->second;
	XBREAK( pProp == nullptr );
	return pProp;
}

void XPropWorld::Serialize( XArchive& ar ) const
{
	ar << VER_PROP_WORLD;
	ar << s_idGlobal << s_ratioProduce << s_ratioCastleProduce << s_ratioRegen;
	ar << m_strIdentifier << m_strImg;
	ar << (int)_m_listSpots.size();
	for( auto pProp : _m_listSpots ) {
		ar << pProp->idSpot;
		ar << (int)pProp->type;
		const xtSpot type = pProp->type;
		switch( type ) {
		case xSPOT_CASTLE: {
			auto pSpot = static_cast<xCASTLE*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_JEWEL: {
			auto pSpot = static_cast<xJEWEL*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_SULFUR: {
			auto pSpot = static_cast<xSULFUR*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_MANDRAKE: {
			auto pSpot = static_cast<xMANDRAKE*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_NPC: {
			auto pSpot = static_cast<xNPC*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_DAILY: {
			auto pSpot = static_cast<xDaily*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_CAMPAIGN: {
			auto pSpot = static_cast<xCampaign*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_VISIT: {
			auto pSpot = static_cast<xVisit*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_CASH: {
			auto pSpot = static_cast<xCash*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_GUILD_RAID: {
			auto pSpot = static_cast<xGuildRaid*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_PRIVATE_RAID: {
			auto pSpot = static_cast<xPrivateRaid*>( pProp );
			pSpot->Serialize( ar );
		} break;
		case xSPOT_COMMON: {
			auto pSpot = static_cast<xCommon*>( pProp );
			pSpot->Serialize( ar );
		} break;

		default:
			XBREAK(1);
			break;
		}
	}
}

void XPropWorld::DeSerialize( XArchive& ar, int )
{
//	AXLOGXN( "1:%s", __TFUNC__ );
	int ver;
	ar >> ver;
	// 스팟프로퍼티는 참조하고있는곳이 많으니 메모리를 재할당하지 않고 동기화 시킨다.
	ar >> s_idGlobal >> s_ratioProduce >> s_ratioCastleProduce >> s_ratioRegen;
	ar >> m_strIdentifier >> m_strImg;
	int num;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		ID idSpot;
		xtSpot type;
		int i0;
		ar >> idSpot;
		ar >> i0;		type = (xtSpot)i0;
// 		CONSOLE("i:%d type:%d", i, type);
// 		XBREAKF( XGAME::IsInvalidSpotType( type ), "i=%d idSpot=%d type=%d ver=%d idglobal=%d ratioproduce=%f ratiocastle=%f ratioregen=%f ids=%s strImg=%s num=%d"
// 																			, i, idSpot, type, ver, s_idGlobal, s_ratioProduce, s_ratioCastleProduce, s_ratioRegen
// 																			, m_strIdentifier.c_str(), m_strImg.c_str(), num );
		auto pProp = GetpProp( idSpot );
		if( pProp == nullptr ) {
			// 스팟이 없음.
			pProp = sCreateSpot( type );
		}
		if( XASSERT( pProp ) ) {
			switch( type ) {
			case xSPOT_CASTLE: {
				auto pPropSpot = static_cast<xCASTLE*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_JEWEL: {
				auto pPropSpot = static_cast<xJEWEL*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_SULFUR: {
				auto pPropSpot = static_cast<xSULFUR*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_MANDRAKE: {
				auto pPropSpot = static_cast<xMANDRAKE*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_NPC: {
				auto pPropSpot = static_cast<xNPC*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_DAILY: {
				auto pPropSpot = static_cast<xDaily*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_CAMPAIGN: {
				auto pPropSpot = static_cast<xCampaign*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_VISIT: {
				auto pPropSpot = static_cast<xVisit*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_CASH: {
				auto pPropSpot = static_cast<xCash*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_GUILD_RAID: {
				auto pPropSpot = static_cast<xGuildRaid*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_PRIVATE_RAID: {
				auto pPropSpot = static_cast<xPrivateRaid*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;
			case xSPOT_COMMON: {
				auto pPropSpot = static_cast<xCommon*>( pProp );
				pPropSpot->DeSerialize( ar, 0 );
				AddSpot( pPropSpot );
			} break;

			default:
				XBREAKF( 1, "unknown spot type:type=%d idspot=%d", type, idSpot );
				break;
			} // switch
		}
	} // for
//	AXLOGXN( "2:%s", __TFUNC__ );
}
