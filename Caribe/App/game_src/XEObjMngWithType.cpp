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

XEObjMngWithType* XEObjMngWithType::s_pInstance = NULL;

////////////////////////////////////////////////////////////////
/**
 @brief 고정된 크기의 오브젝트 풀을 만들고 타입별 분류를 위해 배열형 리스트를 타입수만큼 만든다.
 @param maxObj 오브젝트 최대 개수
 @param maxType 오브젝트 타입의 최대 개수
*/
XEObjMngWithType::XEObjMngWithType( int maxObj, int maxType )
	: XEObjMng( maxObj )
{
	XBREAK( s_pInstance != NULL );
	s_pInstance = this;
	Init();
	//
}

void XEObjMngWithType::Destroy()
{
	DestroyAllObj();
	s_pInstance = NULL;
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
// 	for (int i = 0; i < m_listTypes.GetMax(); ++i)
// 	{
// 		m_listTypes[i].clear();		// 리스트가 클리어되면서 레퍼런스객체들이 반환된다.
// 	}
	XEObjMng::DestroyAllObj();
}
/**
 @brief 오브젝트 추가 명령이 들어오면 독자적인 분류배열에도 넣는다.
*/
ID XEObjMngWithType::AddUnit( const UnitPtr& spObj )
{
	XBREAK( spObj->GetType() == XGAME::xUNIT_NONE );	// 오브젝트는 반드시 타입이 있어야 한다.
	m_listUnits.push_back( spObj );
	return XEObjMng::Add( spObj );
}

void XEObjMngWithType::AddUnit( ID idObj, const UnitPtr& spObj )
{
	XBREAK( spObj->GetType() == XGAME::xUNIT_NONE );	// 오브젝트는 반드시 타입이 있어야 한다.
	m_listUnits.push_back( spObj );
	XEObjMng::Add( idObj, spObj );
}

ID XEObjMngWithType::Add( const WorldObjPtr& spObj )
{
	XBREAK( spObj->GetType() == XGAME::xOT_NONE );	// 오브젝트는 반드시 타입이 있어야 한다.
	if( spObj->GetType() == XGAME::xOT_UNIT )
	{
		UnitPtr spUnit = std::static_pointer_cast<XBaseUnit>( spObj );
		m_listUnits.push_back( spUnit );
	} else
		m_listEtc.push_back( spObj );
	return XEObjMng::Add( spObj );
}

void XEObjMngWithType::Add( ID idObj, const WorldObjPtr& spObj )
{
	XBREAK( spObj->GetType() == XGAME::xOT_NONE );	// 오브젝트는 반드시 타입이 있어야 한다.
	if( spObj->GetType() == XGAME::xOT_UNIT )
	{
		UnitPtr spUnit = std::static_pointer_cast<XBaseUnit>( spObj );
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
	if( type == XGAME::xOT_UNIT )
	{
		std::list<UnitPtr>::iterator itor;
		for( itor = m_listUnits.begin(); itor != m_listUnits.end(); )
		{
			if( ( *itor )->GetsnObj() == pObj->GetsnObj() )
			{
				// 소유권 반환
				( *itor ).reset();
				m_listUnits.erase( itor++ );
				bFound = TRUE;
				break;
			}
			else
				++itor;
		}
	} else
	{
		std::list<WorldObjPtr>::iterator itor;
		for( itor = m_listEtc.begin(); itor != m_listEtc.end(); )
		{
			if( ( *itor )->GetsnObj() == pObj->GetsnObj() )
			{
				// 소유권 반환
				( *itor ).reset();
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
WorldObjPtr XEObjMngWithType::Find( int type, ID idObj )
{
	if( idObj == 0 )
		return NULL;
	if( type == XGAME::xOT_UNIT )
	{

	}
	std::list<UnitPtr>::iterator itor;
	for( itor = m_listUnits.begin(); itor != m_listUnits.end(); ++itor )
	{
		if( (*itor)->GetsnObj() == idObj )
			return *itor;
	}
	// 못찾았으면 NULL을 리턴하는데 에러검증을 위해 원본 리스트에서도 찾아보고 있다면 잘못된것이다.
	XBREAKF( XEObjMng::Find( idObj ), "not found obj:type=%d, id=%d", type, idObj);
	return NULL;
}

/**
 @brief 필터에 맞는 오브젝트 중에 가장 가까운 오브젝트를 찾는다.
 만약 범위내에서 못찾으면 전체 검색으로 다시한번 찾는다.
 @param vwPos 검색할 중심좌표
 @param radius 중심좌표로부터 반경
 @param bitSide 피아식별 필터
*/
//필터를 함수포인터로 쓰는 버전으로 모두 바꿀예정
// WorldObjPtr XEObjMngWithType::FindNearObjByFilter( const XE::VEC3& vwPos, 
// 												float radius,
// 												BIT bitSide )
// {
// 	float minDist = 99999999.f;
// 	WorldObjPtr spMinObj( NULL );
// 	float minDistAll = 99999999.f;
// 	WorldObjPtr spMinObjAll( NULL );
// 	std::list<WorldObjPtr>::iterator itor;
// 	for( itor = m_listTypes[XGAME::xOT_UNIT].begin(); itor != m_listTypes[XGAME::xOT_UNIT].end(); ++itor )
// 	{
// 		if( bitSide & ( *itor )->GetbitSide() )
// 		{
// 			XE::VEC3 vDist = ( *itor )->GetvwPos() - vwPos;
// 			float distsq = vDist.Lengthsq();
// 			// 주어진 거리내에 있는가
// 			if( distsq <= radius * radius )
// 			{
// 				minDist = distsq;
// 				spMinObj = ( *itor );
// 			}
// 			else
// 			{
// 				// 못찾을경우를 대비해서 범위바깥의 오브젝트중에 가장 가까운것도 찾는다.
// 				minDistAll = distsq;
// 				spMinObjAll = ( *itor );
// 			}
// 		}
// 	}
// 	// 만약 범위내에서 못찾았으면 범위바깥에서 찾은거라도 돌려준다.
// 	if( spMinObj == NULL )
// 	{
// 		if( spMinObjAll != NULL )
// 			return spMinObjAll;
// 		else
// 		{
// 			// 에러검증. 여기서 못찾았는데 상위함수로 찾으면 있는 경우는 잘못된것.
// 			XBREAK( XEObjMng::FindNearObjByFilter(vwPos, radius, bitSide) != NULL );
// 		}
// 	}
// 	return spMinObj;
// }

/**
 @brief 가장 가까운 오브젝트를 찾는다. 조건은 함수포인터로 전달한다.
 @param radius 특정거리내에서 검색한다. 0은 거리제한이 없다.
*/
UnitPtr XEObjMngWithType::FindNearObjByFunc( XEBaseWorldObj *pSrcObj, 
												const XE::VEC3& vwPos, 
												float meterRadius, 
							BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ) )
{
	float radius = xMETER_TO_PIXEL(meterRadius);
	float minDist = 99999999.f;
	UnitPtr spMinObj( nullptr );
	float minDistAll = 99999999.f;
	UnitPtr spMinObjAll;
	std::list<WorldObjPtr>::iterator itor;
	for( itor = m_listEtc.begin(); itor != m_listEtc.end(); ++itor ) {
		// 사용자 정의 조건함수를 호출해서 통과한것만 거리테스트를 한다.
		if( pfuncFilter( pSrcObj, (*itor).get() ) ) {
			XE::VEC3 vDist = (*itor)->GetvwPos() - vwPos;
			float distsq = vDist.Lengthsq();
			// 주어진 거리내에 있는가. 0은 거리제한 없음.
			if( (distsq <= radius * radius) || radius == 0 ) {
				if( distsq < minDist ) {
					minDist = distsq;
					spMinObj = std::static_pointer_cast<XBaseUnit>(*itor);
				}
			} else {
				// 못찾을경우를 대비해서 범위바깥의 오브젝트중에 가장 가까운것도 찾는다.
				minDistAll = distsq;
				spMinObjAll = std::static_pointer_cast<XBaseUnit>(*itor);
			}
		}
	}
	// 만약 범위내에서 못찾았으면 범위바깥에서 찾은거라도 돌려준다.
	if( spMinObj == NULL ) {
		if( spMinObjAll != NULL )
			return spMinObjAll;
		else {
			// 에러검증. 여기서 못찾았는데 상위함수로 찾으면 있는 경우는 잘못된것.
			//			XBREAK( XEObjMng::FindNearObjByFilter( vwPos, radius, bitSide ) != NULL );
		}
	}
	return spMinObj;
}

/**
 @brief XBaseUnit버전
*/
UnitPtr XEObjMngWithType::FindNearUnitByFunc( XBaseUnit *pSrcObj,
											const XE::VEC3& vwPos,
											float meterRadius,
											bool bFindOutRange,
								BOOL( *pfuncFilter )( XBaseUnit*, XBaseUnit* ) )
{
	float radius = xMETER_TO_PIXEL( meterRadius );
	float minDist = 99999999.f;
	UnitPtr spMinObj( nullptr );
	float minDistAll = 99999999.f;
	UnitPtr spMinObjAll;
	std::list<UnitPtr>::iterator itor;
	for( itor = m_listUnits.begin(); itor != m_listUnits.end(); ++itor )
	{
		// 사용자 정의 조건함수를 호출해서 통과한것만 거리테스트를 한다.
		if( pfuncFilter( pSrcObj, ( *itor ).get() ) )
		{
			XE::VEC3 vDist = ( *itor )->GetvwPos() - vwPos;
			float distsq = vDist.Lengthsq();
			// 주어진 거리내에 있는가. 0은 거리제한 없음.
			if( ( distsq <= radius * radius ) || radius == 0 )
			{
				if( distsq < minDist )
				{
					minDist = distsq;
					spMinObj = *itor;
				}
			}
			else
			{
				// 못찾을경우를 대비해서 범위바깥의 오브젝트중에 가장 가까운것도 찾는다.
				minDistAll = distsq;
				spMinObjAll = *itor;
			}
		}
	}
	// 만약 범위내에서 못찾았으면 범위바깥에서 찾은거라도 돌려준다.
	if( spMinObj == NULL )
	{
		if( spMinObjAll != NULL && bFindOutRange )
			return spMinObjAll;
		else
		{
			// 에러검증. 여기서 못찾았는데 상위함수로 찾으면 있는 경우는 잘못된것.
		}
	}
	return spMinObj;
}


/**
 @brief 조건함수를 이용해 더 ~한 오브젝트를 찾는다.
	주의: m_listEtc의 오브젝트만 찾는다.
*/
WorldObjPtr XEObjMngWithType::FindNearObjByMore( XEBaseWorldObj *pSrcObj,
												const XE::VEC3& vwPos,
												float radius,
	BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ),
	BOOL( *pfuncCompare )( XEBaseWorldObj*, XEBaseWorldObj*, XEBaseWorldObj* ))
{
	WorldObjPtr spCompObj;
	float minDist = 99999999.f;
	WorldObjPtr spCompObjForAll;
	std::list<WorldObjPtr>::iterator itor;
	for( itor = m_listEtc.begin(); itor != m_listEtc.end(); ++itor )
	{
		BOOL bCondition = FALSE;
		// 사용자 정의 필터함수를 호출해서 TRUE인것만 대상으로 한다.
		if( (*itor)->GetsnObj() != pSrcObj->GetsnObj() &&
			pfuncFilter( pSrcObj, itor->get() ) )
		{
			if( radius > 0 )
			{
				// 일단 주어진 거리내에 있어야 한다.
				XE::VEC3 vDist = ( *itor )->GetvwPos() - vwPos;
				float distsq = vDist.Lengthsq();
				if( distsq <= radius * radius )
				{
					bCondition = TRUE;
				}
				else
				{
					// 거리밖에 있는것들중에서 젤 가까운것도 찾아둔다.
					if( distsq < minDist )	// 자기자신은 검색하지 않는다.
					{
						minDist = distsq;
						spCompObjForAll = ( *itor );
					}
				}
			}
			else
				// 범위가 없으면 거리 검사 안함.
				bCondition = TRUE;
			if( bCondition )
			{
				if( spCompObj == NULL )
					spCompObj = ( *itor );
				else
					// 사용자 정의 비교함수를 호출해서 TRUE인것만 취한다.
					if( pfuncCompare( pSrcObj, itor->get(), spCompObj.get() ) )
						spCompObj = ( *itor );
			}
		}
	}
	// 거리내에 조건을 만족하는게 없다.
	if( spCompObj == NULL )
	{
		// 그렇다면 거리밖에있는것들중에 가장가까운것을 리턴한다.
		if( spCompObjForAll != NULL )
			return spCompObjForAll;
		else
			return spCompObj;	// 거리밖에서 조차 없다면 그냥 널리턴
	}
	return spCompObj;
}

/**
 @brief XBaseUnit버전
*/
UnitPtr XEObjMngWithType::FindNearUnitByMore( XBaseUnit *pSrcObj,
												const XE::VEC3& vwPos,
												float radius,
	BOOL( *pfuncFilter )( XBaseUnit*, XBaseUnit* ),
	BOOL( *pfuncCompare )( XBaseUnit*, XBaseUnit*, XBaseUnit* ))
{
	UnitPtr spCompObj;
	float minDist = 99999999.f;
	UnitPtr spCompObjForAll;
	std::list<UnitPtr>::iterator itor;
	for( itor = m_listUnits.begin(); itor != m_listUnits.end(); ++itor )
	{
		BOOL bCondition = FALSE;
		// 사용자 정의 필터함수를 호출해서 TRUE인것만 대상으로 한다.
		if( (*itor)->GetsnObj() != pSrcObj->GetsnObj() &&
			pfuncFilter( pSrcObj, itor->get() ) )
		{
			if( radius > 0 )
			{
				// 일단 주어진 거리내에 있어야 한다.
				XE::VEC3 vDist = ( *itor )->GetvwPos() - vwPos;
				float distsq = vDist.Lengthsq();
				if( distsq <= radius * radius )
				{
					bCondition = TRUE;
				}
				else
				{
					// 거리밖에 있는것들중에서 젤 가까운것도 찾아둔다.
					if( distsq < minDist )	// 자기자신은 검색하지 않는다.
					{
						minDist = distsq;
						spCompObjForAll = ( *itor );
					}
				}
			}
			else
				// 범위가 없으면 거리 검사 안함.
				bCondition = TRUE;
			if( bCondition )
			{
				if( spCompObj == NULL )
					spCompObj = ( *itor );
				else
					// 사용자 정의 비교함수를 호출해서 TRUE인것만 취한다.
					if( pfuncCompare( pSrcObj, itor->get(), spCompObj.get() ) )
						spCompObj = ( *itor );
			}
		}
	}
	// 거리내에 조건을 만족하는게 없다.
	if( spCompObj == NULL )
	{
		// 그렇다면 거리밖에있는것들중에 가장가까운것을 리턴한다.
		if( spCompObjForAll != NULL )
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
int XEObjMngWithType::GetListUnitRadius( XArrayLinearN<XBaseUnit*, 512> *plistOutInvokeTarget,
									XEBaseWorldObj *pBaseTarget,
									const XE::VEC2& vCenter,
									float pixelRadius,
									BIT bitSideFilter,
									int numApply,
									BOOL bIncludeCenter,
									BIT bitLive )		// 생존필터  
{
	int cnt = 0;
	std::list<UnitPtr>::reverse_iterator itor;
	for( itor = m_listUnits.rbegin(); itor != m_listUnits.rend(); ++itor )
	{
		XBaseUnit *pUnit = SafeCast<XBaseUnit*, XEBaseWorldObj*>( (*itor).get() );
		XBREAK( pUnit == NULL );
		if( ( pUnit->GetCamp() & bitSideFilter ) == 0 )
			continue;
		if( (bitLive & XSKILL::xTL_LIVE) == 0 && pUnit->IsLive() )
			continue;
		if( ( bitLive & XSKILL::xTL_DEAD ) == 0 && pUnit->IsDead() )
			continue;
		if( numApply != 0 && cnt >= numApply )
			return plistOutInvokeTarget->size();
		// 시전대상을 포함하지 않는 조건일때 유닛이 시전대상이면 스킵
		if( pBaseTarget &&
			bIncludeCenter == FALSE && 
			pBaseTarget->GetsnObj() == pUnit->GetsnObj() )
			continue;
		XE::VEC2 vDist = pUnit->GetvwPos().ToVec2() - vCenter;
		float distsq = vDist.Lengthsq();
		if( distsq <= pixelRadius * pixelRadius )
		{
			plistOutInvokeTarget->Add( pUnit );
			++cnt;
		}
	}
	
	return plistOutInvokeTarget->size();
}

/**
 @brief 윈도우좌표vsPos에서 픽킹되는 bitCamp진영 유닛을 찾아낸다.
 @param snExcludeSquad 검색에서 제외해야할 부대
 @param pOutPickExclude snExcludeSquad로 제외시킨부대가 먼저 픽킹된다면 TRUE가 된다.
*/
UnitPtr XEObjMngWithType::GetPickUnit( XWndBattleField *pWndWorld, 
										const XE::VEC2& vsPos, 
										BIT bitCamp,
										ID snExcludeSquad/*=0*/,
										BOOL *pOutPickExclude/*=NULL*/)
{
	std::list<UnitPtr>::iterator itor;
	for( itor = m_listUnits.begin(); itor != m_listUnits.end(); ++itor )
	{
		XBaseUnit *pUnit = SafeCast<XBaseUnit*, XEBaseWorldObj*>( ( *itor ).get() );
		if( XBREAK( pUnit == NULL ) )
			return UnitPtr();
		if( pUnit->IsDead() )
			continue;;
		// bitCamp와 같은진영만 검사한다. 제외부대가 지정되어있을경우 해당부대의 유닛은 제외한다.
		if( pUnit->GetCamp().IsFriendly( bitCamp ) )
		{
			XE::VEC2 vPos = pUnit->GetvsPos();
			XE::VEC2 vLT = vPos + pUnit->GetBoundBox().vLT * pWndWorld->GetscaleCamera();
			XE::VEC2 vRB = vPos + pUnit->GetBoundBox().vRB * pWndWorld->GetscaleCamera();
			if( XE::IsInside( vsPos, vLT, vRB ) )
			{
				if( snExcludeSquad == 0 ||
					( snExcludeSquad && pUnit->GetpSquadObj()->GetsnSquadObj() != snExcludeSquad ) )
					return pUnit->GetThisUnit();
				else
				{
					if( pOutPickExclude )
						*pOutPickExclude = TRUE;	// 제외된 부대가 선택되었다.
				}
			}
		}
	}
	return UnitPtr();
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

