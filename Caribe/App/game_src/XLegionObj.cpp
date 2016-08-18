#include "stdafx.h"
#include "XLegionObj.h"
#include "XSquadObj.h"
#include "XLegion.h"
#include "XSquadron.h"
#include "XHero.h"
#include "XBaseUnit.h"
#include "XWndBattleField.h"
#include "XPropUnit.h"
#include "XPropHero.h"
#include "XFSMUnit.h"
#include "XBattleField.h"
#include "XSkillMng.h"
#include "XStatistic.h"
#ifdef _CHEAT
#include "client/XAppMain.h"
#include "XGame.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

////////////////////////////////////////////////////////////////
int XLegionObj::s_numObj = 0;		// 메모리 릭 추적용
XLegionObj::XLegionObj( /*XAccount *pAccount, */LegionPtr& spLegion, BIT bitCamp, BOOL )
{
	Init();
	
	m_spLegion = spLegion;
//	m_bDestroyLegion = bDestroyLegion;
	m_Camp.SetbitCamp( bitCamp );
	m_pStatObj = new XStatistic;
}

void XLegionObj::Destroy()
{
	SAFE_DELETE( m_pStatObj );

}

void XLegionObj::Release()
{
	for( auto& spSquad : m_listSquad )	{
		spSquad->Release();
	}
	m_listSquad.clear();
	m_spLegion.reset();
}

/**
 m_pLegion을 바탕으로 분대 객체를 만듬
*/
BOOL XLegionObj::CreateLegion( XWndBattleField *pWndWorld, 
								const XE::VEC3& vwStart, 
								XGAME::xtLegionOption bitOption )
{
	XE::VEC2 vDist(166 * 1.5f,117 * 1.5f);	// 분대간 간격
	int col = m_spLegion->GetarySquadrons().GetMax() / 5;	// 몇 라인까지 나오는가
	int mod = m_spLegion->GetarySquadrons().GetMax() % 5;	// 
//	BIT bitSide = m_Camp.GetbitCamp(); // ( idxLegion == 0 ) ? XGAME::xSIDE_PLAYER : XGAME::xSIDE_OTHER;
	for( int i = 0; i < col; ++i )
	{
		for( int j = 0; j < 5; ++j )
		{
			XSquadron *pSquad = m_spLegion->GetSquadron( i * 5 + j );
			bool bCreate = true;
			if( bitOption & XGAME::xLO_NO_CREATE_DEAD ) {
				if( pSquad && pSquad->GetpHero()->GetbLive() == false )
					bCreate = false;
			}
			if( bCreate ) {
				XE::VEC3 vCurr;
				if( IsPlayer() )
					vCurr.x = vwStart.x - ( i * vDist.w );
				else
					vCurr.x = vwStart.x + ( i * vDist.w );
				vCurr.y = vwStart.y + ( j * vDist.h );
				if( pSquad )
				{
					XBREAK( pSquad->GetnumUnit() > XGAME::MAX_UNIT_SMALL );
					XSquadObj *pSquadObj = new XSquadObj( GetThis(), pSquad, vCurr );
					pSquadObj->CreateSquad( pWndWorld, m_Camp );
					AddSquad( XSPSquad( pSquadObj ) );
				}
			}
		}
	}
	for( int j = 0; j < mod; ++j )
	{
		XE::VEC3 vCurr;
		if( IsPlayer() )
			vCurr.x = vwStart.x - ( col * vDist.w );
		else
			vCurr.x = vwStart.x + ( col * vDist.w );
		vCurr.y = vwStart.y + ( j * vDist.h );
		XSquadron *pSquad = m_spLegion->GetSquadron( col * 5 + j );
		if( pSquad )
		{
			XSquadObj *pSquadObj = new XSquadObj( GetThis(), pSquad, vCurr );
			XPropUnit::xPROP *pProp = PROP_UNIT->GetpProp( pSquadObj->GetpHero()->GetUnit() );
			pSquadObj->CreateSquad( pWndWorld, m_Camp );
			AddSquad( XSPSquad( pSquadObj ) );
		}
	}
	return TRUE;	
}

#ifdef _CHEAT
BOOL XLegionObj::CreateLegionDebug( XWndBattleField *pWndWorld,
									XE::VEC3 vwStart, 
									XGAME::xtLegionOption bitOption )
{
	vwStart.y = pWndWorld->GetpWorld()->GetvwSize().h / 2.f;
//	BIT bitSide = m_Camp.GetbitCamp(); // ( idxLegion == 0 ) ? XGAME::xSIDE_PLAYER : XGAME::xSIDE_OTHER;
	XSquadron *pSquad = m_spLegion->GetSquadron( 2 );
#ifdef _XSINGLE
	m_spLegion->SetpLeader( pSquad->GetpHero() );
#endif

	if( pSquad )
	{
		XSquadObj *pSquadObj = new XSquadObj( GetThis(), pSquad, vwStart );
		pSquadObj->CreateSquadsDebug( pWndWorld, m_Camp );
		AddSquad( XSPSquad( pSquadObj ) );
	}
	return TRUE;
}

/**
 @brief 분대객체들을 만든다.
*/
bool XLegionObj::CreateLegion2( XWndBattleField *pWndWorld, 
																const XE::VEC3& vwStart, 
																xtLegionOption bitOption )
{
	const int maxSquad = m_spLegion->GetarySquadrons().GetMax();
	int col = maxSquad / 5;	// 몇 라인까지 나오는가
	int mod = maxSquad % 5;	// 마지막 짜투리 부대수
	for( int i = 0; i < col; ++i ) {
		for( int j = 0; j < 5; ++j ) {
			auto spSquadObj = CreateSquadObj( i, j, vwStart, bitOption, pWndWorld );
			if( spSquadObj )
				AddSquad( spSquadObj );
		}
	}
	for( int j = 0; j < mod; ++j ) {
		auto spSquadObj = CreateSquadObj( col, j, vwStart, bitOption, pWndWorld );
		if( spSquadObj )
			AddSquad( spSquadObj );
	}
	return true;
}

/**
 @brief col,row위치에 분대하나를 생성한다.
*/
XSPSquad XLegionObj::CreateSquadObj( int col, int row
																		, const XE::VEC3& vwBase
																		, xtLegionOption bitOption
																		, XWndBattleField* pWndWorld )
{
	if( !m_spLegion )
		return nullptr;
	const auto pSquad = m_spLegion->GetSquadron( col * 5 + row );
	if( !pSquad )
		return nullptr;
	const float sideSign = ( IsPlayer() ) ? -1.f : 1.f;
	const XE::VEC2 vDist( 166 * 1.5f, 117 * 1.5f );	// 분대간 간격
	//
	if( bitOption & XGAME::xLO_NO_CREATE_DEAD ) {
		// 죽은부대는 생성하지 않는 옵션이면 부대생성하지 않음.
		if( pSquad->GetpHero()->GetbLive() == false )
			return nullptr;
	}
	XE::VEC3 vCurr;
	vCurr.x = vwBase.x + ( ( col * vDist.w ) * sideSign );
	vCurr.y = vwBase.y + ( row * vDist.h );
	XBREAK( pSquad->GetnumUnit() > XGAME::MAX_UNIT_SMALL );
	auto spSquadObj = std::make_shared<XSquadObj>( GetThis(), pSquad, vCurr );
	spSquadObj->CreateSquad( pWndWorld, m_Camp );
	return spSquadObj;
}

/**
 @brief 싱글모드에서 x부대에게 자원을 할당
*/
void XLegionObj::MakeResource()
{
	for( auto spSquad : m_listSquad ) {
		XVector<xRES_NUM> ary;
		ary.push_back( xRES_NUM( xRES_WOOD, 10 ) );
		ary.push_back( xRES_NUM( xRES_IRON, 10 ) );
		ary.push_back( xRES_NUM( xRES_JEWEL, 10 ) );
		ary.push_back( xRES_NUM( xRES_SULFUR, 10 ) );
		ary.push_back( xRES_NUM( xRES_MANDRAKE, 10 ) );
		spSquad->SetaryLoots( ary );
	}
}
#endif
/**
 유닛들의 AI를 켜거나 끈다.
*/
void XLegionObj::SetAI( BOOL bFlag )
{
	for( auto &spSquadObj : m_listSquad ) {
		spSquadObj->SetAI( bFlag );
	}
}

/**
 @brief 양측 군단이 모두 생성된 직후 발생
*/
void XLegionObj::OnSkillEvent( XSKILL::xtJuncture event )
{
	for( auto &spSquadObj : m_listSquad ) {
		spSquadObj->OnSkillEvent( event );
	}
}

void XLegionObj::OnStartBattle()
{
	for( auto &spSquadObj : m_listSquad )	{
		spSquadObj->OnStartBattle();
	}
}

void XLegionObj::OnAfterStartBattle()
{
	for( auto &spSquadObj : m_listSquad )
	{
		spSquadObj->OnAfterStartBattle();
	}
}

XSquadObj* XLegionObj::AddSquad( const XSPSquad& spSquad ) 
{
	XBREAK( spSquad->GetNumLists() != spSquad->GetcntLive() );
	m_listSquad.Add( spSquad );
	AddCntLive( 1 );
	m_pStatObj->AddSquad( spSquad->GetpHero() );
	return spSquad.get();
}

/**
 @brief pFinder와 가장가까운 적부대를 찾는다.
 XSquadObj를 shared_ptr상속받지 않으려고 일반포인터로 넘김
*/
XSPSquad XLegionObj::FindNearSquad( XSquadObj *pFinder )
{
	XSPSquad spMinObj;
	float minDistsq = 99999999.f;
	for( auto &spSquadObj : m_listSquad ) {										
		// 살아있는 부대만 대상으로 검색한다.
		if( spSquadObj->IsLive() && spSquadObj->IsAbleTarget() ) {
			XE::VEC3 vDist = spSquadObj->GetvwPos() - pFinder->GetvwPos();
			float distsq = vDist.Lengthsq();
			if( spMinObj == nullptr || distsq <= minDistsq ) {
				spMinObj = spSquadObj;
				minDistsq = distsq;
			}
		}
	} //END_LOOP;
	return spMinObj;
}

XSPSquad XLegionObj::FindNearSquad( XSquadObj *pFinder, bool (*pFunc)( XSPSquad& ) )
{
	XSPSquad spMinObj;
	float minDistsq = 99999999.f;
	for( auto &spSquadObj : m_listSquad ) {
		// 살아있는 부대만 대상으로 검색한다.
		if( spSquadObj->IsLive() ) {
			// 커스텀 조건에 맞는 부대들만을 대상으로 거리를 계산해서 가장가까운 부대를 리턴한다.
			if( ( *pFunc )( spSquadObj ) ) {
				XE::VEC3 vDist = spSquadObj->GetvwPos() - pFinder->GetvwPos();
				float distsq = vDist.Lengthsq();
				if( spMinObj == nullptr || distsq <= minDistsq ) {
					spMinObj = spSquadObj;
					minDistsq = distsq;
				}
			}
		}
	} //END_LOOP;
	return spMinObj;
}
/**
 @brief pfinder부대로부터 meter범위내에 있는 부대중 가장 hp가 적은/많은 부대를 찾는다.
 @param meter 찾고자 하는 제한범위. 0이면 거리제한 없음
 @param bitCampFind 찾고자 하는 진영
 @param bHighest true면 hp가 가장많은 부대를, false면 가장적은 부대를 찾는다.
*/
XSPSquad XLegionObj::FindNearSquadLeastHp( XSquadObj *pFinder, 
																					float pixelRadius, 
																					BIT bitCampFind,
																					BOOL bHighest )
{
	XSPSquad spMinObj;
	float hpSelect = 0;
	if( bHighest )
		hpSelect = -9999999.f;
	else
		hpSelect = 999999.f;
	for( auto& spSquadObj : m_listSquad ) {
		bool bCondDist = false;
		// 살아있는 부대만 대상으로 검색한다.
		if( spSquadObj->IsLive() ) {
			XE::VEC3 vDist = spSquadObj->GetvwPos() - pFinder->GetvwPos();
			float distsq = vDist.Lengthsq();
			// 일단 제한거리 조건을 따진다.
			if( pixelRadius > 0 ) {
				if( distsq < pixelRadius * pixelRadius )
					bCondDist = true;
			} else
				bCondDist = true;
			if( bCondDist ) {
				bool bCondHp = false;
				float hpMax = spSquadObj->GetTotalHpRate();
				if( bHighest && hpMax > hpSelect )
					bCondHp = true;
				else if( bHighest == false && hpMax < hpSelect )
					bCondHp = true;
				if( spMinObj == nullptr || bCondHp ) {
					spMinObj = spSquadObj;
					hpSelect = hpMax;
				}
			}
		}
	} //END_LOOP;
	return spMinObj;
}

/**
 @brief vwSrc좌표의 반경에 들어온 유닛을 this부대에서 찾는다.
*/
XSPUnit XLegionObj::FindNearUnit( const XE::VEC3& vwSrc, float meterRadius ) const
{
	for( auto spSquadObj : m_listSquad ) {
		if( spSquadObj->IsLive() ) {
			//
			for( auto spUnit : spSquadObj->GetlistUnit() ) {
				if( spUnit && spUnit->IsLive() ) {
					auto distSq = spUnit->GetDistSqBetweenPos( vwSrc );
					const auto pixelRadius = xMETER_TO_PIXEL( meterRadius );
					if( distSq < pixelRadius * pixelRadius ) {
						return spUnit;
					}
				}
			}
		}
	}
	return nullptr;
}

/**
 @brief 아무부대나 랜덤으로 꺼낸다.
*/
XSPSquad XLegionObj::FindSquadRandom( XSquadObj *pFinder, 
										float pixelRadius, 
										bool bIncludeFinder )
{
	XArrayLinearN<XSPSquad,32> ary;
	for( auto& spSquadObj : m_listSquad ) {
		bool bCondDist = false;
// 		XSPSquad *pspSquadObj = m_listSquad.GetNext3( _itor );
		// 살아있는 부대만 대상으로 검색한다.
		if( spSquadObj->IsLive() ) {
			XE::VEC3 vDist = spSquadObj->GetvwPos() - pFinder->GetvwPos();
			float distsq = vDist.Lengthsq();
			// 일단 제한거리 조건을 따진다.
			if( pixelRadius > 0 ) {
				if( distsq < pixelRadius * pixelRadius ) {
					if( pFinder->GetsnSquadObj() != spSquadObj->GetsnSquadObj() )
						ary.Add( spSquadObj );
					else
					if( bIncludeFinder )
						ary.Add( spSquadObj );
				}
			}
		}
	} //END_LOOP;
	if( ary.size() > 0 )
		return ary.GetFromRandom();
	return XSPSquad();
}

void XLegionObj::FrameMove( float dt )
{
	for( auto& spSquad : m_listSquad ) {
		spSquad->FrameMove( dt );
	}
}

BOOL XLegionObj::IsNearestSquad( const XSPSquad& spBase, XSPSquad& spOut )
{
	for( auto& spSquad : m_listSquad ) {
		if( spBase->GetCamp().IsEnemy( spSquad->GetCamp().GetbitCamp() ) ) {
			if( (spBase->IsNear( spSquad.get() )) ) {
				spOut = spSquad;
				return TRUE;
			}
		}
	}
	return FALSE;
}

/**
 @brief 디버깅용..
*/
void XLegionObj::Draw( XEWndWorld *pWndWorld )
{
#ifdef _CHEAT
	if( XAPP->GetbDebugMode() && XAPP->m_bDebugViewSquadRadius ) {
		for( auto& spSquad : m_listSquad ) {
			if( spSquad->IsLive() ) {
				XE::VEC3 vwPos = spSquad->GetvwPos();
				XE::VEC2 vPos = pWndWorld->GetPosWorldToWindow( vwPos );
				float radius = spSquad->GetRadius() * XWndBattleField::sGet()->GetscaleCamera();
				XCOLOR col = XCOLOR_WHITE;
				// 				switch( spSquad->GetUnitType() )
				// 				{
				// 				case XGAME::xUNIT_FALLEN_ANGEL:	col = XCOLOR_RED; break;
				// 				}
				XCOLOR cols[9] = {
					XCOLOR_YELLOW,
					XCOLOR_RGBA( 55,108,147,255 ),	// 궁수
					XCOLOR_RGBA( 163,198,252,255 ),
					XCOLOR_WHITE,					// 미노
					XCOLOR_GREEN,
					XCOLOR_RED,
					XCOLOR_RGBA( 128,0,0,255 ),		// 골렘
					XCOLOR_RGBA( 149,125,66,255 ),
					XCOLOR_BLACK,
				};
				col = cols[spSquad->GetUnit() - 1];
				GRAPHICS->DrawCircle( vPos.x, vPos.y, radius, col );
				// 부대의 평균 중심점에 십자를 그린다.
				XE::VEC3 vwAvg = spSquad->GetvCenterByUnits();
				XE::VEC2 vCenter = XWndBattleField::sGet()->GetPosWorldToWindow( vwAvg );
				float len = 16.f;
				GRAPHICS->DrawHLine( vCenter.x - (len / 2.f), vCenter.y, len, col );
				GRAPHICS->DrawVLine( vCenter.x, vCenter.y - (len / 2.f), len, col );
				// 
				float speed = spSquad->GetAvgSpeedUnits();
				PUT_STRINGF( vPos.x, vPos.y - radius, col, "0x%08x\n%f", spSquad->GetsnSquadObj(), speed );
			}
		}
	}
#endif
}

/**
 @brief 분대가 전멸했다.
*/
void XLegionObj::OnDieSuqad( XSPSquad spSquad )
{
// 	auto spFound = m_listSquad.FindByIDNonPtr( spSquad->getid() );
	auto spFound = m_listSquad.FindByID( spSquad->getid() );
	XBREAK( spFound == nullptr );
	if( spFound ) {
		AddCntLive( -1 );
		XBattleField::sGet()->OnDieSquad( spSquad );
#ifdef _XSINGLE
		spSquad->m_secDie = spSquad->m_timerDie.GetPassSec();
#endif // _XSINGLE
		if( m_cntLive == 0 )
			XBattleField::sGet()->OnDieLegion( GetThis() );
	}
}



/**
 @brief 월드좌표vwPick가 부대반경에 포함되는 부대를 리턴한다.
 @param idExclude 검색에서 제외할 부대아이디
*/
XSPSquad XLegionObj::GetPickSquad( const XE::VEC3& vwPick, BIT bitCamp, ID snExclude /*= 0*/ )
{
	for( auto& spSquad : m_listSquad ) {
		if( spSquad->IsLive() &&
			( snExclude == 0 || ( snExclude && spSquad->GetsnSquadObj() == snExclude ) ) ) {
			const float radius = 110.f;
			// 부대원들의 위치에따른 중앙좌표를 얻는다.
			XE::VEC3 vCenter = spSquad->GetvCenterByUnits();
			if( XE::IsInSphereWithPoint( vCenter, radius, vwPick ) ) {
				return spSquad;
			}
		}
	}
	return nullptr;
}

/**
 @brief src측 부대로부터 인접해있는 부대를 모두 찾는다.
 this부대는 src부대의 적부대여야 한다.
*/
int XLegionObj::GetNearSquad( XSquadObj *pSquadSrc,
							XArrayLinearN<XSPSquad, 64> *pOutAry,
							float radius )
{
	for( auto& spSquad : m_listSquad ) {
		if( ( spSquad->GetsnSquadObj() != pSquadSrc->GetsnSquadObj() ) &&
			spSquad->IsLive() ) {
			XE::VEC3 vCenter = spSquad->GetvCenterByUnits();
			if( XE::IsInSphereWithPoint( vCenter, radius, pSquadSrc->GetvwPos() ) ) {
				pOutAry->Add( spSquad );
			}
		}
	}
	return pOutAry->size();
}

XSPSquad XLegionObj::GetSquadBySN( ID snSquad )
{
	for( auto& spSquad : m_listSquad ) {
		if( spSquad->GetsnSquadObj() == snSquad )
			return spSquad;
	}
	return nullptr;
}

XSPSquad XLegionObj::GetspSquadObjByIdx( int idx )
{
	auto pSquad = m_spLegion->GetpSquadronByIdx( idx );
	if( !pSquad )
		return nullptr;
	for( auto spSquadObj : m_listSquad ) {
		if( spSquadObj->GetsnHero() == pSquad->GetsnHero() )
			return spSquadObj;
	}
	return nullptr;
}

/**
 @brief 치트용. 모든 적유닛을 죽인다.
*/
void XLegionObj::KillAllUnit()
{
#ifdef _CHEAT
	for( auto& spSquad : m_listSquad ) {
		spSquad->DoDamage( nullptr, 1000000.f );
	}
#endif // _CHEAT
}
void XLegionObj::DoDamage()
{
#ifdef _CHEAT
	for( auto& spSquad : m_listSquad ) {
		spSquad->DoDamageByPercent( 0.33f, FALSE );
	}
#endif // _CHEAT
}

XHero* XLegionObj::GetpLeader() 
{
	return m_spLegion->GetpLeader();
}

/**
 @brief 군단내 모든 유닛과 영웅을 담는다.
*/
int XLegionObj::GetAllUnit( XVector<XSKILL::XSkillReceiver*> *pOutAry )
{
	for( auto& spSquad : m_listSquad ) {
		spSquad->GetListMember( pOutAry );
		
	}
	return pOutAry->size();
}

int XLegionObj::GetAllUnit( XVector<XBaseUnit*> *pOutAry )
{
	for( auto& spSquad : m_listSquad ) {
		spSquad->GetListMember( pOutAry );
	}
	return pOutAry->size();
}
/**
 @brief 군단내에서 idHero영웅을 찾음.
*/
XSPUnit XLegionObj::GetHeroUnit( ID idHero )
{
	for( auto& spSquad : m_listSquad ) {
		auto spHeroUnit = spSquad->GetspHeroUnit();
		if( spHeroUnit && spHeroUnit->GetidProp() == idHero ) 
			return spHeroUnit;
	}
	return XSPUnit();
}

/**
 @brief 모든 부대원들에게 hp를 꽉채우라고 명령
*/
void XLegionObj::DoFullHp()
{
	XVector<XBaseUnit*> ary;
	GetAllUnit( &ary );
	for( auto pUnit : ary )	{
		pUnit->DoFullHp();
	}
}

/**
 @brief pTarget을 치고 있는 또다른 아군 부대가 있는지 검사한다.
 @param pFinder 찾는 부대. 이것은 결과에서 제외해야 한다.
 @param unitFilter none이 아니라면 해당유닛부대만 검색한다.
*/
XSPSquad XLegionObj::FindSquadTakeTargetAndClosed( XSquadObj *pFinder, XSquadObj *pTarget, XGAME::xtUnit unitFilter )
{
	// 아군 부대의 루프를 돈다.
	for( auto& spSquad : m_listSquad )
	{
		ID snSquad = spSquad->GetsnSquadObj();
		// 이부대가 Finder부대면 제외
		if( snSquad != pFinder->GetsnSquadObj() )
		{
			// 이부대의 타겟과 파라메터의 타겟과 같은 부대인가
			if( spSquad->GetspTarget() != nullptr &&
				spSquad->GetspTarget()->GetsnSquadObj() == pTarget->GetsnSquadObj() )
			{
				if( unitFilter == XGAME::xUNIT_NONE )
					return spSquad;
				else
				if( unitFilter == spSquad->GetUnit() )
					return spSquad;
			}
		}
	}
	return XSPSquad();
}

/**
 @brief 이 군단의 모든 병사들의 hp합산을 구한다.
*/
float XLegionObj::GetSumHpAllSquad()
{
	float sum = 0;
	for( auto& spSquad : m_listSquad )
	{
		sum += spSquad->GetSumHpAllMember();
	}
	return sum;
}

float XLegionObj::GetMaxHpAllSquad()
{
	float sum = 0;
	for( auto& spSquad : m_listSquad )
	{
		sum += spSquad->GetMaxHpAllMember();
	}
	return sum;
}

/**
 @brief 길드레이드용으로 묶는다.
*/
int XLegionObj::SerializeForGuildRaid( XArchive& ar )
{
	ar << (int)m_listSquad.size();
	for( auto spSquad : m_listSquad ) {
		ID snHero = spSquad->GetpHero()->GetsnHero();
		ar << snHero;
		// 부대가 아직 살아있는지 전멸했는지.
		if( spSquad->IsLive() ) {
			ar << 11;
		} else {
			ar << 0;
		}
	}
	MAKE_CHECKSUM( ar );
	return 1;
}
// Deserialize는 XLegion::DeserializeForGuildRaid에.

void XLegionObj::DrawSquadsBar( const XE::VEC2& vPos )
{
#ifdef _XSINGLE
	auto v = vPos;
	for( auto spSquad : m_listSquad ) {
		if( spSquad->m_bShowHpInfo ) {
			v.y = spSquad->DrawMembersHp( v );
			v.y += 7.f;
		}
	}
#endif // _XSINGLE
}

/**
 @brief 자원부대 리스트를 얻는다.
*/
void XLegionObj::GetArySquadByResource( XVector<XSPSquad>* pOut )
{
	for( auto spSquad : m_listSquad ) {
		if( spSquad->IsResourceSquad() )
			pOut->Add( spSquad );
	}
}
