#include "stdafx.h"
#include "XQuestMng.h"
#include "XQuestCon.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// #if defined(_CLIENT) || defined(_GAME_SERVER)
// #ifdef _XQUEST
using namespace xQuest;

////////////////////////////////////////////////////////////////
XQuestMng::XQuestMng()
{
	Init();
}
void XQuestMng::Destroy()
{
  XLIST4_DESTROY( m_listQuestCurr );
  XLIST4_DESTROY( m_listDelete );
}

int XQuestMng::Serialize( XArchive& ar )
{
	int sizeOld = ar.size();
	ar << (WORD)VER_QUEST_SERIALIZE;
	ar << (WORD)m_listQuestCurr.size();
	for( auto pQuestObj : m_listQuestCurr )
	{
		ar << pQuestObj->GetpProp()->idProp;
		pQuestObj->Serialize( ar );
		MAKE_CHECKSUM( ar );
	}
	ar << m_listQuestsComplete;

	return ar.size() - sizeOld;
}
int XQuestMng::DeSerialize( XArchive& ar )
{
	int ver;
	int size;
	WORD w0;
	ar >> w0;	ver = w0;
	ar >> w0;	size = w0;
	for( int i = 0; i < size; ++i )
	{
		DWORD idProp;
		ar >> idProp;
		XQuestProp::xProp *pProp = XQuestProp::sGet()->GetpProp( idProp );
// 		if( pProp == nullptr )
// 			return 0;
		XBREAKF( pProp == nullptr
			, "QuestMng::Deserialize. pProp==null: idProp=%d", idProp );	// 테스트
		XQuestObj *pObj = new XQuestObj( pProp );
		if( pProp ) {
			m_listQuestCurr.push_back( pObj );
		}
		pObj->DeSerialize( ar, ver );
		if( pProp == nullptr )
			SAFE_DELETE( pObj );
		RESTORE_VERIFY_CHECKSUM( ar );
	}
	ar >> m_listQuestsComplete;
	return 1;
	
}


/**
 @brief 퀘스트 pProp의 등장조건에 부합하는지 검사.
 @param idEvent when 이벤트 종류
 @param infoEvent 이벤트의 정보. 비교가될 이벤트의 정보값이 들어온다.
*/
bool XQuestMng::IsCondWhen( XQuestProp::xProp *pProp, ID idEvent, const xQuest::XEventInfo& infoEvent )
{
	BOOL bCond = TRUE;
//	XARRAYLINEAR_LOOP( pProp->aryWhen, XQuestProp::xWhen*, pWhen ) {
	for( auto pWhen : pProp->aryWhen ) {
		// when조건은 모두 and이므로 하나라도 이벤트가 같지 않으면 실패
		if( pWhen->idWhen != idEvent )
			return false;
		switch( idEvent ) {
		case XGAME::xQC_EVENT_LEVEL:
			// 퀘조건을 레벨이상으로 하지 않고 해당레벨하고 일치할때만 했다.
			// 예를들어 새로운퀘가 추가됐는데 발생레벨이 1일때 플레이어 레벨이 10이면
			// 너무 낮은 퀘이기 때문에 안받는게 낫기때문이다.
			if( infoEvent.GetbLevelUnder() ) {
				// 개발용 옵션.
				if( pWhen->param.dwParam > infoEvent.GetParam( idEvent ) )
					return false;
			} else {
				if( pWhen->param.dwParam != infoEvent.GetParam( idEvent ) )
					return false;
			} break;
		case XGAME::xQC_EVENT_HERO_LEVEL:
		case XGAME::xQC_EVENT_HERO_LEVEL_SQUAD:
		case XGAME::xQC_EVENT_HERO_LEVEL_SKILL:
			if( infoEvent.GetParam( idEvent ) < pWhen->param.dwParam )
				return false;
			break;
		default: {
			bool bFlag = false;
			int nParam = pWhen->param.nParam;
			if( (nParam && nParam == infoEvent.GetParam( idEvent )) || nParam == 0 ) {
				auto str = pWhen->param.strParam;
				if( (!str.empty() && str == infoEvent.GetstrParam()) || str.empty() )
					bFlag = true;;
			}
			return bFlag;
		}
		}
 	}
	return true;
}

/**
 @brief 각 퀘스트 객체들에게 퀘스트이벤트를 보낸다.
 @param idEvent 퀘스트이벤트 아이디
 @param infoEvent 퀘스트이벤트 발생시 관련 정보를 담은 객체
*/
void XQuestMng::DispatchEvent( ID idEvent, const xQuest::XEventInfo& infoEvent )
{
	if( XBREAK( m_pDelegate == nullptr ) )
		return;
	// 이 이벤트로 인해 생성되는 다른 퀘스트가 있는지 검사.
	OccurQuestByDispatchEvent( idEvent, infoEvent );
// 	for( auto pProp : XQuestProp::sGet()->GetaryQuests() ) {
// 		// 퀘스트의 등장조건(when)에 맞는 퀘스트가 있는지 검사.
// 		if( IsCondWhen( pProp, idEvent, infoEvent ) ) {
// 			if( IsHaveCurrQuest( pProp->idProp ) == false ) {
// 				if( IsCompleteQuest( pProp->idProp ) == false ) {
// 					XQuestObj *pObj = CreateAndAddQuestObjCurr( pProp );
// 					XBREAK( pObj == nullptr );
// 					///< 퀘발생 델리게이트 발생
// 					m_pDelegate->DelegateOccurQuest( pObj );
// 					XARRAYLINEAR_LOOP_AUTO( pObj->GetaryConds(), pCond ) {
// 						m_pDelegate->DelegateOccurQuestCond( pObj, pCond );
// 					} END_LOOP;
// 					// 같은 조건의 다른 퀘스트도 있을 수 있으므로 루프를 멈춰선 안된다.
// 				}
// 			}
// 		}
// 	}// END_LOOP;
	// 받은 퀘스트들에게 이벤트를 던진다.
	for( auto pQuestObj : m_listQuestCurr ) {
		bool bUpdate = pQuestObj->DispatchEvent( this, idEvent, infoEvent );
 		if( bUpdate ) { 
			if( XBREAK( m_pDelegate == nullptr ) )
				return;
			UpdatedQuestObj( pQuestObj );
		}
	}
}
/**
 @brief idEvent로 인해 생성되는 퀘스트가 있는지 검사.
*/
bool XQuestMng::OccurQuestByDispatchEvent( ID idEvent, const xQuest::XEventInfo& info )
{
	if( XBREAK( m_pDelegate == nullptr ) )
		return false;
	bool bOccur = false;
	for( auto pProp : XQuestProp::sGet()->GetaryQuests() ) {
		// 퀘스트의 등장조건(when)에 맞는 퀘스트가 있는지 검사.
		if( IsCondWhen( pProp, idEvent, info ) ) {
			if( IsHaveCurrQuest( pProp->idProp ) == false ) {
				if( IsCompleteQuest( pProp->idProp ) == false ) {
					XQuestObj *pObj = CreateAndAddQuestObjCurr( pProp );
					XBREAK( pObj == nullptr );
					bOccur = true;
					///< 퀘발생 델리게이트 발생
					m_pDelegate->DelegateOccurQuest( pObj );
					XARRAYLINEAR_LOOP_AUTO( pObj->GetaryConds(), pCond ) {
						m_pDelegate->DelegateOccurQuestCond( pObj, pCond );
					} END_LOOP;
					// 같은 조건의 다른 퀘스트도 있을 수 있으므로 루프를 멈춰선 안된다.
				}
			}
		}
	}
	return bOccur;
}

/**
 @brief 약식 버전
*/
void XQuestMng::DispatchEvent( ID idEvent, DWORD dwParam, DWORD dwParam2 )
{
	XEventInfo info;
	info.SetParam( idEvent, dwParam );
	info.SetParam2( dwParam2 );
	DispatchEvent( idEvent, info );
}

/**
 @brief pCond퀘에게만 이벤트 전달.
*/
void XQuestMng::DispatchEvent( XQuestObj* pQuestObj, XQuestCon *pCond, ID idEvent, const xQuest::XEventInfo& infoEvent )
{
	if( XBREAK( m_pDelegate == nullptr ) )
		return;
	bool bUpdate = pCond->DispatchEvent( this, pQuestObj, idEvent, infoEvent );
	if( bUpdate ) {
		UpdatedQuestObj( pQuestObj );
	}
}

bool XQuestMng::DispatchEventFast( ID idEvent, const xQuest::XEventInfo& infoEvent, XQuestObj* pQuestObj, XQuestCon *pCond )
{
	if( XBREAK( m_pDelegate == nullptr ) )
		return false;
	return pCond->DispatchEvent( this, pQuestObj, idEvent, infoEvent );
}

void XQuestMng::DispatchEvent( XQuestObj* pQuestObj, XQuestCon *pCond, ID idEvent, DWORD dwParam, DWORD dwParam2 )
{
	if( XBREAK( m_pDelegate == nullptr ) )
		return;
	XEventInfo info;
	info.SetParam( idEvent, dwParam );
	info.SetParam2( dwParam2 );
	bool bUpdate = pCond->DispatchEvent( this, pQuestObj, idEvent, info );
	if( bUpdate ) {
		UpdatedQuestObj( pQuestObj );
	}
}

/**
 @brief UpdateQuestObj()를 안하는 버전.
*/
bool XQuestMng::DispatchEventFast( XQuestObj* pQuestObj, XQuestCon *pCond, ID idEvent, DWORD dwParam, DWORD dwParam2 )
{
	if( XBREAK( m_pDelegate == nullptr ) )
		return false;
	XEventInfo info;
	info.SetParam( idEvent, dwParam );
	info.SetParam2( dwParam2 );
	return pCond->DispatchEvent( this, pQuestObj, idEvent, info );
}

/**
 @brief pQuestObj가 갱신됨
*/
void XQuestMng::UpdatedQuestObj( XQuestObj *pQuestObj )
{
	if( XBREAK( m_pDelegate == nullptr ) )
		return;
#ifdef _SERVER
	SetsecUpdateNow( pQuestObj );
#endif // _SERVER
	if( pQuestObj->IsAllComplete() )
		m_pDelegate->DelegateCompleteQuest( pQuestObj );
	else
	{
		m_pDelegate->DelegateUpdateQuest( pQuestObj );
	}
}
/**
 @brief  아이템이 삭제된 이벤트
*/
bool XQuestMng::DispatchEventByDestroyItem( ID idItem, int numDel )
{
	bool bUpdateMng = false;
	// 받은 퀘스트들에게 이벤트를 던진다.
	for( auto pQuestObj : m_listQuestCurr )
	{
		auto bUpdate = pQuestObj->DispatchEventByDestroyItem( idItem, numDel );
		if( bUpdate )
		{
			bUpdateMng = true;
			if( XBREAK( m_pDelegate == nullptr ) )
				return false;
#ifdef _SERVER
			SetsecUpdateNow( pQuestObj );
#endif // _SERVER
			m_pDelegate->DelegateUpdateQuest( pQuestObj );
		}
	}
	return bUpdateMng;
}
#ifdef _CHEAT
/**
 @brief 강제 퀘스트 업데이트용.(치트)
*/
void XQuestMng::_UpdateQuest() 
{
	for( auto pQuestObj : m_listQuestCurr ) {
		XBREAK( m_pDelegate == nullptr );
		if( pQuestObj->IsAllComplete() )
			m_pDelegate->DelegateCompleteQuest( pQuestObj );
		else
			m_pDelegate->DelegateUpdateQuest( pQuestObj );
	}
}
#endif // _CHEAT

/**
 @brief idProp퀘가 이미 완료한 퀘인지 검사.
*/
bool XQuestMng::IsCompleteQuest( ID idProp )
{
	if( m_listQuestsComplete.Findp( idProp ) == nullptr )
		return false;
	return true;
}
/**
 @brief 퀘가 ?상태인가.
*/
bool XQuestMng::IsFinishedQuest( ID idProp )
{
	auto pQuestObj = GetQuestObj( idProp );
	if( pQuestObj ) {
		return pQuestObj->IsAllComplete();
	}
	return false;
}

/**
 @brief idProp퀘가 현재 퀘목록에 있는지 검사.
*/
bool XQuestMng::IsHaveCurrQuest( ID idProp )
{
	return GetQuestObj( idProp ) != nullptr;
// 	if( m_listQuestCurr.FindIf( [idProp](XQuestObj *pObj)->bool{
// 		if( pObj->GetpProp()->idProp == idProp )
// 			return true;
// 		return false;
// 	} ) )
// 		return true;
// 	return false;
}
bool XQuestMng::IsHaveCurrQuest( const _tstring& idsQuest )
{
	return GetQuestObj( idsQuest ) != nullptr;
}

/**
 @brief 현재 받은 퀘목록에서 idProp퀘를 찾는다.
*/
XQuestObj* XQuestMng::GetQuestObj( ID idProp )
{
	XQuestObj **ppObj = m_listQuestCurr.FindpByID( idProp );
	if( ppObj )
		return (*ppObj);
	return nullptr;
}
XQuestObj* XQuestMng::GetQuestObj( LPCTSTR idsQuest )
{
	auto pProp = XQuestProp::sGet()->GetpProp( idsQuest );
	if( pProp )
		return GetQuestObj( pProp->idProp );
	return nullptr;
}

/**
 @brief 퀘스트 객체를 만들고 현재 퀘스트 목록에 넣는다.
*/
XQuestObj* XQuestMng::CreateAndAddQuestObjCurr( ID idQuest )
{
	// 중복검사.
	XQuestObj **ppQuestObj = m_listQuestCurr.FindpByID( idQuest );
	if( ppQuestObj == nullptr || (*ppQuestObj)->GetbDestroy() ) {
		XQuestObj *pObj = XQuestObj::sCreateQuestObj( idQuest );
		if( XASSERT( pObj ) ) {
			m_listQuestCurr.push_back( pObj );
#ifdef _SERVER
			// 숫자가 너무 크므로 32비트로 만들어서 쓴다.
			SetsecUpdateNow( pObj );
// 			DWORD secUpdate = ( XTimer2::sGetTime() & 0x00000000ffffffff );
// 			pObj->SetsecUpdate( secUpdate );
#endif // server
			SortBysecUpdate();
			return pObj;
		}
	} else
		return (*ppQuestObj);
	return nullptr;
}

/**
 @brief 퀘스트 객체를 만들고 현재퀘스트 목록에 넣는다.
*/
XQuestObj* XQuestMng::CreateAndAddQuestObjCurr( XQuestProp::xProp *pProp )
{
	XQuestObj *pObj = XQuestObj::sCreateQuestObj( pProp );
	if( XASSERT( pObj ) ) {
		m_listQuestCurr.push_back( pObj );
		OnInitQuest( pObj );
		return pObj;
	}
	return nullptr;
}

/**
 @brief 퀘가 처음 생덩되었을때 최초 초기화 부분
*/
void XQuestMng::OnInitQuest( XQuestObj *pObj )
{
#ifdef _SERVER
	// 숫자가 너무 크므로 32비트로 만들어서 쓴다.
	SetsecUpdateNow( pObj );
#endif // server
	SortBysecUpdate();
}

void XQuestMng::SortBysecUpdate()
{
	for( auto pObj : m_listQuestCurr )
	{
		m_listQuestCurr.sort(
			[]( XQuestObj* pObj1, XQuestObj* pObj2 )->
			bool {
			if( pObj1 && pObj2 &&
				pObj1->GetsecUpdate() > pObj2->GetsecUpdate() )
				return true;
			return false;
		}
		);
	}
}

/**
 @brief 현재 퀘목록에서 pObj퀘를 삭제한다.
*/
void XQuestMng::DestroyQuestObj( XQuestObj *pObj )
{
	// 삭제 델리게이트 발생
	if( m_pDelegate )
		m_pDelegate->DelegateDestroyQuest( pObj );
	if( !pObj->IsRepeat() )
	{
		// 중복퀘가 아닐때만 완료목록에 넣는다.
	#ifdef _DEBUG
		// 중복검사.
		if( XBREAK( m_listQuestsComplete.Findp( pObj->GetidProp() ) ) )
			return;
	#endif // _DEBUG
		// 완료 목록에 넣음.ㅅ
 		m_listQuestsComplete.Add( pObj->GetidProp() );
	}
	pObj->SetbDestroy( true );
	// 현재 목록에서 뺌
	m_listQuestCurr.DelByID( pObj->getid() );
	// 삭제될 목록에 넣음.
	m_listDelete.Add( pObj );
}

/**
 @brief 퀘스트 보상완료 후 퀘스트 종료 처리
*/
void XQuestMng::DoExitQuest( XQuestObj *pObj )
{
	bool bItemSync = false;
	XARRAYLINEAR_LOOP_AUTO( pObj->GetaryConds(), pCond )
	{
		if( pCond->IsRewardAfterDelItem() )
		{
			bItemSync = true;
			// DispatchEventDestroyItem을 발생시킨 자신은 이 이벤트를 적용하지 말것.
			pCond->SetbDoNotDestroyItemEvent( true );
		}
		m_pDelegate->DelegateDestroyQuestCond( pObj, pCond );
		pCond->SetbDoNotDestroyItemEvent( false );
	} END_LOOP;
	// 퀘 종료 이벤트
	m_pDelegate->DelegateFinishQuest( pObj, bItemSync );
	if( pObj->IsRepeat() == false )
		DestroyQuestObj( pObj );
	else
	{
		pObj->InitQuest();		// 퀘 첨부터 다시 하도록 조건 클리어
		// 퀘스트시간 업데이트 및 소트
		OnInitQuest( pObj );
		///< 퀘발생 델리게이트 발생
		m_pDelegate->DelegateOccurQuest( pObj );
		XARRAYLINEAR_LOOP_AUTO( pObj->GetaryConds(), pCond )
		{
			m_pDelegate->DelegateOccurQuestCond( pObj, pCond );
		} END_LOOP;
	}
}

// #endif // _XQUEST
// #endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

/**
 @brief 퀘스트로 인해 드롭되는 아이템이 있으면 생성시켜서 ary에 넣는다.
*/
int XQuestMng::DoDropFromQuest( XArrayLinearN<ItemBox,256>& ary, const xQuest::XEventInfo& infoEvent )
{
	for( auto pQuestObj : m_listQuestCurr ) {
		auto& aryDrops = pQuestObj->GetpProp()->aryDrops;
		if( aryDrops.size() > 0 && pQuestObj->IsAllComplete() == false ) {
			bool bOr = false;
			// drop블럭수만큼 드랍시도를 한다.
//			XARRAYLINEAR_LOOP_AUTO( aryDrops, &drop ) {
			for( auto& drop : aryDrops ) {
				bool bWhere = false;
				// where블럭들중 하나라도 맞으면(or) true
// 				XARRAYLINEAR_LOOP_AUTO( drop.aryWhere, &xwhere ) {
				for( auto& xwhere : drop.aryWhere ) {
					// 내부에서 whereAttr들은 and연산하여 평가한다.
					if( IsWhereOperatorAnd( xwhere, infoEvent ) ) {
						bWhere = true;
						break;
					}
				}
				if( bWhere ) {
					if( XE::IsTakeChance( drop.prob ) ) {
						// 아이템 목록중에서 한개를 떨어트린다.
						int idx = xRandom( drop.aryItems.size() );
						if( XASSERT( m_pDelegate ) ) {
							ID idItem = drop.aryItems[ idx ];
							if( idItem ) {
								m_pDelegate->DelegateDropItemFromQuest( 
															pQuestObj, idItem, 
															&ary );
							}
						}
					}
				}
			}
		}
	}
	return ary.size();
}
/**
 @brief 퀘스트로 인해 해당스팟에서 드랍될 가능성이 있는 템목록을 추가해준다.
*/
int XQuestMng::GetDropableItems( std::vector<XGAME::xDropItem> *pOutAry, const xQuest::XEventInfo& infoEvent )
{
	for( auto pQuestObj : m_listQuestCurr ) {
		auto& aryDrops = pQuestObj->GetpProp()->aryDrops;
		if( aryDrops.size() > 0 && pQuestObj->IsAllComplete() == false ) {
			bool bOr = false;
			// drop블럭수만큼 드랍시도를 한다.
//			XARRAYLINEAR_LOOP_AUTO( aryDrops, &drop ) {
			for( auto& drop : aryDrops ) {
				bool bWhere = false;
				// where블럭들중 하나라도 맞으면(or) true
// 				XARRAYLINEAR_LOOP_AUTO( drop.aryWhere, &xwhere ) {
				for( auto& xwhere : drop.aryWhere ) {
					// 내부에서 whereAttr들은 and연산하여 평가한다.
					if( IsWhereOperatorAnd( xwhere, infoEvent ) ) {
						for( auto idDrop : drop.aryItems ) {
							pOutAry->push_back( XGAME::xDropItem( idDrop, 1, drop.prob ) );
						}
						break;
					}
				}
			}
		}
	}
	return pOutAry->size();
}

/**
 @brief xWhereAttr조건들을 and연산으로 평가한다.
*/
bool XQuestMng::IsWhereOperatorAnd( const XQuestProp::xWhere& xwhere, 
									const xQuest::XEventInfo& infoEvent )
{
	bool bFlag = true;
	// where attr들을 and연산으로 평가한다.
	for( auto& attr : xwhere.aryAttr ) {
		// 하나라도 틀린게 있으면 실패
		if( !IsWhereAttr( attr, infoEvent ) ) {
			bFlag = false;
			break;
		}
	}
	return bFlag;
}
bool XQuestMng::IsWhereAttr( const XQuestProp::xWhereAttr& wAttr, const xQuest::XEventInfo& infoEvent )
{
	ID idParam = 0;
	switch( wAttr.idWhere ) {
	case 0:		// 조건없음.
		return true;
	case XGAME::xQC_WHERE_SPOT:		idParam = XGAME::xQC_EVENT_CLEAR_SPOT;	break;
	case XGAME::xQC_WHERE_SPOT_TYPE:idParam = XGAME::xQC_EVENT_CLEAR_SPOT_TYPE;	break;
	case XGAME::xQC_WHERE_AREA:		idParam = XGAME::xQC_EVENT_OPEN_AREA;	break;
	case XGAME::xQC_WHERE_SPOT_CODE:idParam = XGAME::xQC_EVENT_CLEAR_SPOT_CODE;	break;
	default:
		XBREAKF( 1, "unknown where param:%d", wAttr.idWhere );
		break;
	}
	if( wAttr.param.dwParam == infoEvent.GetParam( idParam ) )
		return true;
// 	switch( wAttr.idWhere )
// 	{
// 	case 0:		// 조건없음.
// 		return true;
// 	case XGAME::xQC_WHERE_SPOT:
// 		if( wAttr.param.dwParam == infoEvent.GetidSpot() )
// 			return true;
// 		break;
// 	case XGAME::xQC_WHERE_SPOT_TYPE:
// 		if( wAttr.param.nParam == infoEvent.GettypeSpot() )
// 			return true;
// 		break;
// 	case XGAME::xQC_WHERE_AREA:
// 		if( wAttr.param.dwParam == infoEvent.GetidArea() )
// 			return true;
// 		break;
// 	case XGAME::xQC_WHERE_SPOT_CODE:
// 		if( wAttr.param.dwParam == infoEvent.GetidCode() )
// 			return true;
// 		break;
// 	default:
// 		XBREAKF( 1, "unknown where param:%d", wAttr.idWhere );
// 		break;
// 	}
	return false;
}


void XQuestMng::Process( float dt )
{
	for( auto itor = m_listDelete.begin(); itor != m_listDelete.end(); )
	{
		auto pObj = ( *itor );
		SAFE_DELETE( pObj );
		m_listDelete.erase( itor++ );
	}
	for( auto itor = m_listQuestCurr.begin(); itor != m_listQuestCurr.end(); )
	{
		auto pQuestObj = (*itor);
		if( pQuestObj && pQuestObj->GetbDestroy() )	// 이젠 이건 필요없을듯.
		{
			m_listQuestCurr.erase( itor++ );
			SAFE_DELETE( pQuestObj );
		} else
			++itor;
	}
	
}

void XQuestMng::_DelQuest( ID idQuest )
{
#ifdef _CHEAT
	m_listQuestCurr.DelByID( idQuest );
#endif // _CHEAT
}

void XQuestMng::_DelQuestByCompleted( ID idQuest )
{
#ifdef _CHEAT
	m_listQuestsComplete.Del( idQuest );
#endif // _CHEAT
}
/**
 @brief 현재가진 퀘스트중에 type형태의 퀘스트가 있는지 검사
*/
bool XQuestMng::IsHaveQuestType( XGAME::xtQuestCond type )
{
	for( auto pQuestObj : m_listQuestCurr ) {
		if( pQuestObj->IsQuestType( type ) )
			return true;
	}
	return false;
}
/**
 @brief 현재가진 퀘중에9 type형태이고 param이 일치하는 퀘를 찾아낸다.
*/
bool XQuestMng::IsHaveQuestTypeWithParam( XGAME::xtQuestCond type
																				, XGAME::xParam param1
																				, XGAME::xParam param2 )
{
	for( auto pQuestObj : m_listQuestCurr ) {
		if( pQuestObj->IsQuestTypeWithParam( type, param1, param2 ) )
			return true;
	}
	return false;
}

bool XQuestMng::IsHaveQuestTypeWithParam( XGAME::xtQuestCond type
																				, DWORD dwParam1
																				, DWORD dwParam2 )
{
	for( auto pQuestObj : m_listQuestCurr ) {
		XGAME::xParam param1, param2;
		param1.dwParam = dwParam1;
		param2.dwParam = dwParam2;
		if( pQuestObj->IsQuestTypeWithParam( type, param1, param2 ) )
			return true;
	}
	return false;
}
/**
 @brief 보유퀘스트중 typeParam타입을 what파라메터로 쓰는 퀘가 있는지 검사.
*/
bool XQuestMng::IsHaveQuestParamType( XGAME::xtParam typeParam, DWORD dwParam )
{
	for( auto pQuestObj : m_listQuestCurr ) {
		if( pQuestObj->IsHaveWhatParamTypeCondWithParam( typeParam, dwParam ) )
			return true;
	}
	return false;
}

void XQuestMng::GetQuestsToAry( XVector<XQuestObj*> *pAryOut ) 
{
	for( auto pQuestObj : m_listQuestCurr ) {
		pAryOut->Add( pQuestObj );
	}
}

