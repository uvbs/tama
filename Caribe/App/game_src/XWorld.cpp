#include "stdafx.h"
#include "XWorld.h"
#include "XAccount.h"
#include "XPropWorld.h"
#ifdef _CLIENT
#include "XGame.h"
#include "XSceneWorld.h"
#endif
#include "XSpots.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _CLIENT
XWorld* sGetpWorld()
{
	if( !XAccount::sGetPlayer() )
		return nullptr;
	return XAccount::sGetPlayer()->GetpWorld();
}
#endif // _CLIENT

////////////////////////////////////////////////////////////////
XWorld::XWorld()
{
	Init();
//	m_aryResources.Clear( nullptr );
//	m_aryUsers.Clear( nullptr );
//	m_aryNpcs.Clear( nullptr );
	// 빈 스팟들을 만들어 채워넣고 프로퍼티와 매치시켜둔다.
//	CreateEmptySpots();
	UpdateLastCalc();
	//
}
void XWorld::Destroy()
{
	XLIST4_DESTROY( m_listSpots );
}

void XWorld::Restore()
{
	for( auto pSpot : m_listSpots )
		pSpot->Restore();
}
/**
 @brief 최초 지역만 오픈된리스트에 넣는다.
 @param level 유저가 level일때를 기준으로 오픈된 구름리스트를 갖게 한다.
*/
void XWorld::InitCloudList( int level )
{
	// 모든 지역리스트를 얻는다.
	XArrayLinearN<XPropCloud::xCloud*, 512> ary;
	PROP_CLOUD->GetPropToAry( &ary );
	XARRAYLINEARN_LOOP_AUTO( ary, pProp ) {
		if( pProp->lvArea <= level || pProp->lvArea == 1 )
			m_listOpendArea.Add( pProp->idCloud );
	} END_LOOP;
}
/**
 @brief idClouid지역이 열려있는지 검사한다.
*/
bool XWorld::IsOpenedArea( ID idCloud )
{
	for( auto idOpened : m_listOpendArea ) {
		if( idOpened == idCloud )
			return true;
	}
	return false;
}
bool XWorld::IsOpenedArea( const _tstring& idsArea )
{
	auto pProp = PROP_CLOUD->GetpProp( idsArea );
	if( pProp )
		return IsOpenedArea( pProp->idCloud );
	return false;
}

/**
 @brief 이미 오픈된 구름 리스트를 어레이에 담는다.
*/
int XWorld::GetOpenCloudsToAry( XArrayLinearN<XPropCloud::xCloud*, 512> *pOutAry )
{
	for( auto idOpened : m_listOpendArea ) {
		auto pProp = PROP_CLOUD->GetpProp( idOpened );
		if( pProp ) {
			pOutAry->Add( pProp );
		}
	}
	return pOutAry->size();
}

int XWorld::GetOpenCloudsToAry( XVector<XPropCloud::xCloud*> *pOutAry )
{
	for( auto idOpened : m_listOpendArea ) {
		auto pProp = PROP_CLOUD->GetpProp( idOpened );
		if( pProp ) {
			pOutAry->Add( pProp );
		}
	}
	return pOutAry->size();
}

/**
 @brief pPropCurr의 precedeArea를 depth단계까지 거슬러 올라가며 열린지역이 있으면 true를 리턴한다.
 @param depth 0이상이어야 하며 0이면 pPropCurr의 precedeArea만 검사하고 끝낸다.
*/
bool XWorld::IsOpenPrecedeAreaWithDepth( XPropCloud::xCloud *pPropCurr, int depth )
{
	auto pPropPrecede = PROP_CLOUD->GetpProp( pPropCurr->idsPrecedeArea );
	if( pPropPrecede ) {
		if( IsOpenedArea( pPropPrecede->idCloud ) ) {
			// 선행지역이 열려있는가.
			return true;
		}
		if( depth <= 0 )
			return false;
		return IsOpenPrecedeAreaWithDepth( pPropPrecede, depth - 1 );
	}
	return true;	// precede가 null이면 열린것으로 간주하고 true
}

/**
 @brief 어떤 지역이 닫혀있는데 그지역의 스팟이 열려있으면 삭제시킨다.
*/
void XWorld::UpdateSpots()
{
	std::vector<XSpot*> aryDel(128);
	int idx = 0;
	for( auto pSpot : m_listSpots ) {
		ID idArea = pSpot->GetidArea();
		if( !pSpot->IsEventSpot() ) {
			// 닫힌지역이고 오픈스팟도 아니면 삭제.
			bool bClosedSpot = !PROP_CLOUD->IsOpenedSpot( pSpot->GetidSpot(), idArea, pSpot->GetPosWorld() );
			if( !IsOpenedArea( idArea ) && bClosedSpot )
				aryDel[idx++] = pSpot;
		}
	}
	for( int i = 0; i < idx; ++i ) {
		DestroySpot( aryDel[i] );
	}
}

/**
 보유한 요일스팟중 아무거나 잡아서 주어진 인자로 세팅하고 활성화 시킨다.
*/
XSpotDaily* XWorld::SetActiveDailySpotToRandom( XE::xtDOW dow, int secPass, XSPAcc spAcc )
{
	XArrayLinearN<XSpotDaily*, 32> ary;
	GetSpotsToAry( &ary, XGAME::xSPOT_DAILY );
	XARRAYLINEARN_LOOP( ary, XSpotDaily*, pSpot )
	{
		if( pSpot->IsActive() )		// 이미 활성화 되어있는 스팟이 있으면 새로 만들지 않는다.
			return nullptr;
	} END_LOOP;
	XSpotDaily *pSpot = ary.GetFromRandom();
	if( pSpot == nullptr )
		return nullptr;
	// 작동시작
	pSpot->SetSpot( dow, secPass, spAcc );
	return pSpot;
}

/**
 이미 활성화 되어있는 스페셜스팟이 있는지 확인한다.
*/
// XSpotSpecial* XWorld::GetActivatedSpecialSpot( void )
// {
// 	for( XSpot *pSpot : m_listSpots )
// 	{
// 		if( pSpot->GettypeSpot() == XGAME::xSPOT_SPECIAL )
// 		{
// 			if( pSpot->IsActive() )
// 				return SafeCast<XSpotSpecial*, XSpot*>( pSpot );
// 		}
// 	};
// 	return nullptr;
// }
// /**
//  보유한 스페셜스팟중 아무거나 잡아서 주어진 인자로 세팅하고 활성화 시킨다.
// */
// XSpotSpecial* XWorld::SetActiveSpecialSpotToRandom( int secPass, XSPAcc spAcc )
// {
// 	XArrayLinearN<XSpotSpecial*, 32> ary;
// 	GetSpotsToAry( &ary, XGAME::xSPOT_SPECIAL );
// 	XSpotSpecial *pSpot = nullptr;
// 	if( ary.size() > 0 )
// 		pSpot = ary.GetFromRandom();
// 	if( pSpot == nullptr )
// 		return nullptr;
// 	// 작동시작
// 	pSpot->SetSpot( spAcc->GetLevel(), secPass, spAcc );
// 	return pSpot;
// }

/**
 ,(요일스팟들 중에서) 현재 활성화된 요일스팟을 얻어준다.
 요일스팟은 단 한개만 활성화됨.
*/
XSpotDaily* XWorld::GetActiveDailySpot( void )
{
	for( XSpot *pBaseSpot : m_listSpots )
	{
		if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_DAILY )
		{
 			XSpotDaily *pSpot = SafeCast<XSpotDaily*, XSpot*>( pBaseSpot );
			if( pSpot->GetType() != XGAME::xDS_NONE )
				return pSpot;
		}
	}
	return nullptr;
}

/**
 @brief spotType의 스팟 수를 센다.
 spotType이 0이라면 모든 스팟의 수를 센다.
*/
int XWorld::GetNumSpots( XGAME::xtSpot spotType ) const
{
	if( spotType == XGAME::xSPOT_NONE )
		return m_listSpots.size();
	int num = 0;
	for( XSpot *pBaseSpot : m_listSpots )
	{
		if( pBaseSpot->GettypeSpot() == spotType )
			++num;
	}
	return num;
}

void XWorld::_AddSpot( XSpot *pBaseSpot )
{
	m_listSpots.Add( pBaseSpot );
}

/**
 @brief 활성화된 type스팟들의 평균레벨을 구한다.
*/
int XWorld::GetAvgLevelBySpot( XGAME::xtSpot type )
{
	int sum = 0;
	int cnt = 0;
	for( XSpot *pBaseSpot : m_listSpots )
	{
		if( pBaseSpot->IsActive() && pBaseSpot->GettypeSpot() == type )
		{
			sum += pBaseSpot->GetLevel();
			++cnt;
		}
	}
	if( cnt == 0 )
		return 0;
	return sum / cnt;
}

/**
 @brief 활성화된 type스팟들의 평균 점수를 구한다.
*/
int XWorld::GetAvgScoreBySpot( XGAME::xtSpot type )
{
	int sum = 0;
	int cnt = 0;
	for( XSpot *pBaseSpot : m_listSpots )
	{
		if( pBaseSpot->IsActive() && pBaseSpot->GettypeSpot() == type )
		{
			sum += pBaseSpot->GetScore();
			++cnt;
		}
	}
	if( cnt == 0 )
		return 0;
	return sum / cnt;
}

/**
 주어진 시야내의 typeSpot에 해당하는 모든 스팟의 리스트를 얻어낸다.
*/
int XWorld::GetSpotInSight( XArrayLinearN<XSpot*, 512> *pOutAry,
							XGAME::xtSpot typeSpot,
							const XE::VEC2& vCenter,
							float radius )
{	
	for( XSpot *pSpot : m_listSpots )
	{
		if( pSpot && pSpot->GettypeSpot() == typeSpot )
		{
			float distSq = (pSpot->GetpBaseProp()->vWorld - vCenter).Lengthsq();
			if( distSq <= radius * radius )
				pOutAry->Add( pSpot );
		}
	}
	return pOutAry->size();
}
bool XWorld::IsExistUserSpot(ID idacc)
{
	for (XSpot *pSpot : m_listSpots)
	{
		XBREAK(pSpot == nullptr);
		if (pSpot->GettypeSpot() == XGAME::xSPOT_CASTLE)
		{
			XSpotCastle* pcSpot = (XSpotCastle*)pSpot;
			if (idacc == pcSpot->GetidOwner()) return true;
		}		
	}
	return false;
}
int XWorld::Serialize( XArchive& ar )
{
	int size = ar.size();
	//
	int ver = VER_WORLD_SERIALIZE;
	ar << (BYTE)ver;
//	XBREAK( m_listSpots.size() == 0 );
	XBREAK( m_listSpots.size() > 255 );
	ar << (BYTE)m_listSpots.size();	// 최초 생성계정이면 스팟이 없을수도 있음.
	ar << (WORD)0;
	int idx = 0;
	for( XSpot *pSpot : m_listSpots ) {
		XBREAK( pSpot == nullptr );
		int size2 = ar.size();
		XSpot::sSerialize( ar, pSpot );
		int sizeSpot = ar.size() - size2;
		int sizeWorld = ar.size() - size;
#ifdef _GAME_SERVER
		XTRACE( "%s: %s bytes", pSpot->GetpBaseProp()->strIdentifier.c_str(), XE::NtS( sizeSpot ) );
#endif // _GAME_SERVER
		++idx;
	}
	ar << m_secLastCalc;
	ar << m_listOpendArea;
	// 월드 아카이브의 크기를 리턴한다.
	return ar.size() - size;
}

int XWorld::DeSerialize( XArchive& ar )
{
	int size, ver;
	BYTE b1;
	WORD w1;
	ar >> b1;	ver = (int)b1;
	ar >> b1;	size = (int)b1;
	ar >> w1;
#ifndef _DUMMY_GENERATOR
// 	CONSOLE("ver:%d spot_num:%d", ver, size );
#endif // not _DUMMY_GENERATOR
	for( int i = 0; i < size; ++i ) {
		XSpot *pSpot = XSpot::sCreateDeSerialize( ar, this );
		if (pSpot)
			_AddSpot( pSpot );
			//m_listSpots.Add(pSpot);
		// 없어진 스팟등은 pSpot이 null로 나온다.
	}
	ar >> m_secLastCalc;
	ar >> m_listOpendArea;
	return 1;
}

/**
 @brief idSpot의 스팟을 찾아서 ar로 업데이트 한다.
*/
int XWorld::DeSerializeSpotUpdate( XArchive& ar, ID idSpot )
{
	XSpot *pBaseSpot = GetSpot( idSpot );
	if( XASSERT( pBaseSpot ) )
	{
		return XSpot::sDeSerialize( ar, pBaseSpot );
	}
	return 0;
}

void XWorld::OnCreateSpots( XSPAcc pAcc )
{
	for( auto pSpot : m_listSpots ) {
		pSpot->OnCreate( pAcc );
	}
}

/**
 @brief 스팟들 프로세스
*/
void XWorld::Process( float dt )
{
//	for( XSpot *pSpot : m_listSpots )
	LIST_MANUAL_LOOP_AUTO( m_listSpots, itor, pSpot ) {
		if( pSpot->IsDestroy() == false ) {
			pSpot->Process( dt );
			++itor;
		} else {
			auto type = pSpot->GettypeSpot();
			// 아래 타입스팟만이 리스트에 추가되었다가 이벤트에의해서 사라질수 있다.
#ifndef _xIN_TOOL
			if( XASSERT( type == XGAME::xSPOT_NPC
				|| type == XGAME::xSPOT_CAMPAIGN
				|| type == XGAME::xSPOT_VISIT ) ) 
#endif // not _xIN_TOOL
			{
				m_listSpots.erase( itor++ );
			}
			SAFE_DELETE( pSpot );
		}
			
	} END_LOOP;
}

void XWorld::OnAfterDeSerialize( XDelegateSpot *pDelegator, ID idAcc )
{
	XBREAK( m_secLastCalc == 0 );	// 저장할때 값이 세팅되기때문에 0이되는일은 없어야 함.
#if defined(_DEBUG) && defined(_XUZHU) && defined(WIN32)
	xSec secPass = XTimer2::sGetTime() - m_secLastCalc;
	{
		int h, m, s;
		XSYSTEM::GetHourMinSec( secPass, &h, &m, &s );
		TRACE("%d:%d:%d", h, m, s );
	}
#endif
	for( XSpot *pSpot : m_listSpots )	{
		pSpot->OnAfterDeSerialize( this, pDelegator, idAcc, m_secLastCalc );
	}
	// 마지막 정산(업데이트) 시간
	m_secLastCalc = XTimer2::sGetTime();
}

/**
 광산인덱스 번호로 찾는다.
 0번은 말뚝광산이니 1,2번을 사용한다.
*/
XSpotJewel* XWorld::GetSpotJewelByIdx( int idxJewel )
{
	XBREAK( idxJewel < 0 || idxJewel > 2 );
	for( XSpot *pSpot : m_listSpots )
	{
		if( pSpot->GettypeSpot() == XGAME::xSPOT_JEWEL )
		{
			XSpotJewel *pJewel = SafeCast<XSpotJewel*,XSpot*>( pSpot );
			if( pJewel->GetpProp()->idx == idxJewel )
				return pJewel;
		}
	}
	return nullptr;
}

/**
 만드레이크 자원지 인덱스 번호로 찾는다.
 0번은 말뚝자원지이니 1,2번을 사용한다.
*/
XSpotMandrake* XWorld::GetSpotMandrakeByIdx( int idxMandrake )
{
	XBREAK( idxMandrake < 0 || idxMandrake > 2 );
	for( XSpot *pSpot : m_listSpots )
	{
		if( pSpot->GettypeSpot() == XGAME::xSPOT_MANDRAKE )
		{
			XSpotMandrake *pMandrake = SafeCast<XSpotMandrake*, XSpot*>( pSpot );
			if( pMandrake->GetpProp()->idx == idxMandrake )
				return pMandrake;
		}
	}
	return nullptr;
}

/**
 현재 pWorld의 지역창고에서 약탈할수 있는 모든 자원양
*/
void XWorld::GetNumResLocalStorageByAllSpot( xResourceAry *pOutAry )
{
	pOutAry->assign( pOutAry->size(), 0 );
	for( XSpot *pBaseSpot : m_listSpots ) {
	//	XGAME::xRES_NUM res;
		// 지역창고에 쌓여있는 자원들을 pOutAry에 더한다.
		pBaseSpot->AddLocalStorageResources( pOutAry );
		// 지역창고에 쌓여있는 자원의 종류와 양을 얻어낸다.
	}
}

/**
 모든 지역창고에 rateLoss%손실을 입힌다.
 @param pOutAry가 널이아닐경우 모든스팟 합산으로 각 자원들의 손실양을 담아준다.
*/
void XWorld::LossLocalStorageAll( float rateLoss, std::vector<int> *pOutAry )
{
	XBREAK( pOutAry && (*pOutAry).size() != XGAME::xRES_MAX );
	for( auto pBaseSpot : m_listSpots ) {
		pBaseSpot->LossLocalStorage( rateLoss, pOutAry );
	}
}

/**
 idCloud구름을 오픈(리스트에서 뺌)한다.
*/
void XWorld::DoOpenCloud( ID idCloud )
{
	m_listOpendArea.Add( idCloud );
//	m_listClouds.Del( idCloud );
}
void XWorld::DoCloseArea( ID idArea )
{
	m_listOpendArea.Del( idArea );
}
// 이제 외부에서 스팟삭제시 pSpot->SetbDestroy( true )로 할것.
void XWorld::DestroySpot( XSpot *pSpot )
{
	m_listSpots.DelByID( pSpot->GetidSpot() );
 	SAFE_DELETE( pSpot );
}
void XWorld::DestroySpot( ID idSpot )
{
	XSpot *pSpot = GetSpot( idSpot );
	if( pSpot ) {
 		m_listSpots.DelByID( pSpot->GetidSpot() );
 		SAFE_DELETE( pSpot );
	}
}
/**
 @brief 아직 오픈되지 않은 지역리스트를 얻는다.
*/
int XWorld::GetClosedCloudsToAry( XArrayLinearN<ID, 512> *pOut ) 
{
	// 전체 구름리스트를 얻는다.
	// 이중 오픈하지 않은 리스트만 넣는다.
	XArrayLinearN<ID, 512> aryAll;
 	PROP_CLOUD->GetPropToAry( &aryAll );
	XARRAYLINEARN_LOOP_AUTO( aryAll, idArea ) {
		bool bClosed = true;;
		// 전체 리스트에서 오픈된 지역은 빼고 어레이에 담는다.
		for( auto idOpened : m_listOpendArea ) {
			if( idOpened == idArea ) {
				bClosed = false; 
				break;
			}
		}
		if( bClosed )
			pOut->Add( idArea );
	} END_LOOP;
	return pOut->size();
}
/**
 @brief 오픈한 지역중 가장높은레벨을 얻는다.
*/
int XWorld::GetMaxLevelOpenedArea()
{
	int lvMax = 0;
	for( auto idArea : m_listOpendArea ) {
		auto pProp = PROP_CLOUD->GetpProp( idArea );
		if( XASSERT(pProp) ) {
			if( pProp->lvArea > lvMax )
				lvMax = pProp->lvArea;
		}
	}
	return lvMax;
}
XSpotCastle* XWorld::GetpSpotCastle( ID idSpot ) {
	return GetSpot<XSpotCastle*>( XGAME::xSPOT_CASTLE, idSpot );
}
XSpotJewel* XWorld::GetpSpotJewel( ID idSpot ) {
	return GetSpot<XSpotJewel*>( XGAME::xSPOT_JEWEL, idSpot );
}
XSpotSulfur* XWorld::GetpSpotSulfur( ID idSpot ) {
	return GetSpot<XSpotSulfur*>( XGAME::xSPOT_SULFUR, idSpot );
}
XSpotMandrake* XWorld::GetpSpotMandrake( ID idSpot ) {
	return GetSpot<XSpotMandrake*>( XGAME::xSPOT_MANDRAKE, idSpot );
}
XSpotNpc* XWorld::GetpSpotNpc( ID idSpot ) {
	return GetSpot<XSpotNpc*>( XGAME::xSPOT_NPC, idSpot );
}
XSpotDaily* XWorld::GetpSpotDaily( ID idSpot ) {
	return GetSpot<XSpotDaily*>( XGAME::xSPOT_DAILY, idSpot );
}
// XSpotSpecial* XWorld::GetpSpotSpecial( ID idSpot ) {
// 	return GetSpot<XSpotSpecial*>( XGAME::xSPOT_SPECIAL, idSpot );
// }
XSpotCampaign* XWorld::GetpSpotCampaign( ID idSpot ) {
	return GetSpot<XSpotCampaign*>( XGAME::xSPOT_CAMPAIGN, idSpot );
}
XSpotVisit* XWorld::GetpSpotVisit( ID idSpot ) {
	return GetSpot<XSpotVisit*>( XGAME::xSPOT_VISIT, idSpot );
}
XSpotCash* XWorld::GetpSpotCash( ID idSpot ) {
	return GetSpot<XSpotCash*>( XGAME::xSPOT_CASH, idSpot );
}
/**
 @brief 
*/
XSpot* XWorld::GetSpot( ID idSpot ) 
{
	if( idSpot == 0 )
		return nullptr;
	for( XSpot *pSpot : m_listSpots ) {
		if( pSpot->GetpBaseProp()->idSpot == idSpot )
			return pSpot;
	};
	return nullptr;

}
/**
 @brief pPropArea 지역아래에 있는 스팟들에게 이벤트를 날린다.
*/
void XWorld::DispatchSpotEvent( XPropCloud::xCloud* pPropArea )
{
	XVector<ID> ary;
	pPropArea->GetSpotsToAry( &ary );
	///< 
	for( const auto idSpot : ary ) {
		auto pBaseSpot = GetpSpot( idSpot );
		if( pBaseSpot ) {
			pBaseSpot->OnOpenedArea( pPropArea );
		}
	}
}
