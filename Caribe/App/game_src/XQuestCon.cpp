#include "stdafx.h"
#include "XQuestCon.h"
#include "XQuest.h"
#include "XLegion.h"
#include "XPropCloud.h"
#include "XPropWorld.h"
#include "XPropHero.h"
#include "XPropItem.h"
#include "XPropUnit.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

// XQuestCon::XQuestCon()
// {
// 	Init();
// }

XQuestCon::XQuestCon( XQuestProp::xCond2 *pCond ) 
{
	Init();
//	m_pProp = pCond;
	if( XASSERT(pCond) )
		m_Prop = *pCond;	// Restore될때 프로퍼티가 파괴되므로 사본을 갖고 있게 한다.
// 	if( pCond ) {
// 		m_pCond = new XQuestProp::xCond2;
// 		*m_pCond = *pCond;
// 	}
}

void XQuestCon::Destroy()
{
}

int XQuestCon::Serialize( XArchive& ar )
{
	ar << m_Num;
	ar << (BYTE)m_bClear;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	return 1;
}

int XQuestCon::DeSerialize( XArchive& ar, int ver )
{
	BYTE b0;
	ar >> m_Num;
	ar >> b0;	m_bClear = (b0!=0);
	ar >> b0 >> b0 >> b0;
// 	XQuestProp::xCond2 condProp;
// 	condProp.DeSerialize( ar, ver );	// 읽기만 하고 없앰.
	return 1;
}

/**
 @brief 퀘 첨부터 다시 할수 있도록 조건을 클리어 한다.
*/
void XQuestCon::InitQuestCon() 
{
	m_bClear = false;
	m_Num = 0;
}


/**
 @brief 퀘스트의 퀘스트조건중에 what조건이 맞는지 검사.
*/
bool XQuestCon::IsWhat( ID idEvent, const xQuest::XEventInfo& infoEvent )
{
	// 조건코드가 안맞으면 바로 실패
	if( m_Prop.idWhat != idEvent )
		return false;
	switch( idEvent )
	{
	case XGAME::xQC_EVENT_LEVEL:
	case XGAME::xQC_EVENT_HERO_LEVEL:
	case XGAME::xQC_EVENT_HERO_LEVEL_SQUAD:
	case XGAME::xQC_EVENT_HERO_LEVEL_SKILL:
		if( m_Prop.paramWhat.dwParam <= infoEvent.GetParam( idEvent ) )
			return true;
		break;
	case XGAME::xQC_EVENT_BUY_CASH:
		if( (m_Prop.paramWhat.dwParam && infoEvent.GetParam( idEvent ) >= m_Prop.paramWhat.dwParam)
			|| (m_Prop.paramWhat.dwParam == 0) )
			return true;
		break;
	default: {
		const auto dwParamDynamic = infoEvent.GetParam( idEvent );
		const auto dwParamProp = m_Prop.paramWhat.dwParam;
		if( (dwParamProp && dwParamProp == dwParamDynamic ) 
			|| (dwParamProp == 0) ) {
			const auto dwParam2Prop = m_Prop.paramWhat2.dwParam;
			const auto dwParam2Dynamic = infoEvent.GetParam2();
			if( ( dwParam2Prop && dwParam2Prop == dwParam2Dynamic ) || dwParam2Prop == 0 )
				return true;
		}
	}
// 		if( m_Prop.paramWhat.dwParam == infoEvent.GetParam( idEvent ) ) {
// 			DWORD p2 = m_Prop.paramWhat2.dwParam;
// 			if( (p2 && p2 == infoEvent.GetParam2()) || p2 == 0 )
// 				return true;
// 		}
	}
	return false;
}

/**
 @brief "어디서" 조건이 맞는지 검사.
 조건이 없으면(0) 검사하지 않고 무조건 true를 리턴한다.
*/
bool XQuestCon::IsWhere( const xQuest::XEventInfo& infoEvent )
{
	ID idParam = 0;
	switch( m_Prop.idWhere ) {
	case 0:		// 조건없음.
		return true;
	case XGAME::xQC_WHERE_SPOT:		idParam = XGAME::xQC_EVENT_CLEAR_SPOT;	break;
	case XGAME::xQC_WHERE_SPOT_TYPE:idParam = XGAME::xQC_EVENT_CLEAR_SPOT_TYPE;	break;
	case XGAME::xQC_WHERE_AREA:		idParam = XGAME::xQC_EVENT_OPEN_AREA;	break;
	case XGAME::xQC_WHERE_SPOT_CODE:idParam = XGAME::xQC_EVENT_CLEAR_SPOT_CODE;	break;
	default:
		XBREAKF( 1, "unknown where param:%d", m_Prop.idWhere );
		break;
	}
	if( m_Prop.paramWhere.dwParam == infoEvent.GetParam( idParam ) )
		return true;
	return false;
}

/**
 @brief "어떻게" 조건이 맞는지 검사.
 조건이 없으면(0) 검사하지 않고 무조건 true를 리턴한다.
*/
bool XQuestCon::IsHow( const xQuest::XEventInfo& infoEvent )
{
	switch( m_Prop.idHow )
	{
	case 0:
		return true;
	case XGAME::xQC_HOW_LIMITED_SQUAD:
		if( XBREAK( infoEvent.GetspLegion() == nullptr ) )
			return false;
		if( m_Prop.paramHow.nParam >= infoEvent.GetspLegion()->GetNumSquadrons() )
			return true;
		break;
	case XGAME::xQC_HOW_BAN_UNIT:
		break;
	case XGAME::xQC_HOW_BAN_ARM:
		break;
	case XGAME::xQC_HOW_BAN_CLAN:
		break;
	case XGAME::xQC_HOW_BAN_TRIBE:
		break;
	default:
		XBREAKF( 1, "unknown How param:%d", m_Prop.idHow );
		break;
	}
	return false;
}

/**
 @brief 외부에서 보내준 이벤트에 퀘조건을 검사
 @return 퀘완료상태가 업데이트 되면 true를 리턴
*/
bool XQuestCon::DispatchEvent( XQuestMng* pQuestMng, XQuestObj *pQuestObj, ID idEvent, const xQuest::XEventInfo& infoEvent )
{
	if( m_bClear )		// 이미 완료된 조건은 다시 처리하지 않는다.
		return false;
	bool bUpdate = false;
	// 일단 무엇이 조건이 맞는가
	if( IsWhat( idEvent, infoEvent ) ) {
		if( IsWhere( infoEvent ) ) {
			if( IsHow( infoEvent ) ) {
				bUpdate = true;
				if( m_Prop.num > 0 ) {
					// 퀘템을 삭제해야하는 조건이면 직접 보유한 템개수를 세야 한다.
					if( infoEvent.GetAmount() > 0 )
						m_Num += infoEvent.GetAmount();
					else
						++m_Num;
					if( m_Num >= m_Prop.num )
						m_bClear = true;
				} else
					m_bClear = true;	// 횟수/개수가 0이면 1회로 간주하고 바로 조건완료
			}
		}
	}
	// 퀘상태가 업데이트 되면 true리턴
	return bUpdate;
}

bool XQuestCon::DispatchEventByDestroyItem( ID idItem, int numDel )
{
	if( m_bDoNotDestroyItemEvent )
		return false;
	// 퀘완료후 퀘템 파괴조건인 퀘스트는.
	if( IsRewardAfterDelItem() )
	{
		if( m_Prop.idWhat == XGAME::xQC_EVENT_HAVE_ITEM ||
			m_Prop.idWhat == XGAME::xQC_EVENT_GET_ITEM )
		{
			if( m_Prop.paramWhat.dwParam == idItem )
			{
				// 이 조건이 이 아이템을 갖는것이었으면 얻은숫자에서 빼줌.
				m_Num -= numDel;
				if( m_Num < 0 )
					m_Num = 0;
				if( m_Num >= m_Prop.num )
					m_bClear = true;
				else
					m_bClear = false;
				return true;
			}
		}
	}
	return false;
}
void XQuestCon::Update()
{
}


/**
 @brief
 #spot#, #level#식으로 하지 않은 이유.
	.#spot#의 경우 where의 값인지 what의 값인지 모호해진다.
*/
bool TokenReplace( const XQuestProp::xCond2 *pCond, 
				LPCTSTR szToken, 
				TCHAR *pOutWord, 
				int lenOut )
{
	if( XE::IsSame( szToken, _T("#what#") ) ) {
		switch( pCond->idWhat ) {
		case XGAME::xQC_EVENT_LEVEL:
		case XGAME::xQC_EVENT_HERO_LEVEL:
		case XGAME::xQC_EVENT_HERO_LEVEL_SQUAD:
		case XGAME::xQC_EVENT_HERO_LEVEL_SKILL:
			_tcscpy_s( pOutWord, lenOut, XFORMAT("%d", pCond->paramWhat.nParam) );
			break;
		case XGAME::xQC_EVENT_OPEN_AREA: {
			XPropCloud::xCloud *pProp = PROP_CLOUD->GetpProp( pCond->paramWhat.dwParam );
			if( XASSERT(pProp) )
			{
				LPCTSTR szName = XTEXT( pProp->idName );
				if( pProp->idName == 0 )
					szName = pProp->strIdentifier.c_str();
				_tcscpy_s( pOutWord, lenOut, szName );
			}
		} break;
		case XGAME::xQC_EVENT_RECON_SPOT:
		case XGAME::xQC_EVENT_CLEAR_SPOT:
		case XGAME::xQC_EVENT_VISIT_SPOT: {
			auto pProp = PROP_WORLD->GetpProp( pCond->paramWhat.dwParam );
			if( XASSERT(pProp) ) {
				ID idName = pProp->idName;
				if( idName == 0 )
					idName = 2012;
				_tcscpy_s( pOutWord, lenOut, XTEXT(idName) );
			}
		} break;
		case XGAME::xQC_EVENT_CLEAR_SPOT_CODE: {
			ID idCode = pCond->paramWhat.dwParam;
			auto pList = PROP_WORLD->GetpPropByCode( idCode );
			if( XASSERT(pList) ) {
				if( XASSERT(pList->size()) ) {
					auto ppPropSpot = pList->GetpFirst();
					if( ppPropSpot ) {
						ID idName = (*ppPropSpot)->idName;
						if( idName )
							_tcscpy_s( pOutWord, lenOut, XTEXT( idName ) );
						else
							_tcscpy_s( pOutWord, lenOut, XFORMAT( "code=%d", idCode ) );
					}
				}
			}
// 			pOutWord[0] = 0;
		} break;
		case XGAME::xQC_EVENT_CLEAR_SPOT_TYPE:
		case XGAME::xQC_EVENT_RECON_SPOT_TYPE:
		case XGAME::xQC_EVENT_VISIT_SPOT_TYPE: {
//			XPropWorld::xBASESPOT *pProp = PROP_WORLD->GetpProp( pCond->paramWhat.dwParam );
//			if( XASSERT( pProp ) )
			auto type = (XGAME::xtSpot) pCond->paramWhat.nParam;
			_tcscpy_s( pOutWord, lenOut, XGAME::GetStrSpotType( type ) );
		} break;
		case XGAME::xQC_EVENT_KILL_HERO:
		case XGAME::xQC_EVENT_GET_HERO: {
			if( pCond->paramWhat.dwParam ) {
				// 지정된 영웅을 영입
				auto pProp = PROP_HERO->GetpProp( pCond->paramWhat.dwParam );
				if( XASSERT( pProp ) )
					_tcscpy_s( pOutWord, lenOut, pProp->GetstrName().c_str() );
			} else {
				// 아무영웅이나 영입.
				// 퀘목표엔 "새 영웅 얻기"로 표시되어야 함.
			}
		} break;
			break;
		case XGAME::xQC_EVENT_CLEAR_QUEST: {
			auto pProp = XQuestProp::sGet()->GetpProp( pCond->paramWhat.dwParam );
			if( XASSERT( pProp ) )
				_tcscpy_s( pOutWord, lenOut, XTEXT(pProp->idName) );
		} break;
		case XGAME::xQC_EVENT_DEFENSE:
			_tcscpy_s( pOutWord, lenOut, XTEXT(5017) );	
			break;
		case XGAME::xQC_EVENT_HAVE_RESOURCE:
		case XGAME::xQC_EVENT_GET_RESOURCE: {
			_tcscpy_s( pOutWord, lenOut, XGAME::GetStrResource( (XGAME::xtResource)pCond->paramWhat.nParam ) );
		} break;
		case XGAME::xQC_EVENT_GET_ITEM:
		case XGAME::xQC_EVENT_HAVE_ITEM:{
			auto pProp = PROP_ITEM->GetpProp( pCond->paramWhat.dwParam );
			if( XASSERT( pProp ) )
				_tcscpy_s( pOutWord, lenOut, XTEXT(pProp->idName) );
		} break;
		case XGAME::xQC_EVENT_CLEAR_STAGE:
			_tcscpy_s( pOutWord, lenOut, _T("stage") );
			break;
		case XGAME::xQC_EVENT_UI_ACTION: {
			pOutWord[0] = 0;
		} break;
		default:
			XBREAK(1);
			break;
		}
	} else // #what#
	if( XE::IsSame( szToken, _T( "#where#" ) ) )
	{
		switch( pCond->idWhere )
		{
		case XGAME::xQC_WHERE_SPOT: {
			XPropWorld::xBASESPOT *pProp = PROP_WORLD->GetpProp( pCond->paramWhere.dwParam );
			if( XASSERT( pProp ) )
			{
				ID idName = pProp->idName;
				if( idName == 0 )
					idName = 2012;
				_tcscpy_s( pOutWord, lenOut, XTEXT( idName ) );
			}
		} break;
		case XGAME::xQC_WHERE_SPOT_TYPE: {
			XPropWorld::xBASESPOT *pProp = PROP_WORLD->GetpProp( pCond->paramWhere.dwParam );
			if( XASSERT( pProp ) )
				_tcscpy_s( pOutWord, lenOut, XGAME::GetStrSpotType( pProp->type ) );
		} break;
		case XGAME::xQC_WHERE_AREA: {
			XPropCloud::xCloud *pProp = PROP_CLOUD->GetpProp( pCond->paramWhere.dwParam );
			if( XASSERT( pProp ) )
			{
				LPCTSTR szName = XTEXT( pProp->idName );
				if( pProp->idName == 0 )
					szName = pProp->strIdentifier.c_str();
				_tcscpy_s( pOutWord, lenOut, szName );
			}
		} break;
		case XGAME::xQC_WHERE_SPOT_CODE:
			pOutWord[ 0 ] = 0;
			break;
		default:
			XBREAK(1);
			break;
		}
	} else
	if( XE::IsSame( szToken, _T( "#how#" ) ) )
	{
		switch( pCond->idWhere )
		{
		case XGAME::xQC_HOW_LIMITED_SQUAD:
			break;
		case XGAME::xQC_HOW_BAN_UNIT:
			break;
		case XGAME::xQC_HOW_BAN_ARM:
			break;
		case XGAME::xQC_HOW_BAN_CLAN:
			break;
		case XGAME::xQC_HOW_BAN_TRIBE:
			break;
		default:
			XBREAK(1);
			break;
		}
	}
	return true;
} // TokenReplaces

/**
  @brief szSrc문자열에서 #what# #where#같은 토큰을 찾아서 pFunc을 호출한다.
  토큰은 pFunc이 리턴하는 문자열로 대체된다.
*/
void XQuestCon::sReplaceToken( const XQuestCon *pCond, _tstring& str, 
							bool (*pFunc)( const XQuestProp::xCond2 *pCond, LPCTSTR szToken, TCHAR *pOutWord, int lenOut ) )
{
	int idxSrc = 0;
	bool bOpen = false;
	TCHAR szToken[32];
	int idxToken = 0;
	TCHAR szSrc[ 0x8000 ];
	_tcscpy_s( szSrc, str.c_str() );
	while(1)
	{
		TCHAR c = szSrc[idxSrc++];
		if( c == 0 )
			break;
		else
		if( c == '#' )
		{
			if( bOpen == false )
				bOpen = true;
			else
			{
				// close
				szToken[ idxToken++ ] = c;
				szToken[ idxToken ] = 0;
				TCHAR szReplaceWord[1024];
				pFunc( pCond->GetpProp(), szToken, szReplaceWord, 1024 );
				XE::ReplaceStr( str, szToken, szReplaceWord );
				idxToken = 0;
				bOpen = false;
			}
		}
		if( bOpen ) {
			szToken[ idxToken++ ] = c;
			szToken[ idxToken ] = 0;
		}
	}
}

void XQuestCon::sReplaceToken( const XQuestCon *pCond, _tstring& str )
{
	sReplaceToken( pCond, str, TokenReplace );
}

/**
 @brief this퀘조건의 현재상태를 문자열로 만들어 pOutStr에 붙여야 한다.
 ex) 제국수비대 격파 0/3
*/
void XQuestCon::GetQuestObjective( _tstring *pOutStr )
{
	_tstring strDesc;
	if( m_Prop.idDesc ) {
		strDesc = XTEXT( m_Prop.idDesc );
	} else {
		switch( m_Prop.idWhere )
		{
		case XGAME::xQC_WHERE_AREA:
			strDesc += XTEXT(5042);		// #where#에서
			break;
		}
		// 이방식은 한글에서만 유효하지 외국어에선 통하지 않는다. 
		// #where#에서 #what#으 XX하라 와 
		// #what#을 xx하라 두가지 모두 만들어 두고 idWere가 있을때 없을때 따로 선택해야 한다.
		// 번역은 이 두가지 케이스를 모두 번역한다.
		strDesc += _T(" ");
		switch( m_Prop.idWhat )
		{
		case XGAME::xQC_EVENT_LEVEL:		strDesc += XTEXT(5005);		break;
		case XGAME::xQC_EVENT_OPEN_AREA:	strDesc += XTEXT( 5006 );	break;
		case XGAME::xQC_EVENT_CLEAR_SPOT:	strDesc += XTEXT(5007);		break;
		case XGAME::xQC_EVENT_RECON_SPOT:	strDesc += XTEXT( 5008 );	break;
		case XGAME::xQC_EVENT_VISIT_SPOT:	strDesc += XTEXT( 5009 );	break;
		case XGAME::xQC_EVENT_CLEAR_SPOT_CODE: 	strDesc += XTEXT(5011); break;
		case XGAME::xQC_EVENT_CLEAR_SPOT_TYPE:	strDesc += XTEXT( 5011 );	break;
		case XGAME::xQC_EVENT_RECON_SPOT_TYPE:	strDesc += XTEXT( 5012 );	break;
		case XGAME::xQC_EVENT_VISIT_SPOT_TYPE:	strDesc += XTEXT( 5013 );	break;
		case XGAME::xQC_EVENT_KILL_HERO:	strDesc += XTEXT( 5015 );	break;
		case XGAME::xQC_EVENT_GET_HERO:		strDesc += XTEXT( 5014 );	break;
		case XGAME::xQC_EVENT_CLEAR_QUEST:	strDesc += XTEXT( 5016 );	break;
		case XGAME::xQC_EVENT_DEFENSE:		strDesc += XTEXT( 5017 );	break;
		case XGAME::xQC_EVENT_GET_RESOURCE:		strDesc += XTEXT( 5018 );	break;
		case XGAME::xQC_EVENT_HAVE_RESOURCE:	strDesc += XTEXT( 5019 );	break;
		case XGAME::xQC_EVENT_GET_ITEM:			strDesc += XTEXT( 5020 );	break;
		case XGAME::xQC_EVENT_HAVE_ITEM:		strDesc += XTEXT( 5021 );	break;
		case XGAME::xQC_EVENT_CLEAR_STAGE:		strDesc += XTEXT( 5022 );	break;
		case XGAME::xQC_EVENT_GET_STAR:			strDesc += XTEXT( 5023 );	break;
		case XGAME::xQC_EVENT_HERO_LEVEL:		strDesc += XTEXT( 5100 );		break;
		case XGAME::xQC_EVENT_HERO_LEVEL_SQUAD:		strDesc += XTEXT( 5101 );		break;
		case XGAME::xQC_EVENT_HERO_LEVEL_SKILL:		strDesc += XTEXT( 5102 );		break;
		case XGAME::xQC_EVENT_HIRE_HERO:		
		case XGAME::xQC_EVENT_HIRE_HERO_NORMAL:
			strDesc += XTEXT( 5083 );	
			break;
		case XGAME::xQC_EVENT_HIRE_HERO_PREMIUM:		strDesc += XTEXT( 5043 );			break;
		case XGAME::xQC_EVENT_TRAIN_QUICK_COMPLETE:		strDesc += XTEXT( 5039 );			break;
		case XGAME::xQC_EVENT_RESEARCH_ABIL:		strDesc += XTEXT( 5077 );			break;
		case XGAME::xQC_EVENT_UNLOCK_UNIT:		strDesc += XTEXT( 5078 );			break;
		case XGAME::xQC_EVENT_SUMMON_HERO:		strDesc += XTEXT( 5084 );			break;
		case XGAME::xQC_EVENT_PROMOTION_HERO:		strDesc += XTEXT( 5085 );			break;
		case XGAME::xQC_EVENT_BUY_CASH:		strDesc += XTEXT( 5081 );			break;
		case XGAME::xQC_EVENT_BUY_ITEM:		strDesc += XTEXT( 5080 );			break;
		case XGAME::xQC_EVENT_ATTACK_SPOT:		strDesc += XTEXT( 5103 );			break;
		case XGAME::xQC_EVENT_CLEAR_GUILD_RAID:		strDesc += XTEXT( 5086 );			break;
			
		case XGAME::xQC_EVENT_UI_ACTION: {
			// UI Action
			auto typeAction = ( XGAME::xtUIAction )m_Prop.paramWhat.dwParam;
			switch( typeAction ) {
			case XGAME::xUA_RESEARCH:		strDesc += XTEXT( 5077 );	break;
			case XGAME::xUA_UNLOCK_UNIT:	strDesc += XTEXT( 5078 );	break;
			case XGAME::xUA_TRADE:			strDesc += XTEXT( 5079 );	break;
			case XGAME::xUA_BUY_ARMOR:		strDesc += XTEXT( 5080 );	break;
			case XGAME::xUA_BUY_CASH:		strDesc += XTEXT( 5081 );	break;
			case XGAME::xUA_SELL_ITEM:		strDesc += XTEXT( 5082 );	break;
			case XGAME::xUA_HIRE_HERO:		strDesc += XTEXT( 5083 );	break;
			case XGAME::xUA_PROVIDE_BOOK:	strDesc += XTEXT( 5084 );	break;
			case XGAME::xUA_PROVIDE_SKILL:	strDesc += XTEXT( 5085 );	break;
			case XGAME::xUA_PROVIDE_SQUAD:	strDesc += XTEXT( 5086 );	break;
			case XGAME::xUA_REPOSITION:		strDesc += XTEXT( 5087 );	break;
			case XGAME::xUA_TRAINING_LEVEL:	strDesc += XTEXT( 5088 );	break;
			case XGAME::xUA_TRAINING_SKILL:	strDesc += XTEXT( 5089 );	break;
			case XGAME::xUA_TRAINING_SQUAD:	strDesc += XTEXT( 5090 );	break;
			case XGAME::xUA_JOIN_GUILD:		strDesc += XTEXT( 5091 );	break;
			case XGAME::xUA_EQUIP:			strDesc += XTEXT( 5092 );	break;
			case XGAME::xUA_ENTER_SCENE:	break;
			default:
				XBREAK(1);
				break;
			}
		} break;
		default:
			XBREAKF(1, "unknown what code" );
			break;
		}
	}
	if( strDesc.empty() == false ) {
		TCHAR szBuff[ 0x8000 ];
		sReplaceToken( this, strDesc, TokenReplace );	// #what#같은 토큰을 해당값으로 바꾼다
		XE::ConvertJosaStr( szBuff, strDesc.c_str() );	// 조사처리를 한다.
		strDesc = szBuff;
	}
	strDesc += _T( "  " );
	strDesc += XE::Format(_T("%d/%d"), m_Num, m_Prop.num );
	(*pOutStr) += strDesc;
}

void XQuestCon::GetQuestObjectiveNum( _tstring *pOutStr )
{
	if( m_Prop.num > 1 ) {
		if( m_Prop.idWhat == XGAME::xQC_EVENT_GET_RESOURCE ||
			m_Prop.idWhat == XGAME::xQC_EVENT_HAVE_RESOURCE ) {
			float rate = (float)m_Num / m_Prop.num;
			*pOutStr = XFORMAT( "%.1f%%", rate * 100.f );
		} else
			*pOutStr = XFORMAT("%d/%d", m_Num, m_Prop.num );
	}
}
/**
 @brief 지역오픈조건이냐
*/
bool XQuestCon::IsOpenAreaType()
{
	if( m_Prop.idWhat == xQC_EVENT_OPEN_AREA )
		return true;
	return false;
}

/**
 @brief 지역오픈 조건일때 지역 아이디
*/
ID XQuestCon::GetidAreaByCond()
{
	XASSERT( m_Prop.idWhat == xQC_EVENT_OPEN_AREA );
	return m_Prop.paramWhat.dwParam;
}

bool XQuestCon::IsReconType()
{
	if( m_Prop.idWhat == xQC_EVENT_RECON_SPOT )
		return true;
	return false;
}

/**
 @brief 스팟류 조건일때 스팟 아이디
*/
ID XQuestCon::GetidSpotbyCond()
{
	XASSERT( m_Prop.idWhat == xQC_EVENT_RECON_SPOT ||
			m_Prop.idWhat == xQC_EVENT_CLEAR_SPOT ||
			m_Prop.idWhat == xQC_EVENT_VISIT_SPOT );
	return m_Prop.paramWhat.dwParam;
}

bool XQuestCon::IsSpotClearType()
{
	if( m_Prop.idWhat == xQC_EVENT_CLEAR_SPOT )
		return true;
	return false;
}

/**
 @brief get_item조건일 경우 퀘 완료후 해당 퀘템을 제거해야 하는가.
*/
bool XQuestCon::IsRewardAfterDelItem() 
{
	if( m_Prop.idWhat != xQC_EVENT_GET_ITEM &&
		m_Prop.idWhat != xQC_EVENT_HAVE_ITEM )
		return false;

	return m_Prop.bDelItem ;
}

/**
 @brief 퀘조건이 GET_ITEM일경우 조건템의 아이디를 돌려준다.
*/
ID XQuestCon::GetCondItem()
{
	if( m_Prop.idWhat != xQC_EVENT_GET_ITEM &&
		m_Prop.idWhat != xQC_EVENT_HAVE_ITEM &&
		m_Prop.idWhat != xQC_EVENT_GET_RESOURCE &&
		m_Prop.idWhat != xQC_EVENT_HAVE_RESOURCE )
		return 0;
	return m_Prop.paramWhat.dwParam;
}

/**
 @brief 스팟타입류 조건일때 스팟타입을 돌려준다.
 @pOutIDArea 만약 특정지역조건이 있다면 그 지역아이디도 돌려준다
*/
XGAME::xtSpot XQuestCon::GettypeSpotByCond( ID *pOutIDArea )
{
	XASSERT( m_Prop.idWhat == xQC_EVENT_RECON_SPOT_TYPE ||
		m_Prop.idWhat == xQC_EVENT_CLEAR_SPOT_TYPE ||
		m_Prop.idWhat == xQC_EVENT_VISIT_SPOT_TYPE );
	if( pOutIDArea && m_Prop.idWhere == xQC_WHERE_AREA )
		*pOutIDArea = m_Prop.paramWhere.dwParam;
	return (XGAME::xtSpot)m_Prop.paramWhat.dwParam;
}
ID XQuestCon::GetCodeSpotByCond()
{
	XASSERT( m_Prop.idWhat == xQC_EVENT_CLEAR_SPOT_CODE );
	return m_Prop.paramWhat.dwParam;
}
/**
 @brief 별수집 퀘인가
*/
bool XQuestCon::IsGetStarType()
{
	if( m_Prop.idWhat == xQC_EVENT_GET_STAR )
		return true;
	return false;
}
/**
 @brief 씬 진입 조건인가.
*/
// bool XQuestCon::IsEnterSceneCond()
// {
// 	if( m_Prop.idWhat == xQC_EVENT_UI_ACTION ) {
// 		return ( m_Prop.paramWhat.dwParam == XGAME::xUA_ENTER_SCENE );
// 	}
// 	return false;
// }
/**
 @brief 이 퀘조건이 type조건형태인가.
*/
bool XQuestCon::IsQuestTypeWithParam( XGAME::xtQuestCond type, XGAME::xParam param1, XGAME::xParam param2 ) 
{
	if( m_Prop.idWhat == type ) {
		// 파라메터가 주어졌을땐 파라메터를 비교하고 없으면 타입만 비교
		if( param1.dwParam ) {
			if( m_Prop.paramWhat.dwParam == param1.dwParam ) {
				return( ( param2.dwParam && m_Prop.paramWhat2.dwParam == param2.dwParam )
					|| param2.dwParam == 0 );
			}
		} else
			return true;
	}
	return false;
}
