#include "stdafx.h"
#include "XQuestProp.h"
#include "XPropItem.h"
#include "XPropHero.h"
#include "XPropCloud.h"
#include "XPropWorld.h"
#include "XProps.h"
#include "XGlobalConst.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define TAG		pQuestProp->strIdentifier.c_str()
using namespace XGAME;

XQuestProp::xProp::~xProp() {
// 			XARRAYLINEAR_DESTROY( aryWhen );
	XVECTOR_DESTROY( aryWhen );
	XVECTOR_DESTROY( aryConds );
	XVECTOR_DESTROY( aryReward );
// 			XARRAYLINEAR_DESTROY( aryReward );
}

int XQuestProp::xProp::GetExpReward( int level ) const {
	float adj = 1.f;
	if( aryConds.size() > 0 ) {
		// 정찰퀘나 방문퀘는 경험치를 1/3만 주게
		ID idWhat = aryConds[0]->idWhat;
		if( idWhat == XGAME::xQC_EVENT_RECON_SPOT ||
			idWhat == XGAME::xQC_EVENT_RECON_SPOT_TYPE ||
			idWhat == XGAME::xQC_EVENT_VISIT_SPOT ||
			idWhat == XGAME::xQC_EVENT_VISIT_SPOT_TYPE )
			adj = 0.3f;
	}
	//			float exp = (((float)level * (XGC->m_expBattle * 0.5f)) * ((float)_expReward / 100.f));
	float exp = ( ( (float)level * ( XGC->m_expBattle * 0.25f ) ) );
	exp *= adj;
	if( IsTutorial() )
		exp *= 10.f;
	return (int)( exp * rateExp );
}


//////////////////////////////////////////////////////////////////////////
XQuestProp* XQuestProp::s_pInstance = nullptr;
XQuestProp* XQuestProp::sGet() 
{
	if( s_pInstance == nullptr )
		s_pInstance = new XQuestProp;
	return s_pInstance;
}
////////////////////////////////////////////////////////////////
XQuestProp::XQuestProp()
{
	Init();
}

void XQuestProp::Destroy()
{
	XVECTOR_DESTROY( m_aryQuests );
	m_mapQuests.clear();
	m_mapQuestsStr.clear();
// 	for( auto& itor : m_mapQuests ) {
// 		SAFE_DELETE( itor.second );
// 	}
}

/**
 @brief 퀘스트.xml파일은 읽어 메모리에 올린다.
 퀘스트 xml은 여러개로 분리될수 있으므로 Load()도 여러번 불릴 수 있다.
 보통 시나리오 단위로 분리하면 적당하다.
*/
BOOL XQuestProp::Load( LPCTSTR szXml )
{
	return XXMLDoc::Load( XE::MakePath(DIR_PROP, szXml) );
}

bool XQuestProp::OnDidFinishLoad()
{
	XEXmlNode nodeRoot = FindNode("root");
	if( nodeRoot.IsEmpty() )
		return false;
	XEXmlNode nodeQuest = nodeRoot.GetFirst();
	if( nodeQuest.IsEmpty() )
		return false;
	while( !nodeQuest.IsEmpty() )	{
		auto pNewProp = LoadQuest( nodeQuest );
		m_aryQuests.push_back( pNewProp );
		// 다음 퀘스트 노드로
		nodeQuest = nodeQuest.GetNext();
	}
	if( m_bError ) {
		XALERT("%s:퀘스트 프로퍼티 읽던 중 에러. 콘솔창을 참고하시오.", GetstrFilename().c_str() );
		return false;
	}
	return true;
}

/**
 @brief 잘못된 퀘스트가 없는지 확인.
*/
bool XQuestProp::CheckValidQuest()
{
	// 퀘스트 식별자를 파라메터로 사용하는류의 조건의 경우 그 식별자가 유효한지 검사한다.
	for( auto pProp : m_aryQuests ) {
// 		XARRAYLINEAR_LOOP_AUTO( pProp->aryWhen, pWhen ) {
		for( auto pWhen : pProp->aryWhen ) {
			if( !pWhen->param.strParam.empty() ) {
				if( pWhen->idWhen == xQC_EVENT_END_SEQ ) {
				} else
				if( !IsValidQuest( pWhen->param.strParam ) ) {
					m_bError = true;
					CONSOLE( "%s 퀘스트는 존재하지 않습니다.", pWhen->param.strParam.c_str() );
				}
			}
		}
	}
	if( m_bError ) {
		XALERT( "퀘스트 프로퍼티 에러. 콘솔창을 참고하시오." );
		return false;
	}
	return true;
}

bool XQuestProp::IsValidQuest( const _tstring& ids )
{
	for( auto pProp : m_aryQuests ) {
		if( pProp->strIdentifier == ids )
			return true;
	}
	return false;
}
/**
 @brief 퀘스트블럭(퀘스트식별자)을 읽는다.
*/
XQuestProp::xProp* XQuestProp::LoadQuest( XEXmlNode& nodeQuest )
{
	xProp *pProp = new xProp;
	
// 	XArrayLinearN<xWhen*, 8> aryWhen;
// 	XArrayLinearN<XGAME::xReward*, 8> aryRewards;
// 	XArrayLinearN<xDrop, 8> aryDrops;

	std::string strIdsQuest = nodeQuest.GetcstrName();
	pProp->idProp = XQuestProp::sGetIdsToId( strIdsQuest );
	pProp->strIdentifier = C2SZ( strIdsQuest.c_str() );
	pProp->rateExp = nodeQuest.GetFloat("rate_exp");
	if( nodeQuest.IsNotFound() )
		pProp->rateExp = 1.f;
	XBREAK( pProp->rateExp <= 0.f );
	pProp->idName = nodeQuest.GetInt( "name" );
	pProp->idDesc = nodeQuest.GetInt( "desc" );
	pProp->strIcon = nodeQuest.GetTString("icon");
	XE::MakeExt( pProp->strIcon, _T(".png"), nullptr );

	XEXmlNode nodeCond = nodeQuest.GetFirst();
	while( !nodeCond.IsEmpty() ) {
		if( nodeCond.GetcstrName() == "when" ) {
			xWhen *pWhen = new xWhen;
			LoadWhen( nodeCond, pWhen, pProp );
			pProp->aryWhen.Add( pWhen );
		} else
		if( nodeCond.GetcstrName() == "cond" ) {
			xCond2 *pCond = new xCond2;
			LoadCond( nodeCond, pCond, pProp );
			pProp->aryConds.push_back( pCond );
		} else
		if( nodeCond.GetcstrName() == "reward" )
		{
			XGAME::xReward *pReward = new XGAME::xReward;
			pReward->LoadFromXML( nodeCond, pProp->strIdentifier.c_str() );
			pProp->aryReward.Add( pReward );
		} else
		if( nodeCond.GetcstrName() == "drop" )
		{
			xDrop drop;
			LoadDrops( nodeCond, &drop, pProp );
			pProp->aryDrops.Add( drop );
		} else
		if( nodeCond.GetcstrName() == "legion" )
		{
 			LoadLegion( nodeCond, pProp );
		} else
		{
			XBREAKF( 1, "%s:알수없는 조건이름. %s", pProp->strIdentifier.c_str(), nodeCond.GetstrName().c_str() );
			m_bError = true;
		}
		//
		nodeCond = nodeCond.GetNext();
	}
// 	pProp->aryWhen = aryWhen;
// 	pProp->aryReward = aryRewards;
// 	pProp->aryDrops = aryDrops;
	///< 
	if( IsExist(pProp) )
	{
		XALERT("중복된 퀘스트 식별자:%s", pProp->strIdentifier.c_str() );
	} else
	{
		m_mapQuests[ pProp->idProp ] = pProp;
		m_mapQuestsStr[ pProp->strIdentifier ] = pProp;
	}
	return pProp;
}

/**
 @brief pProp과 같은 아이디나 식별자가 있는 퀘가 있는지 검사.
*/
bool XQuestProp::IsExist( xProp *pProp )
{
	if( XBREAK(pProp == nullptr) )
		return false;
	{
		// 같은 식별자를 가진 퀘스트가 있는지 찾는다.
		auto itor = m_mapQuestsStr.find( pProp->strIdentifier );
		if( itor != m_mapQuestsStr.end() )
			return true;
	}
	// 같은 아이디의 퀘스트가 있는지 찾는다
	auto itor = m_mapQuests.find( pProp->idProp );
	if( itor != m_mapQuests.end() )
		return true;
	return false;
}

/**
 @brief "cond"블럭 읽음.
*/
void XQuestProp::LoadCond( XEXmlNode& nodeCondBlock, xCond2 *pCond, xProp *pQuestProp ) 
{
	pCond->idDesc = nodeCondBlock.GetInt( "desc" );
	XEXmlNode nodeCond = nodeCondBlock.GetFirst();
	while( !nodeCond.IsEmpty() ) {
		if( nodeCond.GetcstrName() == "where" ) {
			LoadWhere( nodeCond, pCond, pQuestProp );
		} else
		if( nodeCond.GetcstrName() == "what" ) {
			LoadWhat( nodeCond, pCond, pQuestProp );
		} else
		if( nodeCond.GetcstrName() == "how" ) {
			LoadHow( nodeCond, pCond, pQuestProp );
		} else {
			XBREAKF( 1, "%s:알수없는 조건이름. %s", pQuestProp->strIdentifier.c_str(), nodeCond.GetstrName().c_str() );
			m_bError = true;
		}
		//
		nodeCond = nodeCond.GetNext();
	}
}

#define CHECK_ONLY_ONE(CSTR) \
	if( idx++ > 0 ) { \
		XBREAK(1); \
		CONSOLE( "%s: %s조건은 1개만 가능합니다.", pQuestProp->strIdentifier.c_str(), _T(CSTR) ); \
		return;\
	}

void XQuestProp::LoadWhen( XEXmlNode& nodeWhen, xWhen *pCond, xProp *pQuestProp )
{
	int idx = 0;
	XEXmlAttr attr = nodeWhen.GetFirstAttribute();
	while( !attr.IsEmpty() )
	{
		if( attr.GetcstrName() == "repeat" ) {
			pQuestProp->repeat = (char)attr.GetInt();
		} else {
			CHECK_ONLY_ONE( "when" );
			ParseEvent( attr, &pCond->idWhen, &pCond->param, pQuestProp );
			switch( pCond->idWhen ) {
			case XGAME::xQC_EVENT_LEVEL:
			case XGAME::xQC_EVENT_OPEN_AREA:
			case XGAME::xQC_EVENT_CLEAR_QUEST:
			case XGAME::xQC_EVENT_UI_ACTION:
			case XGAME::xQC_EVENT_END_SEQ:
				break;
			default:
				XBREAKF( 1, "사용할 수 없는 when조건(when=%d)", pCond->idWhen );
				// 만약 새로운 when조건을 만드려고 한다면 단지 보유만 하고 있어도 퀘가 발생하는 류는 만들어선 안된다.
				// 예를들자면 GET_ITEM/HAVE_ITEM같은것. 아이템은 영자가 넣어줄수도 있기때문에 플레이중 이벤트를 안거칠수도 있기때문이다.(무조건 우편으로만 전달하면 해결?)
				// 이경우는 해당아이템을 사용하면 퀘가 발생하는 식으로 바꾸는게 낫다.
			}
		}
		//
		attr = attr.GetNext();
	}
}

void XQuestProp::LoadWhere( XEXmlNode& nodeWhere, xCond2 *pCond, xProp *pQuestProp )
{
	int idx = 0;
	XEXmlAttr attr = nodeWhere.GetFirstAttribute();
	while( !attr.IsEmpty() )
	{
		CHECK_ONLY_ONE( "where" );
		if( attr.GetcstrName() == "spot" )
		{
			pCond->idWhere = xQC_WHERE_SPOT;
			auto pProp = XGAME::sReadSpotIdentifier( attr, TAG );
			if( pProp )
				pCond->paramWhere.dwParam = pProp->idSpot;
			else
				m_bError = true;
		}
		else
		if( attr.GetcstrName() == "area" )
		{
			pCond->idWhere = xQC_WHERE_AREA;
			auto pProp = XGAME::sReadAreaIdentifier( attr, TAG );
			if( pProp )
				pCond->paramWhere.dwParam = pProp->idCloud;
			else
				m_bError = true;
		} 
		else
		if( attr.GetcstrName() == "spot_type" )
		{
			pCond->idWhere = xQC_WHERE_SPOT_TYPE;
			DWORD val = XGAME::sReadSpotConst( attr, TAG );
			if( CONSTANT->IsFound() )
				pCond->paramWhere.dwParam = val;
			else
				m_bError = true;
		} 
		else
		if( attr.GetcstrName() == "spot_code" )
		{
			pCond->idWhere = xQC_WHERE_SPOT_CODE;
			std::string str = attr.GetString();
			pCond->paramWhere.dwParam = sGetIdsToId( str );;
			pCond->paramWhere.SetDebugStr( C2SZ(str.c_str()) );
		} else
		{
			XBREAKF(1, "%s:알수없는 속성이름. %s", pQuestProp->strIdentifier.c_str(), attr.GetstrName().c_str() );
			m_bError = true;
		}
		//
		attr = attr.GetNext();
		XBREAK( !attr.IsEmpty() );	// 현재 where블럭은 한가지밖에 지정할수 없으므로 블럭이 더 나와선 안됨.
	}
}

XGAME::xtParam GetTypeParamByQuestCond( XGAME::xtQuestCond typeCond )
{
	switch( typeCond )
	{
	case xQC_EVENT_LEVEL:					
	case xQC_EVENT_CLEAR_STAGE:
	case xQC_EVENT_UI_ACTION:
	case xQC_EVENT_GET_STAR:
	case xQC_EVENT_HERO_LEVEL:
	case xQC_EVENT_HERO_LEVEL_SQUAD:
	case xQC_EVENT_HERO_LEVEL_SKILL:
	case xQC_EVENT_BUY_CASH:
		return XGAME::xPT_NUMBER;
	case xQC_EVENT_OPEN_AREA:				
		return XGAME::xPT_AREA;
	case xQC_EVENT_CLEAR_SPOT:			
	case xQC_EVENT_RECON_SPOT:			
	case xQC_EVENT_VISIT_SPOT:			
	case xQC_EVENT_ATTACK_SPOT:
		return XGAME::xPT_SPOT;
	case xQC_EVENT_CLEAR_SPOT_CODE:	
		return XGAME::xPT_SPOT_CODE;
	case xQC_EVENT_CLEAR_SPOT_TYPE:	
	case xQC_EVENT_RECON_SPOT_TYPE:	
	case xQC_EVENT_VISIT_SPOT_TYPE:	
		return XGAME::xPT_SPOT_TYPE;
	case xQC_EVENT_GET_HERO:				
	case xQC_EVENT_KILL_HERO:			
	case xQC_EVENT_HIRE_HERO:
	case xQC_EVENT_HIRE_HERO_NORMAL:
	case xQC_EVENT_HIRE_HERO_PREMIUM:
	case xQC_EVENT_SUMMON_HERO:
	case xQC_EVENT_PROMOTION_HERO:
		return XGAME::xPT_HERO;
	case xQC_EVENT_CLEAR_QUEST:			
		return XGAME::xPT_QUEST;
	case xQC_EVENT_GET_RESOURCE:		
	case xQC_EVENT_HAVE_RESOURCE:		
		return XGAME::xPT_RESOURCE;
	case xQC_EVENT_GET_ITEM:				
	case xQC_EVENT_HAVE_ITEM:				
	case xQC_EVENT_BUY_ITEM:
		return XGAME::xPT_ITEM;
	case xQC_EVENT_RESEARCH_ABIL:				
		return XGAME::xPT_ABIL;
	case xQC_EVENT_UNLOCK_UNIT:					
		return XGAME::xPT_UNIT;
	case xQC_EVENT_CLEAR_GUILD_RAID:			
	case xQC_EVENT_DEFENSE:
	case xQC_EVENT_END_SEQ:
	case xQC_EVENT_TRAIN_QUICK_COMPLETE:
		return XGAME::xPT_ETC;
		break;
	default:
		XBREAK(1);	// 새로추가된 xQC_는 사용하는 파라메터의 타입을 이곳에 추가.
		break;
	}
	return XGAME::xPT_NONE;
}

void XQuestProp::LoadWhat( XEXmlNode& nodeWhat, xCond2 *pCond, xProp *pQuestProp )
{
	int idx = 0;
	XEXmlAttr attr = nodeWhat.GetFirstAttribute();
	//
	while( !attr.IsEmpty() ) {
		if( attr.GetcstrName() == "num" ) {
			pCond->num = attr.GetInt();
		} else
		if( attr.GetcstrName() == "del_item" ) {
			pCond->bDelItem = (attr.GetInt() != 0);
		} else
		if( attr.GetcstrName() == "param" ) {
			DWORD dwParam = XGAME::sReadConst( attr, TAG );
			if( dwParam == XConstant::xERROR )
				m_bError = true;
			else
				pCond->paramWhat2.dwParam = dwParam;
		} else 
		if( attr.GetcstrName().compare(0,4, "xQC_") == 0 ) {
			_tstring _tattr = C2SZ( attr.GetcstrName() );
			ID idWhat = CONSTANT->GetValue( _tattr );   // xQC_XXXX의 값을 찾는다.
			if( CONSTANT->IsFound() ) {
				pCond->idWhat = idWhat;
				_tstring strParam = attr.GetTString();
				if( !strParam.empty() ) {
					// 상수일수도 있으니 한번 찾아봄.
					auto numConstParam = CONSTANT->GetValue( strParam );
					if( CONSTANT->IsFound() ) {
						pCond->paramWhat.nParam = numConstParam;	// 파라메터를 숫자로 씀.
					} else {
						// 상수도 아니면 그냥 숫자일수도 있으니 숫자로 변환해봄.
						TCHAR *szErr = nullptr;
						int num = _tcstol( strParam.c_str(), &szErr, 10 );
						if( szErr == nullptr || (szErr && szErr[0] != 0) ) {
							// 숫자로 변환되지 않음.
							pCond->paramWhat.strParam = strParam;	// 파라메터를 스트링으로 씀
						} else {
							pCond->paramWhat.nParam = num;	// 파라메터를 숫자로 씀.
						}
					}
				}
			} else {
				CONSOLE( "%s:%s라는 조건상수는 없습니다.", TAG, _tattr.c_str() );
			}
		} else {
 			CHECK_ONLY_ONE("what");
			ParseEvent( attr, &pCond->idWhat, &pCond->paramWhat, pQuestProp );
			pCond->typeParamWhat = GetTypeParamByQuestCond( (XGAME::xtQuestCond) pCond->idWhat );
			switch( pCond->idWhat ) {
			case xQC_EVENT_GET_STAR:
				pCond->num = attr.GetInt();
			default:
				break;
			}
		}		
		//
		attr = attr.GetNext();
	}
}

void XQuestProp::ParseEvent( XEXmlAttr& attr, ID *pOutEvent, XGAME::xParam *pOut, 
							xProp *pQuestProp )
{
	auto cstrName = attr.GetcstrName();
	if( cstrName == "level" ) {
		*pOutEvent = xQC_EVENT_LEVEL;
		pOut->nParam = attr.GetInt();
	} else
	if( cstrName == "open_area" ) {
		*pOutEvent = xQC_EVENT_OPEN_AREA;
		auto pProp = XGAME::sReadAreaIdentifier( attr, TAG );
		if( pProp )
			pOut->dwParam = pProp->idCloud;
		else
			m_bError = true;
	} 
	else
	if( cstrName == "clear_spot" ) {
		*pOutEvent = xQC_EVENT_CLEAR_SPOT;
		auto pProp = XGAME::sReadSpotIdentifier( attr, TAG );
		if( pProp )
			pOut->dwParam = pProp->idSpot;
		else
			m_bError = true;
	} else
	if( cstrName == "recon_spot" ) {
		*pOutEvent = xQC_EVENT_RECON_SPOT;
		auto pProp = XGAME::sReadSpotIdentifier( attr, TAG );
		if( pProp )
			pOut->dwParam = pProp->idSpot;
		else
			m_bError = true;
	} else
	if( cstrName == "visit_spot" ) {
		*pOutEvent = xQC_EVENT_VISIT_SPOT;
		auto pProp = XGAME::sReadSpotIdentifier( attr, TAG );
		if( pProp )
			pOut->dwParam = pProp->idSpot;
		else
			m_bError = true;
	} else
	if( cstrName == "clear_spot_type" ) {
		*pOutEvent = xQC_EVENT_CLEAR_SPOT_TYPE;
		DWORD val = XGAME::sReadSpotConst( attr, TAG );
		if( CONSTANT->IsFound() )
			pOut->dwParam = val;
		else
			m_bError = true;
	} else
	if( cstrName == "recon_spot_type" ) {
		*pOutEvent = xQC_EVENT_RECON_SPOT_TYPE;
		DWORD val = XGAME::sReadSpotConst( attr, TAG );
		if( CONSTANT->IsFound() )
			pOut->dwParam = val;
		else
			m_bError = true;
	} else
	if( cstrName == "visit_spot_type" ) {
		*pOutEvent = xQC_EVENT_VISIT_SPOT_TYPE;
		DWORD val = XGAME::sReadSpotConst( attr, TAG );
		if( CONSTANT->IsFound() )
			pOut->dwParam = val;
		else
			m_bError = true;
	} else
	if( cstrName == "clear_spot_code" ) {
		*pOutEvent = xQC_EVENT_CLEAR_SPOT_CODE;
		std::string str = attr.GetString();
		pOut->dwParam = sGetIdsToId( str );;
		pOut->SetDebugStr( C2SZ(str.c_str()) );
	} else
	if( cstrName == "open_area" ) {
		*pOutEvent = xQC_EVENT_OPEN_AREA;
		auto pProp = XGAME::sReadAreaIdentifier( attr, TAG );
		if( pProp )
			pOut->dwParam = pProp->idCloud;
		else
			m_bError = true;
	} else
	if( cstrName == "defense" ) {
		*pOutEvent = xQC_EVENT_DEFENSE;
		pOut->nParam = (DWORD)attr.GetInt();
	} else
	if( cstrName == "get_resource" ) {
		*pOutEvent = xQC_EVENT_GET_RESOURCE;
		DWORD val = XGAME::sReadResourceConst( attr, TAG );
		if( CONSTANT->IsFound() )
			pOut->dwParam = val;
		else
			m_bError = true;

	} else
	if( cstrName == "have_resource" ) {
		*pOutEvent = xQC_EVENT_HAVE_RESOURCE;
		DWORD val = XGAME::sReadResourceConst( attr, TAG );
		if( CONSTANT->IsFound() )
			pOut->dwParam = val;
		else
			m_bError = true;
	} else
	if( cstrName == "get_item" ) {
		*pOutEvent = xQC_EVENT_GET_ITEM;
		auto pProp = XGAME::sReadItemIdentifier( attr, TAG );
		if( pProp ) {
			pOut->dwParam = pProp->idProp;
			pOut->SetDebugStr( pProp->strIdentifier );
		}
		else
			m_bError = true;
	} else
	if( cstrName == "have_item" ) {
		*pOutEvent = xQC_EVENT_HAVE_ITEM;
		auto pProp = XGAME::sReadItemIdentifier( attr, TAG );
		if( pProp ) {
			pOut->dwParam = pProp->idProp;
			pOut->SetDebugStr( pProp->strIdentifier );
		} else
			m_bError = true;
	} else
	if( cstrName == "kill_hero" ) {
		*pOutEvent = xQC_EVENT_KILL_HERO;
		auto pProp = XGAME::sReadHeroIdentifier( attr, TAG );
		if( pProp ) {
			pOut->dwParam = pProp->idProp;
			pOut->SetDebugStr( pProp->strIdentifier );
		} else
			m_bError = true;
	} else
	if( cstrName == "get_hero" ) {
		*pOutEvent = xQC_EVENT_GET_HERO;
		auto pProp = XGAME::sReadHeroIdentifier( attr, TAG );
		if( pProp ) {
			pOut->dwParam = pProp->idProp;
			pOut->SetDebugStr( pProp->strIdentifier );
		} else
			m_bError = true;
	} else
	if( cstrName == "clear_quest" ) {
		*pOutEvent = xQC_EVENT_CLEAR_QUEST;
		std::string ids = attr.GetString();
		if( ids.empty() == false ) {
			pOut->dwParam = XQuestProp::sGetIdsToId( ids );
			pOut->SetDebugStr( C2SZ(ids.c_str()) );
		} else
			m_bError = true;
	} else 
	if( cstrName == "get_star" ) {
		*pOutEvent = xQC_EVENT_GET_STAR;
//		pOut->nParam = attr.GetInt();


	} else
	if( cstrName == "action" ) {
		*pOutEvent = xQC_EVENT_UI_ACTION;
		pOut->dwParam = XGAME::sReadConst( attr, TAG );
		if( pOut->dwParam == XConstant::xERROR )
			m_bError = true;
	} else 
	if( cstrName == "hero_level" ) {
		*pOutEvent = xQC_EVENT_HERO_LEVEL;
		pOut->nParam = attr.GetInt();
	} else
	if( cstrName == "hero_level_squad" ) {
		*pOutEvent = xQC_EVENT_HERO_LEVEL_SQUAD;
		pOut->nParam = attr.GetInt();
	} else
	if( cstrName == "hero_level_skill" ) {
		*pOutEvent = xQC_EVENT_HERO_LEVEL_SKILL;
		pOut->nParam = attr.GetInt();
	} else
	if( cstrName == "end_seq" ) {
		*pOutEvent = xQC_EVENT_END_SEQ;
		pOut->strParam = attr.GetTString();
	} else {
		XBREAKF( 1, "%s:알수없는 속성이름. %s", pQuestProp->strIdentifier.c_str(), attr.GetstrName().c_str() );
		m_bError = true;
	}
} // ParseEvent

void XQuestProp::LoadHow( XEXmlNode& nodehow, xCond2 *pCond, xProp *pQuestProp )
{
	int idx = 0;
	XEXmlAttr attr = nodehow.GetFirstAttribute();
	while( !attr.IsEmpty() )
	{
		CHECK_ONLY_ONE( "when" );
		if( attr.GetcstrName() == "limited_squad" )
		{
			pCond->idHow = xQC_HOW_LIMITED_SQUAD;
			pCond->paramHow.nParam = attr.GetInt();
		} else
		if( attr.GetcstrName() == "ban_unit" )
		{
			pCond->idHow = xQC_HOW_BAN_UNIT;
			_tstring strConst = attr.GetTString();
			pCond->paramHow.dwParam = CONSTANT->GetValue( strConst.c_str() );
			if( CONSTANT->IsFound() == FALSE )
				CONSOLE( "%s:%s라는 유닛은 없습니다.", pQuestProp->strIdentifier.c_str(), strConst.c_str() );
		} else
		if( attr.GetcstrName() == "ban_arm" )
		{
			pCond->idHow = xQC_HOW_BAN_ARM;
			_tstring strConst = attr.GetTString();
			pCond->paramHow.dwParam = CONSTANT->GetValue( strConst.c_str() );
			if( CONSTANT->IsFound() == FALSE )
				CONSOLE( "%s:%s라는 병과는 없습니다.", pQuestProp->strIdentifier.c_str(), strConst.c_str() );
		} else
		if( attr.GetcstrName() == "ban_clan" )
		{
			pCond->idHow = xQC_HOW_BAN_CLAN;
			_tstring strConst = attr.GetTString();
			pCond->paramHow.dwParam = CONSTANT->GetValue( strConst.c_str() );
			if( CONSTANT->IsFound() == FALSE )
				CONSOLE( "%s:%s라는 클랜은 없습니다.", pQuestProp->strIdentifier.c_str(), strConst.c_str() );
		} else
		if( attr.GetcstrName() == "ban_tribe" )
		{
			pCond->idHow = xQC_HOW_BAN_TRIBE;
			_tstring strConst = attr.GetTString();
			pCond->paramHow.dwParam = CONSTANT->GetValue( strConst.c_str() );
			if( CONSTANT->IsFound() == FALSE )
				CONSOLE( "%s:%s라는 종족은 없습니다.", pQuestProp->strIdentifier.c_str(), strConst.c_str() );
		}
		else
		{
			XBREAKF( 1, "%s:알수없는 속성이름. %s", pQuestProp->strIdentifier.c_str(), attr.GetstrName().c_str() );
			m_bError = true;
		}
		//
		attr = attr.GetNext();
	}
}

// void XQuestProp::LoadRewards( XEXmlNode& nodeReward, XGAME::xReward *pReward, xProp *pQuestProp )
// {
// 	int idx = 0;
// 	XEXmlAttr attr = nodeReward.GetFirstAttribute();
// 	while( !attr.IsEmpty() ) {
// 		if( attr.GetcstrName() == "item" ) {
// 			CHECK_ONLY_ONE( "reward" );
// 			auto pProp = XGAME::sReadItemIdentifier( attr, TAG );
// 			if( pProp ) {
// 				pReward->rewardType = xtReward::xRW_ITEM;
// 				pReward->idReward = pProp->idProp;
// 			}
// 			else
// 				m_bError = true;
// 		} else
// 		if( attr.GetcstrName() == "resource" ) {
// 			CHECK_ONLY_ONE( "reward" );
// 			DWORD val = XGAME::sReadResourceConst( attr, TAG );
// 			if( CONSTANT->IsFound() ) {
// 				pReward->rewardType = xtReward::xRW_RESOURCE;
// 				pReward->idReward = val;
// 			} else
// 				m_bError = true;
// 		} else
// 		if( attr.GetcstrName() == "cash" ) {
// 			CHECK_ONLY_ONE( "reward" );
// 			pReward->rewardType = xtReward::xRW_CASH;
// 			int num = attr.GetInt();
// 			if( num != 0 )
// 				pReward->num = num;
// 		} else
// 		if( attr.GetcstrName() == "hero" ) {
// 			CHECK_ONLY_ONE( "reward" );
// 			auto pProp = XGAME::sReadHeroIdentifier( attr, TAG );
// 			if( pProp ) {
// 				pReward->rewardType = xtReward::xRW_HERO;
// 				pReward->idReward = pProp->idProp;
// 			} else
// 				m_bError = true;
// 		} else
// 		if( attr.GetcstrName() == "num" ) {
// 			pReward->num = attr.GetInt();
// 		} else
// 		if( attr.GetcstrName() == "param" ) {
// 			pReward->dwParam = (DWORD)attr.GetInt();
// 		} else {
// 			XBREAKF( 1, "%s:알수없는 속성이름. %s", pQuestProp->strIdentifier.c_str(), attr.GetstrName().c_str() );
// 			m_bError = true;
// 		}
// 		//
// 		attr = attr.GetNext();
// 	}
// }

void XQuestProp::LoadDrops( XEXmlNode& nodeDrop, xDrop *pDrop, xProp *pQuestProp )
{
	int idx = 0;
	ID idItem = XGAME::sReadItemIdsToId( nodeDrop, "item", pQuestProp->strIdentifier.c_str() );
	if( idItem )
		pDrop->aryItems.push_back( idItem );	// 기존호환성을 위해 남겨둠.
// 	_tstring strItem = C2SZ( nodeDrop.GetString("item") );
// 	if( strItem.empty() )
// 		CONSOLE("%s:drop item이 정해지지 않았다.", pQuestProp->strIdentifier.c_str() );
// 	else
// 	{
// 		auto pPropItem = PROP_ITEM->GetpProp( strItem );
// 		if( pPropItem )
// 			//pDrop->idItem =pPropItem->idProp;
// 			pDrop->aryItems.push_back( pPropItem->idProp );	// 기존호환성을 위해 남겨둠.
// 		else {
// 			CONSOLE("%s:drop item=%s 라는 아이템은 없음.", pQuestProp->strIdentifier.c_str(), strItem.c_str() );
// 		}
// 	}
	pDrop->prob = nodeDrop.GetFloat("prob") / 100.f;
	if( pDrop->prob <= 0 )
		CONSOLE("%s:경고:drop item확률이 0이거나 마이너스임.", pQuestProp->strIdentifier.c_str() );

	XEXmlNode nodeChild = nodeDrop.GetFirst();
//	XArrayLinearN<xWhere, 8> aryWhere;
	while( !nodeChild.IsEmpty() ) {
		if( nodeChild.GetcstrName() == "where" ) {
			xWhere xwhere;
			LoadWhere2( nodeChild, &xwhere, pQuestProp );
			if( XASSERT(xwhere.aryAttr.size() > 0) )
				pDrop->aryWhere.Add( xwhere );
		} else
		if( nodeChild.GetcstrName() == "item" ) {
			ID idItem = XGAME::sReadItemIdsToId( nodeChild, "ids", pQuestProp->strIdentifier.c_str() );
			if( idItem )
				pDrop->aryItems.push_back( idItem );
			else
				m_bError = true;
		}
		//
		nodeChild = nodeChild.GetNext();
	}
	if( pDrop->aryWhere.size() == 0 ) {
		// where가 지정되지 않았을경우 조건블럭이 특정 스팟을 지칭하면 그것을 where로 쓴다.
// 		XARRAYLINEAR_LOOP_AUTO( pQuestProp->aryConds, pCond ) {
		for( auto pCond : pQuestProp->aryConds ) {
			xWhereAttr whereAttr;
			if( pCond->idWhat == xQC_EVENT_CLEAR_SPOT )
				whereAttr.idWhere = xQC_WHERE_SPOT;
			if( pCond->idWhat == xQC_EVENT_CLEAR_SPOT_CODE )
				whereAttr.idWhere = xQC_WHERE_SPOT_CODE;
			if( pCond->idWhat == xQC_EVENT_CLEAR_SPOT_TYPE ) 
				whereAttr.idWhere = xQC_WHERE_SPOT_TYPE;
			if( whereAttr.idWhere ) {
				xWhere where;
				where.aryAttr.push_back( whereAttr );
				pDrop->aryWhere.Add( where );
			}
		}
	}
	if( pDrop->aryWhere.size() == 0 ) {
		CONSOLE( "%s:경고:drop에 where가 지정되지 않음.", pQuestProp->strIdentifier.c_str() );
		m_bError = true;
	} else
//		pDrop->aryWhere = aryWhere;
	if( pDrop->aryItems.size() == 0 ) {
		CONSOLE( "%s:drop item이 정해지지 않았다.", pQuestProp->strIdentifier.c_str() );
		m_bError = true;
	}
}

/**
 @brief xml element에서 cKey이름의 attr의 아이템 식별자를 아이템아이디로 변환해서 돌려준다.
*/
// ID XQuestPropXGAME::sReadItemIdsToId( XEXmlNode& node, const char *cKey, LPCTSTR szNodeName )
// {
// 	_tstring strIds = node.GetTString( cKey );
// 	if( !strIds.empty() ) {
// 		auto pProp = PROP_ITEM->GetpProp( strIds );
// 		if( XASSERT(pProp) ) {
// 			return pProp->idProp;
// 		} else {
// 			CONSOLE( "%s:drop item=%s 라는 아이템은 없음.", szNodeName, strIds.c_str() );
// 		}
// 	} else {
// 		CONSOLE( "%s:drop item이 정해지지 않았다.", szNodeName );
// 	}
// 	return 0;
// }

void XQuestProp::LoadWhere2( XEXmlNode& nodeWhere, xWhere *pOutWhere, xProp *pQuestProp )
{
	int idx = 0;
	XEXmlAttr attr = nodeWhere.GetFirstAttribute();
	while( !attr.IsEmpty() ) {
		xWhereAttr wAttr;
		if( attr.GetcstrName() == "spot" ) {
			wAttr.idWhere = xQC_WHERE_SPOT;
			auto pProp = XGAME::sReadSpotIdentifier( attr, TAG );
			if( pProp )
				wAttr.param.dwParam = pProp->idSpot;
			else
				m_bError = true;
		} else
		if( attr.GetcstrName() == "area" ) {
			wAttr.idWhere = xQC_WHERE_AREA;
			auto pProp = XGAME::sReadAreaIdentifier( attr, TAG );
			if( pProp )
				wAttr.param.dwParam = pProp->idCloud;
			else
				m_bError = true;
		} else
		if( attr.GetcstrName() == "spot_type" ) {
			wAttr.idWhere = xQC_WHERE_SPOT_TYPE;
			DWORD val = XGAME::sReadSpotConst( attr, TAG );
			if( CONSTANT->IsFound() )
				wAttr.param.dwParam = val;
			else
				m_bError = true;
		} else
		if( attr.GetcstrName() == "spot_code" )	{
			wAttr.idWhere = xQC_WHERE_SPOT_CODE;
			std::string str = attr.GetString();
			wAttr.param.dwParam = sGetIdsToId( str );;
		} else {
			XBREAKF( 1, "%s:알수없는 속성이름. %s", pQuestProp->strIdentifier.c_str(), attr.GetstrName().c_str() );
			m_bError = true;
		}
		if( wAttr.idWhere )
			pOutWhere->aryAttr.push_back( wAttr );
		//
		attr = attr.GetNext();
	}
}


void XQuestProp::LoadLegion( XEXmlNode& node, xProp *pQuestProp )
{
	_tstring str = node.GetTString("unit");
	if( !str.empty() ) {
		int num = CONSTANT->GetValue( str.c_str() );
		if( num != XConstant::xERROR ) {
			pQuestProp->typeUnit = (xtUnit)num;
		} else {
			CONSOLE( "%s:unit=%s 라는 유닛은 없음.", pQuestProp->strIdentifier.c_str(), str.c_str() );
			m_bError = true;
		}
	}
	int idx = 0;
	XEXmlAttr attr = node.GetFirstAttribute();
	while( !attr.IsEmpty() ) {
		if( attr.GetcstrName() == "test" ) {
		} else {
		}
		//
		attr = attr.GetNext();
	}
}

void XQuestProp::xCond2::Serialize2( XArchive& ar ) const {
	Serialize( ar );
// 	ar << (char)xboolToByte(bDelItem);
// 	ar << (char)typeParamWhat;
// 	ar << (short)0;

}
void XQuestProp::xCond2::DeSerialize2( XArchive& ar, int ver ) {
	DeSerialize( ar, ver );
// 	char c0;
// 	ar >> c0;		bDelItem = xbyteToBool(c0);
// 	ar >> c0;		typeParamWhat = (xtParam)c0;
// 	ar >> c0 >> c0;
}

void XQuestProp::xWhere::Serialize( XArchive& ar ) const {
	ar << aryAttr;
}
void XQuestProp::xWhere::DeSerialize( XArchive& ar, int ) {
	aryAttr.clear();
	ar >> aryAttr;
}

void XQuestProp::xDrop::Serialize( XArchive& ar ) const {
	ar << aryWhere;
	ar << aryItems;
	ar << prob;
}
void XQuestProp::xDrop::DeSerialize( XArchive& ar, int ) {
	aryWhere.clear();
	aryItems.clear();
	ar >> aryWhere;
	ar >> aryItems;
	ar >> prob;
}

void XQuestProp::xProp::Serialize( XArchive& ar ) const {
	ar << strIdentifier;
	ar << strIcon;
	ar << idProp << idName << idDesc;
	ar << (char)typeUnit;
	ar << (char)xboolToByte( bDelQuestItem );
	ar << (char)repeat;
	ar << (char)0;
	ar << rateExp;
//	aryWhen.SerializePtrElem<xWhen>( ar );
	ar.SerializePtr( aryWhen );
	ar << (int)aryConds.size();
	for( auto pConds : aryConds ) {
		pConds->Serialize2( ar );
	}
	ar << aryDrops;
//	aryReward.SerializePtrElem<xReward>( ar );
	ar.SerializePtr( aryReward );

	// 	ar << (int)aryWhen.size();
// 	for( auto pWhen : aryWhen ) {
// 		pWhen->Serialize( ar );
// 	}
// 	ar << aryDrops;
// 	ar << (int)aryReward.size();
// 	for( auto pReward : aryReward ) {
// 		pReward->Serialize( ar );
// 	}

}
void XQuestProp::xProp::DeSerialize( XArchive& ar, int ver ) {
	XVECTOR_DESTROY( aryWhen );
	XVECTOR_DESTROY( aryConds );
	aryDrops.clear();
	XVECTOR_DESTROY( aryReward );
	char c0;
	ar >> strIdentifier;
	ar >> strIcon;
	ar >> idProp >> idName >> idDesc;
	ar >> c0;     typeUnit = (xtUnit)c0;
	ar >> c0;     bDelQuestItem = xbyteToBool(c0);
	ar >> c0;     repeat = c0;
	ar >> c0;
	ar >> rateExp;
//	aryWhen.DeSerializePtrElem<xWhen>( ar, ver );
//	ar.DeSerializePtr( aryWhen, ver );
 	int num;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pWhen = new xWhen();
		pWhen->DeSerialize( ar, ver );
		aryWhen.Add( pWhen );
	}
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pCond = new xCond2();
		pCond->DeSerialize2( ar, ver );
		aryConds.Add( pCond );
	}
	ar >> aryDrops;
//	aryReward.DeSerializePtrElem<xReward>( ar, ver );
//	ar.DeSerializePtr( aryReward, ver );
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pReward = new xReward();
		pReward->DeSerialize( ar, ver );
		aryReward.Add( pReward );
	}
// 	int num;
// 	ar >> num;
// 	for( int i = 0; i < num; ++i ) {
// 		auto pWhen = new xWhen();
// 		pWhen->Destroy
// 	}
// 	for( auto pWhen : aryWhen ) {
// 		pWhen->Serialize( ar );
// 	}
// 	ar >> (int)aryConds.size();
// 	for( auto pConds : aryConds ) {
// 		pConds->Serialize( ar );
// 	}
// 	ar >> aryDrops;
// 	ar >> (int)aryReward.size();
// 	for( auto pReward : aryReward ) {
// 		pReward->Serialize( ar );
// 	}
}


void XQuestProp::Serialize( XArchive& ar ) const
{
	ar << VER_PROP_QUEST;
	ar << (int)m_aryQuests.size();
	for( auto pProp : m_aryQuests ) {
		pProp->Serialize( ar );
	}
}
void XQuestProp::DeSerialize( XArchive& ar, int )
{
	Destroy();
	int ver, num;
	ar >> ver;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pProp = new xProp();
		pProp->DeSerialize( ar, ver );
		m_aryQuests.push_back( pProp );
		m_mapQuests[ pProp->idProp ] = pProp;
		m_mapQuestsStr[ pProp->strIdentifier ] = pProp;
	}
}
