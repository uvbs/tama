#include "stdafx.h"
#include "XPropCamp.h"
#include "XPropHero.h"
#include "XPropLegionH.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xCampaign;

ID XPropCamp::s_Global = 0;
static XVector<XPropHero::xPROP*> s_aryHeroForCamp;			// 영웅캠페인을 위해 선별되고 소트된 리스트

/**
 @brief 패킷최적화용 인듯? 아마도 플레이중 스테이지정보 개별적으로 보내줄때 사용하는것인듯.
*/
int XPropCamp::xStage::SerializeMin( XArchive& ar ) const {
	XBREAK( ar.IsForDB() );	// 
	XBREAK( idProp == 0 );
	XBREAK( idxStage > 0x7f || idxStage < 0 );
	ar << idProp;
	ar << (BYTE)maxWin;
	ar << (BYTE)maxTry;
	ar << (BYTE)levelLimit;
	ar << (char)idxStage;
	ar << aryReward;
	ar << legion;
	MAKE_CHECKSUM( ar );
	return 1;
}
int XPropCamp::xStage::DeSerializeMin( XArchive& ar, int ver ) {
	BYTE b0;
	char c0;
	ar >> idProp;
	XBREAK( idProp == 0 );
	ar >> b0;	maxWin = b0;
	ar >> b0;	maxTry = b0;
	ar >> b0;	levelLimit = b0;
	ar >> c0;	idxStage = c0;
	XBREAK( idxStage < 0 );
	ar >> aryReward;
	ar >> legion;
	RESTORE_VERIFY_CHECKSUM( ar );
	return 1;
}

/**
 @brief 풀버전 아카이빙
*/
int XPropCamp::xStage::Serialize( XArchive& ar ) const {
	XBREAK( ar.IsForDB() );	// 
	XBREAK( idProp == 0 );
	XBREAK( idxStage > 0x7f  );
	ar << idProp;
	ar << (char)maxWin;
	ar << (char)maxTry;
	ar << (char)levelLimit;
	ar << (char)idxStage;
	ar << sidDropItem;
	ar << rateDrop;
	ar << aryReward;
	ar << legion;
	return 1;
}
int XPropCamp::xStage::DeSerialize( XArchive& ar, int ver ) {
	char c0;
	ar >> idProp;
	XBREAK( idProp == 0 );
	ar >> c0;     maxWin = c0;
	ar >> c0;     maxTry = c0;
	ar >> c0;     levelLimit = c0;
	ar >> c0;     idxStage = c0;
	ar >> sidDropItem;
	ar >> rateDrop;
	ar >> aryReward;
	ar >> legion;
	return 1;
}

int XPropCamp::xStageHero::Serialize( XArchive& ar ) const {
	xStage::Serialize( ar );
	ar << m_idHero;
// 	ar << (short)m_lvLegion;
// 	ar << (short)0;
	return 1;
}
int XPropCamp::xStageHero::DeSerialize( XArchive& ar, int ver ) {
	xStage::DeSerialize( ar, ver );
//	short s0;
	ar >> m_idHero;
// 	ar >> s0;		m_lvLegion = s0;
// 	ar >> s0;
	return 1;
}

/////////////////////////////////////////////////////////////////
XPropCamp* XPropCamp::sGet() 
{
	if( s_pInstance == nullptr )
		s_pInstance = new XPropCamp;
	return s_pInstance;
}

int XPropCamp::xProp::Serialize( XArchive& ar ) const {
	XBREAK( !m_Type );
	ar << idProp;
	ar << strIdentifier;
	ar << idName;
	ar << m_strtXml;
	ar << m_strcLayout;
	ar << reward << numTry << secResetCycle;
	ar << (char)xboolToByte(bAutoReset);
	ar << (char)0;
	ar << (char)0;
	ar << (char)0;
	ar << (int)aryStages.size();
//	XARRAYLINEAR_LOOP_AUTO( aryStages, spPropStage ) {
	for( auto spPropStage : aryStages ) {
		spPropStage->Serialize( ar );
	}
	return 1;
}

int XPropCamp::xProp::DeSerialize( XArchive& ar, int ver ) {
	XBREAK( !m_Type );
	aryStages.Clear();
	BYTE b0;
	ar >> idProp;
	ar >> strIdentifier;
	ar >> idName;
	ar >> m_strtXml;
	_tstring strtLayout;
	ar >> strtLayout;
	m_strcLayout = SZ2C( strtLayout);
	ar >> reward >> numTry >> secResetCycle;
	ar >> b0;		bAutoReset = xbyteToBool( b0 );
	ar >> b0 >> b0 >> b0;
	int size;
	ar >> size;
	for( int i = 0; i < size; ++i ) {
//		auto spStage = XSPPropStage( new xStage );
		auto spStage = sCreatePropStage( m_Type );
		spStage->DeSerialize( ar, ver );
		aryStages.Add( spStage );
	}
	return 1;
}

/**
 @brief 이전버전의 DB저장용?
*/
int XPropCamp::xProp::DeSerializeMin( XPropCamp *pPropCamp, XArchive& ar, int ver ) {
	XBREAK( m_Type == xCT_HERO2 );			// 신버전에선 이건 안씀.
	ar >> idProp;
	ar >> idName;
	int size;
	ar >> size;
	for( int i = 0; i < size; ++i ) {
		auto spStage =  XSPPropStage( new xStage );
		spStage->DeSerializeMin( ar, ver );
		aryStages.Add( spStage );
		pPropCamp->AddStage( spStage->idProp, spStage );
	}
	return 1;
}
/**
 @brief idStage로 검색해서 스테이지프로퍼티를 얻는다.
*/
std::shared_ptr<XPropCamp::xStage> 
XPropCamp::xProp::GetspStageByidStage( ID idStage ) 
{
	if( XASSERT( idStage ) ) {
		for( auto spPropStage : aryStages ) {
			if( spPropStage->idProp == idStage )
				return spPropStage;
		}
	}
	return std::shared_ptr<XPropCamp::xStage>();
}

int XPropCamp::xPropHero::Serialize( XArchive& ar ) const
{
	xProp::Serialize( ar );
	ar << (char)m_maxFloor;
	ar << (char)0;
	ar << (short)0;
	return 1;
}
int XPropCamp::xPropHero::DeSerialize( XArchive& ar, int ver )
{
	xProp::DeSerialize( ar, ver );
	char c0;
	ar >> c0;		m_maxFloor = c0;
	ar >> c0 >> c0 >> c0;
	return 1;
}

//////////////////////////////////////////////////////////////////////////
/**
 @brief 영웅식별자로 attr을 읽어 아이디로 돌려준다.
*/
ID XPropCamp::sGetHeroIds( XPropCamp *pThis, XEXmlNode& node, const char *cKey, xProp *pCampProp )
{
	_tstring strIdsHero = C2SZ( node.GetString( cKey ) );
	if( strIdsHero.empty() == false )
	{
		auto pProp = PROP_HERO->GetpProp( strIdsHero );
		if( pProp )
			return pProp->idProp;
		else
		{
			CONSOLE( "%s:%s라는 영웅은 없다.", pCampProp->strIdentifier.c_str(), strIdsHero.c_str() );
			pThis->m_bError = true;
		}
	}
	return 0;
}

XGAME::xtUnit XPropCamp::sGetUnitIds( XPropCamp *pThis, XEXmlNode& node, const char *cKey, xProp *pCampProp )
{
	_tstring strIdsUnit = C2SZ( node.GetString( cKey ) );
	if( strIdsUnit.empty() == false )
	{
		DWORD val = CONSTANT->GetValue( strIdsUnit );
		if( CONSTANT->IsFound() )
		{
			return (XGAME::xtUnit) val;
		} else
		{
			CONSOLE( "%s:%s라는 유닛은 없다.", pCampProp->strIdentifier.c_str(), strIdsUnit.c_str() );
			pThis->m_bError = true;
		}
	}
	return XGAME::xUNIT_NONE;
}

XPropCamp::xProp* XPropCamp::sCreatePropCamp( xtType typeCamp )
{
	switch( typeCamp ) {
#ifdef _XCAMP_HERO2
	case xCT_NORMAL:	return new xProp( xCT_NORMAL );
#else
	case xCT_NONE:
	case xCT_NORMAL:	return new xProp( xCT_NORMAL );
#endif // _XCAMP_HERO2
	case xCT_HERO:		return new xProp( xCT_HERO );
	case xCT_HERO2:		return new xPropHero();
	default:
		XBREAK(1);
		break;
	}
	return nullptr;
}

XSPPropStage XPropCamp::sCreatePropStage( xtType typeCamp )
{
	if( typeCamp == xCT_HERO2 )
		return std::make_shared<xStageHero>();
	return std::make_shared<xStage>();
}

//////////////////////////////////////////////////////////////////////////
XPropCamp* XPropCamp::s_pInstance = nullptr;
////////////////////////////////////////////////////////////////
XPropCamp::XPropCamp()
{
	Init();
}

void XPropCamp::Destroy()
{
	XVECTOR_DESTROY( m_aryCamp );
	m_mapCamp.clear();
	m_mapCampStr.clear();
// 	for( auto& itor : m_mapCamp ) {
// 		SAFE_DELETE( itor.second );
// 	}
}

/**
 @brief 캠페인.xml파일은 읽어 메모리에 올린다.
 캠페인 xml은 여러개로 분리될수 있으므로 Load()도 여러번 불릴 수 있다.
 보통 시나리오 단위로 분리하면 적당하다.
*/
BOOL XPropCamp::Load( LPCTSTR szXml )
{
	return XXMLDoc::Load( XE::MakePath(DIR_PROP, szXml) );
}

bool CompareProp( XPropHero::xPROP *pProp1, XPropHero::xPROP *pProp2 )
{
	return ( pProp1->priority < pProp2->priority );
}

bool XPropCamp::OnDidFinishLoad()
{
	XEXmlNode nodeRoot = FindNode("root");
	if( nodeRoot.IsEmpty() )
		return false;
	m_Ver = nodeRoot.GetInt( "ver" );
	///< 
	XEXmlNode nodeCamp = nodeRoot.GetFirst();
	if( XBREAK( nodeCamp.IsEmpty() ) )
		return false;
	// 영웅던전을 위한 목록작성
	s_aryHeroForCamp.clear();
	PROP_HERO->GetpPropByGetToAry( &s_aryHeroForCamp, XGAME::xGET_GATHA );
	PROP_HERO->GetpPropByGetToAry( &s_aryHeroForCamp, XGAME::xGET_QUEST );
	std::sort( s_aryHeroForCamp.begin(), s_aryHeroForCamp.end(), CompareProp );

	while( !nodeCamp.IsEmpty() ) {
		//
		auto pProp = LoadCamp( nodeCamp );
		///< 
		if( IsExist( pProp ) ) {
			XALERT( "중복된 캠페인 식별자:%s", pProp->strIdentifier.c_str() );
		} else {
			m_aryCamp.Add( pProp );
			m_mapCamp[pProp->idProp] = pProp;
			m_mapCampStr[pProp->strIdentifier] = pProp;
		}
//		aryCamps.Add( pNewProp );
		// 다음 캠페인 노드로
		nodeCamp = nodeCamp.GetNext();
	}
	if( m_bError ) {
		XALERT("%s:캠페인 프로퍼티 읽던 중 에러. 콘솔창을 참고하시오.", GetstrFilename().c_str() );
	}
// 	m_aryCamp = aryCamps;
	s_aryHeroForCamp.clear();		// 모두 사용후엔 클리어.
	return !m_bError;
}

/**
 @brief 기본형 attr들을 읽는다.
*/
void XPropCamp::GetBaseAttr( XEXmlNode& nodeCamp, xProp* pOutProp )
{
	const std::string strIdsCamp = nodeCamp.GetcstrName();
	pOutProp->idProp = XPropCamp::sGetIdsToId( strIdsCamp );
	pOutProp->strIdentifier = C2SZ( strIdsCamp.c_str() );
	pOutProp->idName = nodeCamp.GetInt( "name" );
	pOutProp->reward = nodeCamp.GetInt( "reward" );
	pOutProp->m_numStage = nodeCamp.GetInt( "num_stage" );
	pOutProp->m_Debug = nodeCamp.GetInt( "debug" );
	pOutProp->numTry = nodeCamp.GetInt( "num_try" );
	pOutProp->m_strtXml = nodeCamp.GetTString( "xml" );
	pOutProp->m_strcLayout = nodeCamp.GetString( "layout" );
	int h = nodeCamp.GetInt( "hour_reset" );
	pOutProp->secResetCycle = xHOUR_TO_SEC( h );
	pOutProp->bAutoReset = nodeCamp.GetInt( "auto_reset" ) != 0;
	XBREAKF( pOutProp->m_numStage > XGAME::MAX_STAGE_IN_CAMP
		, "%s:num_stage값이 너무 높습니다. < %d %s", pOutProp->strIdentifier.c_str()
																						, XGAME::MAX_STAGE_IN_CAMP, nodeCamp.GetstrName().c_str() );
}
/**
 @brief 캠페인블럭(캠페인식별자)을 읽는다.
*/
XPropCamp::xProp* XPropCamp::LoadCamp( XEXmlNode& nodeCamp )
{
	const std::string strcType = nodeCamp.GetString( "type" );
	xtType typeCamp = xCT_NONE;
	if( strcType == "camp_hero2" ) {
		typeCamp = xCT_HERO2;
	} else 
	if( strcType == "camp_hero" || nodeCamp.GetcstrName() == "camp.hero" ) {		// 구버전 호환용
		typeCamp = xCT_HERO;
	}	else {
		typeCamp = xCT_NORMAL;
	}
	// 캠페인 프로퍼티 생성
	xProp* pPropCamp = sCreatePropCamp( typeCamp );
	// 캠페인 기본속성 읽음.
	GetBaseAttr( nodeCamp, pPropCamp );
	if( typeCamp == xCT_HERO2 ) {
		// 영웅캠펜 전용속성 읽음.
		auto pPropCampHero = static_cast<xPropHero*>( pPropCamp );
		LoadCampHero( nodeCamp, pPropCampHero );
//		pPropCampHero->m_maxStage = s_aryHeroForCamp.size();

	}
// 	std::string strIdsCamp = nodeCamp.GetcstrName();
// 	pProp->idProp = XPropCamp::sGetIdsToId( strIdsCamp );
// 	pProp->strIdentifier = C2SZ( strIdsCamp.c_str() );
// 	pProp->idName = nodeCamp.GetInt( "name" );
// 	pProp->reward = nodeCamp.GetInt( "reward" );
// 	int numStages = nodeCamp.GetInt( "num_stage" );
// 	int debugCode = nodeCamp.GetInt( "debug" );
// 	pProp->numTry = nodeCamp.GetInt( "num_try" );
// 	pProp->m_strtXml = nodeCamp.GetTString( "xml" );
// 	pProp->m_strcLayout = nodeCamp.GetString( "layout" );
// 	int h = nodeCamp.GetInt( "hour_reset" );
// 	pProp->secResetCycle = xHOUR_TO_SEC( h );
// 	pProp->bAutoReset = nodeCamp.GetInt( "auto_reset" ) != 0;
	XVector<XSPPropStage> aryStages( XGAME::MAX_STAGE_IN_CAMP );
	int numStages = pPropCamp->m_numStage;
	// numStage값을 생략하면 "stage"블럭 개수가 numStages가 된다.
	XEXmlNode nodeStage = nodeCamp.GetFirst();
	int idxStage = 0;
	int maxStageIdx = -1;
	// "stage"블럭이 있다면 읽는다.
	while( !nodeStage.IsEmpty() ) {
		if( nodeStage.GetcstrName() == "stage" ) {
//			auto spStage = XSPPropStage( new xStage );
			auto spStage = sCreatePropStage( typeCamp );
			spStage->idxStage = idxStage;   // 디폴트값.
			// 스테이지 기본값을 읽음.
			LoadStage( nodeStage, spStage, pPropCamp );
			if( typeCamp == xCT_HERO2 ) {
				// 영웅캠페인용 스테이지 전용 속성 읽음.
			}
			// 스테이지 인덱스를 직접 지정했다면 현재 인덱스를 그값으로 바꿔넣는다.
			if( spStage->idxStage >= 0 ) {
				idxStage = spStage->idxStage;
			}
			if( idxStage > maxStageIdx )
				maxStageIdx = idxStage;
			if( numStages > 0 )
				XBREAKF( idxStage >= numStages, "%s:너무큰 스테이지 idx(%d). %s", pPropCamp->strIdentifier.c_str(), idxStage, nodeStage.GetstrName().c_str() );
			XBREAKF( aryStages[ idxStage ] != nullptr, "%s:중복된 스테이지 idx(%d). %s", pPropCamp->strIdentifier.c_str(), idxStage, nodeStage.GetstrName().c_str() );
			aryStages[ idxStage ] = spStage;
			AddStage( idxStage+1, spStage );	// 이제 스테이지아이디는 캠페인내에서만 고유하도록 바뀐다. propCamp에서 캠페인 순서만 바껴도 아이디가 바껴버려서 바꿈.
			++idxStage;
		} else {
			XBREAKF( 1, "%s:알수없는 조건이름. %s", pPropCamp->strIdentifier.c_str(), nodeStage.GetstrName().c_str() );
		}
		//
		nodeStage = nodeStage.GetNext();
	}
	if( numStages == 0 ) {
		numStages = maxStageIdx + 1;
		XBREAK( numStages <= 0 );
	} else
	if( numStages == -1 ) {
		// 스테이지는 런타임중 다이나믹으로 생성
		if( typeCamp == xCT_HERO2 ) {
			// 영웅캠펜2는 미리 스테이지를 만들어둔다.
			numStages = s_aryHeroForCamp.Size();
		}
	}
	// 직접 인덱스를 지정한곳외에는 디폴트 값으로 채워넣는다.
	idxStage = 0;
	if( numStages > 0 ) {
		for( int i = 0; i < numStages; ++i ) {
			auto spStage = aryStages[ i ];
			if( spStage == nullptr ) {
//				spStage = XSPPropStage( new xStage );
				spStage = sCreatePropStage( typeCamp );
				spStage->idxStage = i;
				if( typeCamp == xCT_HERO2 ) {
					auto spStageHero = std::static_pointer_cast<xStageHero>( spStage );
					XASSERT( numStages == s_aryHeroForCamp.Size() );
					XBREAK( i >= s_aryHeroForCamp.Size() );
					spStageHero->m_idHero = s_aryHeroForCamp[ i ]->idProp;
					XGAME::xReward reward;
					reward.SetItem( XPropItem::sGetidHeroToidItemSoul( spStageHero->m_idHero ) );
					spStageHero->aryReward.push_back( reward );		// 드랍할 영혼석을 미리 설정.
					spStageHero->maxTry = 0;		// 도전가능한 횟수는 무한대
					spStageHero->maxWin = 2;		// 승리가능한 횟수는 2번
//					spStageHero->m_lvLegion = sGetlvLegionByidxHero( i, numStages );
					auto pPropCampHero = SafeCast<xPropHero*>( pPropCamp );
//					spStageHero->legion.lvLegion = sGetlvLegionByidxHero( i, numStages );
// 					XBREAK( spStageHero->legion.lvLegion <= 0 
// 							|| spStageHero->legion.lvLegion > XGAME::MAX_NPC_LEVEL * 2 );		// 
				}
				aryStages[ i ] = spStage;
				AddStage( i+1, spStage );
			}
			XBREAK( aryStages[ i ] == nullptr );
			pPropCamp->aryStages.Add( spStage );
		}
	}
	// 검증
	int size = pPropCamp->aryStages.size();
	for( int i = 0; i < size-1; ++i ) {
		for( int k = i+1; k < size; ++k ) {
			if( pPropCamp->aryStages[i]->idProp == pPropCamp->aryStages[k]->idProp ) {
				XBREAKF( 1, "%s:중복된 스테이지 아이디:id=%d idx=%d, idx2=%d", pPropCamp->strIdentifier.c_str(),
																			pPropCamp->aryStages[i]->idProp,
																			i, k );
			}
		}
	}
// 	///< 
// 	if( IsExist(pProp) ) {
// 		XALERT("중복된 캠페인 식별자:%s", pProp->strIdentifier.c_str() );
// 	} else {
// 		m_mapCamp[ pProp->idProp ] = pProp;
// 		m_mapCampStr[ pProp->strIdentifier ] = pProp;
// 	}
	return pPropCamp;
}

/**
 @brief 영웅캠페인 전용 속성들을 읽음.
*/
void XPropCamp::LoadCampHero( XEXmlNode& nodeCamp, xPropHero* pOutProp )
{
	pOutProp->m_maxFloor = nodeCamp.GetInt( "max_floor" );
}

/**
 @brief pProp과 같은 아이디나 식별자가 있는 퀘가 있는지 검사.
*/
bool XPropCamp::IsExist( xProp *pProp )
{
	if( XBREAK(pProp == nullptr) )
		return false;
	{
		// 같은 식별자를 가진 캠페인가 있는지 찾는다.
		auto itor = m_mapCampStr.find( pProp->strIdentifier );
		if( itor != m_mapCampStr.end() )
			return true;
	}
	// 같은 아이디의 캠페인가 있는지 찾는다
	auto itor = m_mapCamp.find( pProp->idProp );
	if( itor != m_mapCamp.end() )
		return true;
	return false;
}

void XPropCamp::LoadStage( XEXmlNode& nodeStage, XSPPropStage& spStage, xProp *pCampProp )
{
	XBREAK( spStage->idxStage < 0 );
	XEXmlAttr attr = nodeStage.GetFirstAttribute();

	while( !attr.IsEmpty() )
	{
		if( attr.GetcstrName() == "num_win" )
			spStage->maxWin = attr.GetInt();
		else
		if( attr.GetcstrName() == "num_try" )
			spStage->maxTry = attr.GetInt();
		else
		if( attr.GetcstrName() == "elite" )
			if( m_Ver >= 1 ) {
				spStage->legion.gradeLegion = (XGAME::xtGradeLegion)(attr.GetInt() + 1);
				XBREAK( spStage->legion.gradeLegion <= XGAME::xGL_NONE || spStage->legion.gradeLegion >= XGAME::xGL_MAX );
			}
			else
				spStage->legion.gradeLegion = (attr.GetInt() == 1 ) ? XGAME::xGL_ELITE : XGAME::xGL_NORMAL;

		else
		if( attr.GetcstrName() == "limit_level" )
			spStage->levelLimit = attr.GetInt();
		else
		if( attr.GetcstrName() == "point" ) {
//			spStage->point = attr.GetInt();
//			spStage->point = 40;	// 일단 40으로 고정
		} else
		if( attr.GetcstrName() == "drop_item" ) {
			spStage->sidDropItem = attr.GetTString();
		} else
		if( attr.GetcstrName() == "idx" )
			spStage->idxStage = attr.GetInt() - 1;    // 직관적으로 하려고 스테이지번호가 인덱스.
		else
		if( attr.GetcstrName() == "drop_rate" ) {
			float f = attr.GetFloat();
			spStage->rateDrop = f / 100.f;
		} else {
			CONSOLE( "%s:알수없는 속성이름. %s", pCampProp->strIdentifier.c_str(), attr.GetstrName().c_str() );
			m_bError = true;
		}
		attr = attr.GetNext();
	}
	if( spStage->sidDropItem.empty() == false && spStage->rateDrop == 0 )
		XALERT( "경고: 드랍확률이 지정되어 있지 않음. 캠페인%s:%d stage",
				pCampProp->strIdentifier.c_str(), spStage->idxStage+1 );
	//
	XEXmlNode nodeLegion = nodeStage.FindNode("legion");
	if( !nodeLegion.IsEmpty() ) {
		//LoadLegion( nodeLegion, spStage, pCampProp );
		spStage->legion.LoadFromXML( nodeLegion, pCampProp->strIdentifier.c_str() );
	} else {
		CONSOLE("%s:경고: legion블럭이 없음.", pCampProp->strIdentifier.c_str() );
		m_bError = true;
	}
	auto node = nodeStage.GetFirst();
	while( !node.IsEmpty() ) {
		if( node.GetcstrName() == "reward" ) {
			XGAME::xReward reward;
			reward.LoadFromXML( node, pCampProp->strIdentifier.c_str() );
			spStage->aryReward.push_back( reward );
		}
		node = node.GetNext();
	}
	//
} // LoadStage

void XPropCamp::AddStage( ID idStage, XSPPropStage& spStage )
{
	spStage->idProp = idStage;
}

void XPropCamp::Serialize( XArchive& ar ) const
{
	ar << VER_CAMP_SERIALIZE;
	ar << s_Global;
	ar << (short)m_aryCamp.size();
	ar << (char)0;
	ar << (char)0;
	for( auto pPropCamp : m_aryCamp ) {
		ar << (char)pPropCamp->m_Type;
		ar << (char)0;
		ar << (short)0;
		pPropCamp->Serialize( ar );
	}
}
void XPropCamp::DeSerialize( XArchive& ar, int )
{
	Destroy();	
	//
	int ver, num;
	char c0;
	ar >> ver >> s_Global;
	if( ver >= 18 ) {
		ar >> num;
	} else {
		ar >> c0;	num = c0;
		ar >> c0 >> c0 >> c0;
	}
	for( int i = 0; i < num; ++i ) {
//		auto pPropCamp = new xProp();
		xtType typeCamp;
		if( ver >= 18 ) {
			ar >> c0;		typeCamp = (xtType)c0;
			ar >> c0 >> c0 >> c0;
		} else {
			typeCamp = xCT_NORMAL;
		}
		XBREAK( typeCamp == xCT_NONE );
		auto pPropCamp = sCreatePropCamp( typeCamp );
		pPropCamp->m_Type = typeCamp;
		pPropCamp->DeSerialize( ar, ver );
		m_aryCamp.Add( pPropCamp );
		m_mapCamp[ pPropCamp->idProp ] = pPropCamp;
		m_mapCampStr[ pPropCamp->strIdentifier ] = pPropCamp;
		
	}
}

/**
 @brief 해당스테이지의 군단레벨을 계산한다.
*/
int XPropCamp::sGetlvLegionByidxHero( int idxStage, int maxStage )
{
	const float lvStart = 12.f;	// 1스테이지의 레벨
	const float lvLastStage = 55.f;	// 마지막스테이지의 레벨
	return (int)(lvStart + (((lvLastStage - lvStart) / maxStage) * idxStage));
}

