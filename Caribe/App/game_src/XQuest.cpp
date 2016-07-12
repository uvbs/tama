#include "stdafx.h"
#include "XQuest.h"
#include "XQuestCon.h"
#include "XLegion.h"
#include "XQuestMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XQuestObj* XQuestObj::sCreateQuestObj( ID idQuest )
{
	XQuestProp::xProp *pProp = XQuestProp::sGet()->GetpProp( idQuest );
	if( XBREAK( pProp == nullptr ) )
		return nullptr;
	return sCreateQuestObj( pProp );
}

XQuestObj* XQuestObj::sCreateQuestObj( XQuestProp::xProp *pProp )
{
	if( XBREAK( pProp == nullptr ) )
		return nullptr;
	XQuestObj *pObj = new XQuestObj( pProp );
	if( XBREAK( pObj == nullptr ) )
		return nullptr;
	pObj->OnCreate();
	return pObj;
}

int XQuestObj::sSerialize( XQuestObj *pObj, XArchive& ar )
{
	ar << VER_QUEST_SERIALIZE;
	ar << pObj->GetsecUpdate();
	return pObj->Serialize( ar );
}

int XQuestObj::sDeSerialize( XQuestObj *pObj, XArchive& ar )
{
	int ver;
	ar >> ver;
	DWORD sec;
	ar >> sec;	pObj->SetsecUpdate( sec );
	return pObj->DeSerialize( ar, ver );
}


//////////////////////////////////////////////////////////////////////////
void XQuestObj::Destroy()
{
	DestroyAllCond();
}

void XQuestObj::DestroyAllCond()
{
	XARRAYLINEAR_DESTROY( m_aryConds );
	
}

void XQuestObj::OnCreate()
{
	if( XBREAK( GetpProp() == nullptr ) )
		return;
	XBREAK( m_aryConds.size() > 0 );
	const auto pProp = GetpProp();
	// 완료조건 어레이를 만든다.
	XArrayLinearN<XQuestCon*, 16> ary;
	for( auto pCond : pProp->aryConds ) {
		if( pCond->idWhat ) {
			XQuestCon *pCondObj = new XQuestCon( pCond );
			ary.Add( pCondObj );
		}
	}
	XBREAK( m_aryConds.size() != 0 );
	m_aryConds = ary;
#ifdef _CLIENT
	m_secClicked = XTimer2::sGetTime();		// 퀘가 생성되면 타이머 작동 시작.
#endif // _CLIENT
}

int XQuestObj::Serialize( XArchive& ar )
{
	ar << m_aryConds.size();
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj ) {
		ar << (DWORD)0;			// reserved
		pCondObj->Serialize( ar );
		MAKE_CHECKSUM( ar );
	} END_LOOP;
	return 1;
}
int XQuestObj::DeSerialize( XArchive& ar, int ver )
{
	int size;
	XArrayLinearN<XQuestCon*, 16> ary;
	ar >> size;
	if( XBREAK( size > ary.GetMax() ) )
		return 0;
	// size저장하는거 없애고 프로퍼티걸 쓸 계획
	DestroyAllCond();
	for( int i = 0; i < size; ++i ) {
		DWORD dw0;
		ar >> dw0;		// reserved
		XQuestProp::xCond2 *pPropCond = nullptr;
		if( GetpProp() && GetpProp()->aryConds.size() > (size_t)i ) 
			pPropCond = GetpProp()->aryConds[i];
		XQuestCon *pCondObj = new XQuestCon( pPropCond );
		pCondObj->DeSerialize( ar, ver );
		if( pPropCond )
			ary.Add( pCondObj );
		RESTORE_VERIFY_CHECKSUM( ar );
	}
  XBREAK( m_aryConds.size() != 0 );
	m_aryConds = ary;
#ifdef _CLIENT
	// 클릭 타이머 작동 시작
	UpdatesecClicked( true );	// random reset
#endif // _CLIENT
	return 1;
}

/**
 @brief 퀘 다시 첨부터 할수 있도록 모든 조건을 클리어 한다.
*/
void XQuestObj::InitQuest()
{
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj )
	{
		pCondObj->InitQuestCon();
	} END_LOOP;
}

/**
 @brief 퀘스트 이벤트를 처리한다.
*/
bool XQuestObj::DispatchEvent( XQuestMng* pQuestMng, ID idEvent, const xQuest::XEventInfo& infoEvent )
{
	bool bUpdate = false;
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj ) {
		bool bUp = pCondObj->DispatchEvent( pQuestMng, this, idEvent, infoEvent );
		if( bUp )
			bUpdate = true;
	} END_LOOP;
	return bUpdate;
}

bool XQuestObj::DispatchEventByDestroyItem( ID idItem, int numDel )
{
	bool bUpdate = false;
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj )
	{
		bool bUp = pCondObj->DispatchEventByDestroyItem( idItem, numDel );
		if( bUp )
			bUpdate = true;
	} END_LOOP;
	return bUpdate;
}
/**
 @brief 퀘스트의 모든 완료조건이 완료되었는가.
*/
bool XQuestObj::IsAllComplete()
{
	const auto pProp = GetpProp();
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj )
	{
		if( pCondObj->IsClear() == false )
			return false;
	} END_LOOP;
	return true;
}

/**
 @brief 퀘 강제 클리어(치트용)
*/
void XQuestObj::SetAllComplete()
{
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj )
	{
		pCondObj->_SetClear( true );
	} END_LOOP;
}

/**
 @brief 퀘스트이름과 현재수행상태를 간단한 문자열로 만들어 pOutStr,에 붙인다.
*/
void XQuestObj::GetQuestObjective( _tstring *pOutStr, bool bIncludeName )
{
	TCHAR szBuff[ 256 ];
	szBuff[0] = 0;
	// 퀘 제목
	if( bIncludeName )
	{
		if( GetpProp()->idName == 0 )
			_tcscpy_s( szBuff, _T( "제목없음" ) );
		else
			XE::ConvertJosaStr( szBuff, XTEXT( GetpProp()->idName ) );
		( *pOutStr ) += szBuff;
		( *pOutStr ) += XE::Format( _T( ":%s(%d)\n" ), GetpProp()->strIdentifier.c_str(), GetpProp()->idProp );
	}
//	( *pOutStr ) += _T( "\n" );
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj )
	{
		(*pOutStr) += _T("  -");
		// 개별 조건들에 대한 문자열 만듬.
		pCondObj->GetQuestObjective( pOutStr );
		(*pOutStr) += _T("\n");
	} END_LOOP;

}

int XQuestObj::GetRewardToAry( XArrayLinearN<XGAME::xReward*, 128> *pOutAry )
{
//	*pOutAry = GetpProp()->aryReward;
	for( auto pReward : GetpProp()->aryReward )
		pOutAry->Add( pReward );
	return pOutAry->size();
}

/**
 @brief 지역오픈류 퀘스트냐.
*/
bool XQuestObj::IsAreaQuest()
{
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj )
	{
		if( pCondObj->IsOpenAreaType() )
			return true;
	} END_LOOP;
	return false;
}

bool XQuestObj::IsReconQuest()
{
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj )
	{
		if( pCondObj->IsReconType() )
			return true;
	} END_LOOP;
	return false;
}

/**
 @brief 특정스팟 클리어 퀘인가
*/
bool XQuestObj::IsSpotClearQuest()
{
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj )
	{
		if( pCondObj->IsSpotClearType() )
			return true;
	} END_LOOP;
	return false;
}

/**
 @brief 
*/
bool XQuestObj::IsQuestType( XGAME::xtQuestCond type )
{
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj ) {
		if( pCondObj->IsQuestType( type ) )
			return true;
	} END_LOOP;
	return false;
}

bool XQuestObj::IsQuestTypeWithParam( XGAME::xtQuestCond type, XGAME::xParam param1, XGAME::xParam param2 )
{
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj ) {
		if( pCondObj->IsQuestTypeWithParam( type, param1, param2 ) )
			return true;
	} END_LOOP;
	return false;
}

// /**
//  @brief 씬 진입액션 조건이 포함된 퀘인가.
///< IsQuestTypeXXX() 시리즈로 통합됨.
// */
// bool XQuestObj::IsEnterScene()
// {
// 	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj ) {
// 		if( pCondObj->IsEnterSceneCond() )
// 			return true;
// 	} END_LOOP;
// 	return false;
// }

/**
 @brief 이 퀘에 지역오픈조건이 있을때 그 지역의 아이디를 반환한다.
 만약 지역오픈조건이 2개이상인경우는 제대로 처리하지 못한다.
*/
ID XQuestObj::GetidOpenAreaCondition()
{
	ID idArea = 0;
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj )
	{
		if( pCondObj->GetpProp()->idWhat == xQC_EVENT_OPEN_AREA )
		{
			XBREAKF( idArea != 0, "지역오픈 조건이 두개이상인 경우 제대로 처리가 안됨." );
			idArea = pCondObj->GetpProp()->paramWhat.dwParam;
		}
	} END_LOOP;
	return idArea;
}

XGAME::xtQuestCond XQuestObj::GetQuestType() 
{
	return m_aryConds[ 0 ]->GetQuestType();
}

ID XQuestObj::GetidSpotOfCond()
{
	return m_aryConds[ 0 ]->GetidSpotbyCond();
}
/**
 @brief type_spot류 퀘스트의 경우 스팟타입을 돌려줌
*/
XGAME::xtSpot XQuestObj::GettypeSpotByCond()
{
	return m_aryConds[ 0 ]->GettypeSpotByCond( nullptr );
}

/**
 @brief 대표 조건이 아이템 획득류일때 아이템 아이디를 얻는다.
*/
ID XQuestObj::GetCondItem() 
{
	if( m_aryConds.size() == 0 )
		return 0;
	return m_aryConds[ 0 ]->GetCondItem();
}

void XQuestObj::GetQuestObjectiveNum( _tstring *pOutStr )
{
	XBREAK( m_aryConds.size() == 0 );
	m_aryConds[0]->GetQuestObjectiveNum( pOutStr );	// 일단 첫번째 조건만 하는걸로.
}

/**
 @brief 퀘설명문자열을 pOut에 담아준다.
 #nick#같은것도 변환해준다.
*/
void XQuestObj::GetQuestDescReplacedToken( _tstring *pOut )
{
	*pOut = XTEXT( GetpProp()->idDesc );
	// #lv_hero#변환
	ReplaceToken( pOut, _T("#param#") );
}

void XQuestObj::ReplaceToken( _tstring *pOut, const _tstring& strToken )
{
	if( strToken == _T("#param#") ) {
		// 조건중에 xQC_EVENT_HERO_LEVEL을 사용하는 조건이 있는지 찾아서 그파라메터를 대체한다.
		XQuestCon* pCondObj = GetCondObjByWhat( XGAME::xQC_EVENT_HERO_LEVEL );
		if( pCondObj == nullptr )
			pCondObj = GetCondObjByWhat( XGAME::xQC_EVENT_HERO_LEVEL_SQUAD );
		if( pCondObj == nullptr )
			pCondObj = GetCondObjByWhat( XGAME::xQC_EVENT_HERO_LEVEL_SKILL );
		if( pCondObj ) {
			LPCTSTR szReplace = XFORMAT( "%d", pCondObj->GetpProp()->paramWhat.nParam );
			XGAME::sReplaceToken( *pOut, strToken, szReplace );
		}
	}
}
/**
 @brief 이 퀘스트의 조건들중 condType을 사용하는 조건을 찾아 리턴한다.
*/
XQuestCon* XQuestObj::GetCondObjByWhat( XGAME::xtQuestCond condType )
{
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj ) {
		if( condType == pCondObj->GetQuestType() )
			return pCondObj;
	}	END_LOOP;
	return nullptr;
}
/**
 @brief typeParam형태의 what파라메터를 쓰면서 파라메터값이 dwParam을 쓰는 조건이 있으면 true
 @param dwParam 0이면 type만 맞아도 true
*/
bool XQuestObj::IsHaveWhatParamTypeCondWithParam( XGAME::xtParam typeParam, DWORD dwParam )
{
	XARRAYLINEAR_LOOP( m_aryConds, XQuestCon*, pCondObj ) {
		auto pPropCond = pCondObj->GetpProp();
		if( pPropCond->typeParamWhat == typeParam )
			if( pPropCond->paramWhat.dwParam == dwParam || dwParam == 0 )
				return true;
	}	END_LOOP;
	return false;
}