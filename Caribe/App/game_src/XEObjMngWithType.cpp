#include "stdafx.h"
#include "XEObjMngWithType.h"
#include "XBaseUnit.h"
#include "XWndBattleField.h"
#include "XSquadObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XSKILL;
using namespace XGAME;

XEObjMngWithType* XEObjMngWithType::s_pInstance = nullptr;

////////////////////////////////////////////////////////////////
/**
 @brief 고정된 크기의 오브젝트 풀을 만들고 타입별 분류를 위해 배열형 리스트를 타입수만큼 만든다.
 @param maxObj 오브젝트 최대 개수
 @param maxType 오브젝트 타입의 최대 개수
*/
XEObjMngWithType::XEObjMngWithType( int maxObj, int maxType )
	: XEObjMng( maxObj )
{
	XBREAK( s_pInstance != nullptr );
	s_pInstance = this;
	Init();
	//
}

void XEObjMngWithType::Destroy()
{
	DestroyAllObj();
	s_pInstance = nullptr;
}

void XEObjMngWithType::Release()
{
	m_listUnits.clear();
	m_listEtc.clear();
	// 리스트가 클리어되면서 레퍼런스객체들이 반환된다.
	XEObjMng::Release();
}

void XEObjMngWithType::DestroyAllObj( void )
{
	m_listUnits.clear();
	m_listEtc.clear();
	XEObjMng::DestroyAllObj();
}
/**
 @brief 오브젝트 추가 명령이 들어오면 독자적인 분류배열에도 넣는다.
*/
ID XEObjMngWithType::AddUnit( const XSPUnit& spObj )
{
	XBREAK( spObj->GetType() == XGAME::xUNIT_NONE );	// 오브젝트는 반드시 타입이 있어야 한다.
	m_listUnits.push_back( spObj );
	return XEObjMng::Add( spObj );
}

void XEObjMngWithType::AddUnit( ID idObj, const XSPUnit& spObj )
{
	XBREAK( spObj->GetType() == XGAME::xUNIT_NONE );	// 오브젝트는 반드시 타입이 있어야 한다.
	m_listUnits.push_back( spObj );
	XEObjMng::Add( idObj, spObj );
}

ID XEObjMngWithType::Add( const XSPWorldObj& spObj )
{
	XBREAK( spObj->GetType() == XGAME::xOT_NONE );	// 오브젝트는 반드시 타입이 있어야 한다.
	if( spObj->GetType() == XGAME::xOT_UNIT )
	{
		XSPUnit spUnit = std::static_pointer_cast<XBaseUnit>( spObj );
		m_listUnits.push_back( spUnit );
	} else
		m_listEtc.push_back( spObj );
	return XEObjMng::Add( spObj );
}

void XEObjMngWithType::Add( ID idObj, const XSPWorldObj& spObj )
{
	XBREAK( spObj->GetType() == XGAME::xOT_NONE );	// 오브젝트는 반드시 타입이 있어야 한다.
	if( spObj->GetType() == XGAME::xOT_UNIT )
	{
		XSPUnit spUnit = std::static_pointer_cast<XBaseUnit>( spObj );
		m_listUnits.push_back( spUnit );
	} else
		m_listEtc.push_back( spObj );
	XEObjMng::Add( idObj, spObj );
}


/**
 @brief pObj가 FrameMove()에서 파괴되기전에 호출된다.
 유닛을 제외한 오브젝트 삭제용
*/
void XEObjMngWithType::OnDestroyObj( XEBaseWorldObj *pObj )
{
	int type = pObj->GetType();
	XBREAK( type == XGAME::xOT_NONE );	// 오브젝트는 반드시 타입이 있어야 한다.
	BOOL bFound = FALSE;
	if( type == XGAME::xOT_UNIT )	{
		for( auto itor = m_listUnits.begin(); itor != m_listUnits.end(); )		{
			auto& spUnit = (*itor).lock();
			if( spUnit->GetsnObj() == pObj->GetsnObj() )			{
				// 소유권 반환
				spUnit.reset();
				m_listUnits.erase( itor++ );
				bFound = TRUE;
				break;
			}
			else
				++itor;
		}
	} else
	{
		for( auto itor = m_listEtc.begin(); itor != m_listEtc.end(); )		{
			auto& spEtc = (*itor).lock();
			if( spEtc->GetsnObj() == pObj->GetsnObj() )			{
				// 소유권 반환
				spEtc.reset();
				m_listEtc.erase( itor++ );
				bFound = TRUE;
				break;
			}
			else
				++itor;
		}
	}
	// 이런경우는 없어야 한다.
	XBREAK( bFound == FALSE );
}


/**
 @brief type을이용해서 더 빨리 찾는다.
*/
XSPWorldObj XEObjMngWithType::Find( int type, ID snObj )
{
	if( snObj == 0 )
		return nullptr;
	return XEObjMng::Find( snObj );
// 	if( type == XGAME::xOT_UNIT )
// 	{
// 
// 	}
// 	std::list<XSPUnit>::iterator itor;
// 	for( itor = m_listUnits.begin(); itor != m_listUnits.end(); ++itor )
// 	{
// 		if( (*itor)->GetsnObj() == snObj )
// 			return *itor;
// 	}
// 	// 못찾았으면 nullptr을 리턴하는데 에러검증을 위해 원본 리스트에서도 찾아보고 있다면 잘못된것이다.
// 	XBREAKF( XEObjMng::Find( snObj ), "not found obj:type=%d, id=%d", type, snObj);
	return nullptr;
}

void XEObjMngWithType::DestroyObjWithType( int type, ID snObj )
{
	if( snObj == 0 )
		return;
	auto spObj = Find( type, snObj );
	if( spObj ) {
		spObj->SetDestroy( 1 );
	}
}


/**
 @brief 가장 가까운 오브젝트를 찾는다. 조건은 함수포인터로 전달한다.
 @param radius 특정거리내에서 검색한다. 0은 거리제한이 없다.
*/
XSPUnit XEObjMngWithType::FindNearObjByFunc( XEBaseWorldObj *pSrcObj, 
												const XE::VEC3& vwPos, 
												float meterRadius, 
							BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ) )
{
	float radius = xMETER_TO_PIXEL(meterRadius);
	float minDist = 99999999.f;
	XSPUnit spMinObj( nullptr );
	float minDistAll = 99999999.f;
	XSPUnit spMinObjAll;
	for( auto itor = m_listEtc.begin(); itor != m_listEtc.end(); ++itor ) {
		auto& spEtc = (*itor).lock();
		// 사용자 정의 조건함수를 호출해서 통과한것만 거리테스트를 한다.
		if( pfuncFilter( pSrcObj, spEtc.get() ) ) {
			const XE::VEC3 vDist = spEtc->GetvwPos() - vwPos;
			float distsq = vDist.Lengthsq();
			// 주어진 거리내에 있는가. 0은 거리제한 없음.
			if( (distsq <= radius * radius) || radius == 0 ) {
				if( distsq < minDist ) {
					minDist = distsq;
					spMinObj = std::static_pointer_cast<XBaseUnit>(spEtc);
				}
			} else {
				// 못찾을경우를 대비해서 범위바깥의 오브젝트중에 가장 가까운것도 찾는다.
				minDistAll = distsq;
				spMinObjAll = std::static_pointer_cast<XBaseUnit>(spEtc);
			}
		}
	}
	// 만약 범위내에서 못찾았으면 범위바깥에서 찾은거라도 돌려준다.
	if( spMinObj == nullptr ) {
		if( spMinObjAll != nullptr )
			return spMinObjAll;
		else {
			// 에러검증. 여기서 못찾았는데 상위함수로 찾으면 있는 경우는 잘못된것.
			//			XBREAK( XEObjMng::FindNearObjByFilter( vwPos, radius, bitSide ) != nullptr );
		}
	}
	return spMinObj;
}

/**
 @brief XBaseUnit버전
*/
XSPUnit XEObjMngWithType::FindNearUnitByFunc( XBaseUnit *pSrcObj,
											const XE::VEC3& vwPos,
											float meterRadius,
											bool bFindOutRange,
								BOOL( *pfuncFilter )( XBaseUnit*, XBaseUnit* ) )
{
	float radius = xMETER_TO_PIXEL( meterRadius );
	float minDist = 99999999.f;
	XSPUnit spMinObj( nullptr );
	float minDistAll = 99999999.f;
	XSPUnit spMinObjAll;
	for( auto itor = m_listUnits.begin(); itor != m_listUnits.end(); ++itor ) {
		auto& spUnit = (*itor).lock();
		// 사용자 정의 조건함수를 호출해서 통과한것만 거리테스트를 한다.
		if( pfuncFilter( pSrcObj, spUnit.get() ) )	{
			XE::VEC3 vDist = spUnit->GetvwPos() - vwPos;
			float distsq = vDist.Lengthsq();
			// 주어진 거리내에 있는가. 0은 거리제한 없음.
			if( ( distsq <= radius * radius ) || radius == 0 ) {
				if( distsq < minDist ) {
					minDist = distsq;
					spMinObj = spUnit;
				}
			} else {
				// 못찾을경우를 대비해서 범위바깥의 오브젝트중에 가장 가까운것도 찾는다.
				minDistAll = distsq;
				spMinObjAll = spUnit;
			}
		}
	}
	// 만약 범위내에서 못찾았으면 범위바깥에서 찾은거라도 돌려준다.
	if( spMinObj == nullptr ) {
		if( spMinObjAll != nullptr && bFindOutRange )
			return spMinObjAll;
		else {
			// 에러검증. 여기서 못찾았는데 상위함수로 찾으면 있는 경우는 잘못된것.
		}
	}
	return spMinObj;
}


/**
 @brief 조건함수를 이용해 더 ~한 오브젝트를 찾는다.
	주의: m_listEtc의 오브젝트만 찾는다.
*/
XSPWorldObj XEObjMngWithType::FindNearObjByMore( XEBaseWorldObj *pSrcObj,
												const XE::VEC3& vwPos,
												float radius,
	BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ),
	BOOL( *pfuncCompare )( XEBaseWorldObj*, XEBaseWorldObj*, XEBaseWorldObj* ))
{
	XSPWorldObj spCompObj;
	float minDist = 99999999.f;
	XSPWorldObj spCompObjForAll;
	for( auto itor = m_listEtc.begin(); itor != m_listEtc.end(); ++itor ) {
		auto& spEtc = (*itor).lock();
		BOOL bCondition = FALSE;
		// 사용자 정의 필터함수를 호출해서 TRUE인것만 대상으로 한다.
		if( spEtc->GetsnObj() != pSrcObj->GetsnObj() 
				&& pfuncFilter( pSrcObj, spEtc.get() ) ) {
			if( radius > 0 ) {
				// 일단 주어진 거리내에 있어야 한다.
				const XE::VEC3 vDist = spEtc->GetvwPos() - vwPos;
				float distsq = vDist.Lengthsq();
				if( distsq <= radius * radius ) {
					bCondition = TRUE;
				} else	{
					// 거리밖에 있는것들중에서 젤 가까운것도 찾아둔다.
					if( distsq < minDist ) {	// 자기자신은 검색하지 않는다.
						minDist = distsq;
						spCompObjForAll = spEtc;
					}
				}
			} else {
				// 범위가 없으면 거리 검사 안함.
				bCondition = TRUE;
			}
			if( bCondition ) {
				if( spCompObj == nullptr )
					spCompObj = spEtc;
				else
					// 사용자 정의 비교함수를 호출해서 TRUE인것만 취한다.
					if( pfuncCompare( pSrcObj, spEtc.get(), spCompObj.get() ) )
						spCompObj = spEtc;
			}
		}
	}
	// 거리내에 조건을 만족하는게 없다.
	if( spCompObj == nullptr )	{
		// 그렇다면 거리밖에있는것들중에 가장가까운것을 리턴한다.
		if( spCompObjForAll != nullptr )
			return spCompObjForAll;
		else
			return spCompObj;	// 거리밖에서 조차 없다면 그냥 널리턴
	}
	return spCompObj;
}

/**
 @brief XBaseUnit버전
*/
XSPUnit XEObjMngWithType::FindNearUnitByMore( XBaseUnit *pSrcObj,
												const XE::VEC3& vwPos,
												float radius,
	BOOL( *pfuncFilter )( XBaseUnit*, XBaseUnit* ),
	BOOL( *pfuncCompare )( XBaseUnit*, XBaseUnit*, XBaseUnit* ))
{
	XSPUnit spCompObj;
	float minDist = 99999999.f;
	XSPUnit spCompObjForAll;
	for( auto itor = m_listUnits.begin(); itor != m_listUnits.end(); ++itor ) {
		auto& spUnit = (*itor).lock();
		BOOL bCondition = FALSE;
		// 사용자 정의 필터함수를 호출해서 TRUE인것만 대상으로 한다.
		if( spUnit->GetsnObj() != pSrcObj->GetsnObj() 
				&& pfuncFilter( pSrcObj, spUnit.get() ) ) {
			if( radius > 0 ) {
				// 일단 주어진 거리내에 있어야 한다.
				XE::VEC3 vDist = spUnit->GetvwPos() - vwPos;
				float distsq = vDist.Lengthsq();
				if( distsq <= radius * radius ) {
					bCondition = TRUE;
				} else {
					// 거리밖에 있는것들중에서 젤 가까운것도 찾아둔다.
					if( distsq < minDist ) {	// 자기자신은 검색하지 않는다.
						minDist = distsq;
						spCompObjForAll = spUnit;
					}
				}
			} else {
				// 범위가 없으면 거리 검사 안함.
				bCondition = TRUE;
			}
			if( bCondition ) {
				if( spCompObj == nullptr ) {
					spCompObj = spUnit;
				} else
				// 사용자 정의 비교함수를 호출해서 TRUE인것만 취한다.
				if( pfuncCompare( pSrcObj, spUnit.get(), spCompObj.get() ) ) {
					spCompObj = spUnit;
				}
			}
		}
	}
	// 거리내에 조건을 만족하는게 없다.
	if( spCompObj == nullptr ) {
		// 그렇다면 거리밖에있는것들중에 가장가까운것을 리턴한다.
		if( spCompObjForAll != nullptr )
			return spCompObjForAll;
		else
			return spCompObj;	// 거리밖에서 조차 없다면 그냥 널리턴
	}
	return spCompObj;
}

/**
 @brief 조건에 맞는 유닛을 찾아낸다.
 시전대상을 중심으로(시전대상을 포함하거나/제외하거나) 반경내 bitSideFilter에 맞는 오브젝트 invokeNumApply개의 리스트를 요청한다
* numApply가 0이면 개수제한이 없다.
*/
// int XEObjMngWithType::GetListUnitRadius( XArrayLinearN<XBaseUnit*, 512> *plistOutInvokeTarget,
// 									XEBaseWorldObj *pBaseTarget,
// 									const XE::VEC2& vCenter,
// 									float pixelRadius,
// 									BIT bitSideFilter,
// 									int numApply,
// 									BOOL bIncludeCenter,
// 									BIT bitLive )		// 생존필터  
// {
// 	int cnt = 0;
// 	for( auto itor = m_listUnits.rbegin(); itor != m_listUnits.rend(); ++itor )	{
// 		auto pUnit = SafeCast<XBaseUnit*>( (*itor).get() );
// 		XBREAK( pUnit == nullptr );
// 		if( ( pUnit->GetCamp() & bitSideFilter ) == 0 )
// 			continue;
// 		if( (bitLive & XSKILL::xTL_LIVE) == 0 && pUnit->IsLive() )
// 			continue;
// 		if( ( bitLive & XSKILL::xTL_DEAD ) == 0 && pUnit->IsDead() )
// 			continue;
// 		if( numApply != 0 && cnt >= numApply )
// 			return plistOutInvokeTarget->size();
// 		// 시전대상을 포함하지 않는 조건일때 유닛이 시전대상이면 스킵
// 		if( pBaseTarget &&
// 			bIncludeCenter == FALSE && 
// 			pBaseTarget->GetsnObj() == pUnit->GetsnObj() )
// 			continue;
// 		XE::VEC2 vDist = pUnit->GetvwPos().ToVec2() - vCenter;
// 		float distsq = vDist.Lengthsq();
// 		if( distsq <= pixelRadius * pixelRadius )
// 		{
// 			plistOutInvokeTarget->Add( pUnit );
// 			++cnt;
// 		}
// 	}
// 	
// 	return plistOutInvokeTarget->size();
// }

/**
 @brief pCenter(or vCenter)를 중심으로 radius범위내의 유닛들을 얻어 어레이에 담는다.
 @return 얻어낸 타겟수
 @param pCenter 중심타겟. null이면 vCenter를 사용한다.
 @param vCenter 중심좌표. pCenter가 있다면 사용되지 않는다.
 @param pixelRadius 픽셀단위 반지름.
 @param bitSideFilter 검색해야할 진영
 @param numCost 총 코스트값. 각 유닛은 크기 코스트가 있으며 타겟이 검색될때마다 numCost에서 차감한다. 0이되면 더이상 담지 않는다. 차감한 코스값이 마이너스가 될수는 없다.
                0은 코스트에 관계없이 무조건 타겟1개만 검색. -1은 무효값. 
 @param bIncludeCenter 중심타겟(pCenter가 있을경우)을 대상에 포함할지 말지. pCenter가 없다면 이 옵션은 무시된다.
*/
int XEObjMngWithType::GetListUnitRadius2( XVector<XSPUnit> *pOutAry,
																					XEBaseWorldObj *pCenter,
																					const XE::VEC2& vCenter,
																					float pixelRadius,
																					BIT bitSideFilter,
																					int numCost,
																					bool bIncludeCenter,
																					BIT bitFlag ) const		// 생존필터  
{
	if( numCost < 0 )
		return 0;
	//
	XVector<XSPUnit> aryInNew;				// pOutAry에 이미 타겟이 있는채로 들어왔을때 그것은 제한 리스트
	XVector<XSPUnit> aryIn = *pOutAry;	// 기존타겟에 반경검사타겟까지 포함.
	int costSum = 0;
	for( auto spwUnit : m_listUnits ) {
		auto spUnit = spwUnit.lock();
		XBREAK( spUnit == nullptr );
		if( (spUnit->GetCamp() & bitSideFilter) == 0 )
			continue;
		if( (bitFlag & XSKILL::xTF_LIVE) == 0 && spUnit->IsLive() )
			continue;
		if( (bitFlag & XSKILL::xTF_DEAD) == 0 && spUnit->IsDead() )
			continue;
		// 시전대상을 포함하지 않는 조건일때 유닛이 시전대상이면 스킵
		if( pCenter
				&& pCenter->GetsnObj() == spUnit->GetsnObj()		// spUnit이 중심타겟일때
				&& bIncludeCenter == false )										// 중심타겟안포함 옵션이면 스킵한다.
			continue;
		if( bitFlag & XSKILL::xTF_DIFF_SQUAD ) {
			// spUnit이 이미 찾은 유닛과 같은 부대면 스킵
			bool bExisted = false;
			for( auto& spFinded : aryIn ) {
				if( spUnit->GetpSquadObj()->GetsnSquadObj() == spFinded->GetpSquadObj()->GetsnSquadObj() ) {
					bExisted = true;
					break;
				}
			}
			if( bExisted )
				continue;
		}
		const XE::VEC2 vDist = spUnit->GetvwPos().ToVec2() - vCenter;
		const float distsq = vDist.Lengthsq();
		// 코스트와 관계없이 일단 범위안에 들어가는 타겟은 모두 담은후 거기서 다시 코스트에 따라 랜덤으로 꺼낸다.
		if( distsq <= pixelRadius * pixelRadius ) {
			aryIn.Add( spUnit );
			aryInNew.Add( spUnit );
			costSum += spUnit->GetSizeCost();		// 배열에 들어간 유닛들의 토탈코스트
		} // in radius
	} // for
	// 목표코스트를 다 못채웠으면 랜덤으로 뽑아낼필요 없음.
	if( costSum <= numCost )
		return pOutAry->Size();
	int currCost = numCost;
// 	XVector<XSPUnit> ary = *pOutAry;
// 	pOutAry->Clear();
	// 영역안에 들어온 타겟들을 대상으로 다시 코스트에 따라 실제 타겟을 선정한다.
	while( aryInNew.Size() > 0 ) {
		auto spUnit = aryInNew.PopFromRandom();
		if( numCost == 0 ) {
			if( pOutAry->Size() == 0 )
				pOutAry->Add( spUnit );
			return 1;
		} else {
			const auto costUnit = spUnit->GetSizeCost();
			if( costUnit <= currCost ) {
				// 유닛 코스트가 남은 코스트를 깔수 있을때만.
				currCost -= costUnit;
				XBREAK( currCost < 0 );
				pOutAry->Add( spUnit );
				if( currCost == 0 )				// 코스트를 다 채웠으면 리턴.
					return pOutAry->Size();
			}
		}
	}
	return pOutAry->size();
}
// int XEObjMngWithType::GetListUnitRadius2( XVector<XSPUnit> *pOutAry,
// 																				 XEBaseWorldObj *pCenter,
// 																				 const XE::VEC2& vCenter,
// 																				 float pixelRadius,
// 																				 BIT bitSideFilter,
// 																				 int numCost,
// 																				 bool bIncludeCenter,
// 																				 BIT bitFlag ) const		// 생존필터  
// {
// 	if( numCost < 0 )
// 		return 0;
// 	int currCost = numCost;
// 	for( auto spUnit : m_listUnits ) {
// 		XBREAK( spUnit == nullptr );
// 		if( (spUnit->GetCamp() & bitSideFilter) == 0 )
// 			continue;
// 		if( (bitFlag & XSKILL::xTF_LIVE) == 0 && spUnit->IsLive() )
// 			continue;
// 		if( (bitFlag & XSKILL::xTF_DEAD) == 0 && spUnit->IsDead() )
// 			continue;
// 		// 시전대상을 포함하지 않는 조건일때 유닛이 시전대상이면 스킵
// 		if( pCenter 
// 				&& pCenter->GetsnObj() == spUnit->GetsnObj()		// spUnit이 중심타겟일때
// 				&& bIncludeCenter == false )										// 중심타겟안포함 옵션이면 스킵한다.
// 			continue;
// 		if( bitFlag & XSKILL::xTF_DIFF_SQUAD ) {
// 			// spUnit이 이미 찾은 유닛과 같은 부대면 스킵
// 			bool bExisted = false;
// 			for( auto& spFinded : *pOutAry ) {
// 				if( spUnit->GetpSquadObj()->GetsnSquadObj() == spFinded->GetpSquadObj()->GetsnSquadObj() ) {
// 					bExisted = true;
// 					break;
// 				}
// 			}
// 			if( bExisted )
// 				continue;
// 		}
// 		const XE::VEC2 vDist = spUnit->GetvwPos().ToVec2() - vCenter;
// 		const float distsq = vDist.Lengthsq();
// 		if( distsq <= pixelRadius * pixelRadius ) {
// 			// 코스트가 0이면 코스트에 관계없이 타겟하나만 선택한다.
// 			if( numCost == 0 ) {
// 				pOutAry->Add( spUnit );
// 				return 1;			
// 			} else {
// 				const auto costUnit = spUnit->GetSizeCost();
// 				if( costUnit <= currCost ) {
// 					// 유닛 코스트가 남은 코스트를 깔수 있을때만.
// 					currCost -= costUnit;
// 					XBREAK( currCost < 0 );
// 					pOutAry->Add( spUnit );
// 					if( currCost == 0 )				// 코스트를 다 채웠으면 리턴.
// 						return pOutAry->Size();
// 				}
// 			}
// 		} // in radius
// 	} // for
// 
// 	return pOutAry->size();
// }
/**
 @brief 윈도우좌표vsPos에서 픽킹되는 bitCamp진영 유닛을 찾아낸다.
 @param snExcludeSquad 검색에서 제외해야할 부대
 @param pOutPickExclude snExcludeSquad로 제외시킨부대가 먼저 픽킹된다면 TRUE가 된다.
*/
XSPUnit XEObjMngWithType::GetPickUnit( XWndBattleField *pWndWorld, 
										const XE::VEC2& vsPos, 
										BIT bitCamp,
										ID snExcludeSquad/*=0*/,
										BOOL *pOutPickExclude/*=nullptr*/)
{
	for( auto itor = m_listUnits.begin(); itor != m_listUnits.end(); ++itor )	{
		auto pUnit = SafeCast<XBaseUnit*>( ( *itor ).lock().get() );
		if( XBREAK( pUnit == nullptr ) )
			return nullptr;
		if( pUnit->IsDead() )
			continue;;
		// bitCamp와 같은진영만 검사한다. 제외부대가 지정되어있을경우 해당부대의 유닛은 제외한다.
		if( pUnit->GetCamp().IsFriendly( bitCamp ) )	{
			XE::VEC2 vPos = pUnit->GetvsPos();
			XE::VEC2 vLT = vPos + pUnit->GetBoundBox().vLT * pWndWorld->GetscaleCamera();
			XE::VEC2 vRB = vPos + pUnit->GetBoundBox().vRB * pWndWorld->GetscaleCamera();
			if( XE::IsInside( vsPos, vLT, vRB ) )			{
				if( snExcludeSquad == 0 ||
					( snExcludeSquad && pUnit->GetpSquadObj()->GetsnSquadObj() != snExcludeSquad ) )
					return pUnit->GetThisUnit();
				else		{
					if( pOutPickExclude ) {
						*pOutPickExclude = TRUE;	// 제외된 부대가 선택되었다.
					}
				}
			}
		}
	}
	return nullptr;
}

void XEObjMngWithType::DrawVisible( XEWndWorld *pWndWorld, const XVector<XEBaseWorldObj*>& aryVisible )
//void XEObjMngWithType::DrawVisible( XEWndWorld *pWndWorld, const XList4<XEBaseWorldObj*>& aryVisible )
{
	XPROF_OBJ_AUTO();
	for( auto pObj : aryVisible ) {
		if( pWndWorld ) {	// 바닥에 찍히는 오브젝트먼저 찍는다.
			if( pObj->GetType() == XGAME::xOT_FLOOR_OBJ ) {
				// 각 오브젝트들의 월드좌표를 스크린좌표로 변환하여 draw를 시킴
				float scale = 1.f;
				// 투영함수에서 카메라 스케일값을 받아온다.
				XE::VEC2 vsPos;
				{
					XPROF_OBJ( "projection" );
					vsPos = pWndWorld->GetPosWorldToScreen( pObj->GetvwPos(), &scale );
				} {
					XPROF_OBJ( "draw each" );
					pObj->Draw( vsPos, scale );
				}
			} else
			// 유닛의 경우는 그림자를 먼저 찍어주도록 한다.
			if( pObj->GetType() == XGAME::xOT_UNIT ) {
				XPROF_OBJ( "draw shadow" );
				XBaseUnit *pUnit = SafeCast<XBaseUnit*, XEBaseWorldObj*>( pObj );
				if( XASSERT( pUnit ) ) {
					// 각 오브젝트들의 월드좌표를 스크린좌표로 변환하여 draw를 시킴
					float scale = 1.f;
					// 투영함수에서 카메라 스케일값을 받아온다.
					XE::VEC2 vsPos;
					{
						//						XPROF_OBJ( "projection" );
						vsPos = pWndWorld->GetPosWorldToScreen( pObj->GetvwPos(), &scale );
					}
					pUnit->DrawShadow( vsPos, scale );
				}
			}
		} else {
			XE::VEC2 vs = pObj->GetvwPos().ToVec2();
			pObj->Draw( vs );
		}
	}
	// 일반 오브젝트들을 찍는다.
	for( auto pObj : aryVisible ) {
		if( pWndWorld ) {	
			// 바닥오브젝트가 아닌것만 찍는다.
			if( pObj->GetType() != XGAME::xOT_FLOOR_OBJ && pObj->GetType() != XGAME::xOT_UI ) {
				// 각 오브젝트들의 월드좌표를 스크린좌표로 변환하여 draw를 시킴
				float scale = 1.f;
				// 투영함수에서 카메라 스케일값을 받아온다.
				XE::VEC2 vsPos;
				{
					XPROF_OBJ( "projection" );
					vsPos = pWndWorld->GetPosWorldToScreen( pObj->GetvwPos(), &scale );
				}
				{
					XPROF_OBJ( "draw each" );
					pObj->Draw( vsPos, scale );
				}
			}
		} else {
			XE::VEC2 vs = pObj->GetvwPos().ToVec2();
			pObj->Draw( vs );
		}
	}
	// UI오브젝트를 찍는다.
	for( auto pObj : aryVisible ) {
		if( pWndWorld ) {
			// 바닥오브젝트가 아닌것만 찍는다.
			if( pObj->GetType() == XGAME::xOT_UI ) {
				// 각 오브젝트들의 월드좌표를 스크린좌표로 변환하여 draw를 시킴
				float scale = 1.f;
				// 투영함수에서 카메라 스케일값을 받아온다.
				XE::VEC2 vsPos;
				{
					XPROF_OBJ( "projection" );
					vsPos = pWndWorld->GetPosWorldToScreen( pObj->GetvwPos(), &scale );
				}
				{
					XPROF_OBJ( "draw each" );
					pObj->Draw( vsPos, scale );
				}
			}
		} else {
			XE::VEC2 vs = pObj->GetvwPos().ToVec2();
			pObj->Draw( vs );
		}
	}
}

