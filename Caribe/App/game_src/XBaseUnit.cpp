#include "stdafx.h"
#include "XBaseUnit.h"
#include "XWndBattleField.h"
#include "XPropUnit.h"
#include "XPropTech.h"
#include "XFSMUnit.h"
#include "XGame.h"
#include "XSkillMng.h"
#include "XBattleField.h"
#include "XUnitArcher.h"
#include "XUnitTreant.h"
#include "XUnitCyclops.h"
#include "XUnitHero.h"
#include "XLegionObj.h"
#include "XSquadObj.h"
#include "XStatistic.h"
#include "../XDrawGraph.h"
#include "XHero.h"
#ifdef _CHEAT
#include "client/XAppMain.h"
#endif
#include "XEObjMngWithType.h"
#include "XSceneBattle.h"
#include "XLegion.h"
#include "XSquadron.h"
#include "XGlobalConst.h"
#include "XSystem.h"
#include "XMsgUnit.h"
#include "XFramework/XFType.h"


using namespace XSKILL;
using namespace XGAME;
using namespace xnUnit;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

bool XBaseUnit::s_bNotUseActiveByEnemy = false;		// 적영웅 스킬사용금지
const float XBaseUnit::c_maxDmgShake = 5.f;


XE_NAMESPACE_START( xnUnit )
//

xDmg::xDmg( XSPWorldObj spAtkObj
					, XSPUnit spTarget
					, float damage
					, float ratioPenet
					, XSKILL::xtDamage typeDamage
					, const BIT bitAttrHit
					, XGAME::xtDamageAttr attrDamage
					, bool bCritical )
	: m_spAtkObj( std::static_pointer_cast<XEBaseWorldObj>( spAtkObj ) )
	, m_spTarget( spTarget )
	, m_Damage( damage )
	, m_ratioPanet( ratioPenet )
	, m_typeDmg( typeDamage )
	, m_bitAttrHit( bitAttrHit )
	, m_attrDamage( attrDamage )
	, m_bCritical( bCritical ) {
	//
	if( spAtkObj && spAtkObj->GetType() == XGAME::xOT_UNIT ) {
		m_spUnitAtker = std::static_pointer_cast<XBaseUnit>( m_spAtkObj );
	}
}
//
XE_NAMESPACE_END; // xnUnit


#ifdef WIN32
void ADD_LOG( _tstring& str, const char* format, ... )
{
	if( XAPP->m_bDebugMode && XAPP->m_bBattleLogging ) {
		char cLog[4096];
		va_list vl;
		va_start( vl, format );
		vsprintf_s( cLog, format, vl );
		va_end( vl );
		str += C2SZ( cLog );
	}
}
#else
#define ADD_LOG(A,F,...)			{}
#endif // WIN32

// 표준 영웅 키(영웅마다 그림 바운딩 박스가 달라서 hp와 이름 그려지는 위치가 달라서)
#define HEIGHT_HERO			75.f

/**
 @brief
 @param pSquadObj XSquadObj를 shared_ptr상속받게 하지 않으려고 일반포인터로 넘김. weak임.
*/
XBaseUnit* XBaseUnit::sCreateUnit( XSPSquad spSquadObj,
																	ID idProp,
																	BIT bitSide,
																	const XE::VEC3& vwPos,
																	float multipleAbility )
{
	XBaseUnit *pBaseUnit = nullptr;
	switch( idProp ) {
	case XGAME::xUNIT_ARCHER: {
			auto pUnit = new XUnitArcher( spSquadObj, idProp, bitSide, vwPos, multipleAbility );
			pBaseUnit = pUnit;
	} break;
	case XGAME::xUNIT_TREANT: {
		auto pUnit = new XUnitTreant( spSquadObj, idProp, bitSide, vwPos, multipleAbility );
			pBaseUnit = pUnit;
	} break;
	case XGAME::xUNIT_CYCLOPS: {
			auto pUnit = new XUnitCyclops( spSquadObj, idProp, bitSide, vwPos, multipleAbility );
			pBaseUnit = pUnit;
	} break;
	default:
		pBaseUnit = new XUnitCommon( spSquadObj, idProp, bitSide, vwPos, multipleAbility );
		break;
	}
	XBREAK( pBaseUnit == nullptr );
	pBaseUnit->OnCreate();
	return pBaseUnit;
}

XBaseUnit* XBaseUnit::sCreateHero( XSPSquad spSquadObj,
									XHero *pHero,
									ID idPropUnit,
									BIT bitSide,
									const XE::VEC3& vwPos,
									float multipleAbility )
{
	auto pBaseUnit = new XUnitHero( spSquadObj, pHero, idPropUnit, bitSide, vwPos, multipleAbility );
	XBREAK( pBaseUnit == nullptr );
	pBaseUnit->OnCreate();
	return pBaseUnit;
}

////////////////////////////////////////////////////////////////
XBaseUnit::XBaseUnit( XSPSquad spSquadObj,
											ID idProp,
											BIT bitCamp,
											const XE::VEC3& vPos,
											float multipleAbility )
	: XEBaseWorldObj( XWndBattleField::sGet(), XGAME::xOT_UNIT, vPos )
	, XSKILL::XSkillReceiver( 32, XGAME::xMAX_PARAM, XGAME::xST_MAX )
	, XSKILL::XSkillUser( SKILL_MNG )
	, m_vLocalFromSquad(vPos - spSquadObj->GetvwPos())
{
	Init();
	m_idProp = idProp;





	XEBaseWorldObj::SetType( XGAME::xOT_UNIT );
	auto pHero = spSquadObj->GetpHero();
	m_pHero = pHero;

	m_pSquadObj = spSquadObj.get();
	m_spMsgQ1 = std::make_shared<XMsgQ>();
	m_spMsgQ2 = std::make_shared<XMsgQ>();
//	auto pHero = GetpHero();
	XBREAK( pHero == nullptr );
	m_pPropUnit = PROP_UNIT->GetpProp( pHero->GetUnit() );
	XBREAK( m_pPropUnit == nullptr );
	SetstrcIds( SZ2C(m_pPropUnit->strIdentifier) );
	m_Timer.Set( xRandomF(2.f) );
	m_Camp.SetbitCamp( bitCamp );
	SetTribe( m_pPropUnit->tribe );
	XBREAK( m_pPropUnit->tribe == 0 );
	m_multipleAbility = multipleAbility;
	XBREAK( m_multipleAbility <= 0.f );
	//
	CreateFSMObj();
	//
	m_psfcShadow = IMAGE_MNG->Load( TRUE, XE::MakePath(DIR_IMG,_T("shadow.png")) );
	// 특성스킬 세팅
#if defined(WIN32) && defined(_CHEAT)
	if( IsCheatFiltered() )
#endif // defined(WIN32) && defined(_CHEAT)
	{
		for( auto itor : pHero->GetTechTree( spSquadObj->GetUnit() ) ) {
			XGAME::xAbil *pAbil = &(itor.second);
			if( pAbil->point > 0 ) {
				XBREAK( pAbil->point > 5 );
				ID idAbil = itor.first;
				auto pNode = XPropTech::sGet()->GetpNode( spSquadObj->GetUnit(), idAbil );
				XBREAK( pNode == nullptr );
				XBREAK( pNode->strSkill.empty() );
				CreateAddUseSkillByIdentifier( pNode->strSkill.c_str() );
			}
		}
	}
}

void XBaseUnit::Destroy()
{
	//
	SAFE_RELEASE2(IMAGE_MNG, m_psfcShadow);
	SAFE_DELETE( m_psoHit );
	SAFE_DELETE( m_psoSelect );
//  	SAFE_RELEASE2( FONTMNG, m_pfdName );
// 	SAFE_DELETE( m_pfoName );
	for( auto& icon : m_aryBuffIcon ) {
		SAFE_RELEASE2( IMAGE_MNG, icon.m_psfcIcon );
	}
}

void XBaseUnit::OnCreate()
{
	SetScaleObj( GetPropScale() );
	//
	if( GetUnitSize() <= XGAME::xSIZE_MIDDLE ) {
		m_psoHit = new XSprObj( _T( "eff_hit02.spr" ) );
		m_psoHit->SetAction( 1, xRPT_1PLAY );
		m_psoHit->DoFinish();	// 사전 로딩한거기땜에 애니메이션이 나오면 안되서.
	}
	// 유닛의 spr을 읽는다.
// 	XE::VEC3 vHSL = GetHSL();  // virtual
// 	XE::VEC2 vHSLRange1, vHSLRange2;
	XE::xHSL hsl = GetHSL2();
	_tstring strSpr = GetszSpr();
	_tstring strTitle = XE::GetFileTitle( GetszSpr() );	// 파일명만 떼넴

#ifdef _XUNIT_HSL
	const bool bUnit = (GetidProp() <= XGAME::xUNIT_FALLEN_ANGEL);
		if( m_Camp == XGAME::xSIDE_OTHER && bUnit ) {
			// 적 유닛의 경우만 2버전을 쓴다. 영웅은 2버전이 없다.
			strSpr = strTitle + _T( "2.spr" );
		}
	LoadSpr( strSpr.c_str(), hsl, ACT_IDLE1 );
#else
	if( m_Camp == XGAME::xSIDE_OTHER &&	// 상대편만 해당
		GetidProp() <= XGAME::xUNIT_FALLEN_ANGEL ) {	// 유닛만 해당
			strSpr = strTitle + _T( "2.spr" );
	}
// 	else {
// 		LoadSpr( GetszSpr(), vHSL, ACT_IDLE1 );
// 	}
	LoadSpr( strSpr.c_str(), vHSL, ACT_IDLE1, xRPT_1PLAY );
#endif // not _XUNIT_HSL
	XBREAK( GetpSprObj() == nullptr );
	XBREAK( GetpSprObj()->GetAction() == nullptr );
	XE::VEC2 vScale( GetScaleObj().x, GetScaleObj().z );
	GetpSprObj()->SetScale( vScale );		// 꼼수로 GetBoundBoxLocal()을 위해 확대시켜둠
	// IDLE상태크기의 바운딩박스를 미리 구해둠
	m_bbLocal = GetBoundBoxLocal();	// propUnit의 vScale이 반영된 결과
	if( GetUnitType() == xUNIT_GOLEM ) {
		m_bbLocal.vLT.Set( -36.f, -106.f );			// spr파일에 바운딩박스 값이 잘못돼서 직접 넣음.
		m_bbLocal.vRB.Set( 36, 10 );
	}
	m_HP = GetMaxHp();
// 	if( IsPlayer() )
// 		SetRotateY( 180.f );
	if( IsPlayer() )
		m_Dir = XE::HDIR_RIGHT;
	else
		m_Dir = XE::HDIR_LEFT;
	XSkillUser::OnCreate();
	// 전투 시작 이벤트.
}

XSPSquad XBaseUnit::GetspSquadObj() const 
{
	return m_pSquadObj->GetThis();
}


/**
 @brief FSM객체를 생성한다. virtual
*/
void XBaseUnit::CreateFSMObj()
{
	// 상태 객체들을 미리 만들어둔다.
	XEBaseFSM *pFSM = new XFSMIdle( this );
	XEControllerFSM::RegisterFSM( pFSM );
	SetpFSM( pFSM );		// 처음 시작 상태로 설정
	pFSM = new XFSMChase( this );
	XEControllerFSM::RegisterFSM( pFSM );
	pFSM = new XFSMNormalAttack( this );
	XEControllerFSM::RegisterFSM( pFSM );
	pFSM = new XFSMDie( this );
	XEControllerFSM::RegisterFSM( pFSM );
	pFSM = new XFSMStun( this );
	XEControllerFSM::RegisterFSM( pFSM );
}

void XBaseUnit::Release()
{
	m_spTarget->Release();
	m_spTarget.reset();
	for( int i = 0; i < m_aryAttacked.GetMax(); ++i )
		m_aryAttacked[i]->Release();
	m_aryAttacked.Clear();
	m_spMsgQ1->Release();
	m_spMsgQ1.reset();
	m_spMsgQ2->Release();
	m_spMsgQ2.reset();
	XEControllerFSM::Release();
}

// XHero* XBaseUnit::GetpHero()
// {
// 	if( !m_spSquadObj )
// 		return nullptr;
// 	XBREAK( m_spSquadObj->GetpHero() == nullptr );
// 	return const_cast<XHero*>( m_spSquadObj->GetpHero() );
// }

// const XHero* XBaseUnit::GetpHeroConst() const
// {
// 	XBREAK( m_spSquadObj == NULL );
// 	XBREAK( m_spSquadObj->GetpHero() == NULL );
// 	return m_spSquadObj->GetpHero();
// }

ID XBaseUnit::GetsnHero() const
{
//	const auto pHero = GetpHeroConst();
// 	return pHero->GetsnHero();
	return m_pHero->GetsnHero();
}

const XPropHero::xPROP* XBaseUnit::GetpPropHero()
{
// 	auto pHero = GetpHeroConst();
// 	XBREAK( pHero->GetpProp() == nullptr );
// 	return pHero->GetpProp();
	return m_pHero->GetpProp();
}

void XBaseUnit::CreateHitSfx( const XBaseUnit *pAttacker, BOOL bCritical, BOOL bAbsolute )
{
// #ifdef _XUZHU_HOME
// 	return;
// #endif
	if( pAttacker == NULL )
		return;
	if( pAttacker->GetUnitSize() == XGAME::xSIZE_BIG )	// 공격자가 대형이면 타격이펙트를 표시한다.
		bAbsolute = TRUE;
	if( bAbsolute || 1 ) {	// 강제생성 메시지가 들어오면 유닛크기에 관계없이 sfx를 로딩한다.
		if( m_psoHit == NULL ) {
			if( GetUnitSize() <= XGAME::xSIZE_MIDDLE || bAbsolute ) {
				m_psoHit = new XSprObj( _T( "eff_hit02.spr" ) );
				if( bCritical )
					m_psoHit->SetAction( 2, xRPT_1PLAY );
				else
					m_psoHit->SetAction( 1, xRPT_1PLAY );
				m_psoHit->DoFinish();	// 사전 로딩한거기땜에 애니메이션이 나오면 안되서.
			}
		}
	}
	if( m_psoHit ) {
		if( GetUnitSize() <= XGAME::xSIZE_MIDDLE || bAbsolute ) {
			if( m_psoHit->IsFinish() ) {
				m_vHitOffset.x = (float)( 16 - random( 32 ) );
				m_vHitOffset.y = (float)( 16 - random( 32 ) );
				m_psoHit->ResetAction();
				if( bCritical )
					m_psoHit->SetAction( 2, xRPT_1PLAY );
				else
					m_psoHit->SetAction( 1, xRPT_1PLAY );
			}
		}
	}
}

/**
 @brief 객체를 월드에 추가
*/
WorldObjPtr XBaseUnit::AddObj( XEBaseWorldObj *pNewObj )
{
	WorldObjPtr spObj = WorldObjPtr(pNewObj);
	GetpWndWorld()->AddObj( spObj );
	return spObj;
}

void XBaseUnit::FrameMove( float dt )
{
	m_cntDmgShake -= (1.f * dt);		// > 0인동안은 캐릭터가 흔들린다.
	const auto vwOld = GetvwPos();
	m_vDelta.Set( 0 );		// 이동벡터는 매프레임 초기화. 힘벡터가 필요하다면 별도로 만들것.
	if( IsAI() ) {
		FrameMoveAI( dt );
	}
	// AI와 관계없이 도는부분.
	// 방향 전환
	if( m_Dir == XE::HDIR_RIGHT )
		SetRotateY( 180.f );
	else
		if( m_Dir == XE::HDIR_LEFT )
			SetRotateY( 0 );
	// 타격 이펙트 애니메이션
	if( m_psoHit ) {
		if( m_psoHit->IsFinish() ) {
//			SAFE_DELETE( m_psoHit );
		} else
			m_psoHit->FrameMove( dt );
	}
	// 선택 애니메이션
	if( m_psoSelect )
		m_psoSelect->FrameMove( dt );
	//
	if( m_timerDead.IsOver() ) {
		if( m_timerDisappear.IsOff() )
			m_timerDisappear.Set( 2.f );
		if( m_timerDisappear.IsOver() ) {
			// 무덤 객체 생성
			ID idAct = (ID)xRandom(1, 3);
			auto pObj = new XObjLoop( GetvwPos(), _T( "obj_grave.spr" ), idAct, 60.f );
			const auto scaleFactor = GetScaleFactor();
			pObj->SetScaleObj( scaleFactor );
			GetpWndWorld()->AddObj( WorldObjPtr( pObj ) );
			//
			SetDestroy( 1 );
		} else {
			float lerp = m_timerDisappear.GetSlerp();
			SetAlpha( 1.f - lerp );
		}
	}
	++m_Count;
	XEBaseWorldObj::FrameMove( dt );
	auto pWorld = GetpWndWorld()->GetpWorld();
	if( pWorld ) {
		auto size = pWorld->GetvwSize();
		auto vwPos = GetvwPos();
		if( vwPos.x < 50.f )
			vwPos.x = 50.f;
		if( vwPos.x > size.w - 50.f )
			vwPos.x = size.w - 50.f;
		if( vwPos.y < 217 )
			vwPos.y = 217;
		if( vwPos.y > size.h )
			vwPos.y = size.h;
		SetvwPos( vwPos );
	}
#ifdef WIN32
	if( IsCheatFiltered() ) {
		if( XAPP->m_bDebugMode && XAPP->m_bBattleLogging && !m_strLog.empty() ) {
			CONSOLE( "%s", m_strLog.c_str() );
			m_strLog.clear();
		}
	}
#endif // WIN32
}

/**
 @brief 유닛들이 움직이고 있는상태(AI상태)의 FrameMove
*/
void XBaseUnit::FrameMoveAI( float dt )
{
#ifdef _DEBUG
	// 브레이크포인트용 코드
	static auto _state = XGAME::xST_FROZEN;
	if( IsState(_state) ) {
		int a = 0;
	}
#endif // _DEBUG
	if( IsLive() ) {
		// 살았을때만 돌아가는 동작.
		FrameMoveLive( dt );
	}
	else {
		// 죽었을때만 돌아가는 동작
	}
	bool bFSMFrameMove = true;
	//
	if( IsState( XGAME::xST_ICE ) || IsState(XGAME::xST_PARALYSIS) ) {
		// 얼음상태는 움직이지 못함.(얼음상태에서 죽었을때는 들어가야 하지 않나?
		bFSMFrameMove = false;
		// 시푸르딩딩하게 바꿈.
		if( IsState( XGAME::xST_ICE) )
			GetpSprObj()->SetColor( XCOLOR_RGBA( 149, 239, 255, 255 ) );
	}
	// 생존과 관계없이 돌아야 하는 부분.
	if( bFSMFrameMove ) {
		// FSM 처리를 한다.
		GetpBaseFSM()->FrameMove( dt );
		// 라이칸 하드코딩
		XBuffObj *pBuff = nullptr;
		if( GetUnitType() == xUNIT_LYCAN )
			pBuff = FindBuffSkill( _T( "invoke_immortal_blood" ) );
		// 독감염시 색 변환
		if( IsState( xST_POISON ) )
			GetpSprObj()->SetColor( XCOLOR_RGB(2,94,0) );
		else
		if( pBuff )
			// 라이칸의 경우 불사의피가 발동되면 빨간색으로 변화시킨다.
			GetpSprObj()->SetColor( XCOLOR_RGB( 255, 0, 0 ) );
		else
			// 평상시는 정상색으로표시
			GetpSprObj()->SetColor( 1.f, 1.f, 1.f );
		// 투명상태
		if( IsState( xST_INVISIBLE ) )
			GetpSprObj()->SetfAlpha( 0.5f );
		//
	} else {
		// 못움직일땐 이동속도를 0으로 만든다.
		AddAdjParam( XGAME::xADJ_MOVE_SPEED, xFIXED_VAL, 0 );
	}
	GetpSprObj()->SetbPause( !bFSMFrameMove );
}
/**
 @brief 살아있는 상태에서만 하는 동작
*/
void XBaseUnit::FrameMoveLive( float dt )
{
	// HP 리제네레이션 처리
	const ADJ_PARAM *pAdjParam = GetAdjParam( XGAME::xADJ_HP_REGEN );
	if( pAdjParam && pAdjParam->valPercent > 0 ) {
		if( m_timerHpRegen.IsOff() )
			m_timerHpRegen.Set( 1.f );
		if( m_timerHpRegen.IsOver() ) {
			float hpMax = (float)GetMaxHp();
			float hpAdd = GetAdjValue( hpMax, XGAME::xADJ_HP_REGEN );
			if( IsLive() ) {
				DoHeal( hpAdd );
			}
			m_timerHpRegen.Reset();
		}
	}
	// hp가 일정 이하가 되면 작동하는 이벤트를 실행함.
	{
		int ratio100 = (int)( ( (float)GetHp() / GetMaxHp() ) * 100.f );
		if( ratio100 < 100 ) {
//			if( ratio100 <= 기준값 )
				OnEventJunctureCommon( xJC_HP_UNDER, ratio100 );
//			기준값 = ratio100 + 1;
		}
	}
	// 미노타우르스 하드코딩
	if( GetUnitType() == XGAME::xUNIT_MINOTAUR ) {
		// 거대화 특성에 의한 공격력 보정
		auto pBuff = GetAdjParamByBuff( nullptr, _T( "grows" ), XGAME::xADJ_ATTACK );
		if( pBuff ) {
			float currAdj = pBuff->GetAbilMinbyLevel() * GetRatioHit();
			AddAdjParam( pBuff->GetEffectIndex( 0 )->invokeParameter, xPERCENT, currAdj );
		}
	}
}

/**
 @brief 프레임무브가 호출되기전에 한번 호출된다. 이것은 매프레임 한번만 불린다.
*/
void XBaseUnit::OnBeforeFrameMove()
{
	auto def = GetpSquadObj()->GetspLegionObj()->GetDef();
	AddAdjParam( XGAME::xADJ_DEFENSE, xVAL, def );
// 	m_bInvokeHitter = false;
}

void XBaseUnit::TransformByObj( XE::VEC2 *pOutLocal )
{
	XE::VEC2 vScale( GetScaleObj().x, GetScaleObj().z );
	*pOutLocal *= vScale;
	if( m_Dir == XE::HDIR_RIGHT )
	{
		pOutLocal->x = -pOutLocal->x;
	}
}

void XBaseUnit::TransformByObj( XE::VEC3 *pOutLocal )
{
	*pOutLocal *= GetScaleObj();
	if( m_Dir == XE::HDIR_RIGHT )
	{
		pOutLocal->x = -pOutLocal->x;
	}
}

void XBaseUnit::DrawShadow( const XE::VEC2& vPos, float scale )
{
	float scaleDraw = GetScaleObj().x * scale;
	const float scaleFactor = GetScaleFactor();

	scaleDraw *= scaleFactor;
	// 정식선택되었을때 인디케이터
	const SquadPtr& spSelect = XWndBattleField::sGet()->GetspSelectSquad();
	BOOL bDrawSelect = FALSE;
	if( spSelect != nullptr
		&& spSelect->GetsnSquadObj() == m_pSquadObj->GetsnSquadObj()
		&& IsLive() ) {
		// 현재 우리 부대가 선택된상태다.
		if( m_psoSelect == NULL ) {
			m_psoSelect = new XSprObj(_T("select_unit.spr"));
			m_psoSelect->SetAction( 1 );
		}
		m_psoSelect->SetScale( scaleDraw );
		m_psoSelect->Draw( vPos );
		bDrawSelect = TRUE;
	}
	// 임시선택되었을때 인디케이터
	const SquadPtr& spTemp = XWndBattleField::sGet()->GetspTempSelect();
	if( spTemp != nullptr
		&& spTemp->GetsnSquadObj() == m_pSquadObj->GetsnSquadObj()
		&& IsLive() ) {
		if( m_psoSelect == NULL ) {
			m_psoSelect = new XSprObj( _T( "select_unit.spr" ) );
			m_psoSelect->SetAction( 2 );
		}
		m_psoSelect->SetScale( scaleDraw );
		m_psoSelect->SetColor( 1.f, 1.f, 1.f );
		m_psoSelect->Draw( vPos );
		bDrawSelect = TRUE;
	}
	const SquadPtr& spSelectEnemy = XWndBattleField::sGet()->GetspTempSelectEnemy();
	if( spSelectEnemy != nullptr
		&& spSelectEnemy->GetsnSquadObj() == m_pSquadObj->GetsnSquadObj()
		&& IsLive() )	{
		if( m_psoSelect == NULL ) {
			m_psoSelect = new XSprObj( _T( "select_unit.spr" ) );
			m_psoSelect->SetAction( 3 );
		}
		m_psoSelect->SetScale( scaleDraw );
		m_psoSelect->SetColor( 1.f, 0, 0 );
		m_psoSelect->Draw( vPos );
		bDrawSelect = TRUE;
	}

	if( bDrawSelect == FALSE ) {
		// 선택안된 상태면 리소스 지워버림.
		SAFE_DELETE( m_psoSelect );
	}

	if( m_psfcShadow /*&& m_psoSelect == NULL*/ ) {
		XE::VEC2 vAdj = ( m_psfcShadow->GetSize() / -2.f ) * scaleDraw;;
		m_psfcShadow->SetScale( scaleDraw );
// 		m_psfcShadow->SetfAlpha( 0.8f * GetAlpha() );
		m_psfcShadow->SetfAlpha( 1.0f * GetAlpha() );
		m_psfcShadow->Draw( vPos + vAdj );
	}
}

/**
 @brief 유닛사이즈에 따른 스케일 상수값을 얻는다. 주로 sfx등의 크기조절에 사용한다.
*/
float XBaseUnit::GetScaleFactor()
{
	float scaleFactor = 1.f;
	if( IsHero() ) {
		scaleFactor = 2.0f;
	} else {
		switch( GetUnitSize() ) {
		case XGAME::xSIZE_SMALL:
		if( m_pSquadObj->GetUnit() == XGAME::xUNIT_PALADIN )
			scaleFactor = 1.5f;
		break;
		case XGAME::xSIZE_MIDDLE:
			scaleFactor = 1.5f;
			break;
		case XGAME::xSIZE_BIG:
			scaleFactor = 2.0f;
			break;
		}
	}
	return scaleFactor;
}

/**
 @brief this유닛이 필터링에 걸리는지.
 0x01: 플레이어: 영웅
 0x02: 플레이어: 유닛
 0x04: 적군: 영웅
 0x08: 적군: 유닛
*/
bool XBaseUnit::IsCheatFiltered()
{
	return( (IsPlayer() && IsHero() && (XAPP->m_dwFilter & 0x01))	
		|| (IsPlayer() && IsUnit() && (XAPP->m_dwFilter & 0x02))
		|| (!IsPlayer() && IsHero() && (XAPP->m_dwFilter & 0x04)) 
		|| (!IsPlayer() && IsUnit() && (XAPP->m_dwFilter & 0x08)) );
}

void XBaseUnit::Draw( const XE::VEC2& vPos, float scale, float alpha )
{
	XPROF_OBJ_AUTO();
	//
	m_vsPos = vPos;	// 최적화를 위하여 draw시에 사용했던 프로젝션된 좌표를 받아둔다.
//	float scaleDraw = GetScaleObj().x * scale;
	const float scaleFactor = GetScaleFactor();// * GetPropScale();
	const float scaleProp = GetPropScale();
	auto vSizeUnit = GetSize();		// this 유닛의 크기
	if( IsHero() ) {
//		scaleFactor = 1.f;
		vSizeUnit.h = HEIGHT_HERO * GetPropScale();		// 영웅은 그림마다 크기가 달라서 일률적으로 맞춤.
	}
//	scaleDraw *= scaleFactor;
	//
	{
		XPROF_OBJ( "draw unit" );
		if( IsState( XGAME::xST_INVISIBLE ) )
			alpha = 0.5f;	// 현재 투명도에서 다시 1/2한다.
		const float xAdjShake = ( m_cntDmgShake > 0 )? xRandomF( -2, 2 ) : 0;
		float col = (m_cntDmgShake > 0 )? (255.f * (1.0f - (m_cntDmgShake / c_maxDmgShake))) : 255.f;
		GetpSprObj()->SetColor( XCOLOR_RGBA(255, (int)col, (int)col, 255) );
		XEBaseWorldObj::Draw( vPos + (XE::VEC2(xAdjShake, 0) * scale), scale, alpha );
	}
	// 이부분도 느림 피격이펙이 안나오고 있을때도 draw하는지 검사
//#ifndef _XUZHU_HOME
	if( m_psoHit && m_psoHit->IsFinish() == FALSE )	  {
		XPROF_OBJ( "draw hit" );
		XE::VEC2 v = GetvsCenter();
		v += m_vHitOffset * scale;
		// 맞는측의 크기에 비례해서 타격이펙트도 커진다.
		m_psoHit->SetScale( scaleFactor * scaleProp * 0.5f );
		m_psoHit->Draw( v );
	}
//#endif
	XE::VEC2 vDrawHp = vPos;
	// drawhp
	if( IsLive() ) {
		XE::VEC2 sizeBar = XE::VEC2( 35 * scaleFactor * scale, 5 * scale );
		if( sizeBar.w < 1.f )
			sizeBar.w = 1.f;
		if( sizeBar.h < 1.f )
			sizeBar.h = 1.f;
		const XE::VEC3 _vSize = vSizeUnit * scale;
		vDrawHp.x -= sizeBar.w / 2.f;
		vDrawHp.y -= _vSize.h + ( 2.f * scaleFactor );
		if( (m_timerDamage.IsOn() && !m_timerDamage.IsOver()) || IsHero() )	{// 영웅은 항상 표시
			XPROF_OBJ( "draw bar" );
			float lerp = (float)GetHp() / GetMaxHp();
			XCOLOR col = XCOLOR_RGBA( 210, 31, 1, 255 );
			XCOLOR colBg = XCOLOR_RGBA( 43, 14, 8, 255 );
			if( IsPlayer() ) {
				col = XCOLOR_RGBA( 1, 211, 2, 255 );
				colBg = XCOLOR_RGBA( 9, 43, 10, 255 );
			}
			XUTIL::DrawProgressBar( vDrawHp, lerp, sizeBar, col, colBg );
		}
	}
	if( m_timerDamage.IsOver() )
		m_timerDamage.Off();
	auto vDrawName = vDrawHp;
	// draw name
	if( IsHero() && IsLive() ) {
		vDrawName = DrawName( vPos, scaleFactor, scale, vDrawHp );
	}
	if( /*IsHero() &&*/ IsLive() ) {
//		vDrawName = DrawName( vPos, scaleFactor, scale, vDrawHp );
		// 버프 디버프 아이콘 표시
		{
			int idx = 1;
			for( auto pBuffObj : GetlistSkillRecvObj() ) {
				// 이미 로딩한건지 검사.
				auto pSkillDat = pBuffObj->GetpDat();
				// 액티브 스킬만 표시한다(특성의 액티브발동효과는 idName이 없다)
//				if( pSkillDat->IsActive() && pSkillDat->GetidName() ) {
//			if( pSkillDat->IsActive() && !pSkillDat->GetstrIcon().empty() ) {
//				if( pSkillDat->IsActive() ) {
				// 버프중에서도 지속시간이 유한한 버프만 출력.
				if( pSkillDat->IsBuffShort() ) {
					auto strIcon = pSkillDat->GetstrIcon();
					if( strIcon.empty() ) {
						strIcon = pBuffObj->GetstrIconByCaller();
					}
					XSurface *psfc = nullptr;
					// 같은 종류가 두개걸렸을때는 하나만 표시한다.
					for( auto& icon : m_aryBuffIcon ) {
						if( icon.m_idSkill == pSkillDat->GetidSkill() ) {
							psfc = icon.m_psfcIcon;
							break;
						}
					}
					if( psfc == nullptr && !strIcon.empty() ) {
						xIconBuff icon;
						icon.m_idSkill = pSkillDat->GetidSkill();
						icon.m_psfcIcon = IMAGE_MNG->Load( XE::MakePath( DIR_IMG, strIcon ) );
						psfc = icon.m_psfcIcon;
						XBREAK( psfc == nullptr );
						m_aryBuffIcon.push_back( icon );
					}
					if( psfc ) {
						const auto bitCaster = pBuffObj->GetbitCampCaster();
						if( m_Camp.IsEnemy( bitCaster ) )
							psfc->SetColor( XCOLOR_RGBA(200,0,0,255) );
						const float scaleBuff = (GetUnitSize() == xSIZE_SMALL)? 0.25f : 0.5f;
						const float scaleIcon = scaleBuff * scale;
						const XE::VEC2 sizeIcon = psfc->GetSize() * scaleIcon;
						XE::VEC2 vDrawBuffIcon = vPos;
						vDrawBuffIcon.x -= ( sizeIcon.w * idx ) * 0.5f;
						vDrawBuffIcon.y = vDrawName.y - ( ( 2.f * scale ) + sizeIcon.h );
						psfc->SetScale( scaleIcon );
						psfc->Draw( vDrawBuffIcon );
						if( pSkillDat->GetstrIdentifier() == _T( "invoke_protect_shell" ) ) {
							PUT_STRINGF_SMALL( vDrawBuffIcon + XE::VEC2(1,1), XCOLOR_WHITE, _T("%d"), m_cntShell );
						}
					}
					++idx;
				}
			}
		}
	}
#ifdef _CHEAT
	if( XAPP->GetbDebugMode() ) {
		_tstring strDebug;
		XCOLOR colDebug = XCOLOR_WHITE;
		if( XAPP->m_bDebugViewRefCnt && IsCheatFiltered() ) {
			int cntRef = GetThis().use_count();
			strDebug += XE::Format( _T( "refCnt=%d\n" ), cntRef );
		}
		if( XAPP->m_bDebugViewUnitSN && IsCheatFiltered() ) {
			strDebug += XE::Format(_T("sn=%d\n"), GetsnObj() );
		}
		if( XAPP->m_bDebugViewTarget && GetidTarget() != 0 && IsCheatFiltered() ) {
			strDebug += XE::Format( _T( "target=%d\n" ), GetidTarget() );
			strDebug += XE::Format( _T( "vwbind=%d,%d\n" ), (int)m_vwBind.x, (int)m_vwBind.y );
			if( m_spTarget != nullptr && IsBindTarget() ) {
				float scale = 1.f;
				XE::VEC3 vDst = m_vwBind;
				XE::VEC2 vsDst = XWndBattleField::sGet()->GetPosWorldToWindow( vDst, &scale );
				XE::VEC2 vsSrc = XWndBattleField::sGet()->GetPosWorldToWindow( GetvwPos(), &scale );
				GRAPHICS->DrawCircle( vsDst.x, vsDst.y, 25.f, XCOLOR_WHITE );
				GRAPHICS->DrawLine( vsSrc, vsDst, XCOLOR_WHITE );
			}
		}
		if( XAPP->m_bDebugViewBoundBox && IsCheatFiltered() ) {
			XE::VEC2 vLT = vPos + m_bbLocal.vLT * scale;
			XE::VEC2 vRB = vPos + m_bbLocal.vRB * scale;
			XE::DrawRectangle( vLT, vRB, XCOLOR_WHITE );
		}
		if( XAPP->m_bDebugViewBuff && IsCheatFiltered() ) {
			for( auto pBuffObj : GetlistSkillRecvObj() ) {
				_tstring s = XE::Format( _T( "+%s(%d)\n" ), pBuffObj->GetpDat()->GetszIdentifier()
																									, pBuffObj->GetpDat()->GetidSkill() );
				strDebug += s;
			}
			if( m_cntShell > 0 ) {
				strDebug += XFORMAT("shell:%d\n", m_cntShell );
			}
			// 상태이상 표시
			int max = GetMaxState();
			int len = strDebug.length();
			for( int i = 1; i < XGAME::xST_MAX; ++i ) {
				if( IsState( i ) ) {
					strDebug += GetStrState( ( XGAME::xtState )i );
					if( ( XGAME::xtState )i == XGAME::xST_TAUNT ) {
						if( m_spTarget != nullptr )
							strDebug += XFORMAT( "(%s)", XGAME::GetStrUnit( m_spTarget->GetSquadUnit() ) );
					}
					strDebug += _T( "/" );
				}
			}
			if( strDebug.length() != len )
				strDebug += _T( "\n" );
		}
		// 공격력 보정치 출력(버프없이 adjParam만 변하는 효과에 사용.
#ifdef _DEBUG
		{
			float adjVal = GetAdjParam( XGAME::xADJ_EVADE_RATE_RANGE )->GetValImm();
			if( adjVal != 0 )
				strDebug += XFORMAT("evade:%.2f\n", adjVal);
		}
#endif // _DEBUG
		// hp표시
		if( XAPP->m_bDebugViewHp && IsCheatFiltered() )
			strDebug += XE::Format(_T("%d/%d\n"), GetHp(), GetMaxHp() );
		OnDebugStr( strDebug );	// virtual
		if( strDebug.empty() == false )		{
			int numCR = XE::GetNumCR( strDebug.c_str() );
			XE::VEC2 vTop = vDrawHp;
			float sizeFont = BASE_FONT_SMALL->GetFontSize() / 2.f;
			vTop.y -= (sizeFont * (numCR)) * scaleFactor;
			if( IsPlayer() ) {
				if( IsHero() )
					colDebug = XCOLOR_WHITE;
				else
					colDebug = XCOLOR_ARGB( 255, 200, 200, 200 );
			} else {
				if( IsHero() )
					colDebug = XCOLOR_RED;
				else
					colDebug = XCOLOR_ARGB( 255, 255, 100, 100 );
			}
			DrawDebugStr( &vTop, colDebug, sizeFont, strDebug );
		}
	} // if( XAPP->GetbDebugMode() )
#endif // 치트정보를 맨 마지막에 찍을것
} // Draw

void XBaseUnit::DrawDebugStr( XE::VEC2* pvLT, XCOLOR col, float sizeFont, const _tstring& strDebug )
{
	XVector<_tstring> aryStr;
	XSYSTEM::CutStringByChar( strDebug, '\n', &aryStr );
	for( auto& str : aryStr ) {
		PUT_STRING_SMALL( pvLT->x, pvLT->y, col, str.c_str() );
		pvLT->y += sizeFont;
	}
}

/**
 @brief spr의 타점이벤트때 호출
*/
void XBaseUnit::OnEventHit( const xSpr::xEvent& event )
{
	if( IsDead() )
		return;
	/*
	상태와 동작은 반드시 일치해야한다.  공격모션중인데 IDLE상태라거나 이러면 안된다.
	아래와 같은 상황은 공격모션이 시작되었을때 IDLE상태로 강제 전환 시켜버려서 생기는 문제다.
	공격상태에서 벗어날때 none으로 만들기 때문.
	비주얼적으로 자연스럽게 보이기 위해서는 공격중에 idle명령을 받았더라도 공격모션은 끝나고
	idle상태가 되어야 자연스럽다. 따라서 강제 상태변화는 되면 안되며, 상태변화는 메시지 형태로
	전달되어야 한다. 공격모션(공격상태)이 끝나면 idle이 될 수 있도록.
	명령을 메시지화 시키면 중간에 값이 꼬이는 일도 막을 수 있다.
	*/
	if( m_typeCurrMeleeType == XGAME::xMT_NONE )
		return;
// 	if( XBREAK( m_typeCurrMeleeType == XGAME::xMT_NONE ) )
// 		return;
	XE::VEC3 vwSrc = GetvwPos();
//	XE::VEC2 vLx( lx, ly );	// 트랜스폼 안한 원본 좌표
	auto vLx = event.m_vLocal;
	TransformByObj( &vLx );
	const auto v3Lx = XE::VEC3( vLx.x, 0.f, vLx.y );
	vwSrc += v3Lx;
	m_vlActionEvent = v3Lx;
	if( m_spTarget != nullptr )	{
		bool bHit = IsHit( m_spTarget );
		// 근접/원거리 공통 데미지 계산부.
		float damageFinal = 0;
		float damage = 0;
		//
		ADD_LOG( m_strLog, "공격:%s, 방어:%s ", GetstrcIds().c_str()
																					, m_spTarget->GetstrcIds().c_str() );
		if( m_typeCurrMeleeType == XGAME::xMT_MELEE )	// 근접유닛의 근접공격 혹은 원거리유닛의 근접공격
			damage = GetAttackMeleeDamage( m_spTarget );
		else
			damage = GetAttackRangeDamage( m_spTarget );
		damage = hardcode_OnToDamage( m_spTarget, damage, m_typeCurrMeleeType );
		bool bCritical = IsCritical( m_spTarget );
		if( bCritical )
			damageFinal = damage * GetCriticalPower();
		else
			damageFinal = damage;
		float ratioPenet = GetPenetrationRatio();
		//
		if( m_typeCurrMeleeType == XGAME::xMT_MELEE ) {
			UnitPtr spTarget = m_spTarget;
			// 근접공격시는 곧바로 데미지를 입힌다.
			// this가 공격하는 대상이 골렘이 아닐때만 검사
			if( m_spTarget->GetUnitType() != xUNIT_GOLEM ) {
				// this가 골렘으로부터 invoke_sacrifice에 걸렸을때만 검사
				auto pBuff = FindBuffSkill( _T( "invoke_sacrifice" ) );
				if( pBuff ) {
					float prob = pBuff->GetInvokeRatioByLevel();
					if( prob > 0 && XE::IsTakeChance(prob)) {
						// 대신맞아주는 골렘이 있으면 그 골렘에게 데미지를 준다.
						UnitPtr spGolem = GetSacrificeGolemInAttacker();
						if( spGolem != nullptr &&		// 대신맞아주는 골렘이 있고
							m_spTarget->GetsnObj() != spGolem->GetsnObj() )
							spTarget = spGolem;		// 데미지 주는 대상을 희생골렘으로 바꾼다..
					}
				}
			}
			BIT bitAttrHit = xBHT_HIT;
			// 빗나가면 데미지를 0으로 준다.
			if( bHit == false ) {
				damageFinal = 0;
				bitAttrHit &= ~xBHT_HIT;		// 적중 속성을 꺼줌.
			}
			if( bCritical )
				bitAttrHit |= xBHT_CRITICAL;
			//
//			spTarget->DoDamage( this, damageFinal, ratioPenet, XSKILL::xDMG_MELEE, bitAttrHit, xDA_NONE );
			auto pMsg = std::make_shared<xnUnit::XMsgDmg>( GetThisUnit()
																									, spTarget
																									, damageFinal
																									, ratioPenet
																									, xDMG_MELEE
																									, bitAttrHit
																									, xDA_NONE );
			spTarget->PushMsg( pMsg );
		} else {
			// xMT_RANGE
			auto spTarget = GetspTarget();
			if( spTarget ) {
				// 쏘려는 타점에서 적이 이미 죽었으면 다른타겟으로 쏜다.
				if( spTarget->IsDead() ) {
					auto spTargetTemp = m_pSquadObj->GetAttackTargetForUnit( GetThisUnit() );
					if( spTargetTemp ) {
						spTarget = spTargetTemp;
					}
				}
			}
			auto vwDst = spTarget->GetvCenterWorld();
			// virtual처리로 공격케이스별로 프로세스를 달리한다.
			// 빗나가면 데미지를 0으로 준다.
			if( bHit == false )
				damageFinal = 0;
			ShootRangeAttack( spTarget, vwSrc, vwDst, damageFinal, bCritical, event.m_aryStrParam[0], event.m_strSpr );
			// 발사체에 실어보낼 발동스킬 목록을 클리어.
			m_aryInvokeSkillByAttack.Clear();
		}
	}
}

/**
 @brief this의 공격자들중에 희생 특성을 가진 골렘이 있는지 찾는다
*/
UnitPtr XBaseUnit::GetSacrificeGolemInAttacker()
{
	XARRAYN_LOOP_AUTO( m_aryAttacked, &spAttacker )
	{
		if( spAttacker != nullptr && spAttacker->GetUnitType() == xUNIT_GOLEM )
		{
			auto pBuff = spAttacker->FindBuffSkill(_T("sacrifice"));
			if( pBuff )
			{
				// 희생특성을 가진 골렘이 있다.
				return spAttacker;
			}
		}
	} END_LOOP;
	return UnitPtr();
}

void XBaseUnit::cbOnArriveBullet( XObjArrow *pArrow, float damage )
{
}

void XBaseUnit::OnArriveBullet( XObjBullet *pBullet,
								UnitPtr spAttacker,
								UnitPtr spTarget,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical,
								LPCTSTR sprArrive, ID idActArrive,
								DWORD dwParam )
{
	if( spTarget && spTarget->IsLive() ) {
		const float ratioPenet = GetPenetrationRatio();
		if( damage ) {	// 스킬 발사체의 경우 damage가 0인채로 오므로 들어가지 않게 함. 스킬발사체의 데미지는 스킬시스템에서 줌.
			BIT bitAttrHit = xBHT_HIT;
			if( bCritical )
				bitAttrHit |= xBHT_CRITICAL;
			if( damage == 0 )
				bitAttrHit &= ~xBHT_HIT;
			auto pMsg = std::make_shared<xnUnit::XMsgDmg>( spAttacker
																									, spTarget
																									, damage
																									, ratioPenet
																									, xDMG_RANGE
																									, bitAttrHit
																									, xDA_NONE );
			spTarget->PushMsg( pMsg );
//			spTarget->DoDamage( spAttacker.get(), damage, ratioPenet, XSKILL::xDMG_RANGE, bitAttrHit, xDA_NONE );
		}
	}
}

/**
 @brief 화살쏘기 전용(영웅+궁수)
*/
XObjArrow* XBaseUnit::ShootArrow( UnitPtr& spTarget,
								const XE::VEC3& vwSrc,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical )
{
	_tstring strSpr = _T("arrow.spr");
	bool bFrozenArrow = false;
	// 냉기폭발화살이 발동되었는지 확인.
	XARRAYLINEARN_LOOP_AUTO( m_aryInvokeSkillByAttack, &eff ) {
		if( eff.pDat->GetstrIdentifier() == _T( "chill_blast_arrow" ) )
			bFrozenArrow = true;
	} END_LOOP;
	if( bFrozenArrow )
		strSpr = _T( "arrow2.spr" );
	// 메인 화살
	auto pArrow = new XObjArrow( GetpWndWorld(),
																GetThisUnit(),
																spTarget,
																vwSrc,
																vwDst,
																damage,
																bCritical,
																strSpr.c_str(), 1, 11.f );
//										strSpr.c_str(), 1, 15.f );
	if( XASSERT( pArrow ) ) {
			XARRAYLINEARN_LOOP_AUTO( m_aryInvokeSkillByAttack, &eff ) {
				_tstring strInvokeSkill;
				if( bFrozenArrow ) {
					// 빙결화살 특성이 있으면 빙결화살의 발동스킬을 화살에 넣음.
					auto pBuff = FindBuffSkill( _T("freeze_arrow") );
					if( pBuff )
						strInvokeSkill = pBuff->GetEffectIndex( 0 )->strInvokeSkill;
					else
						strInvokeSkill = eff.pEffect->strInvokeSkill;
					pArrow->AddInvokeSkill( strInvokeSkill );
				}
				{
					auto pBuff = FindBuffSkill( _T( "instant_death_arrow" ) );
					if( pBuff )
						strInvokeSkill = pBuff->GetEffectIndex( 0 )->strInvokeSkill;
					else
						strInvokeSkill = eff.pEffect->strInvokeSkill;
					pArrow->AddInvokeSkill( strInvokeSkill );
				}
			} END_LOOP;
		pArrow->SetpDelegate( this );
		GetpWndWorld()->AddObj( WorldObjPtr( pArrow ) );
	}
	// 추가화살
	// 애기살 특성이 있는가.
	XBuffObj *pBuff = FindBuffSkill( _T( "pyeonjeon" ) );
	if( pBuff ) {
		float ratioInvoke = pBuff->GetInvokeRatioByLevel();
		// 확률에 걸렸는가.
		if( XE::IsTakeChance( ratioInvoke ) ) {
			// 애기살은 무조건 크리.
			float damageLocal = damage * GetCriticalPower();
			auto pSubArrow = new XObjArrow( GetpWndWorld(),
																			GetThisUnit(),
																			spTarget,
																			vwSrc,
																			vwDst,
																			damageLocal,
																			true,
																			_T("arrow.spr"), 2, 11.f );
			if( XASSERT( pSubArrow ) ) {
				pSubArrow->SetpDelegate( this );
				GetpWndWorld()->AddObj( WorldObjPtr( pSubArrow ) );
			}
		}
	}
	if( IsHero() && IsRange() ) {
		// 조준사격버프가 있을때
		pBuff = FindBuffSkill(_T("aim_shoot"));
		if( pBuff ) {
			float ratioInvoke = pBuff->GetAbilMinbyLevel();
			if( XE::IsTakeChance( ratioInvoke ) ) {
				float damageLocal = damage * GetCriticalPower();
				auto pSubArrow = new XObjArrow( GetpWndWorld(),
																				GetThisUnit(),
																				spTarget,
																				vwSrc,
																				vwDst,
																				damageLocal,
																				true,
																				_T("arrow_critical.spr"), 1, 13.f );
// 																				_T("arrow.spr"), 2, 13.f );
				if( XASSERT( pSubArrow ) ) {
					pSubArrow->SetpDelegate( this );
					GetpWndWorld()->AddObj( WorldObjPtr( pSubArrow ) );
				}
			}
		}
	}
	return pArrow;
}

/**
 @brief pAttacker가 this에게 damage의 대미지를 가했다.
 @param ratioPenetration 관통율. 기본 0%이며 100%면 상대 방어력을 완전히 무시한다. -1이면 파라메터 값을 무시하고 pAttacker로부터 얻는다.
 @param typeDamage 데미지타입(근접/원거리/마법). 0은 속성없음
*/
void XBaseUnit::DoDamage( XSPWorldObj spAtkObj,
													float damage,
													float ratioPenetration,
													XSKILL::xtDamage typeDamage,
													const BIT bitAttrHit,
													XGAME::xtDamageAttr attrDamage )
{
	const bool bCritical = (bitAttrHit & xBHT_CRITICAL) != 0;
	const bool bBySkill = (bitAttrHit & xBHT_BY_SKILL) != 0;	// 스킬이나 특성에의한 타격이다.
	const bool bMiss = (bitAttrHit & xBHT_HIT) == 0;
	const bool bPoison = (bitAttrHit & xBHT_POISON) != 0;
	const auto damageOrig = damage;
//	XSPWorldObj spAtkObj =  pAtkObj->GetThisConst();
// 	XBaseUnit* pUnitAttacker = ( pAtkObj && pAtkObj->GetType() == xOT_UNIT )
// 																			? SafeCast<const XBaseUnit*>( pAtkObj ) 
// 																			: nullptr;
	XSPUnit spUnitAtker = (spAtkObj)
														? std::static_pointer_cast<XBaseUnit>( spAtkObj )
														: nullptr;
	// pUnitAtker는 const로 하고 공격자의 메시지큐객체만 mutable로 해서 메시지를 푸쉬할수 있게 한다.
//	auto spMsgQAtker = std::static_pointer_cast<XMsgQ>( pUnitAttacker->GetspMsgQ() );
	auto pUnitAttacker = spUnitAtker.get();
//	auto spMsgQAtker = pUnitAttacker->GetspMsgQ();
	if( IsDead() ) {
		// 피격자가 죽었을때라도 스킬타입은 데미지를 표시함.
		// 스킬(특성)타입 데미지일때 예외처리
		if( pUnitAttacker && (bitAttrHit & xBHT_BY_SKILL) ) {
			// 시전자가 소형병사일 경우 스킬(특성)으로 인한 데미지ui는 표시하지 않는다.
			bool bDraw = true;
			if( pUnitAttacker && !pUnitAttacker->IsHero() && pUnitAttacker->IsSmall() ) {
				bDraw = false;
			}
			if( bDraw )
				CreateDmgNum( damage, bitAttrHit );
		}
		return;
	}
	if( damage < 0 ) {
		damage = -damage;
	}
	if( pUnitAttacker ) {
		ADD_LOG( m_strLog, "Damage:공격:%s, 방어:%s 피해량:", pUnitAttacker->GetstrcIds().c_str()
																							, GetstrcIds().c_str() );
	} else {
		ADD_LOG( m_strLog, "Damage:방어:%s 피해량:", GetstrcIds().c_str() );
	}
	// 공격자에게 "타격시"이벤트를 발생시킨다.
// 	if( pUnitAttacker && !bBySkill ) {	// DoDamage()재귀호출이 일어날수 있으니 스킬에 의한 이벤트는 발생하지 않는다.
// 		pUnitAttacker->OnAttackToDefender( this, damage, bCritical, ratioPenetration, typeDamage );
// 	}
	if( IsDead() )
		return;
// 	// 공격받은 데미지 누적(받는데미지는 방어자의 능력으로 보정되기전 순수 데미지를 누적한다.)
// 	GetpStatObj()->AddDamageAttacked( GetHero()->GetsnHero(), damage );
	// 맞는측에서 데미지 받는양보정값이 있으면 데미지를 보정시킴
	ADD_LOG( m_strLog, "피해보정전:%.1f, 크리:%d, 피해타입:%d, ", damage, xboolToByte(bCritical), (int)typeDamage );
	damage = GetAdjDamage( damage, bCritical, typeDamage, attrDamage );
	ADD_LOG( m_strLog, "보정후:%.1f, ", damage );
	float multiply = 1.f;
	// 병과패널티
	if( pUnitAttacker ) {
		// 일단 병과/크기간에 확실한 상성이 생기게 하기 위해 패널티와 보너스 둘다를 임시로 적용함
		// 전투 밸런스 작업 본격적으로 할때 다시 조절함.
		if( pUnitAttacker->IsPenaltyMOS( this ) )
			multiply -= 0.25f;
		// 크기패널티
		if( pUnitAttacker->IsPenaltySize( this ) )
			multiply -= 0.25f;
		// 우세상성이면 25%데미지가 더들어간다.
		if( pUnitAttacker->IsSuperiorMOS( this ) )
			multiply += 0.25f;
		// 크기패널티
		if( pUnitAttacker->IsSuperiorSize( this ) )
			multiply += 0.25f;
	}
	damage *= multiply;
	ADD_LOG( m_strLog, "상성적용후:%.1f, ", damage );
	float def = GetDefensePower();
	// 대형의 경우 16부대렙일경우 유닛배수가 4가되어 영웅능력치까지 곱해지면 골렘같은건 방어력100%가 넘어가게 된다.
	if( def > 100.f )
		def = 90.f;
	if( GetUnitType() == xUNIT_GOLEM ) {
		// 표피강화 특성이 있을때
		auto pBuff = FindBuffSkill( _T( "cuticle_up" ) );
		if( pBuff ) {
			// 현재 this골렘이 타운팅중이면
			auto pBuff2 = FindBuffSkill( _T( "invoke_taunt_me" ) );
			if( pBuff2 )
				def += def * pBuff->GetAbilMinbyLevel();
		}
	}
	// 일단은 데미지시 공격자가 살아있을때만 관통이 먹히는걸로.
	if( pUnitAttacker ) {
		// 관통율 1.0=모든 방어력무시
		if( ratioPenetration < 0 )
			ratioPenetration = pUnitAttacker->GetPenetrationRatio();
		XBREAK( ratioPenetration < 0 || ratioPenetration > 1.f );
		def *= ( 1.f - ratioPenetration );
		damage = damage * ( 1.f - ( def / 100.f ) );
		//		damage -= def;
	} else
		damage = damage * ( 1.f - ( def / 100.f ) );
	// 		damage -= def;
	if( damage < 0 )
		damage = 1.f;
	ADD_LOG( m_strLog, "방어도적용후:%.1f, def=%.1f, ", damage, def );
	// damage가 0이면 면역
	bool bImmune = false;
	if( damageOrig != 0 && damage == 0 )
		bImmune = true;
	bool bEvade = IsEvade( typeDamage, pUnitAttacker );
	if( bEvade ) {
		// 회피로 피한 데미지 누적
		GetpStatObj()->AddDamageByEvade( GetpHero()->GetsnHero(), damage );
		damage = 0;
		ADD_LOG( m_strLog, "회피성공, " );
	}
	if( m_cntShell > 0 ) {
		damage = 0;
		--m_cntShell;
		auto pBuff = FindBuffSkill( _T("invoke_protect_shell") );
		if( pBuff ) {
			pBuff->SetbDestroy( TRUE );
		}
	}
	XBREAK( IsDead() );
	if( damage > 0 ) {
		if( pUnitAttacker ) {
			const ID snHero = pUnitAttacker->GetsnHero();
			float d = ( m_HP < damage ) ? m_HP : damage;
			// 공격한 데미지 누적. 공격데미지는 실제 입힌 데미지만 누적시킨다.
//			auto pStatObj = const_cast<XStatistic*>( pUnitAttacker->GetpStatObj() );
			auto pStatObj = pUnitAttacker->GetpStatObj();
			pStatObj->AddDamageDeal( snHero, damage );
//			pUnitAttacker->AddDamageDeal( d );
			// 크리티컬로 입힌 데미지 누적
			if( bCritical )
				pStatObj->AddDamageDealByCritical( snHero, d );
			if( bBySkill && pUnitAttacker->IsHero() )
				pStatObj->AddDamageDealBySkill( snHero, d );
		}
	}
	// 피해데미지 누적.(실제 입힌 데미지만 누적시킨다.),
	float hpDec = damage;
	if( damage > (float)m_HP )
		hpDec = (float)m_HP;
	GetpStatObj()->AddDamageAttacked( GetpHero()->GetsnHero(), hpDec );
	bool bVorpal = false;
	const auto hpCurr = GetHp();
	// 데미지양이 최대hp의 x배 크면 즉사로 처리
	if( damage >= GetMaxHp() * 3 )
		bVorpal = true;
	ADD_LOG( m_strLog, "피해적용전:hp=%0.f", (float)GetHp() );
	//
	AddHp( -damage );
	//
	ADD_LOG( m_strLog, "피해적용:hp=%.0f/%.0f, 최종피해량:%1.f ", (float)GetHp(), (float)GetMaxHp(), damage );
	bool bNumberEffect = false;
	// 스킬류데미지면 데미지숫자를 표시
	if( bBySkill ) {
		if( pUnitAttacker ) {
			// 공격자가 소형이 아니거나 영웅일경우는 데미지숫자를 표시
			if( pUnitAttacker->GetUnitSize() != XGAME::xSIZE_SMALL
				|| pUnitAttacker->IsHero() )
				bNumberEffect = true;
		}
	}
	if( damage > 0 ) {		// 회피시에는 이벤트를 발생시키지 않는다.
		AddCntHit();
		m_cntDmgShake = XBaseUnit::c_maxDmgShake;
#ifdef _CHEAT
		if( XAPP->m_bDebugMode && IsCheatFiltered() ) {
			if( XAPP->m_bDebugHeroImmortal )
				if( IsDead() )
					m_HP = 1;
			if( XAPP->m_bDebugUnitImmortal && !IsHero() )
				if( IsDead() )
					m_HP = 1;
			// 타격이나 피격옵션이 켜져있으면 출력.
// 			if( XAPP->m_bDebugViewAttackedDamage || XAPP->m_bDebugViewDamage ) {		
// 				bNumberEffect = true;
//			}
		}
#endif
		xnUnit::xDmg dmg( spAtkObj
										, GetThisUnit(), damage, ratioPenetration, typeDamage, bitAttrHit, attrDamage, bCritical );
//		OnDamage( pUnitAttacker, damage, bCritical, typeDamage, bitAttrHit );
		OnDamage( dmg );
	}
	if( bBySkill ) {	// 스킬타입 데미지일때
		if( pUnitAttacker // 공격자가 소형일경우 데미지숫자 표시하지 않음.
			&& !pUnitAttacker->IsHero() && pUnitAttacker->IsSmall() )
				bNumberEffect = false;
	}
#ifdef _CHEAT
	if( XAPP->m_bDebugMode && !bNumberEffect ) {
		// 디버깅 모드에선 옵션에 따라 표시한다.
		if( (XAPP->m_bDebugViewAttackedDamage || XAPP->m_bDebugViewDamage) 
				&& IsCheatFiltered() )
			bNumberEffect = true;
		else
			bNumberEffect = false;		// 스킬데미지가 떠서 귀찮아서 치트모드에선 여기서 완전히 컨트롤함.
	}
#endif // _CHEAT
	// 크리거나 회피거나 즉사의경우는 무조건 메시지 표시
	if( bNumberEffect || bCritical || bEvade || bVorpal || bImmune) {
		BIT bitHit = bitAttrHit;
		if( bCritical )
			bitHit |= xBHT_CRITICAL;
		if( bEvade )
			bitHit |= xBHT_EVADE;
		if( bVorpal )
			bitHit |= xBHT_VORPAL;
		if( bImmune )
			bitHit |= xBHT_IMMUNE;
		CreateDmgNum( damage, bitHit );
	}
	if( damage > 0 ) {
		if( (pUnitAttacker && pUnitAttacker->IsHero()) || bCritical )
			CreateHitSfx( pUnitAttacker, bCritical, TRUE );
		else
			CreateHitSfx( pUnitAttacker, bCritical, FALSE );
	}
	if( GetUnitType() == xUNIT_LYCAN ) {
		auto pBuff = FindBuffSkill( _T( "invoke_immortal_blood" ) );
		if( pBuff )
			// 불사의 피가 발동되었다면 OnDie를 호출하지 않음.
			m_HP = 1;
	}
	if( GetHp() <= 0 ) {
		ADD_LOG( m_strLog, "사망: " );
		DoDie( spUnitAtker );
	}
	m_timerDamage.Set(3.f);
} // DoDamage
/**
 @brief 데미지숫자 이펙트를 생성한다.
*/
void XBaseUnit::CreateDmgNum( float damage, BIT bitAttrHit )
{
	xtHit typeHit = xHT_HIT;
	if( bitAttrHit & xBHT_CRITICAL )
		typeHit = xHT_CRITICAL;
	else if( bitAttrHit & xBHT_EVADE )
		typeHit = xHT_EVADE;
	else if( bitAttrHit & xBHT_VORPAL )
		typeHit = xHT_VORPAL;
	else if( bitAttrHit & xBHT_IMMUNE )
		typeHit = xHT_IMMUNE;
	XCOLOR col = XCOLOR_RED;
	if( bitAttrHit & xBHT_POISON ) {
		col = XCOLOR_RGB(2,94,0);
	} else {
		if( IsPlayer() )
			col = XCOLOR_WHITE;
	}
	auto pDmg = new XObjDmgNum( damage, typeHit, 0, GetvwTop(), col );
	AddObj( pDmg );
}

void XBaseUnit::DoDie( XSPUnit spAtker )
{
	m_HP = 0;
	XTRACE( "Die:%d ", GetsnObj() );
	// 아직은 DoDie시 다른 데미지 파라메터가 필요없어서 초기값으로 지정함.
	xnUnit::xDmg dmg( spAtker
									, GetThisUnit(), 0.f, 0.f, xDMG_NONE, 0, xDA_NONE, false );
	if( OnDie( dmg ) ) {
		m_timerDead.Set( 10.f );
		//
		ChangeFSM( XFSMBase::xFSM_DIE );
	}
}

/**
 @brief 데미지를 입었을때 이벤트
*/
// void XBaseUnit::OnDamage( const XBaseUnit *pAttacker
// 												, float damage
// 												, BOOL bCritical
// 												, XSKILL::xtDamage typeDamage
// 												, const BIT bitAttrHit )
void XBaseUnit::OnDamage( const xnUnit::xDmg& dmgInfo )
{
	// 반사데미지 형태는 '피격시' 이벤트를 발생시키지 않는다.
	if( !(dmgInfo.m_bitAttrHit & xBHT_THORNS_DAMAGE) ) {
		XSkillReceiver::OnHitFromAttacker( (dmgInfo.m_spUnitAtker)? dmgInfo.m_spUnitAtker.get() : nullptr
																			, dmgInfo.m_typeDmg );
	}
	GetpFSM()->OnDamage();
	// 공격자에게 데미지 피드백메시지
	if( dmgInfo.m_spUnitAtker ) {
		auto spMsg = std::make_shared<XMsgDmgFeedback>( dmgInfo );
		dmgInfo.m_spUnitAtker->PushMsg( spMsg );
//		// 공격자가 흡혈중이면 공격자에게 hp를 채운다.
// 		float ratioVampiric = pAttacker->GetVampiricRatio();
// 		if( ratioVampiric > 0 ) {
// 			float hpSteal = damage * ratioVampiric;
// 			if(  (int)hpSteal > 0 )
// 				pAttacker->DoHeal( hpSteal );
// 		}
	}
	if( IsDead() )
		return;
	if( GetUnitType() == XGAME::xUNIT_MINOTAUR ) {
		float adjMax;
		auto pBuff = GetAdjParamByBuff( &adjMax, _T("grows"), XGAME::xADJ_ATTACK );
		if( pBuff ) {
			float scale = 1.f + GetRatioHit();
			const float scaleMax = 2.f;
			if( scale > scaleMax )
				scale = scaleMax;
			SetScaleObj( GetScaleUnitOrg() * scale );
//			XBREAK( pBuff->GetEffectIndex(0)->invokeParameter != XGAME::xADJ_ATTACK );
// 			float maxAdj = pBuff->GetAbilMinbyLevel();
// 			float currAdj = scale / scaleMax;
// 			AddAdjParam( pBuff->GetEffectIndex(0)->invokeParameter, xPERCENT, currAdj );
		}
	}
	{
		// 고통의 재생
		auto pBuff = FindBuffSkill(_T("regen_suffering"));
		if( pBuff ) {
			//
//			auto hpMax = GetMaxHp();
			auto hp = GetHp();
			auto abilMin = pBuff->GetAbilMinbyLevel();
			DoHeal( hp * abilMin );
		}
	}
}

/**
 @brief this가 m_spTarget에게 데미지를 입혔다.
*/
void XBaseUnit::OnDamagedToTarget( const xDmg& dmg )
{
	// this는 공격자여야 한다.
	XASSERT( GetsnObj() == dmg.m_spAtkObj->GetsnObj() );
	// this(공격자)가 흡혈속성이 있었으면 그만큼 힐을 한다.
	const float ratioVampiric = GetVampiricRatio();
	if( ratioVampiric > 0 ) {
		float hpSteal = dmg.m_Damage * ratioVampiric;
		if( (int)hpSteal > 0 )
			DoHeal( hpSteal );
	}
}

bool XBaseUnit::OnDie( const xDmg& dmg )
{
	if( dmg.m_spUnitAtker )	{
		// 타겟(this)을 "사살"했음을 메시지로 날림
		auto spMsg = std::make_shared<XMsgKillTarget>( dmg );
		dmg.m_spUnitAtker->GetspMsgQ()->AddMsg( spMsg );
//		pAttacker->OnEventJunctureCommon( xJC_KILL_ENEMY, 0, this );
		OnEventJunctureCommon( xJC_DEAD, 0, dmg.GetpUnit() );
	}
	// 불사의 피
	if( GetUnitType() == xUNIT_LYCAN )	{
		auto pBuff = FindBuffSkill(_T("immortal_blood"));
		if( pBuff )
		{
			// 죽었을때 불사의피를 가지고 있었다면 일정시간동안 죽지 않는다.
			m_HP = 1;
			// invoke_불사의피를 발동시키고 다시 발동되지 않도록 불사의피는 삭제한다.
			pBuff->SetbDestroy( TRUE );
			return false;
		}
	}
	// 적 유닛을 죽일때마다 용맹포인트 획득
	if( !IsPlayer() )
	{
		int addBrave = XGC->m_aryAddBravePoint[ GetUnitSize() ];
//		SCENE_BATTLE->AddBrave( addBrave );
	}
	return true;
}

/**
 @brief this가 defender를 타격했을때 이벤트
 @param damage는 방어자의 방어보정값이 적용되기 전 순수공격자의 데미지다.
*/
void XBaseUnit::OnAttackToDefender( XBaseUnit *pDefender, float damage, BOOL bCritical, float ratioPenetration, XSKILL::xtDamage typeDamage )
{
	XSkillReceiver::OnAttackToDefender( pDefender, damage, bCritical, ratioPenetration, typeDamage );
}

/// 타겟카운팅이 가장 적은 오브젝트
//static XBaseUnit *s_pMinUnit = nullptr;
/**
 @brief 타겟카운팅이 가장 적은 오브젝트를 고른다.
 필터로 걸러진 오브젝트들을 비교한다.
*/
BOOL CompareSmallerTargetCnt( XEBaseWorldObj *pSrc, XEBaseWorldObj *pDst, XEBaseWorldObj *pCompObj )
{
	XBREAK( pCompObj == nullptr );
	XBaseUnit *pUnitSrc = SafeCast<XBaseUnit*, XEBaseWorldObj*>( pSrc );
	XBaseUnit *pUnitDst = SafeCast<XBaseUnit*, XEBaseWorldObj*>( pDst );
	XBaseUnit *pUnitComp = SafeCast<XBaseUnit*, XEBaseWorldObj*>( pCompObj );
//	if( pUnitSrc->IsEnemy(pUnitDst) )
	{
		// 비교 객체보다 더 작으면 TRUE를 리턴
		if( pUnitDst->GetcntTargeting() < pUnitComp->GetcntTargeting() )
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**
 @brief 비교대상을 걸러내는 필터함수
*/
BOOL FilterEnemy( XBaseUnit *pSrc, XBaseUnit *pDst )
{
	XBaseUnit *pUnitSrc = SafeCast<XBaseUnit*, XEBaseWorldObj*>( pSrc );
	XBaseUnit *pUnitDst = SafeCast<XBaseUnit*, XEBaseWorldObj*>( pDst );
	if( pUnitSrc->IsEnemyWithUnit( pUnitDst ) && pUnitDst->IsLive() )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL FilterFriendly( XBaseUnit *pSrc, XBaseUnit *pDst )
{
	XBREAK( pSrc->GetType() == XGAME::xUNIT_NONE );
	XBREAK( pDst->GetType() == XGAME::xUNIT_NONE );
	XBaseUnit *pUnitSrc = SafeCast<XBaseUnit*, XEBaseWorldObj*>( pSrc );
	XBaseUnit *pUnitDst = SafeCast<XBaseUnit*, XEBaseWorldObj*>( pDst );
	if( pUnitSrc->IsEnemyWithUnit( pUnitDst ) == FALSE && pUnitDst->IsLive() )
	{
		return TRUE;
	}
	return FALSE;
}

/**
 @brief 이번 프레임에 움직여야할 이동량을 계산한다.
 @param vwDst 목표지점
 @param speedMove 이동속도
 @param vDelta 결과를 저장할 벡터
 @return 목표에 도달한상태면 TRUE를 리턴한다.
*/
BOOL XBaseUnit::sGetMoveDstDelta( const XE::VEC3& vwCurr,
								const XE::VEC3& vwDst,
								float speedMove, XE::VEC3 *pOutDelta )
{
	BOOL bArrive = FALSE;
	float distMove = speedMove;
	// 목표와의 현재거리
	float distsq = ( vwDst - vwCurr ).Lengthsq();
	if( distsq < distMove * distMove )
	{
		// 목표와의 거리가 이동속도보다 짧다면 이번턴 이동거리는 남은거리로만 한다.
		distMove = sqrt( distsq );
		bArrive = TRUE;		// 도착
	}
	// 목표방향으로의 각도
	XE::VEC2 v2Src = vwCurr.ToVec2();
	XE::VEC2 v2Dst = vwDst.ToVec2();
	float dAng = XE::CalcAngle( v2Src, v2Dst );
	// 이번턴에 이동해야할 이동량
	*pOutDelta = XE::GetAngleVector( dAng, distMove );
	return bArrive;
}

/**
 @brief 인공지능을 켜거나 끈다.
*/
void XBaseUnit::SetAI( BOOL bFlag )
{
	if( bFlag ) {
		m_bitFlags |= xFL_AI;
		OnAISet( true );
//		RequestNewMission();		// 유닛 개별로 인공지능을 껏다켯다 할일은 없으므로 뺐음.
		// AI가 켜지면 제일먼저 공격대상을 잡아서 추적하는 모드에 들어간다.
	} else {
		// AI가 꺼지면 대기모드로 들어간다.
		m_bitFlags &= (~xFL_AI);
		OnAISet( false );
		DoIdle();
	}
}

/**
 @brief this가 spObj와 근접해있느냐
*/
BOOL XBaseUnit::IsNear( const UnitPtr& spUnit )
{
	XE::VEC3 vDist = spUnit->GetvwPos() - GetvwPos();
	float sizeRadius = spUnit->GetSizeRadius();
	if( vDist.Lengthsq() <= sizeRadius * sizeRadius )
		return TRUE;
	return FALSE;
}

/**
 @brief spTarget을 추적하도록 한다.
*/
XFSMChase* XBaseUnit::DoChaseAndAttack( const UnitPtr& spTarget )
{
	if( IsDead() )
		return NULL;
	XFSMChase *pChase = GetpFSM()->DoChase( spTarget, XFSMBase::xFSM_NORMAL_ATTACK );
	return pChase;
}

void XBaseUnit::DoChaseAndAttackCurrent()
{
	if( m_spTarget == nullptr ||
		(m_spTarget != nullptr && m_spTarget->IsDead()))
		RequestNewMission();
	else
		DoChaseAndAttack( m_spTarget );
}

void XBaseUnit::DoChase( const XE::VEC3& vwDst )
{
	GetpFSM()->DoChase( vwDst );
}

void XBaseUnit::DoIdle()
{
	if( IsLive() )
		GetpBaseFSM()->ChangeFSM( XFSMBase::xFSM_IDLE );
}

/**
 @brief 타겟이 죽어 없어져 새로운 임무를 요청함.
*/
BOOL XBaseUnit::RequestNewMission()
{
	ClearBind();		// 바인드를 해제시킴
	// 현재상대하고 있는 타겟부대에서 새 타겟을 요청함.
	UnitPtr spTarget = m_pSquadObj->GetAttackTargetForUnit( GetThisUnit() );
	if( spTarget != nullptr )
	{
		// 새로운 추적/공격모드로 변경
		XFSMBase::xtFSM idNextFSM = XFSMBase::xFSM_NORMAL_ATTACK;
		GetpFSM()->DoChase( spTarget, idNextFSM );
		auto pFSMChase = SafeCast<XFSMChase*, XFSMBase*>( GetpFSM() );
		if( pFSMChase )
			pFSMChase->OnFirstNewTarget();
		// 부대에 새타겟설정 이벤트 발생
		m_pSquadObj->OnNewTarget( spTarget );
		return TRUE;
	}
	else
	{
		// 자신의 부대에게 이동모드로 바꿔달라고 요청
//		m_spSquadObj->SetbRequestMoveModeChange( TRUE );
		m_pSquadObj->DoRequestMoveMode();
		// 나는 IDLE상태로 들어감
		GetpFSM()->ChangeFSM( XFSMBase::xFSM_IDLE );
		return FALSE;
	}
}

/**
 @brief spTarget을 공격할수 있는 좌표를 찾아 바인딩 시키고 추적을 시작한다.
 @param bFirstDash 부대끼리 맞닥드렸을때 최초 대쉬하는 모드
*/
void XBaseUnit::DoAttackTargetByBind( const UnitPtr& spTarget, BOOL bFirstDash )
{
	// 타겟과 나의 목표좌표를 바인드 시키고 추적모드로 전환
	XSquadObj::sCalcBattlePos( GetThisUnit(), spTarget );
	XFSMChase *pChase = DoChaseAndAttack( spTarget );
	XBREAK( GetspTarget() == nullptr );
	if( pChase )
		pChase->Init( bFirstDash );		// 대쉬모드임을 구분
}

void XBaseUnit::DoDirToTarget( const XE::VEC3& vwDst )
{
	auto vsDst = GetpWndWorld()->GetPosWorldToWindow( vwDst );
	auto vsThis = GetpWndWorld()->GetPosWorldToWindow( GetvwPos() );
//	if( vwDst.x > GetvwPos().x )
	if( vsDst.x > vsThis.x )
		SetDir( XE::HDIR_RIGHT );
	else
		SetDir( XE::HDIR_LEFT );
}


XSPLegionObj XBaseUnit::GetspLegionObj()
{
	if( m_pSquadObj == nullptr )
		return nullptr;
	return m_pSquadObj->GetspLegionObj();
}

XSPLegionObjConst XBaseUnit::GetspLegionObjConst() const
{
	if( m_pSquadObj == nullptr )
		return nullptr;
	return m_pSquadObj->GetspLegionObj();
}

/**
 @brief this가 공격을 당할때 공격자의 적절한 위치를 잡아준다.
*/
XE::VEC2 XBaseUnit::GetAttackedPos( const UnitPtr& unitAttacker )
{
// 	if( unitAttacker->IsRange() )
// 		return XE::VEC2();
	static float sdAng[8] = { 0, 45.f, 315.f, 180.f, 135.f, 225.f, 90.f, 270.f };
	int idx = m_idxAttackedAngle;
	int cnt = 0;
	float dAng = 0;
	do
	{
		idx = m_idxAttackedAngle++ % 8;
		dAng = sdAng[ idx ];
		if( IsRight( unitAttacker.get() ) == FALSE )
		{
			dAng += 180.f;
			idx += 4;
			idx %= 8;
		}
		if( ++cnt >= 9 )	// 무한루프 방지용
			break;
		// 해당 위치에 공격자가 아직 살아있으면 다른 위치를 찾음.
	} while( m_aryAttacked[ idx ] != nullptr &&
			m_aryAttacked[ idx ]->IsLive() == TRUE );
	float radiusAttacker = 0;
	if( unitAttacker->IsRange() )
		radiusAttacker = XGAME::DIST_MELEE_ATTACK;	// 근접전때는 사거리 고정
	else
		radiusAttacker = unitAttacker->GetAttackRadiusByPixel();
	m_aryAttacked[ idx ] = unitAttacker;
	dAng += 22.5f - xRandomF(22.5f);
	radiusAttacker -= (float)random( (int)radiusAttacker / 4 );
	XE::VEC2 v = XE::GetAngleVector( dAng, radiusAttacker );
	v.y /= 4.f;
	return v;
}

/**
 @brief 병과 패널티가 적용되는지 검사
*/
BOOL XBaseUnit::IsPenaltyMOS( XBaseUnit *pDefender ) const
{
	if( GetTypeAtk() == XGAME::xAT_TANKER &&
		pDefender->GetTypeAtk() == XGAME::xAT_RANGE )
		return TRUE;
	if( GetTypeAtk() == XGAME::xAT_RANGE &&
		pDefender->GetTypeAtk() == XGAME::xAT_SPEED )
		return TRUE;
	if( GetTypeAtk() == XGAME::xAT_SPEED &&
		pDefender->GetTypeAtk() == XGAME::xAT_TANKER )
		return TRUE;
	return FALSE;
}

/**
 @brief 크기 패널티가 적용되는지 검사
*/
BOOL XBaseUnit::IsPenaltySize( XBaseUnit *pDefender ) const
{
	if( GetUnitSize() == XGAME::xSIZE_SMALL &&
		pDefender->GetUnitSize() == XGAME::xSIZE_MIDDLE )
		return TRUE;
	if( GetUnitSize() == XGAME::xSIZE_MIDDLE &&
		pDefender->GetUnitSize() == XGAME::xSIZE_BIG )
		return TRUE;
	if( GetUnitSize() == XGAME::xSIZE_BIG &&
		pDefender->GetUnitSize() == XGAME::xSIZE_SMALL )
		return TRUE;
	return FALSE;
}

/**
 @brief this가 pDefender에 비해 우세상성인가.
*/
bool XBaseUnit::IsSuperiorMOS( XBaseUnit *pDefender ) const
{
	if( GetTypeAtk() == XGAME::xAT_TANKER &&
		pDefender->GetTypeAtk() == XGAME::xAT_SPEED )
		return true;
	if( GetTypeAtk() == XGAME::xAT_RANGE &&
		pDefender->GetTypeAtk() == XGAME::xAT_TANKER )
		return true;
	if( GetTypeAtk() == XGAME::xAT_SPEED &&
		pDefender->GetTypeAtk() == XGAME::xAT_RANGE )
		return true;
	return false;
}

/**
 @brief 크기 패널티가 적용되는지 검사
*/
bool XBaseUnit::IsSuperiorSize( XBaseUnit *pDefender ) const
{
	if( GetUnitSize() == XGAME::xSIZE_SMALL &&
		pDefender->GetUnitSize() == XGAME::xSIZE_BIG )
		return true;
	if( GetUnitSize() == XGAME::xSIZE_MIDDLE &&
		pDefender->GetUnitSize() == XGAME::xSIZE_SMALL )
		return true;
	if( GetUnitSize() == XGAME::xSIZE_BIG &&
		pDefender->GetUnitSize() == XGAME::xSIZE_MIDDLE )
		return true;
	return false;
}

void XBaseUnit::OnEventCreateSfx( XSprObj *pSprObj,
									XBaseKey *pKey,
									float lx, float ly,
									float scale,
									LPCTSTR szSpr, ID idAct,
									xRPT_TYPE typeLoop,
									float secLifeTime,
									BOOL bTraceParent,
									float fAngle, float fOverSec )
{
	float secLife = secLifeTime;
	XE::VEC3 vLocal = XE::VEC3(lx, 0, ly);
	TransformByObj( &vLocal );
	XE::VEC3 vwPos = GetvwPos();
	vwPos.y += 0.1f;
	vwPos += vLocal;
	XObjLoop *pEff = new XObjLoop( vwPos, szSpr, idAct, secLife );
	pEff->SetDir( m_Dir );
	pEff->SetScaleObj( scale );
	GetpWndWorld()->AddObj( WorldObjPtr(pEff) );
}

/**
 @brief this의 가장가까운 근처에 있는 아군유닛을 찾는다.
*/
UnitPtr XBaseUnit::GetNearUnit( float meter, BIT bitCamp, bool bFindOutRange )
{
	XArrayLinearN<XBaseUnit*, 256> ary;
	if( bitCamp == XGAME::xSIDE_PLAYER )
	{
		UnitPtr spUnit
			= XEObjMngWithType::sGet()->FindNearUnitByFunc( this,
															GetvwPos(),
															meter,
															bFindOutRange,
															FilterFriendly );
		return spUnit;
	} else
	{
		UnitPtr spUnit
			= XEObjMngWithType::sGet()->FindNearUnitByFunc( this,
															GetvwPos(),
															meter,
															bFindOutRange,
															FilterEnemy );
		return spUnit;
	}
}

//////////////////////////////////////////////////////////////////////////
// SKILL
XSKILL::XDelegateSkill* XBaseUnit::GetpDelegate()
{
//	return SafeCast<XBattleField*, XEWorld*>( GetpWndWorld()->GetpWorld() );
	return this;
}

XSKILL::XSkillReceiver* XBaseUnit::GetTarget( ID snObj )
{
	return XBattleField::sGet()->GetTarget( snObj );
}
XSKILL::XSkillUser* XBaseUnit::GetCaster( ID snObj )
{
	return XBattleField::sGet()->GetCaster( snObj );
}

/**
 @brief 비보정 파라메터 효과가 적용될때 호출된다. 파라메터가 xHP같은것이 대표적인 예.
*/
int XBaseUnit::OnApplyEffectNotAdjParam( XSkillUser *pCaster
																			, XSKILL::XSkillDat* pSkillDat
																			, const EFFECT *pEffect
																			, float abilMin )
{
	switch( pEffect->invokeParameter ) {
	case XGAME::xHP: {
			// TODO: 여기서 this의 hp를 깎는등의 처리를 한다.
			XBaseUnit *pAttacker = dynamic_cast<XBaseUnit*>( pCaster );
			if( pEffect->valtypeInvokeAbility == xFIXED_VAL ) {
				abilMin = -999999.f;		// 한번에 킬
			}
			if( abilMin <= 0 ) {		// 스킬데미지가 0이라도 데미지표시함(왜?)
				if( IsLive() ) {
					XBREAK( pAttacker == nullptr );
					BOOL bCritical = pAttacker->IsCritical( UnitPtr() );
					if( bCritical )
						abilMin *= pAttacker->GetCriticalPower();
					float ratioPenet = 0.f;
					XSKILL::xtDamage typeDamage = XSKILL::xDMG_NONE;
					// 발사체있는건 원거리속성, 없는건 근거리속성으로 했음.
					if( pSkillDat->IsShootingType() )
						typeDamage = XSKILL::xDMG_RANGE;
					else
						typeDamage = XSKILL::xDMG_MELEE;
					BIT bitAttr = xBHT_HIT | xBHT_BY_SKILL;
					if( pEffect->invokeState == XGAME::xST_POISON )
						bitAttr |= xBHT_POISON;
					if( bCritical )
						bitAttr |= xBHT_CRITICAL;
					if( abilMin == 0 )
						bitAttr &= ~xBHT_HIT;
					if( pEffect->IsInvokeByHit() )
						bitAttr |= xBHT_THORNS_DAMAGE;		// 반사형 데미지
					// 상대가 죽은상태라도 데미지UI는 뜰것.
					// 스킬로 맞은 데미지는 데미지숫자를 표시
//					DoDamage( pAttacker, -abilMin, -1, typeDamage, bitAttr, xDA_NONE );
					auto pMsg = std::make_shared<xnUnit::XMsgDmg>( pAttacker->GetThis()
																											, GetThisUnit()
																											, -abilMin
																											, -1.f
																											, typeDamage
																											, bitAttr
																											, xDA_NONE );
					PushMsg( pMsg );
				}
				// 상대가 죽은상태라도 데미지UI는 뜰것.
				// 스킬로 맞은 데미지는 데미지숫자를 표시
// 				if( abilMin <= 0 ) {
// 					bool bDraw = true;
// 					auto pUnit = dynamic_cast<XBaseUnit*>( pCaster );
// 					// 시전자가 소형병사일 경우 스킬(특성)으로 인한 데미지ui는 표시하지 않는다.
// 					if( pUnit && !pUnit->IsHero() &&
// 						pUnit->GetUnitSize() == XGAME::xSIZE_SMALL )
// 						bDraw = false;		// 소형이
// 					if( bDraw ) {
// //							BOOL bCritical = FALSE;
// 						xtHit typeHit = xHT_HIT;
// 						XCOLOR col = XCOLOR_RED;
// 						if( pEffect->invokeState == XGAME::xST_POISON )
// 							col = XCOLOR_RGB(2,94,0);
// 						auto pDmg = new XObjDamageNumber( -abilMin,
// 															typeHit, 0,
// 															GetvwTop(),
// 															col );
// 						AddObj( pDmg );
// 					}
// 				}
			} else
			if( abilMin > 0 ) {
				if( IsLive() ) {
					DoHeal( abilMin );
					XBREAK( m_HP <= 0 );
				}
			}
			return 1;	// 효과를 적용했으면 1을 리턴한다.
		}
	case XGAME::xADJ_HARDCORD:
		// 하드코딩을 위한.
		return 1;
	}
	return 0;	// 아무효과도 적용하지 않았으면 0을 리턴한다.
}

//
/**
 @brief 기준타겟(혹은 기준좌표)을 중심으로 반경내 조건에 맞는 오브젝트를 요청한다.
 @param pBaseTarget 기준타겟. null일수도 있다.
 @param pvBasePos 기준좌표. null일수도있다. 그러나 기준타겟과 기준좌표가 모두 null로 넘어오진 않는다.
 @param bitSideFilter 선택해야할 우호 조건
 @param numApply 최대 선택해야할 오브젝트수. 0이면 제한이 없다.
 @param bIncludeCenter 기준타겟을 포함하여 선택할것인지 아닌지.
*/
int XBaseUnit::GetListObjsRadius( XVector<XSkillReceiver*> *plistOutInvokeTarget,
																	const XSkillDat* pSkillDat,
																	const XSKILL::EFFECT *pEffect,
																	XSKILL::XSkillReceiver *pBaseTarget,
																	const XE::VEC2& vBasePos,
																	float meter,
																	BIT bitSideFilter,
																	int numApply,
																	BOOL bIncludeCenter )
{
	XVector<XSPUnit> ary;
	XE::VEC2 vCenter;
	XEBaseWorldObj *pTarget = nullptr;
	if( pBaseTarget )	{
		pTarget = dynamic_cast<XEBaseWorldObj*>( pBaseTarget );
		if( XBREAK( pTarget == nullptr ) )
			return 0;
		vCenter = pTarget->GetvwPos().ToVec2();
	} else	{
		vCenter = vBasePos;
	}
	BIT bitFlag = 0;
	// 산자만 검색
	if( pEffect->liveTarget == XSKILL::xTL_LIVE )
		bitFlag |= XSKILL::xTF_LIVE;
	else
	// 죽은자만 검색
	if( pEffect->liveTarget == XSKILL::xTL_DEAD )
		bitFlag |= xTF_DEAD;
	else
	if( pEffect->liveTarget == XSKILL::xTL_ALL )
		bitFlag |= xTF_ALL;
	if( pSkillDat->GetstrIdentifier() == _T( "greater_taunt" ) ) {
		// 타겟을 선별할때 각각 다른 부대의 타겟을 얻도록 함.
		bitFlag |= xTF_DIFF_SQUAD;
	}
	for( auto pRecver : (*plistOutInvokeTarget) ) {
		auto pUnit = static_cast<XBaseUnit*>( pRecver );
		auto spUnit = (pUnit)? pUnit->GetThisUnit() : nullptr;
		if( spUnit ) {
			ary.Add( spUnit );
		}
	}
	XEObjMngWithType::sGet()->GetListUnitRadius2( &ary,
												pTarget,
												vCenter,
												xMETER_TO_PIXEL(meter),
												bitSideFilter,
												numApply,
												bIncludeCenter != FALSE,
												bitFlag );
	plistOutInvokeTarget->clear();
	for( auto spUnit : ary )	{
		auto pRecv = static_cast<XSkillReceiver*>( spUnit.get() );
		if (pRecv)		{
			plistOutInvokeTarget->Add( pRecv );
		}
	}
	return plistOutInvokeTarget->size();
}

/**
 @brief 새 스킬시스템에서 공통으로 사용할 이펙트 생성 함수.
 @param pvPos null이 아니면 이 좌표에 생성시켜야 한다.
*/
ID XBaseUnit::OnCreateSkillSfx( XSKILL::XSkillDat *pSkillDat,
																XSKILL::xtPoint createPoint,
																LPCTSTR szSpr,
																ID idAct,
																float secPlay,
																const XE::VEC2& vPos )
{
	auto pSfx = CreateSfxObj( createPoint, szSpr, idAct, secPlay, TRUE, 25.f, vPos );
	if( pSfx ) {
		if( pSkillDat->GetstrIdentifier() == _T( "lava_fragments" ) ) {
			const float gravity = 0.1f;
			pSfx->SetBounce( XE::VEC2( 1, 2 ), XE::VEC2( 205, 335 ), gravity );
		}
		return pSfx->GetsnObj();
	}
	return 0;
}

XSkillSfx* XBaseUnit::OnCreateSkillSfxShootTarget(
																				XSkillDat *pSkillDat,
																				XSkillReceiver *pBaseTarget,
																				int level,
																				const _tstring& strSpr,
																				ID idAct,
																				xtPoint createPoint,
																				float secPlay,
																				const XE::VEC2& vPos)
{
	auto pSfx = CreateSfxObj( createPoint, strSpr.c_str(), idAct, secPlay, FALSE, 25.f, vPos );
	return pSfx;
}

/**
 @brief 이펙트객체생성 일반화 함수
 @param wAdjZ 만약 createPoint가 TARGET_TOP일경우 필요하다면 추가보정치를 준다.
 @param bScale this객체의 크기에 비례해서 sfx의 크기도 변해야 한다면 TRUE
*/
XObjLoop* XBaseUnit::CreateSfxObj( XSKILL::xtPoint createPoint,
																		LPCTSTR szSpr,
																		ID idAct,
																		float secPlay,
																		BOOL bScale /*= FALSE*/,
																		float wAdjZ/* = 0.f*/,
																		const XE::VEC2& vPos/* = XE::VEC2(0)*/ )
{

	XObjLoop *pSfx = NULL;
	if( vPos.IsZero() == FALSE ) {
		pSfx = new XObjLoop( XGAME::xOT_SFX, XE::VEC3(vPos.x, 0, vPos.y),
												szSpr, idAct, secPlay );
	} else {
		if( createPoint == XSKILL::xPT_TARGET_CENTER ) {
			// this를 따라다니지 않음.
			pSfx = new XObjLoop( XGAME::xOT_SFX, GetThisUnit(), GetvCenterWorld(),
													szSpr, idAct, secPlay );
		} else
		if( createPoint == XSKILL::xPT_TARGET_BOTTOM ) {
			// this를 따라다니지 않음.
			pSfx = new XObjLoop( XGAME::xOT_FLOOR_OBJ, GetThisUnit(), GetvwPos(),
													szSpr, idAct, secPlay );
		} else
		if( createPoint == XSKILL::xPT_TARGET_POS ) {
			if( secPlay > 0.f )
				// this를 따라다님
				pSfx = new XObjLoop( XGAME::xOT_SFX, GetThisUnit()
													, szSpr, idAct, secPlay );
			else
				pSfx = new XObjLoop( XGAME::xOT_SFX, GetvwPos(),
														szSpr, idAct, secPlay );
		} else
		if( createPoint == XSKILL::xPT_TARGET_TRACE_CENTER ) {
			// 타겟의 중심을 따라다님
			pSfx = new XObjLoop( XGAME::xOT_SFX, GetThisUnit(),
													szSpr, idAct, secPlay );
			pSfx->SetvAdjust( 0.f, 0.f, -GetSize().h * 0.5f );
		} else
		if( createPoint == XSKILL::xPT_TARGET_TRACE_POS ) {
			// 타겟의 좌표를 따라다님
			pSfx = new XObjLoop( XGAME::xOT_SFX, GetThisUnit(),
													szSpr, idAct, secPlay );
		} else
		if( createPoint == XSKILL::xPT_TARGET_TRACE_BOTTOM ) {
			pSfx = new XObjLoop( XGAME::xOT_FLOOR_OBJ, GetThisUnit(),
													szSpr, idAct, secPlay );
		} else
		if( createPoint == XSKILL::xPT_ACTION_EVENT ) {
			TransformByObj( &m_vlActionEvent );
			auto vwActionPos = GetvwPos() + m_vlActionEvent;
			pSfx = new XObjLoop( XGAME::xOT_SFX, GetThisUnit(), vwActionPos,
													szSpr, idAct, secPlay );
		} else
		if( createPoint == XSKILL::xPT_TARGET_TOP ) {
			pSfx = new XObjLoop( XGAME::xOT_SFX, GetThisUnit(), GetvwTop( wAdjZ ),
													szSpr, idAct, secPlay );
		} else 
		if( createPoint == XSKILL::xPT_TARGET_TRACE_TOP ) {
			// 타겟의 좌표를 따라다님
			pSfx = new XObjLoop( XGAME::xOT_SFX, GetThisUnit(), 
													szSpr, idAct, secPlay );
			pSfx->SetvAdjust( 0.f, 0.f, -GetSize().h * 0.5f );
		} else {
			pSfx = new XObjLoop( XGAME::xOT_SFX, GetThisUnit(), GetvCenterWorld(),
													szSpr, idAct, secPlay );
		}
	}
	XBREAK( pSfx == NULL );
	pSfx->SetDir( GetDir() );
	if( XE::IsSame(szSpr, _T("sfx_frozen.spr")) ) {
		pSfx->SetAlpha( 0.5f );
	}
	if( bScale ) {
		float scaleFactor = GetScaleFactor();
		if( IsHero() )
			scaleFactor = 1.f;
		pSfx->SetScaleObj( scaleFactor );
	}
	XBattleField::sGet()->AddObj( WorldObjPtr( pSfx ) );
	return pSfx;
}

/**
 @brief
*/
XSkillReceiver* XBaseUnit::CreateSfxReceiver( EFFECT *pEffect, float sec )
{
	BIT bitSide = 0;
	if( pEffect->invokefiltFriendship == xfHOSTILE ) {
		if( IsPlayer() )
			bitSide = XGAME::xSIDE_PLAYER;	// 적에게 써야 하는 이펙트면 sfx이펙트의 편은 우리편으로 해야함.
		else
			bitSide = XGAME::xSIDE_OTHER;
	} else {
		if( IsPlayer() )
			bitSide = XGAME::xSIDE_OTHER;
		else
			bitSide = XGAME::xSIDE_PLAYER;
	}
	auto pReceiver = new XSkillSfxReceiver( bitSide, GetvwPos(), sec );
	AddObj( pReceiver );
	return pReceiver;
}

/**
 @brief
*/
void XBaseUnit::cbOnArriveSkillObj( XSkillShootObj *pArrow,
									XSKILL::XSkillDat *pSkillDat,
									int level,
									XSKILL::XSkillReceiver *pBaseTarget )
{
	if( pSkillDat->GetstrIdentifier() == _T("throw_spear_shoot" ))	{
		XBuffObj *pBuffObj = FindBuffSkill( _T("gungnir") );
		if( pBuffObj )		{
			// 궁니르버프가 있을때 창으로 대상을 맞춤.
			pBuffObj->OnEventJunctureCommon( XSKILL::xCOND_HARD_CODE );
		}
	}
	XSkillUser::CastSkillToBaseTarget( pSkillDat, level, pBaseTarget, XE::VEC2(0), pSkillDat->GetidSkill() );
}

/**
 @brief
*/
XSKILL::XSkillUser* XBaseUnit::CreateAndAddToWorldShootObj(
										XSKILL::XSkillDat *pSkillDat,
// 										XSKILL::EFFECT *pEffect,
										int level,
										XSKILL::XSkillReceiver *pBaseTarget,
// 										XSKILL::XSkillReceiver *pCastingTarget,
										XSKILL::XSkillUser *pCaster,
										const XE::VEC2& vPos )
{
	TransformByObj( &m_vlActionEvent );
	XE::VEC3 vwSrc = GetvwPos() + m_vlActionEvent;
	if( vwSrc.IsZero() )
		vwSrc = GetvCenterWorld();
	XBaseUnit *pTargetUnit = dynamic_cast<XBaseUnit*>( pBaseTarget );
	auto pArrow = new XSkillShootObj( GetpWndWorld(),
									GetThisUnit(),
									pTargetUnit->GetThisUnit(),
									vwSrc,
									pTargetUnit->GetvwPos(),
									0,
									pSkillDat->GetstrShootObj().c_str(),
									pSkillDat->GetidShootObj() );
	pArrow->RegisterCallback( this, &XBaseUnit::cbOnArriveSkillObj, pSkillDat,
// 																	pEffect,
																	level,
																	pBaseTarget );
// 																	pCastingTarget );
	// 타격 sfx를 지정한다.
	pArrow->SetpDelegate( this );
	pArrow->SetstrIdentifier( _tstring(_T("skill.shoot.obj")) );
	GetpWndWorld()->AddObj( WorldObjPtr( pArrow ) );
	return this;
}

void XBaseUnit::OnDestroySFX( XBuffObj *pSkillRecvObj, ID idSFX )
{
}

int XBaseUnit::GetGroupList( XVector<XSKILL::XSkillReceiver*> *pAryOutGroupList,
							XSKILL::XSkillDat *pSkillDat,
							const XSKILL::EFFECT *pEffect,
							xtGroup typeGroup )
{
	if( typeGroup == xGT_ME )	{
		GetpSquadObj()->GetListMember( pAryOutGroupList );
	} else
	if( typeGroup == xGT_TARGET_PARTY )	{
		if( m_spTarget != nullptr )
			m_spTarget->GetpSquadObj()->GetListMember( pAryOutGroupList );
	} else
	if( typeGroup == xGT_RANDOM_PARTY_FRIENDLY )	{
		auto spLegionObj = GetspLegionObj();
		XBREAK( spLegionObj == nullptr );
		auto spSquad = spLegionObj->FindSquadRandom( GetpSquadObj(),
													xMETER_TO_PIXEL(999.f),
													false );
		spSquad->GetListMember( pAryOutGroupList );
	} else
	if( typeGroup == xGT_RANDOM_PARTY_ENEMY )	{
		auto spLegionObj = XBattleField::sGet()->GetEnemyLegionObj( m_pSquadObj );
		XBREAK( spLegionObj == nullptr );
		auto spSquad = spLegionObj->FindSquadRandom( GetpSquadObj(),
													xMETER_TO_PIXEL(999.f),
													false );
		if( spSquad )
			spSquad->GetListMember( pAryOutGroupList );
	} else
	if( typeGroup == xGT_FRIENDLY_ALL )	{
		GetspLegionObj()->GetAllUnit( pAryOutGroupList );
	}
	return pAryOutGroupList->size();
}

/**
 @brief 특성 pSkillObj의 레벨을 돌려준다.
*/
int XBaseUnit::GetSkillLevel( XSKILL::XSkillObj* pSkillObj )
{
// 	if( IsPlayer() == false )
// 		return 0;
	if( pSkillObj->IsAbility() )
	{
		XGAME::xtUnit unit = GetSquadUnit();
		XPropTech::xNodeAbil *pNode
			= XPropTech::sGet()->GetpNodeBySkill( unit, pSkillObj->GetStrIdentifier() );
		XBREAK( pNode == nullptr );
		auto pHero = GetpHero();
		XBREAK( pHero == nullptr );
		const auto abil = pHero->GetAbilNode( unit, pNode->idNode );
// 		auto pAbil = GetspLegionObj()->GetpAccount()->GetAbilNode( unit, pNode->idNode );
// 		XBREAK( pAbil == nullptr );
		XBREAK( abil.point == 0 );	// 일단은 테스트중이니 삭제.
		return abil.point;
	} else {
		auto pHero = GetpHero();
		if( XASSERT(pHero) ) {
			if( pSkillObj->IsPassive() )
				return pHero->GetlvPassive();
			else if( pSkillObj->IsActive() )
				return pHero->GetlvActive();
		}
	}

	return 0;
}

/**
 @brief pDat스킬을 사용할 적절한 타겟을 선정해서 돌려줘야한다.
*/
XSKILL::XSkillReceiver* XBaseUnit::GetSkillBaseTarget( XSkillDat *pDat )
{
	if( pDat->GetbaseTarget() == xBST_CURR_TARGET ) {
		float meter = 999.f;
		if( pDat->GetbitBaseTarget() == xfALLY ) {
			auto spUnit = GetNearUnit( meter, xSIDE_PLAYER, true );		// 가장가까운 우리편 유닛을 찾는다.
			return spUnit.get();
		} else
		if( pDat->GetbitBaseTarget() == xfHOSTILE )	{
			bool bFindOutRange = true;
			// 적에게 사용하는 타겟스킬이면서 근접유닛의 경우는 근접사거리를 적용한다.
			if( !IsRange() ) {
				bFindOutRange = false;	// 사거리 밖의 유닛은 검색하지 않는다.
				meter = xPIXEL_TO_METER( XGAME::DIST_MELEE_ATTACK );
			}
			auto spUnit = GetNearUnit( meter, xSIDE_OTHER, bFindOutRange );		// 가장가까운 우리편 유닛을 찾는다.
			return spUnit.get();
		} else {
#ifdef _DEBUG
			XBREAK(1);	// 이런경우가 있는지 확인할것.
#endif // _DEBUG
		}
	} else	{
#ifdef _DEBUG
		XBREAK(1);	// 이런경우가 있음?
#endif // _DEBUG
	}
	return this;	// 일단 이렇게 땜빵.
}
BOOL XBaseUnit::IsInvokeAddTarget( ID idAddTarget ) const
{
	return idAddTarget == m_idProp;
}
/**
 @brief 상태이상 변화에 따른 처리를 해야한다.
*/
BOOL XBaseUnit::OnApplyState( XSkillUser *pCaster,
								XSkillReceiver* pInvoker,
								const EFFECT *pEffect,
								int idxState,
								BOOL flagState )
{
	auto pBuff = FindBuffSkill(_T("invoke_phytoncide"));
	if( pBuff ) {
		// 피톤치트가 걸려있을때 독속성이 들어오면 걸리지 않는다.
		if( pEffect->invokeState == XGAME::xST_POISON )
			return FALSE;
	}
	return TRUE;
}

/**
 @brief 상태가 변할때 최초 한번 호출됨.
 xState
*/
BOOL XBaseUnit::OnFirstApplyState( XSkillUser *pCaster,
									XSkillReceiver* pInvoker,
									const EFFECT *pEffect,
									int idxState,
									BOOL flagState,
									int level )
{
//	CONSOLE("상태발동:%s", XGAME::GetStrState( (XGAME::xtState)idxState ));
	// 상태발동 텍스트
//#ifndef _XUZHU_HOME
	auto pDmg = new XObjDmgNum( 0, xHT_STATE, idxState, GetvwTop() );
	AddObj( pDmg );
//#endif
	switch( idxState )
	{
	case XGAME::xST_STUN:
	case XGAME::xST_SLEEP: {
		// 기절 이펙트
		float secDuration = pEffect->GetDuration( level );
		XBREAK( secDuration == 0 );
		CreateSfxObj( XSKILL::xPT_TARGET_TOP, _T( "sfx_star.spr" ), 1, secDuration, TRUE, 24.f );
		ChangeFSMStun( secDuration );
	} break;
	case XGAME::xST_TAUNT: {
		bool bTaunt = true;
		if( GetUnitType() == xUNIT_LYCAN )		{
			float prob = GetInvokeRatioByBuff( GetUnitType(), _T("disregard") );
			if( prob > 0 && XE::IsTakeChance(prob) )
				bTaunt = false;		// 도발저항
		}
		if( bTaunt )		{
			// this는 공격상대를 pCaster로 바꿔야 한다.
			auto pAttacker = dynamic_cast<XBaseUnit*>( pCaster );
			if( pAttacker ) {
				// 공격자 부대
				auto spSquadAtker = pAttacker->GetspSquadObj();
				// this의 부대에게 공격자부대를 공격할것을 명령.
				GetpSquadObj()->DoAttackSquad( spSquadAtker );
//				auto spTarget = pAttacker->GetThisUnit();
//				SetTarget( spTarget );
			}
		}
	} break;
	case XGAME::xST_FEAR: {
		XE::VEC3 vwPos = GetpSquadObj()->GetvwPos();
		vwPos.x += xMETER_TO_PIXEL(30.f) * -m_Dir;
		// 공포에 걸리면 자기부대를 강제로 뒤쪽으로 이동하게 한다.
		GetpSquadObj()->DoMoveSquad( vwPos );
	} break;
	default:
		break;
	}
	return TRUE;
}

/**
 @brief 버프효과가 끝나면 호출됨
*/
int	XBaseUnit::OnClearSkill( XBuffObj* pBuffObj, XSkillDat *pSkillDat, EFFECT_OBJ *pEffObj )
{
	auto pEffect = &pEffObj->m_effect;
	if( pEffect->invokeState )	{
		switch( pEffect->invokeState )		{
		case xST_FEAR:
			GetpSquadObj()->DoAttackAutoTargetEnemy();
			break;
		default:
			break;
		}
	}
	// 버프에 딸린 sfx가 있었으면 삭제시킴.
	if( pBuffObj->GetidSfx() ) {
		XEObjMngWithType::sGet()->DestroyObjWithType( xOT_SFX, pBuffObj->GetidSfx() );
	}
	return 1;
}

/**
 @brief
*/
XSKILL::XSkillReceiver* XBaseUnit::GetTargetObject( XSKILL::EFFECT *pEffect,
													XSKILL::xtTargetCond cond )
{
	XBaseUnit *pFind = NULL;;
	if( cond == XSKILL::xTC_NEAREST )	{
		if( pEffect->castfiltFriendship & XSKILL::xfHOSTILE )		{
			WorldObjPtr spObj;
			spObj = XEObjMngWithType::sGet()->FindNearUnitByFunc( this,
																GetvwPos(),
																0,
																true,
																FilterEnemy );
			if( spObj != nullptr )
				pFind = static_cast<XBaseUnit*>( spObj.get() );
		} else
		if( pEffect->castfiltFriendship & XSKILL::xfALLY )		{
			// 아군중에서 검색
		} else		{
			XBREAK(1);		// 아직 이런조건은 쓰지 않음.
		}
	} else	{
		if( GetspTarget() != nullptr )
			pFind = GetspTarget().get();
	}
	if( pFind )
		return pFind;
	return NULL;
}

/**
 @brief hp가 더 적은쪽을 찾는다.
*/
BOOL CompLeastHp( XBaseUnit *pSrc, XBaseUnit *pUnit1, XBaseUnit *pUnit2 )
{
	if( pUnit1->GetHp() < pUnit2->GetHp() )
		return TRUE;
	return FALSE;
}

/**
 @brief 조건에 맞는 기준타겟을 찾아서 돌려줘야 한다.
*/
XSkillReceiver* XBaseUnit::GetBaseTargetByCondition( XSkillDat *pSkillDat,
												xtBaseTargetCond cond,
												float meter,
												int level,
												XSkillReceiver *pCurrTarget,
												const XE::VEC2& vPos )
{
	XSPLegionObj spLegionObj;
	if( pSkillDat->GetbitBaseTarget() == xfALLY )
		spLegionObj = GetspLegionObj();
	else
		spLegionObj = XBattleField::sGet()->GetEnemyLegionObj( m_pSquadObj );
	// 가장 hp가 적은 파티
	if( cond == xBTC_LEAST_HP_PARTY || cond == xBTC_HIGHEST_HP_PARTY ) {
		BOOL bHighest = FALSE;
		if( cond == xBTC_HIGHEST_HP_PARTY )
			bHighest = TRUE;
		auto spSquad = spLegionObj->FindNearSquadLeastHp(
															GetpSquadObj(),
															xMETER_TO_PIXEL(meter),
															XGAME::xSIDE_OTHER,
															bHighest );
		if( spSquad != nullptr ) {
			XBaseUnit *pUnit = spSquad->GetLiveMember();
			return pUnit;
		}
	} else
	// 기준타겟리더
	if( cond == xBTC_CURR_TARGET_LEADER ) {
		auto pUnitTarget = static_cast<XBaseUnit*>( pCurrTarget );
		if( pUnitTarget ) {
			auto spLeader = pUnitTarget->GetspLeader();
			if( spLeader && spLeader->IsLive() ) {
				return spLeader.get();
			} else {
				// 리더가 없거나 죽었으면 기준타겟을 그대로 돌려준다.
				return pCurrTarget;
			}
		}
	} else
	if( cond == xBTC_RANDOM ) {
		auto spSquad = spLegionObj->FindSquadRandom( GetpSquadObj(),
														xMETER_TO_PIXEL(999.f),
														true );
		if( spSquad != nullptr ) {
			return spSquad->GetLiveMember();
		}
	}
	return nullptr;
}
/**
 @brief 효과발동전 발동대상의 조건
*/
BOOL XBaseUnit::IsInvokeTargetCondition( XSKILL::XSkillDat *pSkillDat,
										const XSKILL::EFFECT *pEffect,
										XSKILL::xtCondition condition,
										DWORD condVal )
{
	if( pSkillDat->GetstrIdentifier() == _T("surprise_attack") )
	{
		if( m_spTarget == nullptr )
			return FALSE;
		if( m_spTarget->GetspTarget() == nullptr )
			return FALSE;
		// 내가 잡고있는 타겟의 공격타겟부대가 우리 부대면 발동안함.
		XBREAK( m_spTarget->GetspTarget()->GetpSquadObj() == nullptr );
		if( m_spTarget->GetspTarget()->GetpSquadObj()->GetsnSquadObj() == m_pSquadObj->GetsnSquadObj() )
			return FALSE;
	} else
	if( condition == xATTACK_TARGET_JOB )
	{
// 		if( m_spTarget == nullptr )
// 			return FALSE;
// 		if( m_spTarget != nullptr &&
// 			m_spTarget->GetSquadUnit() != pEffect->dwParam[0] )
// 			return FALSE;
		if( GetSquadUnit() != condVal )
			return FALSE;
	} else
	if( condition == xATTACK_TARGET_SIZE )
	{
		if( GetUnitSize() != condVal )
			return FALSE;
	} else
	if( condition == xATTACK_TARGET_TRIBE )
	{
		if( GetTribe() != condVal )
			return FALSE;
	} else
	if( condition == xTARGET_ARM )
	{
		if( GetTypeAtk() != condVal )
			return FALSE;
	} else
	if( condition == xTARGET_RANK )
	{
		if( (condVal == XGAME::xUR_SOLDIER && IsHero()) ||
			(condVal == XGAME::xUR_HERO && !IsHero() ))
				return FALSE;
	}
	return XSkillReceiver::IsInvokeTargetCondition( pSkillDat, pEffect, condition, condVal );
}

/**
 @brief 현재 공격대상을 요청받았다. 없으면 바로 찾아줘야 한다.
*/
XSkillReceiver* XBaseUnit::GetCurrTarget()
{
	if( m_spTarget == nullptr ) {
		auto spTarget = XBattleField::sGet()->FindNearSquadEnemy( m_pSquadObj );
		if( spTarget != nullptr )		{
			auto spUnitTarget = spTarget->FindAttackTarget( TRUE );
			SetTarget( spUnitTarget );
		}
	}

	return m_spTarget.get();
}

// SKILL
//////////////////////////////////////////////////////////////////////////

/**
 @brief 현재 크리티컬율을 바탕으로 랜덤돌려서 크리티컬인지 리턴받는다.
*/
bool XBaseUnit::IsCritical( UnitPtr spTarget )
{
	float ratioCritical = GetCriticalRatio();
	if( spTarget != nullptr )	{
		ratioCritical = spTarget->CalcAdjParam( ratioCritical, XGAME::xADJ_CRITICAL_RATE_RECV );
		auto pBuff = FindBuffSkill( _T( "nearing_fire" ) );
		if( pBuff && m_typeCurrMeleeType == XGAME::xMT_RANGE ) {
			XE::VEC2 vDist = spTarget->GetvwPos().ToVec2() - GetvwPos().ToVec2();
			float distSkill = xMETER_TO_PIXEL((float)pBuff->GetInvokeSizeByLevel());
			if( vDist.Lengthsq() < distSkill * distSkill )
				ratioCritical += pBuff->GetInvokeRatioByLevel();
		}
		if( GetUnitType() == xUNIT_FALLEN_ANGEL ) {
			// 계략 특성
			pBuff = FindBuffSkill( _T( "stratagem" ) );
			if( pBuff ) {
				// spTarget의 부대를 치고 있는 또다른 타천사 부대가 있는지 찾는다.
				auto spSquadWith 
					= GetspLegionObj()->FindSquadTakeTargetAndClosed( GetpSquadObj()
																													, spTarget->GetpSquadObj()
																													, GetUnitType() );
				if( spSquadWith != nullptr )
					ratioCritical += pBuff->GetAbilMinbyLevel();
			}
		}
	}
	int prob = (int)(ratioCritical * 1000);
	int dice = random( 1000 );
	if( dice < prob )
		return TRUE;
	return FALSE;
}

/**
 @brief 현재 회피율을 바탕으로 랜덤돌려서 회피했는지 리턴받는다.
*/
bool XBaseUnit::IsEvade( XSKILL::xtDamage typeDamage, const XBaseUnit* pAttacker ) const
{
	float ratio = GetEvadeRatio( typeDamage, pAttacker );
	int prob = (int)( ratio * 1000 );
	int dice = random( 1000 );
	if( dice < prob )
		return true;
	return false;
}

float XBaseUnit::GetEvadeRatio( XSKILL::xtDamage typeDamage, const XBaseUnit* pAttacker ) const
{
	float ratio = 0.f;
	if( typeDamage == XSKILL::xDMG_MELEE )
		ratio = CalcAdjParam( ratio, XGAME::xADJ_EVADE_RATE_MELEE );
	else if( typeDamage == XSKILL::xDMG_RANGE )
		ratio = CalcAdjParam( ratio, XGAME::xADJ_EVADE_RATE_RANGE );
	if( pAttacker )
	{
		switch( pAttacker->GetUnitSize() )
		{
		case xSIZE_SMALL:
			ratio += CalcAdjParam( ratio, xADJ_EVADE_RATE_SMALL );
			break;
		case xSIZE_MIDDLE:
			ratio += CalcAdjParam( ratio, xADJ_EVADE_RATE_MIDDLE );
			break;
		case xSIZE_BIG:
			ratio += CalcAdjParam( ratio, xADJ_EVADE_RATE_BIG );
			break;
		}
	}
	// 원래 ratio를 이렇게 넣으면 안되지만 회피율은 모두 xVAL타입이라 가정하고 사용함.
	ratio = CalcAdjParam( ratio, XGAME::xADJ_EVADE_RATE );
	return ratio;
}

/**
 @brief 적중검사. 디폴트는 100%임
*/
bool XBaseUnit::IsHit( UnitPtr spTarget )
{
	auto pAdj = GetAdjParam( xADJ_HIT_RATE );
	float ratio = CalcAdjParam( 1.f, xADJ_HIT_RATE );
	if( ratio < 1.f )	{
		if( XE::IsTakeChance( ratio ) )
			return true;
		else
			return false;
	}
	return true;
}
/**
 @brief 타겟에게 데미지를 입혔을때 하드코딩용 코드
*/
float XBaseUnit::hardcode_OnToDamage( UnitPtr& spTarget, float damage, XGAME::xtMelee typeMelee )
{
	float ratioAdd = 0.f;
	// 종족 추가데미지
	{
		float tempRatio = 0;
		const XSKILL::ADJ_PARAM *pAdjParam = GetAdjParam( XGAME::xADJ_ADD_DAMAGE_TRIBE );
		tempRatio = CalcAdjParam( tempRatio, XGAME::xADJ_ADD_DAMAGE_TRIBE );
		if( tempRatio > 0.f &&
			(pAdjParam->dwParam[ 0 ] == spTarget->GetTribe() ||
			 pAdjParam->dwParam[ 0 ] == XGAME::xTB_NONE) ) {
			ratioAdd += tempRatio;
			ratioAdd /= 100.f;
		}
	}
	// 직업 추가데미지
	{
		float tempRatio = 0;
		const XSKILL::ADJ_PARAM *pAdjParam = GetAdjParam( XGAME::xADJ_ADD_DAMAGE_CLASS );
		tempRatio = CalcAdjParam( tempRatio, XGAME::xADJ_ADD_DAMAGE_CLASS );
		if( tempRatio > 0.f &&
			(pAdjParam->dwParam[ 0 ] == spTarget->GetSquadUnit() ||
			pAdjParam->dwParam[ 0 ] == XGAME::xTB_NONE) ) {
			ratioAdd += pAdjParam->GetValPercent();
			ratioAdd /= 100.f;
		}
	}
	if( spTarget != nullptr ) {
		if( spTarget->IsState( XGAME::xST_FROZEN ) ) {
			// 타겟이 빙결상태고 공격자가 파쇄화살 특성이 있으면
			XBuffObj *pBuff = FindBuffSkill( _T("crushing_arrow") );
			if( pBuff ) {
				auto pEffect = pBuff->GetEffectIndex(0);
				if( pEffect )
					ratioAdd += pEffect->invokeAbilityMin[pBuff->GetLevel()];
			}
		}
	}
	// 백병전 하드코딩
	if( IsRange() && m_spTarget && typeMelee == XGAME::xMT_MELEE ) {
		auto pBuff = FindBuffSkill( _T("close_combat") );
		if( pBuff ) {
			auto pEffect = pBuff->GetEffectIndex( 0 );
			if( pEffect ) {
				damage = GetAttackRangeDamage( m_spTarget )
					* pEffect->invokeAbilityMin[ pBuff->GetLevel() ];
			}
		}
	}
	damage += damage * ratioAdd;
	return damage;
}

/**
	@brief this부대의 유닛.  GetType()을 쓰지말고 이걸 쓸것.
	영웅이라도 부대유닛을 돌려준다.
*/
XGAME::xtUnit XBaseUnit::GetSquadUnit() const
{
	return m_pSquadObj->GetUnit();
}

float XBaseUnit::GetAdjDamage( float damage, BOOL bCritical, XSKILL::xtDamage typeDamage, XGAME::xtDamageAttr attrDamage )
{
	if( bCritical == FALSE ) {
		switch( typeDamage ) {
		case XSKILL::xDMG_MELEE:
			damage = CalcAdjParam( damage, XGAME::xADJ_MELEE_DAMAGE_RECV );
			break;
		case XSKILL::xDMG_RANGE:
			damage = CalcAdjParam( damage, XGAME::xADJ_RANGE_DAMAGE_RECV );
			break;
		case XSKILL::xDMG_MAGIC:
			damage = CalcAdjParam( damage, XGAME::xADJ_MAG_DAMAGE_RECV );
			break;
		case XSKILL::xDMG_NONE:
		default:
			break;
		}
	} else {
		// 치명타
		switch( typeDamage ) {
		case XSKILL::xDMG_MELEE:
			damage = CalcAdjParam( damage, XGAME::xADJ_MELEE_CRITICAL_RECV );
			break;
		case XSKILL::xDMG_RANGE:
			damage = CalcAdjParam( damage, XGAME::xADJ_RANGE_CRITICAL_RECV );
			break;
		case XSKILL::xDMG_NONE:
		default:
			break;
		}
	}
	switch( attrDamage ) {
	case XGAME::xDA_FIRE:
		damage = CalcAdjParam( damage, XGAME::xADJ_DAMAGE_FIRE );
		break;
	}
	return CalcAdjParam( damage, XGAME::xADJ_DAMAGE_RECV );
}

void XBaseUnit::OnStartBattle()
{
	// 전투시작 이벤트
	XSkillReceiver::OnEventJunctureCommon( XSKILL::xJC_START_BATTLE );
}

void XBaseUnit::OnAfterStartBattle()
{
}

/**
 @brief 발동타겟에게 효과가 걸리기 직전에 발생한다.
*/
bool XBaseUnit::OnEventApplyInvokeEffect( XSKILL::XSkillUser* pCaster,
										XSKILL::XBuffObj *pBuffObj,
										XSKILL::XSkillDat *pSkillDat,
										const XSKILL::EFFECT *pEffect,
										int level )
{
	if( pSkillDat->GetstrIdentifier() == _T("morale") ||
		pSkillDat->GetstrIdentifier() == _T("divine_protection") ||
		pSkillDat->GetstrIdentifier() == _T("blessing") )	{
		XBaseUnit *pCasterUnit = dynamic_cast<XBaseUnit*>( pCaster );
		if( !pCasterUnit->IsHero() )
			return false;
	} else
	if( pSkillDat->GetstrIdentifier() == _T("protect") ||
		pSkillDat->GetstrIdentifier() == _T("view_blocked") ||
		pSkillDat->GetstrIdentifier() == _T("stigma"))	{
		// 하드코딩
		// 이들 특성의 경우 한부대만 걸려야 하는데 부대엔 영웅까지 2명이 있어서
		// 두부대가 걸리게 된다. 그래서 영웅의 발동은 제외시킴.
		XBaseUnit *pCasterUnit = dynamic_cast<XBaseUnit*>( pCaster );
		if( pCasterUnit->IsHero() )
			return false;
	} else
	if( pSkillDat->GetstrIdentifier() == _T("invoke_protect"))	{
		float val = pEffect->GetAbilityMin( level );
		m_cntShell = (int)val;
		XBREAK( m_cntShell == 0 );
	} else
	if( pSkillDat->GetstrIdentifier() == _T("destruct"))	{
		this->DoDie( nullptr );
	} else
	if( pSkillDat->GetstrIdentifier() == _T("phytoncide"))	{
		XBaseUnit *pCasterUnit = dynamic_cast<XBaseUnit*>( pCaster );
		// 피톤치트의 경우는 같은 부대원(예를들어 영웅)에게는 발동되지 않는다.
		if( pCasterUnit->GetpSquadObj()->GetsnSquadObj() == GetpSquadObj()->GetsnSquadObj() )
			return false;
	}// else
// 	if( pSkillDat->GetstrIdentifier() == _T("photosynthesis") )	{
// 		static int si = 0;
// 		if( GetUnitType() == xUNIT_TREANT )		{
// 			if( m_cntPerSec < 50 )
// 				++m_cntPerSec;
// 			XBREAK( pBuffObj == nullptr );
// 			float scale = 1.f + (m_cntPerSec * 0.01f);
// 			const float scaleMax = 1.5f;
// 			if( scale > scaleMax )
// 				scale = scaleMax;
// 			SetScaleObj( GetScaleUnitOrg() * scale );
// 		}
// 	}

	return true;
}

/**
 @brief 지속형 효과의 경우 최초 발동될때 한번 호출된다.
*/
void XBaseUnit::OnEventFirstApplyEffect( XSKILL::XSkillDat *pDat,
										XSKILL::XSkillUser* pCaster,
										XSKILL::EFFECT *pEffect,
										int level )
{
}

/**
 @brief 공격모션이 정해지고 난 직후 호출된다.
 공격타입에 따라 근접이나 원거리시점이 파라메터로 넘어온다.
*/
void XBaseUnit::OnAfterAttackMotion( XSKILL::xtJuncture junc )
{
	XSkillReceiver::OnEventBeforeAttack( junc );
}

/**
 @brief XSkillReceiver::OnEventBeforeAttack의 결과로 날아온다.
*/
void XBaseUnit::DelegateResultEventBeforeAttack( XSKILL::XBuffObj *pBuffObj, XSKILL::EFFECT *pEffect )
{
	xInvokeEffect eff;
	eff.pDat = pBuffObj->GetpDatMutable();
	eff.pEffect = pEffect;
	// 특성발동 외치기
//	if( IsPlayer() && IsHero() )
	if( IsHero() ) {
		auto v = GetvwPos();
		v.z -= 80.f;
		auto pNode = XPropTech::sGet()->GetpNodeBySkill( GetUnitType(), pBuffObj->GetpDat()->GetstrIdentifier() );
		_tstring str = XFORMAT( "%s!", XTEXT( pNode->idName ) );
		XCOLOR col = ( IsPlayer() ) ? XCOLOR_WHITE : XCOLOR_RED;
		auto pObjText = new XObjYellSkill( str.c_str(), GetThisUnit(), v, col );
		AddObj( pObjText );
	}

	m_aryInvokeSkillByAttack.Add( eff );
}

// /**
//  @brief 이대상에게 버프스킬이 추가된 직후 호출된다.
// */
// void XBaseUnit::OnAddSkillRecvObj( XBuffObj *pSkillRecvObj, EFFECT *pEffect )
// {
// 	pSkillRecvObj->setde
// }
//
//
//
/**
 @brief SkillUser에서 어떤 효과값을 적용하기전에 스킬에 따라 그 값을 증폭시킴.
*/
void XBaseUnit::OnSkillAmplifyUser( XSKILL::XSkillDat *pDat,
									XSkillReceiver *pIvkTarget,
									const EFFECT *pEffect,
									XSKILL::xtEffectAttr attrParam,
									float *pOutRatio, float *pOutAdd )
{
	if( pDat->GetstrIdentifier() == _T("chill_explosion") ||
		pDat->GetstrIdentifier() == _T("nvoke_freeze_arrow"))	{
		if( attrParam == XSKILL::xEA_CAST_RADIUS )		{
			XBuffObj *pBuff = FindBuffSkill( _T( "chill_spread" ) );
			if( pBuff )			{
				int level = pBuff->GetLevel();
				auto pEffect = pBuff->GetEffectIndex( 0 );
				if( pOutAdd && pEffect )				{
					float abilityAdd = pEffect->invokeAbilityMin[ level ];
					// 원래능력치가 음수였으면 증폭능력치도 음수로 바꾼다.
// 					if( *pOutAdd < 0 && abilityAdd > 0 )
// 						abilityAdd = -abilityAdd;
// 					if( *pOutAdd > 0 && abilityAdd < 0 )
// 						abilityAdd = -abilityAdd;
					*pOutAdd = abilityAdd;
				}
			}
		} else
		if( attrParam == XSKILL::xEA_ABILITY )		{
			XBuffObj *pBuff = FindBuffSkill( _T( "chill_up" ) );
			if( pBuff )			{
				int level = pBuff->GetLevel();
				auto pEffect = pBuff->GetEffectIndex( 0 );
				if( pOutAdd && pEffect )				{
					float abilityAdd = pEffect->invokeAbilityMin[ level ];
					// 원래능력치가 음수였으면 증폭능력치도 음수로 바꾼다.
// 					if( *pOutAdd < 0 && abilityAdd > 0 )
// 						abilityAdd = -abilityAdd;
// 					if( *pOutAdd > 0 && abilityAdd < 0 )
// 						abilityAdd = -abilityAdd;
					*pOutAdd = abilityAdd;
				}
			}
		}
	}
}

/**
 @brief 발동효과의 능력치값을 공격력의 x배 와같은식으로 한번더 가공해서 쓴다.
*/
void XBaseUnit::OnAdjustEffectAbility( XSkillDat *pSkillDat,
																			const EFFECT *pEffect,
																			int invokeParam,
																			float *pOutMin )
{
	if( invokeParam == XGAME::xHP) {
		// 데미지 스킬류의 경우 공격력 * Abilitymin형태로 데미지를 만든다.
		if( ( *pOutMin ) < 0 ) {
			// 데미지를 공격력의 * n% 형태로 만든다.
			if( IsRange() )
				*pOutMin = GetAttackRangeDamage( UnitPtr() ) * ( *pOutMin );
			else
				*pOutMin = GetAttackMeleeDamage( UnitPtr() ) * ( *pOutMin );
		}
		else
			*pOutMin = GetMaxHp() * ( *pOutMin );
	} else
	if( invokeParam == XGAME::xADJ_ATTACK ) {
		// 공격력보정시에 this가 "열손실" 특성이 있으면.
		if( pSkillDat->GetstrIdentifier() == _T( "heat_loss" ) ) {
			if( m_spTarget != nullptr ) {
				XE::VEC2 vDist = m_spTarget->GetvwPos().ToVec2() - GetvwPos().ToVec2();
				float radiusPixel = GetAttackRadiusByPixel();	// 사거리
				float rate = vDist.Length() / (radiusPixel * 0.5f);
				if( rate > 1.f )
					rate = 1.f;
				if( rate < 0.f )
					rate = 0.f;
				rate = 1.f - rate;
				float abil = *pOutMin;
				abil *= rate;
				*pOutMin = abil;
			}
		}
	}
}

/**
 @brief 치명타배수
*/
float XBaseUnit::GetCriticalPower()
{
	float power = 2.f;		// 기본 크리티컬 데미지는 두배
	power = CalcAdjParam( power, XGAME::xADJ_CRITICAL_POWER );
	XBREAK( power == 0 );
	return power;
}
/**
 @brief 크리티컬 확률을 돌려준다.
*/
float XBaseUnit::GetCriticalRatio()
{
	float ratioCri = 0.f;
	ratioCri = CalcAdjParam( ratioCri, XGAME::xADJ_CRITICAL_RATE );
	XBuffObj *pBuff = FindBuffSkill( _T("deadly_firstattack") );
	if( pBuff ) {
		if( m_cntAttack == 0 )		// 치명적선공 특성이 있을때 첫타는 무조건 치명타.
			ratioCri = 1.f;
	}
	return ratioCri;
}

int XBaseUnit::GetHp()
{
	float hp = (float)m_HP;
	return (int)CalcAdjParam( hp, XGAME::xADJ_MAX_HP );
}

/*
일단 추가공속을 추가공속보정 같은 항목으로 AddAdjParam시켜둔다.
실제 공속값 구할때 타겟에 따라 이 추가값을 기존 공속보정값에 더한다.
*/

/**
 @brief 타겟에따라 보정비율이 달라져야 하는류의 효과에 사용
*/
float XBaseUnit::GetAddRatioDamage( UnitPtr spTarget )
{
	// hardcode_OnToDamage도 이쪽으로 옮겨올것.
	float addRatio = 0.f;
	if( spTarget != nullptr ) {
		switch( spTarget->GetTypeAtk() ) {
		case XGAME::xAT_TANKER: {
			addRatio += GetAdjParam( XGAME::xADJ_DAMAGE_TANKER )->valPercent;
		} break;
		case XGAME::xAT_RANGE: {
			addRatio += GetAdjParam( XGAME::xADJ_DAMAGE_RANGE )->valPercent;
		} break;
		case XGAME::xAT_SPEED: {
			addRatio += GetAdjParam( XGAME::xADJ_DAMAGE_SPEED )->valPercent;
		} break;
		}
		switch( spTarget->GetUnitSize() ) {
		case XGAME::xSIZE_SMALL: {
			addRatio += GetAdjParam( XGAME::xADJ_DAMAGE_SMALL )->valPercent;
		} break;
		case XGAME::xSIZE_MIDDLE: {
			addRatio += GetAdjParam( XGAME::xADJ_DAMAGE_MIDDLE )->valPercent;
		} break;
		case XGAME::xSIZE_BIG: {
			addRatio += GetAdjParam( XGAME::xADJ_DAMAGE_BIG )->valPercent;
		} break;
		}
		// 유닛대상 피해보정
		int idxUnit = spTarget->GetUnitType() - 1;
		addRatio += GetAdjParam( XGAME::xADJ_DAMAGE_SPEARMAN + idxUnit )->valPercent;
		// 상대가 기절한 상태라면 추가피해
		if( spTarget->IsState( XGAME::xST_STUN ) ) {
			float adj;
			if( GetAdjParamByBuff( &adj, _T("giant_axe"), XGAME::xADJ_ATTACK ) )
				addRatio += adj;
		} else
		if( spTarget->IsState( xST_BLEEDING ) ) {
			if( GetUnitType() == xUNIT_LYCAN ) {
				// 야비한공격 특성
				// 출혈에 걸린적에게 공격력이 더 올라간다.
				auto pBuff = FindBuffSkill( _T( "scurrilous_attack" ) );
				if( pBuff )
				{
					XBREAK( pBuff->GetEffectIndex(0)->invokeParameter != xADJ_ATTACK );
					addRatio += pBuff->GetAbilMinbyLevel();
				}
			}
		}
	}
	return addRatio;
}

/**
 @brief sid버프로부터 adjParam보정치의 변동이 있으면 리턴해준다.
*/
XBuffObj* XBaseUnit::GetAdjParamByBuff( float *pOutAdj, LPCTSTR sid, XGAME::xtParameter adjParam )
{
	float adjVal = 0.f;
	auto pBuff = FindBuffSkill( sid );
	if( pBuff )
	{
		for( int i = 0; i < pBuff->GetNumEffect(); ++i )
		{
			auto pEffect = pBuff->GetEffectIndex( i );
			if( pEffect && pEffect->invokeParameter == adjParam )
			{
				// 특성 발동효과가 2개인것이 있어 뺌.
//				XBREAK( pEffect->invokeParameter != adjParam );
				if( pOutAdj )
					*pOutAdj = pBuff->GetAbilMinbyLevel(i);
				return pBuff;
			}
		}
	}
	return nullptr;
}

/**
 @brief sid특성으로인한 발동확률이 있으면 리턴한다.
*/
float XBaseUnit::GetInvokeRatioByBuff( xtUnit unit, LPCTSTR sid )
{
	float ratioInvoke = 0.f;
	auto pBuff = FindBuffSkill( sid );
	if( pBuff )
		return pBuff->GetInvokeRatioByLevel();
	return 0;
}
//////////////////////////////////////////////////////////////////////////
// xStat
/**
 @brief 근접공격력을 얻는다. 실제 데미지와는 다르다.
*/
float XBaseUnit::GetAttackMeleePower()
{
	// 유닛능력치와 영웅능력치를 곱한다.
//	const auto pHero = GetpHero();
	auto pHero = m_pHero;
	const float mulByLvSquad = GetmultipleAbility();
	const float mulByHero = pHero->GetAttackMeleeRatio();
	float power = (m_pPropUnit->atkMelee * mulByLvSquad) * mulByHero;
// 	ADD_LOG( m_strLog, "atkMelee:%.1f, 병과보정율:+%.1f, 영웅보정율:+%.1f => 공격력:%.1f, "
// 			, m_pPropUnit->atkMelee, mulByLvSquad, mulByHero, power );
	if( IsHero() ) {
		switch( m_pPropUnit->size ) {
		case XGAME::xSIZE_SMALL:	power *= 2.5f; break;
		case XGAME::xSIZE_MIDDLE:	power *= 0.5f;	break;
		case XGAME::xSIZE_BIG:
			power = power / GetmultipleAbility();
			power *= 0.125f;
			break; // 대형은 1이하값으로 보정되므로 역수를 나눠서 보정되기전 오리지날값(부대16렙값)으로 사용한다.
		default:
			XBREAK( 1 );
			break;
		}
	}
	float addRatio = 0.f;
	// hardcoding
	if( GetUnitType() == XGAME::xUNIT_PALADIN ) {
		auto pBuff = FindBuffSkill(_T("wind_lance"));
		if( pBuff ) {
			int level = pBuff->GetLevel();
			// 이동속도 보정치를 공격력 보정치로 쓴다.
			addRatio += GetAdjParam( XGAME::xADJ_MOVE_SPEED )->valPercent * level;
		}
	} 
// 	else
// 	if( GetUnitType() == XGAME::xUNIT_TREANT ) {
// 		addRatio += (GetcntPerSec() * 0.01f);
// 	}
	addRatio += GetAdjParam( XGAME::xADJ_ATTACK_MELEE_TYPE )->GetValPercent();
	return CalcAdjParam( power, XGAME::xADJ_ATTACK, addRatio );
}
/**
 @brief 실제 타겟에게 가해지는 데미지를 얻는다
*/
float XBaseUnit::GetAttackMeleeDamage( UnitPtr spTarget )
{
	float damage = GetAttackMeleePower();
	float addRatio = GetAddRatioDamage( spTarget );
	// 추가로 더해질 피해율을 누적시켜둔다.
	const auto oldDmg = damage;
	damage = CalcAdjParam( damage, XGAME::xADJ_DAMAGE, addRatio );
	ADD_LOG( m_strLog, "기본피해량:%.1f, 추가피해:+%.1f(%.1f%%)=%.1f, ", oldDmg, damage-oldDmg, addRatio * 100.f, damage );

	auto gradeLegion = GetspLegionObj()->GetspLegion()->GetgradeLegion();
	// 기본hp에 추가값이 있으면 곱한다. 엘리트나 레이드군단이라던가 보석광산으로 인해 곱해지는값이 변할수 있다.
	float rateAtk = GetspLegionObj()->GetspLegion()->GetRateAtk();
	if( gradeLegion != XGAME::xGL_NORMAL && rateAtk == 1.f ) {
		// 정예나 레이드인데 배수가 지정되지 않으면 디폴트 배수를 사용함.
		if( gradeLegion == XGAME::xGL_ELITE )
			rateAtk = RATE_ATK_DEFAULT_ELITE;
		else if( gradeLegion == XGAME::xGL_RAID )
			rateAtk = RATE_ATK_DEFAULT_RAID;
	}
	const float rateAtkSq = GetpSquadObj()->GetpSquadron()->GetmulAtk();
	XBREAK( rateAtkSq == 0 );
	damage *= rateAtk * rateAtkSq;

	return damage;
}
/**
 @brief 원거리공격력을 얻는다. 데미지와는 다르다.
*/
float XBaseUnit::GetAttackRangePower()
{
	auto pHero = GetpHero();
	const float mulByLvSquad = GetmultipleAbility();
	const float mulByHero = pHero->GetAttackRangeRatio();
	float power = ( m_pPropUnit->atkRange * mulByLvSquad ) * mulByHero;
// 	float power = (m_pPropUnit->atkRange * GetmultipleAbility()) * m_pHero->GetAttackRangeRatio();
	if( IsHero() ) {
		switch( m_pPropUnit->size ) {
		case XGAME::xSIZE_SMALL:	power *= 2.5f; break;
		case XGAME::xSIZE_MIDDLE:	power *= 0.5f;	break;
		case XGAME::xSIZE_BIG:
			power = power / GetmultipleAbility();
			power *= 0.125f;
			break; // 대형은 1이하값으로 보정되므로 역수를 나눠서 보정되기전 오리지날값(부대16렙값)으로 사용한다.
		default:
			XBREAK( 1 );
			break;
		}
	}
	float addRatio = 0.f;
	addRatio += GetAdjParam( XGAME::xADJ_ATTACK_RANGE_TYPE )->GetValPercent();
	return CalcAdjParam( power, XGAME::xADJ_ATTACK, addRatio );
}
/**
 @brief 실제 가해지는 원거리 데미지를 얻는다.
*/
float XBaseUnit::GetAttackRangeDamage( UnitPtr spTarget )
{
	float damage = GetAttackRangePower();
	float addRatio = GetAddRatioDamage( spTarget );
	const auto oldDmg = damage;
	damage = CalcAdjParam( damage, XGAME::xADJ_DAMAGE, addRatio );
	ADD_LOG( m_strLog, "기본피해량:%.1f, 추가피해:+%.1f(%.1f%%)=%.1f, ", oldDmg, damage - oldDmg, addRatio * 100.f, damage );
	auto gradeLegion = GetspLegionObj()->GetspLegion()->GetgradeLegion();
	// 기본hp에 추가값이 있으면 곱한다. 엘리트나 레이드군단이라던가 보석광산으로 인해 곱해지는값이 변할수 있다.
	float rateAtk = GetspLegionObj()->GetspLegion()->GetRateAtk();
	if( gradeLegion != XGAME::xGL_NORMAL && rateAtk == 1.f ) {
		// 정예나 레이드인데 배수가 지정되지 않으면 디폴트 배수를 사용함.
		if( gradeLegion == XGAME::xGL_ELITE )
			rateAtk = RATE_ATK_DEFAULT_ELITE;
		else if( gradeLegion == XGAME::xGL_RAID )
			rateAtk = RATE_ATK_DEFAULT_RAID;
	}
	auto pSq = GetpSquadObj()->GetpSquadron();
	const float rateAtkSq = pSq->GetmulAtk();
	XBREAK( rateAtkSq == 0 );
	damage *= rateAtk * rateAtkSq;
	return damage;
}

/**
 @brief spTarget에 대한 원거리공격력 증가율을 구한다.
 디버깅모드의 출력용으로만 사용하는거라 부정확할수 있음.
 공격력만 더함. 추가피해보정은 더하지 않았음. 
 장기적으로 공격력과 피해보정을 하나로 합쳐야 할듯하다.
 추가피해보정은 말하자면 공격력에 복리로 곱하는개념.
*/
float XBaseUnit::GetAddRateByStat( XGAME::xtStat statType, XSPUnit spTarget )
{
	switch( statType ) {
	case XGAME::xSTAT_NONE:
		break;
	case XGAME::xSTAT_ATK_MELEE: {
		float rateAdd = GetAdjValue( xADJ_ATTACK );
		rateAdd += GetAdjValue( xADJ_ATTACK_MELEE_TYPE );
		return rateAdd;
	} break;
	case XGAME::xSTAT_ATK_RANGE: {
		float rateAdd = GetAdjValue( xADJ_ATTACK );
		rateAdd += GetAdjValue( xADJ_ATTACK_RANGE_TYPE );
		return rateAdd;
	} break;
	case XGAME::xSTAT_DEF: {
		return GetAdjValue( xADJ_DEFENSE );
	} break;
	case XGAME::xSTAT_HP: {
		return GetAdjValue( xADJ_MAX_HP );
	} break;
	case XGAME::xSTAT_SPEED_ATK: {
		float speedAdd = GetAdjValue( xADJ_ATTACK_SPEED );
		speedAdd += GetAdjValue( xADJ_ATTACK_SPEED_SMALL );
		speedAdd += GetAdjValue( xADJ_ATTACK_SPEED_MIDDLE );
		speedAdd += GetAdjValue( xADJ_ATTACK_SPEED_BIG );
		return speedAdd;
	} break;
	case XGAME::xSTAT_SPEED_MOV: {
		float speedAdd = GetAdjValue( xADJ_MOVE_SPEED );
		return speedAdd;
	} break;
	default:
		XBREAK(1);
		break;
	}
	return 0;
}

/**
 @brief 초당 이동속도(픽셀)
*/
float XBaseUnit::GetSpeedMovePerSec()
{
	const auto pHero = GetpHero();
	float speed = m_pPropUnit->movSpeedPerSec * pHero->GetMoveSpeed();
	float addRatio = 0.f;
	if( GetpSquadObj()->GetUnit() == XGAME::xUNIT_MINOTAUR ) {
		if( FindBuffSkill( _T( "wide" ) ) ) {
			float adjVal;
			auto pBuff = GetAdjParamByBuff( &adjVal, _T( "mad_cow" ), XGAME::xADJ_MOVE_SPEED );
			if( pBuff )
				addRatio += adjVal;
		}
	}
	float prevSpeed = speed;
	float newSpeed = CalcAdjParam( speed, XGAME::xADJ_MOVE_SPEED, addRatio );
#ifdef _DEBUG
//	XBREAK( newSpeed < 0 );
#endif // _DEBUG
	if( newSpeed < 0 )
		newSpeed = 1.f;
	// 분노상태에선 이동속도 저하 효과를 무시한다.
	if( IsState( xST_FURY ) && newSpeed < prevSpeed )
		return prevSpeed;
	return newSpeed;
}
/**
 @brief
*/
float XBaseUnit::GetMovePixelPerFrame( float movPixelPerFrame )
{
	const auto pHero = GetpHero();
	float speed = movPixelPerFrame * pHero->GetMoveSpeed();
	return CalcAdjParam( speed, XGAME::xADJ_MOVE_SPEED );
}
// float XUnitCommon::GetSpeedMovePerSec( float movSpeedPerSec )
// {
// 	float speed = movSpeedPerSec * m_pHero->GetMoveSpeed();
// 	return CalcAdjParam( speed, XGAME::xADJ_MOVE_SPEED );
// }
float XBaseUnit::GetAttackRadiusByPixel()
{
	const auto pHero = GetpHero();
	float radius = 0;
	if( IsRange() )
		radius = m_pPropUnit->radiusAtkByPixel;
	else
		radius = XGAME::DIST_MELEE_ATTACK;	// 근접유닛은 사거리 3미터로 고정
	radius *= pHero->GetAttackRadiusRatio();
	return CalcAdjParam( radius, XGAME::xADJ_ATTACK_RANGE );
}

float XBaseUnit::GetSpeedAttack( UnitPtr spTarget )
{
	float speed = GetSpeedAttackBase();
	float addAdjRatio = 0;
	float addAdjVal = 0;
	if( spTarget != nullptr ) {
		switch( spTarget->GetUnitSize() ) {
		case XGAME::xSIZE_SMALL: {
			addAdjRatio += GetAdjParam( XGAME::xADJ_ATTACK_SPEED_SMALL )->valPercent;
		} break;
		case XGAME::xSIZE_MIDDLE: {
			addAdjRatio += GetAdjParam( XGAME::xADJ_ATTACK_SPEED_MIDDLE )->valPercent;
		} break;
		case XGAME::xSIZE_BIG: {
			addAdjRatio += GetAdjParam( XGAME::xADJ_ATTACK_SPEED_BIG )->valPercent;
		} break;
		}
		if( GetpSquadObj()->GetUnit() == XGAME::xUNIT_ARCHER ) {
			auto pBuff = FindBuffSkill( _T( "quick_firing" ) );
			if( pBuff && m_typeCurrMeleeType == XGAME::xMT_RANGE ) {
				XE::VEC2 vDist = spTarget->GetvwPos().ToVec2() - GetvwPos().ToVec2();
				float distSkill = xMETER_TO_PIXEL( (float)pBuff->GetInvokeSizeByLevel() );
				if( vDist.Lengthsq() < distSkill * distSkill )
					addAdjRatio += pBuff->GetAbilMinbyLevel();
			}
		} else
		if( GetpSquadObj()->GetUnit() == XGAME::xUNIT_MINOTAUR ) {
			// 광폭 특성이 있을때
			if( FindBuffSkill(_T("wide")) ) {
				float adjVal;
				auto pBuff = GetAdjParamByBuff( &adjVal, _T( "mad_cow" ), XGAME::xADJ_ATTACK_SPEED );
				if( pBuff )
					addAdjRatio += adjVal;
			}
		}
	}
	return CalcAdjParam( speed, XGAME::xADJ_ATTACK_SPEED, addAdjRatio, addAdjVal );
}

/**
 @brief
*/
int XBaseUnit::GetMaxHp()
{
	auto pHero = GetpHero();
	auto hpMax = GetspLegionObj()->GetspLegion()->GethpMaxEach( pHero->GetsnHero()
																														, IsHero() != FALSE );
	return (int)CalcAdjParam( hpMax, XGAME::xADJ_MAX_HP );
}

float XBaseUnit::GetDefensePower()
{
	auto pHero = GetpHero();
// 	float def = ( m_pPropUnit->def * GetmultipleAbility() ) * pHero->GetDefenseRatio();
	// 방어력은 유닛크기에 따른 보정율을 곱하지 않았음. 골렘이 팔라딘보다 방어력이 낮아지는 문제 생김
	float def = ( m_pPropUnit->def ) * pHero->GetDefenseRatio();
	return CalcAdjParam( def, XGAME::xADJ_DEFENSE );
}
float XBaseUnit::GetPenetrationRatio() const
{
	float ratio = 0.f;
	return CalcAdjParam( ratio, XGAME::xADJ_PENETRATION_RATE );
}
// xStat end
//////////////////////////////////////////////////////////////////////////
void XBaseUnit::StartAttackDelay( UnitPtr spTarget, float secMin )
{
	float secSpeed = GetSpeedAttack( spTarget );
	if( secMin > 0.f && secSpeed < secMin )
		m_timerAttack.Set( secMin );
	else
		m_timerAttack.Set( secSpeed );
}

/**
 @brief 스킬시스템에 발동스킬이 실행되기전에 호출된다. 다른 발동스킬을 원한다면 이곳에 하드코딩
*/
bool XBaseUnit::OnInvokeSkill( XSKILL::XSkillDat *pDat,
															const XSKILL::EFFECT *pEffect,
															XSKILL::XSkillReceiver* pTarget,
															int level,
															_tstring* pstrOut )

{
	if( pDat->GetstrIdentifier() == _T("chill_blast_arrow") )	{
		// 냉기폭발 화살의 스킬이 발동될때 대신 빙결화살이 발사되도록 한다.흡혈
		*pstrOut = _T("invoke_freeze_arrow");
	} else
	// 골렘의 용암파편 특성
	if( *pstrOut == _T("invoke_lava_fragments"))	{
		return true;
// 		if( m_timerInvokeHitter.IsOff() )		{		이렇게 하면 쏜즈가 무슨소용인가.
// 			m_timerInvokeHitter.Set( 1.f );
// 			return true;
// 		}
// 		// 쏜즈가 발동되고 1초는 지나야 다음 쏜즈가 발동된다.
// 		if( m_timerInvokeHitter.IsOver() )		{
// 			m_timerInvokeHitter.Reset();
// 			return true;
// 		}
// 		return false;
	}
	return true;
}

/**
 @brief 힐을 받아 hp가 오를때 쓰는 전용함수.
 힐 sfx가 함께 발동된다.
*/
void XBaseUnit::DoHeal( float addHp )
{
	if( IsDead() )
		return;
	// 힐 받은양 누적
	GetpStatObj()->AddHeal( GetpHero()->GetsnHero(), addHp );
	AddHp( addHp );
//	if( this->IsPlayer() )
	{
		XObjDmgNum *pDmg = new XObjDmgNum( addHp,
														xHT_HIT, 0,
// 														FALSE,
// 														false,
														GetvwTop() );
		pDmg->SetCol( XCOLOR_GREEN );
		AddObj( pDmg );
	}
}

/**
 @brief 좌표에 생성되어야 하는 지속효과객체를 생성한다.
 시전자는 this
*/
void XBaseUnit::OnCastEffectToPos( XSKILL::XSkillDat *pSkillDat,
									XSKILL::EFFECT *pEffect,
									int level,
									float sec,
									float radiusMeter,
									BIT bitSideInvokeTarget,	// 발동되어야 하는 발동대상우호
									XSKILL::XSkillReceiver *pBaseTarget,
									const XE::VEC2& vPos )
{
	float dmg = 1.f;
	XBREAK( vPos.IsZero() || vPos.IsMinus() );
	auto pFlame = new XObjFlame( GetThisUnit(),
								vPos,
								dmg,
								xMETER_TO_PIXEL(radiusMeter),
								sec,
								bitSideInvokeTarget,
								_T("eff_ghost_diesmoke.spr"), 1 );
	AddObj( pFlame );
}

/**
 @brief this가 현재 현재 흡혈양보정치를 얻는다. 0이면 흡혈중이 아니다.
*/
float XBaseUnit::GetVampiricRatio()
{
	return GetAdjValue( XGAME::xADJ_VAMPIRIC );
}

/**
 @brief GetInvokeTarget()에서 발동범위를 결정하기전에 호출되어진다.
*/
float XBaseUnit::OnInvokeTargetSize( XSKILL::XSkillDat *pSkillDat,
									const XSKILL::EFFECT *pEffect,
									int level,
									XSKILL::XSkillReceiver *pCastingTarget,
									float size )
{
	if( GetUnitType() == xUNIT_TREANT ) {
		const auto idsSkill = pSkillDat->GetstrIdentifier();
		if( idsSkill == _T( "view_blocked" ) || idsSkill == _T( "phytoncide" ) 
			|| idsSkill == _T( "archer_concealment" ) || idsSkill == _T( "entangling" ) ) {
			// 무성한 나무 특성이 있으면 위 스킬들의 발동반경을 증폭시킨다.
			auto pBuff = FindBuffSkill( _T( "cordyceps" ) );
			if( pBuff ) {
				// 시야차단 특성이 발동될때 "무성한나무"특성이 있다면 반경 증가
				auto abilMin = pBuff->GetAbilMinbyLevel();
				return size + (size * abilMin);
			}
		}
	}
// 	if( pSkillDat->GetstrIdentifier() == _T( "view_blocked" ) ) {
// 		auto pBuff = FindBuffSkill( _T( "cordyceps" ) );
// 		if( pBuff ) {
// 			// 시야차단 특성이 발동될때 "무성한나무"특성이 있다면 반경 증가
// 			return pBuff->GetInvokeSizeByLevel();
// 		}
// 	}
	return size;
}

/**
 @brief 자신의 전투 통계 객체를 꺼낸다.
*/
XStatistic* XBaseUnit::GetpStatObj() const
{
	return const_cast<XStatistic*>( GetspLegionObjConst()->GetpStatObj() );
}

// const XStatistic* GetpStatObjConst() const 
// {
// 	return GetspLegionObjConst()->GetpStatObj();
// }

/**
 @brief this유닛의 데미지 딜링양을 누적시킨다.
*/
float XBaseUnit::AddDamageDeal( float damage )
{
	auto pStatObj = GetpStatObj();
	if( XBREAK(pStatObj == nullptr) )
		return 0;
	return pStatObj->AddDamageDeal( GetpHero()->GetsnHero(), damage );
}
/**
 @brief this가 목표에 도달함.
*/
void XBaseUnit::OnArriveTarget( UnitPtr spUnit, const XE::VEC3& vwDst )
{
}
UnitPtr XBaseUnit::GetspLeader()
{
	return GetpSquadObj()->GetspLeaderUnit();
}

void XBaseUnit::OnApplyEffectAdjParam( XSKILL::XSkillUser *pCaster
																		, XSKILL::XSkillDat* pSkillDat
																		, const XSKILL::EFFECT *pEffect
																		, float abilMin )
{
	// adjParam이 적용될때 거기에 맞는 텍스트(xxx상승/하락)를 띄워준다.
	const auto adjParam = (XGAME::xtParameter)pEffect->invokeParameter;
	_tstring strParam
		= XGAME::GetStrAdjParam( adjParam );
	if( !strParam.empty() ) {
		if( pEffect->valtypeInvokeAbility == xPERCENT ) {
//			XBREAK( abilMin < 0 );		// 이런경우가 있으면 아래코드 바꿔야 함.
			if( adjParam == xADJ_ATTACK_SPEED ) {
				if( abilMin < 0 )
					strParam = XE::Format( XTEXT( 2242 ), strParam.c_str() );	// xx상승
				else if( abilMin > 0 )
					strParam = XE::Format( XTEXT( 2243 ), strParam.c_str() );	// xx하락
				else
					strParam.clear();
			} else {
				if( abilMin > 0 )
					strParam = XE::Format( XTEXT( 2242 ), strParam.c_str() );	// xx상승
				else if( abilMin < 0 )
					strParam = XE::Format( XTEXT( 2243 ), strParam.c_str() );	// xx하락
				else
					strParam.clear();
			}
		} else
		if( pEffect->valtypeInvokeAbility == xVAL ) {
			if( abilMin > 0 )
				strParam = XE::Format( XTEXT( 2242 ), strParam.c_str() );	// xx상승
			else if( abilMin < 0 )
				strParam = XE::Format( XTEXT( 2243 ), strParam.c_str() );	// xx하락
			else
				strParam.clear();
		}
	}
	if( IsHero() && !strParam.empty() ) {
		// 일단은 영웅만 텍스트 표시되게
		const XCOLOR col = ( IsPlayer() )? XCOLOR_GREEN : XCOLOR_RED;
//#ifndef _XUZHU_HOME
		auto pDmg = new XObjDmgNum( strParam, GetvwTop(), col );
		AddObj( pDmg );
//#endif
	}
}

/**
 @brief 유닛 크기에 따른 크기비용.
*/
int XBaseUnit::GetSizeCost()
{
	return _XGC->m_arySizeCost[ GetUnitSize() ];
}

/**
 @brief front 메시지큐에 메시지를 추가한다.
*/
void XBaseUnit::PushMsg( XSPMsg spMsg )
{
	m_spMsgQ1->AddMsg( spMsg );
}

/**
 @brief ::FrameMove()에서 쌓였던 front 메시지들을 처리한다.
*/
void XBaseUnit::ProcessMsgQ()
{
	m_spMsgQ1->Process();
}

XE::VEC3 XBaseUnit::GetSize() const 
{
	XE::VEC2 vSize = m_bbLocal.GetSize();
	return XE::VEC3( vSize.w, 0.f, vSize.h );
}
