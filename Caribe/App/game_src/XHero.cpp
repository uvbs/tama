#include "stdafx.h"
#include "XHero.h"
#include "XArchive.h"
#include "etc/Timer.h"
#include "XPropHero.h"
#include "XExpTableHero.h"
#include "XSkillMng.h"
#include "XPropSquad.h"
#include "XBaseItem.h"
#include "XAccount.h"
#include "XPropUnit.h"
#include "XPropUpgrade.h"
#include "XGlobalConst.h"
#ifdef _XSINGLE
#include "XLegion.h"
#endif

using namespace XSKILL;
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// ID XHero::s_keySkillMng = 0;
#ifdef _CLIENT
//volatile float XHero::s_fMultiply = 100;				// 메모리 치팅방지
#endif // _CLIENT

void XHero::xItem::Set( XBaseItem *_pItem ) {
	if( _pItem ) {
		pItem = _pItem;
		snItem = _pItem->GetsnItem();
	}
	else {
		pItem = nullptr;
		// snItem값은 남아있을 수 있음.
	}
}
XBaseItem* XHero::xItem::GetpItem() {
	XBREAK( pItem && pItem->GetsnItem() != snItem );
	return pItem;
}
//////////////////////////////////////////////////////////////////////////
// static
int XHero::sSerialize( XArchive& ar, XHero *pHero )
{
	ar << VER_HERO_SERIALIZE;
	pHero->Serialize( ar );
	return 1;
}

XHero* XHero::sCreateDeSerialize( XArchive& ar, XSPAcc spAcc )
{
	int ver;
	ar >> ver;
	XHero *pHero = new XHero;
	pHero->DeSerialize( ar, spAcc, ver );
	spAcc->AddHero( pHero );
	return pHero;
}

#if defined(_XSINGLE) || !defined(_CLIENT)
// XHero* XHero::sCreateHero( XPropHero::xPROP *pProp, XGAME::xtUnit unit )
// {
// #if defined(_XSINGLE) || !defined(_CLIENT)
// #else
// 	XBREAK(1);	// 클라에선 이걸 사용하면 안됨.
// #endif // XSINGLE || not CLIENT
// 
// 	XHero *pHero = new XHero( pProp, unit, numUnit );
// 	return pHero;
// }
XHero* XHero::sCreateHero( XPropHero::xPROP *pProp, 
						int levelSquad, 
						XGAME::xtUnit unit )
{
// 	XGAME::xtClan clan = pProp->clan;
#if defined(_XSINGLE) || !defined(_CLIENT)
// 	if( clan == XGAME::xCL_RANDOM )
// 		clan = ( XGAME::xtClan )( 1 + random( XGAME::xCL_MAX - 1 ) );
// 	XBREAK( clan <= 0 || clan >= XGAME::xCL_MAX );
#else
	XBREAK( 1 );	// 클라에선 이걸 사용하면 안됨.
#endif // XSINGLE || not CLIENT

	XHero *pHero = new XHero( pProp, levelSquad, unit );
// 	XHero *pHero = new XHero( pProp, levelSquad, unit, clan );
	return pHero;
}
// XHero* XHero::sCreateHero( ID idProp, XGAME::xtUnit unit )
// {
// 	XPropHero::xPROP *pProp = PROP_HERO->GetpProp( idProp );
// 	if( XBREAK( pProp == nullptr ) )
// 		return nullptr;
// 
// 	return sCreateHero( pProp, unit, numUnit );
// }
#endif // defined(_XSINGLE) || !defined(_CLIENT)


////////////////////////////////////////////////////////////////
XHero::XHero()
	: XAdjParam( XGAME::xMAX_PARAM, 0 )
	, m_aryUpgrade(XGAME::xTR_MAX)
	, m_aryUnitsAbil( XGAME::xUNIT_MAX )
{
	// DeSerialize용
	Init();
}

XHero::XHero( XPropHero::xPROP *pProp, int levelSquad, XGAME::xtUnit unit )
// XHero::XHero( XPropHero::xPROP *pProp, int levelSquad, XGAME::xtUnit unit, XGAME::xtClan clan )
	: XAdjParam( XGAME::xMAX_PARAM, 0 )
	, m_aryUpgrade(XGAME::xTR_MAX)
	, m_aryUnitsAbil( XGAME::xUNIT_MAX )
{
	Init();
	XBREAK( pProp == nullptr );
	auto& propSquad = PROP_SQUAD->GetTable( levelSquad );
	int numUnit = propSquad.GetMaxUnit( unit );
	m_snHero = XE::GenerateID();
//	SetpDelegateLevel( this );
	XBREAK( unit >= XGAME::xUNIT_MAX || unit <= XGAME::xUNIT_NONE );
	XBREAK( numUnit > XGAME::MAX_UNIT_SMALL );
//	m_levelSquad = levelSquad;
	m_aryUpgrade[ XGAME::xTR_SQUAD_UP ].SetLevel( levelSquad );
	m_Unit = unit;
//	m_numUnit = numUnit;
	_m_pProp = pProp;
	m_keyPropHero = PROP_HERO->GetidKey();
	m_idProp = pProp->idProp;
#if defined(_CLIENT) || defined(_GAME_SERVER)
	AssignSkillPtr();
#endif
// 	m_Clan = clan;
// 	XBREAK( m_Clan <= 0 || m_Clan >= XGAME::xCL_MAX );
//	m_Grade = pProp->GetGrade();
	m_Grade = XGAME::xGD_COMMON;
}

void XHero::Destroy()
{
}

void XHero::InitAryAbil() 
{
	m_aryUnitsAbil.Clear();
	for( int i = 0; i < XGAME::xUNIT_MAX; ++i ) {
		std::map<ID, XGAME::xAbil> mapNew;
		m_aryUnitsAbil.Add( mapNew );	// 0번 인덱스
	}
}

XPropHero::xPROP* XHero::GetpProp()
{
	if( m_keyPropHero != PROP_HERO->GetidKey() ) {
		_m_pProp = PROP_HERO->GetpProp( m_idProp );
		m_keyPropHero = PROP_HERO->GetidKey();
	}
	return _m_pProp;
}

void XHero::SetpProp( XPropHero::xPROP* pProp, ID idKey )
{
	_m_pProp = pProp;
	m_keyPropHero = idKey;
//	m_Grade = _m_pProp->GetGrade();
}
void XHero::SetpProp( ID idProp )
{
	auto pProp = PROP_HERO->GetpProp( idProp );
	if( XASSERT( pProp ) ) {
		m_idProp = idProp;
		_m_pProp = pProp;
		m_keyPropHero = PROP_HERO->GetidKey();
//		m_Grade = _m_pProp->grade;
	}
}

int XHero::Serialize( XArchive& ar )
{
	int _sizeAr = ar.size();
	//
	ar << m_snHero;
	if( XBREAK( m_idProp > 0xffff ) )
		return 0;
	XBREAK( m_numRemainAbilPoint >= 0xff );
	XBREAK( m_numRemainAbilUnlock >= 0xff );
	XBREAK( m_numInitAbil >= 0xff );
	ar << (WORD)m_idProp;
	ar << (BYTE)m_Unit;
	ar << (BYTE)m_numRemainAbilPoint;
	ar << xboolToByte( m_bLive );
	ar << (BYTE)m_Grade;
	ar << (BYTE)m_numRemainAbilUnlock;
	ar << (BYTE)m_numInitAbil;
	SerializeUpgrade( ar );
	SerializeAbil( ar );
	for (int parts = 1; parts < XGAME::xPARTS_MAX; ++parts) {
		ar << parts;
		// 검사루틴 반드시 넣을것. -xuzhu-
		if( XBREAK( XGAME::IsInvalidParts( (XGAME::xtParts)parts ) ) )
			return 0;
		auto& item = m_aryEquip[parts];
		if (item.GetpItem()) {
			XBREAK( item.GetpItem()->GetsnItem() != item.GetsnItem() );
			ar << item.GetpItem()->GetsnItem();
		} else
			ar << 0;
	}

	return ar.size() - _sizeAr;
}
int XHero::DeSerialize(XArchive& ar, XSPAcc spAcc, int verHero )
{
	ID idProp;
	BYTE b0;
	WORD w0;
	
	ar >> m_snHero;
	ar >> w0;	idProp = w0;
	SetpProp( idProp );
	auto pProp = _m_pProp;
	if( XBREAK( pProp == nullptr ) )
		return FALSE;
	ar >> b0;	m_Unit = ( XGAME::xtUnit )b0;;
	if( verHero >= 8 ) {
		ar >> b0;		m_numRemainAbilPoint = b0;
	} else {
		ar >> b0;
	}
	ar >> b0;	m_bLive = xbyteToBool( b0 );
	ar >> b0;	m_Grade = ( XGAME::xtGrade )b0;
//	ar >> w0; 
	if( verHero >= 8 ) {
		ar >> b0;		m_numRemainAbilUnlock = b0;
	} else {
		ar >> b0;
	}
	ar >> b0;		m_numInitAbil = b0;
	XBREAK( !m_Grade );
	DeSerializeUpgrade( ar );
	DeserializeAbil( ar, verHero );
	m_aryEquip.Clear( xItem() );
	for (int parts = 1; parts < XGAME::xPARTS_MAX; ++parts) {
		ID idParts, snItem;
		ar >> idParts;
		ar >> snItem;
		// 검사루틴 반드시 넣을것. -xuzhu-
		if( XBREAK( XGAME::IsInvalidParts((XGAME::xtParts)idParts) ) )
			return 0;
		if (spAcc && snItem != 0) {
			m_aryEquip[idParts].Set( spAcc->GetItem(snItem) );
		}
	}
#if defined(_CLIENT) || defined(_GAME_SERVER)
	AssignSkillPtr();
#endif
	
	return 1;
}

int XHero::SerializeUpgrade( XArchive& ar ) const 
{
	for( int i = 1; i < (int)m_aryUpgrade.size(); ++i )
		m_aryUpgrade[ i ].Serialize( ar );
	return 1;
}

int XHero::DeSerializeUpgrade( XArchive& ar )
{
	for( int i = 1; i < (int)m_aryUpgrade.size(); ++i )
		m_aryUpgrade[ i ].DeSerialize( ar, 0 );
	return 1;
}

///< level에서의 최대 exp의 값을 돌려줘야 한다.
DWORD XHero::OnDelegateGetMaxExp( XFLevel *pLevel, int level, DWORD param1, DWORD param2 ) 
{ 
	auto type = (XGAME::xtTrain)pLevel->GetidLevel();
	if( XASSERT(XGAME::IsValidTrainType( type )) ) {
		return GetMaxExpWithLevel( type, level );
	}
	return 0;
}
///< 최대 레벨값을 돌려줘야 한다.
int XHero::OnDelegateGetMaxLevel( XFLevel *pLevel, DWORD param1, DWORD param2 ) 
{ 
	auto type = ( XGAME::xtTrain )pLevel->GetidLevel();
	if( XASSERT( XGAME::IsValidTrainType( type ) ) ) {
		return GetMaxLevel( type );
	}
	return 0;
}

int XHero::GetnumUnit()
{
	const int lvSquad = GetlevelSquad();
	auto& propSquad = PROP_SQUAD->GetTable( lvSquad );
	return propSquad.GetMaxUnit( GetUnit() );
}
/**
 @brief type의 현재레벨의 최대경험치를 돌려준다.
*/
int XHero::GetMaxExp( XGAME::xtTrain type )
{
	int level = m_aryUpgrade[ type ].GetLevel();
	switch( type )
	{
	case XGAME::xTR_LEVEL_UP:
		return EXP_TABLE_HERO->GetMaxExp( level );
	case XGAME::xTR_SQUAD_UP:
		return PROP_SQUAD->GetMaxExp( level );
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP:
		return XPropUpgrade::sGet()->GetMaxExp( level );
	default:
		XBREAK(1);
		break;
	}
	return 0;
}
// int XHero::GetMaxExpWithLevel( XGAME::xtTrain type, int level )
// {
// 	if( level == 0 )
// 		return 0;
// 	switch( type ) {
// 	case XGAME::xTR_LEVEL_UP:
// 		return EXP_TABLE_HERO->GetMaxExp( level );
// 	case XGAME::xTR_SQUAD_UP:
// 		return PROP_SQUAD->GetMaxExp( level );
// 	case XGAME::xTR_SKILL_ACTIVE_UP:
// 	case XGAME::xTR_SKILL_PASSIVE_UP:
// 		return XPropUpgrade::sGet()->GetMaxExp( level );
// 	default:
// 		XBREAK( 1 );
// 		break;
// 	}
// 	return 0;
//}
int XHero::sGetMaxExpWithLevel( XGAME::xtTrain type, int level )
{
	if( level == 0 )
		return 0;
	switch( type ) {
	case XGAME::xTR_LEVEL_UP:
		return EXP_TABLE_HERO->GetMaxExp( level );
	case XGAME::xTR_SQUAD_UP:
		return PROP_SQUAD->GetMaxExp( level );
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP:
		return XPropUpgrade::sGet()->GetMaxExp( level );
	default:
		XBREAK( 1 );
		break;
	}
	return 0;
}

/**
 @brief 이 영웅이 올라갈수 있는 최대 레벨
*/
int XHero::GetMaxLevel() const
{
	return XGAME::MAX_HERO_LEVEL;
//	XBREAK( EXP_TABLE_HERO->GetMaxLevel() != 50 );
// 	std::vector<int> ary = {30, 30, 30, 30, 40, 50};
// 	return ary[ m_Grade ];
}

/**
 @brief 이 영웅이 올라갈수 있는 레벨/부대레벨/스킬레벨을 얻는다.
*/
int XHero::GetMaxLevel( XGAME::xtTrain type ) const
{
	switch( type )
	{
	case XGAME::xTR_LEVEL_UP:	return GetMaxLevel();
	case XGAME::xTR_SQUAD_UP: {
		return XGAME::MAX_SQUAD_LEVEL;
// 		std::vector<int> ary = { 10, 10, 10, 10, 13, 16 };
// 		return ary[ m_Grade ];
	}	break;
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP: {
		return XGAME::MAX_SKILL_LEVEL - 1;
// 		std::vector<int> ary = {6, 6, 6, 6, 8, 10};
// 		return ary[ m_Grade ];
	}	break;
	default:
		XBREAK(1);
		break;
	}
	return 0;
}

#if defined(_CLIENT) || defined(_GAME_SERVER)
XSKILL::XSkillDat* XHero::GetSkillDatPassive( void ) 
{
	// 스킬매니저가 재로딩되었으면 스킬포인터를 다시 할당받는다.
	if( m_keySkillMng != SKILL_MNG->GetidKey() ) {
		m_pPassive = nullptr;
		m_pActive = nullptr;
		AssignSkillPtr();
	}
	return m_pPassive;
}
XSKILL::XSkillDat* XHero::GetSkillDatActive( void ) 
{
	// 스킬매니저가 재로딩되었으면 스킬포인터를 다시 할당받는다.
	if( m_keySkillMng != SKILL_MNG->GetidKey() ) {
		m_pPassive = nullptr;
		m_pActive = nullptr;
		AssignSkillPtr();
	}
	return m_pActive;
}

XSKILL::XSkillDat* XHero::GetSkillDat( XGAME::xtTrain type )
{
	if( type == XGAME::xTR_SKILL_ACTIVE_UP )
		return GetSkillDatActive();
	if( type == XGAME::xTR_SKILL_PASSIVE_UP )
		return GetSkillDatPassive();
	return nullptr;
}


void XHero::AssignSkillPtr( void )
{
	// 패시브는 3성부터 활성화 됨.
 	if( m_Grade >= XGAME::xGD_RARE ) {
		if( GetpProp()->strPassive.empty() == false ) {
			if( m_pPassive == nullptr ) {
				m_pPassive = SKILL_MNG->FindByIdentifier( GetpProp()->strPassive.c_str() );
				m_keySkillMng = SKILL_MNG->GetidKey();
			}
		}
	}
// 	if( m_Grade >= XGAME::xGD_EPIC ) {
	// 액티브스킬은 1성부터 활성화됨.
		if( GetpProp()->strActive.empty() == false ) {
			if( m_pActive == nullptr ) {
				m_pActive = SKILL_MNG->FindByIdentifier( GetpProp()->strActive.c_str() );
				m_keySkillMng = SKILL_MNG->GetidKey();
			}
		}
// 	}
}
/**
 @brief 액티브/패시브가 활성화 되었는지 확인한다.
*/
bool XHero::IsActivationSkill( XGAME::xtTrain type )
{
	// 액티브는 항상 활성화
	if( type == XGAME::xTR_SKILL_ACTIVE_UP )
		return true;
	if( type == XGAME::xTR_SKILL_PASSIVE_UP )
		if( GetGrade() >= XGAME::xGD_RARE )
			return true;
	return false;
}

/**
 @brief 각 훈련타입에 따라 현재 남은 exp를 리턴한다.
*/
int XHero::GetTrainExpRemain( XGAME::xtTrain typeTrain )
{
	return 0;
}
#endif // #if defined(_CLIENT) && defined(_GAME_SERVER)

BOOL XHero::SetEquip(XBaseItem *pItem)
{
	XBREAK(pItem == NULL);
	XBREAK(pItem->GetbEquip() == TRUE);	//사용중인 장비를 누군가 또 장착 시도하면 문제있는거임
	XGAME::xtParts parts = pItem->GetpProp()->parts;
	if (pItem->GetpProp()->type == XGAME::xIT_EQUIP)
	{
		if (m_aryEquip[parts].GetpItem() != nullptr)
			m_aryEquip[parts].GetpItem()->SetbEquip(FALSE);
		pItem->SetbEquip(TRUE);
		m_aryEquip[parts].Set( pItem );
		return TRUE;
	}
	return FALSE;
}

XBaseItem* XHero::GetEquipItem(XGAME::xtParts parts)
{
	if (XGAME::xPARTS_NONE < parts && parts < XGAME::xPARTS_MAX)
		return m_aryEquip[parts].GetpItem();
	return NULL;
}

void XHero::SetUnequipAll()
{
	for (int i = XGAME::xPARTS_HEAD; i < XGAME::xPARTS_MAX; ++i)
	{
		if (m_aryEquip[i].GetpItem())
			m_aryEquip[i].GetpItem()->SetbEquip(FALSE);
	}
}

BOOL XHero::SetUnequip(XBaseItem *pItem)
{
	XBREAK(pItem == NULL);
	XBREAK(pItem->GetbEquip() == FALSE);	//사용중이지 않은 장비를 누군가 탈착하면 문제있는거
	if (pItem->GetpProp()->type == XGAME::xIT_EQUIP)
	{
		pItem->SetbEquip(FALSE);
		XGAME::xtParts parts = pItem->GetpProp()->parts;
		m_aryEquip[parts].Set( nullptr );
		return TRUE;
	}
	return FALSE;
}

// float XHero::GetSquadAttackMeleePower()
// {
// //	float power = PROP_UNIT->GetAttackMeleePowerSquad( m_Unit, m_levelSquad );
// 	// 
// 	return 0;
// }

/**
 @brief 유닛의 기본능력치 * 영웅능력치 를 얻는다.
*/
void XHero::GetSquadStat( int levelSquad, xSquadStat *pOut )
{
	pOut->meleePower = PROP_UNIT->GetAttackMeleePowerSquad( this, levelSquad );
	pOut->rangePower = PROP_UNIT->GetAttackRangePowerSquad( this, levelSquad );
	pOut->def = PROP_UNIT->GetDefensePowerSquad( this, levelSquad );
	pOut->hp = PROP_UNIT->GetMaxHpSquad( this, levelSquad );
	pOut->speedAtk = PROP_UNIT->GetAttackSpeed( this->GetUnit() );
	pOut->speedMoveForMeter = xPIXEL_TO_METER( PROP_UNIT->GetMoveSpeedPerSec( this->GetUnit() ) );
}

/**
 @brief 
 @param bForShow UI에 표기되는 용도의 값
*/
float XHero::GetAttackMeleeRatio( int lvHero, bool bForShow )
{
	// 기본프로퍼티 값에 레벨에 따른 증가량을 곱해서 보정한다.
// 	float addGrade = GetGrade() * 0.1f;		// 등급당 10%씩 증가
// 	float atkMelee = GetpProp()->atkMelee + addGrade;	// 기본스탯에 등급스탯을 더함.
// 	float val = atkMelee + ( lvHero * GetpProp()->atkMeleeAdd );
	float val = GetpProp()->GetStat2( XGAME::xSTAT_ATK_MELEE, GetGrade(), lvHero );
	float multiply = 0;
	multiply += GetAdjParamByItem( XGAME::xADJ_ATTACK );	// 공격력증가 옵션을 더한다.
	multiply += GetAdjParamByItem( XGAME::xADJ_ATTACK_MELEE_TYPE );	// 근접공격력 증가 옵션을 더한다.
	const auto statVal = val * (1.f + multiply);
// 	if( bForShow ) {
// 		if( IsRange() )
// 			return statVal * 10.f;
// 		else
// 			return statVal * 100.f;
// 	}
	if( bForShow )
		return statVal * HERO_STAT_MULTIPLY;
	return statVal;
}

float XHero::GetAttackRangeRatio( int lvHero, bool bForShow )
{
	// 기본프로퍼티 값에 레벨에 따른 증가량을 곱해서 보정한다.
	float addGrade = GetGrade() * 0.1f;		// 등급당 10%씩 증가
	float atkRange = GetpProp()->atkRange + addGrade;	// 기본스탯에 등급스탯을 더함.
	float val = atkRange + ( lvHero * GetpProp()->atkRangeAdd );
	float multiply = 0;
	multiply += GetAdjParamByItem( XGAME::xADJ_ATTACK );	// 공격력증가 옵션을 더한다.
	multiply += GetAdjParamByItem( XGAME::xADJ_ATTACK_RANGE_TYPE );	// 근접공격력 증가 옵션을 더한다.
	const auto statVal = val * ( 1.f + multiply );
// 	if( bForShow ) {
// 		if( IsRange() )
// 			return statVal * 100.f;
// 		else
// 			return statVal * 0.f;
// 	}
	if( bForShow ) {
		if( IsRange() )
			return statVal * HERO_STAT_MULTIPLY;
		else
			return statVal * 0.f;
	}
	return statVal;
}
float XHero::GetDefenseRatio( int lvHero, bool bForShow )
{
	float addGrade = GetGrade() * 0.1f;		// 등급당 10%씩 증가
	float defense = GetpProp()->defense + addGrade;	// 기본스탯에 등급스탯을 더함.
	float val = defense + ( lvHero * GetpProp()->defenseAdd );
	float multiply = 0;
	multiply += GetAdjParamByItem( XGAME::xADJ_DEFENSE );
	const auto statVal = val * ( 1.f + multiply );
	if( bForShow )
		return statVal * HERO_STAT_MULTIPLY;
	return statVal;
}
float XHero::GetHpMaxRatio( int lvHero, bool bForShow )
{
	float addGrade = GetGrade() * 0.1f;		// 등급당 10%씩 증가
	float hpMax = GetpProp()->hpMax + addGrade;	// 기본스탯에 등급스탯을 더함.
	float val = hpMax + ( lvHero * GetpProp()->hpAdd );
	float multiply = 0;
	multiply += GetAdjParamByItem( XGAME::xADJ_MAX_HP );
// 	return val * ( 1.f + multiply );
	const auto statVal = val * ( 1.f + multiply );
	if( bForShow )
		return statVal * HERO_STAT_MULTIPLY;
	return statVal;
}
/**
	@brief 몇초에 한번씩 공격하는가
*/
float XHero::GetAttackSpeed( int lvHero, bool bForShow )
{
	float val = GetpProp()->attackSpeed + ( lvHero * GetpProp()->attackSpeedAdd );
	float multiply = 0;
	multiply += GetAdjParamByItem( XGAME::xADJ_ATTACK_SPEED );
// 	return val * ( 1.f + multiply );
	const auto statVal = val * ( 1.f + multiply );
	if( bForShow )
		return statVal * HERO_STAT_MULTIPLY;
	return statVal;
}
float XHero::GetMoveSpeed( int lvHero, bool bForShow )
{
	float val = GetpProp()->moveSpeed + (lvHero * GetpProp()->moveSpeedAdd);
	float multiply = 0;
	multiply += GetAdjParamByItem( XGAME::xADJ_MOVE_SPEED );
// 	return val * ( 1.f + multiply );
	const auto statVal = val * ( 1.f + multiply );
	if( bForShow )
		return statVal * HERO_STAT_MULTIPLY;
	return statVal;
}

/**
 @brief adjParam보정파라메터를 가진 아이템의 보정값을 모두 합하여 리턴한다.
*/
float XHero::GetAdjParamByItem( XGAME::xtParameter adjParam )
{
	float add = 0.f;
	XARRAYN_LOOP_AUTO( m_aryEquip, &item ) {
		if( item.GetpItem() ) {
//			XARRAYLINEARN_LOOP_AUTO( item.GetpItem()->GetpProp()->aryAdjParam, &adj )	{
			for( const auto& adj : item.GetpItem()->GetpProp()->aryAdjParam ) {
				if( adj.adjParam == adjParam ) {
					add += adj.param / 100.f;
					break;
				}
			}// END_LOOP;
		}
	} END_LOOP;
	return add;
}

/**
 @brief snItem이 영웅이 갖고 있던 아이템이라면 pNewItem으로 포인터를 교체한다.
*/
void XHero::UpdateEquipItem( ID snItem, XBaseItem *pNewItem )
{
	bool bChanged = false;
	XARRAYN_LOOP_AUTO( m_aryEquip, &item )
	{
		if( item.GetsnItem() ) {
			if( snItem == item.GetsnItem() ) {
				XBREAK( bChanged == true );		// 같은 시리얼번호의 장착아이템이 하나더 있다는뜻.
				item.Set( pNewItem );
				bChanged = true;
			}
#ifndef _DEBUG
			return;
#endif
		}
		} END_LOOP;
}

/**
 @brief snItem을 장착중인지 확인
*/
bool XHero::IsEquip( ID snItem )
{
	XARRAYN_LOOP_AUTO( m_aryEquip, &item )
	{
		if( item.GetpItem() && item.GetpItem()->GetsnItem() == snItem )
			return true;
	} END_LOOP;
	return false;
}

/**
 @brief pHero의 렙업을 시키기 위해 필요한 자원의 양은?
 @param lvAcc 군주레벨
 @return 목재/철의 수량
*/
// std::pair<int,int> XHero::GetCostHeroLevelUpByRes( int lvAcc ) const
// {
// 	if( XBREAK(lvAcc > XGAME::GetLevelMaxAccount()) )
// 		return std::make_pair( 0, 0 );
// 	auto propLevelUp = XPropUpgrade::sGet()->GetpTableForLevelup( lvAcc );
// 	return propLevelUp->GetNeedRes( m_Clan );
// 
// }
// 
// int XHero::GetCostHeroSkillupByRes( XGAME::xtTrain type ) const
// {
// 	int lvCurr = GetLevelSkill( type );
// 	if( lvCurr >= EXP_TABLE_HERO->GetMaxLevel() )
// 		return 0;
// 	// 다음레벨의 스킬프로퍼티를 꺼낸다.
// 	auto propSkillup = XPropUpgrade::sGet()->GetpTableForSkillup( lvCurr + 1 );
// 	return propSkillup->GetNeedRes();
// 
// }

/**
 @brief pHero의 스킬을 렙업시키기 위한 프로퍼티를 꺼낸다.
*/
const XPropUpgrade::xPropSkill* XHero::GetpPropSkillupNext( XGAME::xtTrain type ) const
{
	if( XBREAK(XGAME::IsInvalidSkillTrain(type)) )
		return nullptr;
	int level = GetLevel( type );
	XBREAK( level <= 0 );
	if( level >= XGAME::GetMaxSkillLevel() )
		return nullptr;
	return XPropUpgrade::sGet()->GetpTableForSkillupNext( level );
}

/**
 @brief 다다음 프로퍼티를 꺼냄
*/
const XPropUpgrade::xPropSkill* XHero::GetpPropSkillupNextNext( XGAME::xtTrain type ) const
{
	if( XBREAK( XGAME::IsInvalidSkillTrain( type ) ) )
		return nullptr;
	int level = GetLevel( type );
	XBREAK( level <= 0 );
	if( level >= XGAME::GetMaxSkillLevel() )
		return nullptr;
	return XPropUpgrade::sGet()->GetpTableForSkillupNext( level );
}

/**
 @brief pHero의 부대렙업을 시키기 위해 필요한 자원의 양은?
 @return 보석의 수량
*/
const XPropSquad::xPROP* XHero::GetpPropSquadupNext() const
{
	int level = GetLevel( XGAME::xTR_SQUAD_UP ) + 1;
	// 얻으려는 레벨이 최대레벨이면 실패
	if( level > PROP_SQUAD->GetMaxLevel() )
		return nullptr;
	return PROP_SQUAD->GetpTable( level );
}

const XPropSquad::xPROP* XHero::GetpPropSquadupNextNext() const
{
	int level = GetLevel( XGAME::xTR_SQUAD_UP ) + 2;
// 	int level = m_levelSquad + 2;
	// 얻으려는 레벨이 최대레벨이면 실패
	if( level > PROP_SQUAD->GetMaxLevel() )
		return nullptr;
	return PROP_SQUAD->GetpTable( level );
}

// int XHero::SerializeLevelupReady( XArchive& ar )
// {
// 	ar << m_aryLevelupReady;
// 	return 1;
// }
// int XHero::DeSerializeLevelupReady( XArchive& ar )
// {
// 	m_aryLevelupReady.Clear();
// 	ar >> m_aryLevelupReady;
// 	return 1;
// }

// int XHero::SerializeProvided( XArchive& ar )
// {
// 	ar << (BYTE)m_aryProvided[ XGAME::xTR_SQUAD_UP ];
// 	ar << (BYTE)m_aryProvided[ XGAME::xTR_SKILL_ACTIVE_UP ];
// 	ar << (BYTE)m_aryProvided[ XGAME::xTR_SKILL_PASSIVE_UP ];
// 	ar << (BYTE)0;
// 	return 1;
// }
// int XHero::DeSerializeProvided( XArchive& ar )
// {
// 	m_aryProvided.Clear(0);
// 	BYTE b0;
// 	ar >> b0;	m_aryProvided[ XGAME::xTR_SQUAD_UP ] = b0;
// 	ar >> b0;	m_aryProvided[ XGAME::xTR_SKILL_ACTIVE_UP ] = b0;
// 	ar >> b0;	m_aryProvided[ XGAME::xTR_SKILL_PASSIVE_UP ] = b0;
// 	ar >> b0;	
// 	return 1;
// }

/**
 @brief 업글템을 영웅에게 제공한다. 
 @param pOutOver 만약 더하고 오버된값이 있으면 여기에 담긴다.
 @return 렙업하면 true를 리턴한다.

*/
// bool XHero::AddProvide( XGAME::xtTrain type, int add, int *pOutOver ) 
// {
// 	if( pOutOver )
// 		*pOutOver = 0;
// 	if( XBREAK( XGAME::IsInvalidTrainType( type ) ) )
// 		return false;
// 	int maxItem;
// 	if( type == XGAME::xTR_SQUAD_UP )
// 	{
// 		auto pPropSquad = GetpPropSquadupNext();
// 		maxItem = 999;
// 		if( XASSERT( pPropSquad ) )
// 			maxItem = pPropSquad->numItem;
// 	} else
// 	if( type == XGAME::xTR_SKILL_ACTIVE_UP ||
// 		type == XGAME::xTR_SKILL_PASSIVE_UP )
// 	{
// 		auto pPropNext = GetpPropSkillupNext( type );
// 		maxItem = 999;
// 		if( XASSERT( pPropNext ) )
// 			maxItem = pPropNext->numItem;
// 	} else
// 	{
// 		XBREAKF(1, "잘못된 호출:typeTrain=%d, add=%d", type, add );
// 		return false;
// 	}
// 	// 일단 더해야할 개수를 더함.
// 	m_aryProvided[ type ] += add;
// 	// 렙업
// 	if( m_aryProvided[ type ] >= maxItem )
// 	{
// 		// 최대치에서 현재개수를 빼서 남은개수를 얻는다.
// 		int numRemain = maxItem - m_aryProvided[ type ];
// 		XBREAK( numRemain > 0 );
// 		// 오버된 개수를 얻는다.
// 		int numOver = m_aryProvided[ type ] - maxItem;
// 		XBREAK( numOver < 0 );
// 		if( pOutOver > 0 )
// 			*pOutOver = numOver;
// 		SetbLevelupReady( type, true );	// 렙업
// 		return true;
// 	}
// 	return false;
// }
// 
// /**
//  @brief 제공해야할 메달이 꽉찼는지
// */
// bool XHero::IsFullMedal()
// {
// 	auto pPropSquad = GetpPropSquadupNext();
// 	if( XASSERT( pPropSquad ) )
// 		return m_aryProvided[XGAME::xTR_SQUAD_UP] >= pPropSquad->numItem;
// 	return false;	
// }
// 
// bool XHero::IsFullScroll( XGAME::xtTrain type )
// {
// 	if( XBREAK( XGAME::IsInvalidSkillTrain(type) ) )
// 		return false;
// 	auto pPropNext = GetpPropSkillupNext( type );
// 	if( XASSERT( pPropNext ) )
// 		return m_aryProvided[ type ] >= pPropNext->numItem;
// 	return false;
// }
// 
// /**
//  @brief 남은 메달수
// */
// int XHero::GetNumRemainMedal( const XPropSquad::xPROP* pPropSquadNext )
// {
// 	if( XBREAK( pPropSquadNext == nullptr ) )
// 		return 0;
// 	return pPropSquadNext->numItem - GetNumProvidedMedal();
// }
// /**
//  @brief pHero,가 현재 부대업글을 위해 필요한 아이템과 수량을 얻는다.
// */
// bool XHero::GetItemLevelUpSquad(ID *pOutID, int *pOutNum)
// {
// 	// 업글 아이템이 충분히 있는가.
// 	auto pPropSquad = GetpPropSquadupNext();
// 	if( pPropSquad == nullptr )
// 		return false;
// 	// 일단 최대 레벨이면 더이상 업글 안됨
// 	if( GetlevelSquad() >= PROP_SQUAD->GetMaxLevel() )
// 		return false;
// 	// 영웅 렙제에 걸리면 더이상 안됨.
// 	if( GetLevel() < pPropSquad->levelLimitByHero )
// 		return false;
// 	int numItem = 0;
// 	// 유닛의 공격타입에 따라 필요한 업글템의 아이디를 얻음.
// 	ID idNeed = XGAME::GetSquadLvupItem(GetType(), pPropSquad->gradeNeed );
// 	XBREAK(idNeed == 0);
// 	//XBREAK( XGAME::IsInvalidGrade(pPropSquad->gradeNeed) );
// // 	idNeed += (pPropSquad->gradeNeed - 1);
// 	if (pOutID)
// 		*pOutID = idNeed;	// UI등에 사용하기 위해 필요한 아이템의 아이디를 넣어줌.
// 	if (pOutNum)
// 		*pOutNum = pPropSquad->numItem;
// 	return true;
// }
// 
// /**
//  @brief 다음 스킬업글을 위해 필요한 아이템과 수량을 얻는다.
// */
// bool XHero::GetItemLevelUpSkill( XGAME::xtTrain type, ID *pOutID, int *pOutNum)
// {
// 	// 업글 아이템이 충분히 있는가.
// 	auto pPropSkillup = GetpPropSkillupNext( type );
// 	// 일단 최대 레벨이면 더이상 업글 안됨
// 	if( GetLevelSkill(type) >= XGAME::GetMaxSkillLevel() )
// 		return false;
// 	// 영웅 렙제에 걸리면 더이상 안됨.
// // 	if( GetLevel() < pPropSkillup->levelLimitByHero )
// // 		return false;
// 	int numItem = 0;
// 	// 유닛의 공격타입에 따라 필요한 업글템의 아이디를 얻음.
// 	ID idNeed = XGAME::GetSkillLvupItem( pPropSkillup->gradeNeed );
// 	XBREAK(idNeed == 0);
// 	//XBREAK( XGAME::IsInvalidGrade(pPropSkill->gradeNeed) );
// 	if (pOutID)
// 		*pOutID = idNeed;	// UI등에 사용하기 위해 필요한 아이템의 아이디를 넣어줌.
// 	if (pOutNum)
// 		*pOutNum = pPropSkillup->numItem;
// 	return true;
// }
// 
// /**
//  @brief 현재 메달이 제공가능한가
// */
// bool XHero::IsAbleProvideSquad()
// {
// 	// 업글 아이템이 충분히 있는가.
// 	auto pPropSquad = GetpPropSquadupNext();
// 	return IsAbleProvideSquad( pPropSquad );
// }
// 
// /**
//  @brief 현재 메달이 제공가능한 상태인가.
//  pHero로만 봤을때 상태임.
// */
// bool XHero::IsAbleProvideSquad( const XPropSquad::xPROP* pPropNext )
// {
// 	// 일단 최대 레벨이면 더이상 제공 안됨
// 	if( GetlevelSquad() >= PROP_SQUAD->GetMaxLevel() )
// 		return false;
// 	// 이미 풀상태면 안됨.
// 	if( m_aryProvided[XGAME::xTR_SQUAD_UP] >= pPropNext->numItem )
// 		return false;
// 	return true;
// }
// 
// /**
//  @brief 현재 메달이 제공가능한가
// */
// bool XHero::IsAbleProvideSkill( XGAME::xtTrain type )
// {
// 	// 업글 아이템이 충분히 있는가.
// 	auto pPropSkill = GetpPropSkillupNext( type );
// 	return IsAbleProvideSkill( pPropSkill, type );
// }
// 
// /**
//  @brief 현재 메달이 제공가능한 상태인가.
// */
// bool XHero::IsAbleProvideSkill( const XPropUpgrade::xPropSkill* pPropNext, XGAME::xtTrain type )
// {
// 	// 스킬이 아예없으면 실패
// 	if( !IsHaveSkill(type) )
// 		return false;
// #if defined(_CLIENT) || defined(_GAME_SERVER)
// 	if( GetSkillDat( type ) == nullptr )
// 		return false;
// #endif // 
// 	// 일단 최대 레벨이면 더이상 업글 안됨
// 	if( GetLevelSkill( type ) >= XGAME::GetMaxSkillLevel() )
// 		return false;
// 	// 영웅 렙제에 걸리면 더이상 안됨.
// 	if( GetLevel() < pPropNext->levelLimitByHero )
// 		return false;
// 	// 이미 풀상태면 안됨.
// 	if( m_aryProvided[type] >= pPropNext->numItem )
// 		return false;
// 	return true;
// }

bool XHero::IsMaxLevel( XGAME::xtTrain type ) const 
{
	return GetLevel( type ) >= GetMaxLevel( type );
}

/**
 @brief 영웅을 승급시킨다.
 에러검사는 외부에서 하고 와야 함.
*/
XGAME::xtError XHero::DoPromotion() 
{
	auto bOk = _IsPromotionForXAccount();
	XBREAK( bOk != XGAME::xE_OK );
// 	if( XBREAK( m_Grade < XGAME::xGD_RARE || m_Grade >= XGAME::xGD_LEGENDARY ) )
// 		return XGAME::xE_ERROR_CRITICAL;
// 	if( XBREAK( !IsMaxLevelLevel() ) )
// 		return XGAME::xE_NOT_ENOUGH_LEVEL;
	m_Grade = ( XGAME::xtGrade )( m_Grade + 1 );
// 	m_aryUpgrade[ XGAME::xTR_LEVEL_UP ].SetLevel( 1 );
// 	m_aryUpgrade[ XGAME::xTR_LEVEL_UP ].SetExp( 0 );
//	m_Level.SetLevel( 1 );
//	m_Level.SetExp(0);
	return XGAME::xE_OK;
}
/**
 @brief 승급이 가능한지
*/
XGAME::xtError XHero::_IsPromotionForXAccount()
{
	if( m_Grade >= XGAME::xGD_LEGENDARY )
		return XGAME::xE_NO_MORE;
	return XGAME::xE_OK;
}

_tstring XHero::GetsidSkill( XGAME::xtIdxSkilltype idxType ) 
{
	if( idxType == XGAME::xPASSIVE )
		return GetpProp()->strPassive;
	if( m_Grade < XGAME::xGD_EPIC )
		return _tstring();
	return GetpProp()->strActive;
}

/**
 @brief 계정레벨에 의해 더이상 오르지 못하는 레벨이 몇인지
*/
int XHero::GetLvLimitByAccLv( int lvAcc, XGAME::xtTrain type )
{
	switch( type ) {
	case XGAME::xTR_LEVEL_UP:
		return lvAcc;	// 영웅레벨은 계정레벨이 한계다.(영웅레벨이 lvAcc랑 같아지면 더이상 경험치가 들어오지 않는다.
	case XGAME::xTR_SQUAD_UP:
		return PROP_SQUAD->GetLevelSquadMaxByHero( GetLevelHero() );
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP:
		return XPropUpgrade::sGet()->GetLevelSkillMaxByHero( GetLevelHero() );
	default:
		XBREAK(1);
		break;
	}
	return 0;
}
/**
 @brief 영웅의 unit특성중 idNode특성의 구조체를 꺼낸다.
*/
const XGAME::xAbil XHero::GetAbilNode( XGAME::xtUnit unit, ID idNode ) 
{
	do {
		if( !unit )
			break;
		if( idNode == 0 )
			break;
		auto& mapAbil = m_aryUnitsAbil[ unit ];
		XBREAK( unit <= XGAME::xUNIT_NONE || unit >= XGAME::xUNIT_MAX );
		auto itor = mapAbil.find( idNode );
		if( itor != mapAbil.end() )
			return (*itor).second;	// 복사로 넘기게 바뀜
	} while (0);
	return XGAME::xAbil();
}
/**
 @brief unit트리의 idNode특성의 잠금을 해제한다.
 @return 잠금해제한 노드의 레퍼런스를 리턴한다.
*/
bool XHero::SetUnlockAbil( XGAME::xtUnit unit, ID idNode )
{
	auto& mapAbil = m_aryUnitsAbil[ unit ];
	XBREAK( unit <= XGAME::xUNIT_NONE || unit >= XGAME::xUNIT_MAX );
	XBREAK( idNode == 0 );
	auto itor = mapAbil.find( idNode );
	if( XASSERT(itor == mapAbil.end()) ) {
		// 못찾으면 아직 잠겨있는거.
		XGAME::xAbil abil;
		abil.SetUnlock();
		mapAbil[ idNode ] = abil;
		return true;
	}
	// 찾아지면 이미 잠금해제되어있는것.
	XBREAK(1);
	return false;
}

void XHero::SetAbilPoint( XGAME::xtUnit unit, ID idNode, int point )
{
	auto& mapAbil = m_aryUnitsAbil[ unit ];
	XBREAK( unit <= XGAME::xUNIT_NONE || unit >= XGAME::xUNIT_MAX );
	XBREAK( idNode == 0 );
	auto itor = mapAbil.find( idNode );
	if( itor == mapAbil.end() ) {
		// 못찾으면 아직 잠겨있는거.
		XGAME::xAbil abil;
		abil.SetUnlock();
		abil.point = point;
		mapAbil[ idNode ] = abil;
		return;
	}
	auto& abil = itor->second;
	abil.point = point;
}

void XHero::SetAbilPoint( const _tstring& idsAbil, int point )
{
	auto pAbil = XPropTech::sGet()->GetpNode( idsAbil );
	if( pAbil ) {
		SetAbilPoint( pAbil->unit, pAbil->idNode, point );
	}
}

#ifdef _GAME_SERVER
// xXPropTech, xAbil
/**
@brief pNode의 특성포인트를 1올린다.
*/
int XHero::AddAbilPoint( XGAME::xtUnit unit, ID idAbil )
{
	auto& mapAbil = m_aryUnitsAbil[ unit ];
	XBREAK( unit <= XGAME::xUNIT_NONE || unit >= XGAME::xUNIT_MAX );
	XBREAK( idAbil == 0 );
	auto itor = mapAbil.find( idAbil );
	if( itor == mapAbil.end() ) {
		// 못찾았으면 아직 잠겨있는것.
		XGAME::xAbil abil;
		abil.SetUnlock();
		mapAbil[ idAbil ] = abil;
		return AddAbilPoint( unit, idAbil );
	}
	// 찾으면 포인트 증가시킴.
	auto& abil = itor->second;
	if( abil.point < 5 )
		++abil.point;
	return abil.point;
}
#endif // _GAME_SERVER

#if defined(_CLIENT) || defined(_GAME_SERVER)
/**
@brief 특성에 의한 스탯 보정치를 구한다.
*/
float XHero::GetAdjByTech(XGAME::xtUnit unt, XGAME::xtParameter adjParam, float baseStat)
{
	float adjRatio = 1.f;
	float adjAdd = 0;
	XBREAK(m_aryUnitsAbil.size() == 0);
	XArrayLinearN<XPropTech::xNodeAbil*, 256> ary;
	//
	for (int i = 1; i < XGAME::xUNIT_MAX; ++i) {
		auto unit = (XGAME::xtUnit) i;
		auto& mapAbil = m_aryUnitsAbil[unit];
//		for (auto itor = mapAbil.begin(); itor != mapAbil.end(); ++itor) {
		for( auto itor : mapAbil ) {
			ID idNode = itor.first;
			XGAME::xAbil& abil = itor.second;
			if (abil.point > 0) {
				auto pProp = XPropTech::sGet()->GetpNode(unit, idNode);
				if (XBREAK(pProp == nullptr))
					return 0;
				auto pSkillDat = SKILL_MNG->FindByIdentifier(pProp->strSkill);
				if (XBREAK(pSkillDat == nullptr))
					return 0;
				for (auto pEffect : pSkillDat->GetlistEffects()) {
					if (adjParam == pEffect->invokeParameter) {
						XBREAK(pEffect->invokeAbilityMin[abil.point] == 0);
						if (pEffect->valtypeInvokeAbility == XSKILL::xPERCENT)
							adjRatio += pEffect->invokeAbilityMin[abil.point];
						else
							adjAdd += pEffect->invokeAbilityMin[abil.point];
					}
				}
			}
		}
	}
	float result = baseStat + baseStat * adjRatio;
	result += adjAdd;
	return result;
}
void XHero::GetSquadStatWithTech( int levelSquad, xSquadStat *pOut )
{
	pOut->meleePower = GetSquadAttackMeleePower(levelSquad);
	pOut->rangePower = GetSquadAttackRangePower(levelSquad);
	pOut->def = GetSquadDefensePower(levelSquad);
	pOut->hp = (int)GetSquadMaxHp(levelSquad);
	pOut->speedAtk = GetSquadSpeedAtk(levelSquad);
	pOut->speedMoveForMeter = GetSquadSpeedMove(levelSquad);
}

float XHero::GetSquadAttackMeleePower( int levelSquad)
{
	xSquadStat statCurr;
	GetSquadStat(levelSquad, &statCurr);
	// 특성보정치를 곱한다.
	float adjVal = GetAdjByTech(m_Unit, XGAME::xADJ_ATTACK, statCurr.meleePower);
	return adjVal;
}

float XHero::GetSquadAttackRangePower( int levelSquad)
{
	xSquadStat statCurr;
	GetSquadStat(levelSquad, &statCurr);
	// 특성보정치를 곱한다.
	float adjVal = GetAdjByTech(m_Unit, XGAME::xADJ_ATTACK, statCurr.rangePower);
	return adjVal;
}

float XHero::GetSquadDefensePower( int levelSquad)
{
	xSquadStat statCurr;
	GetSquadStat(levelSquad, &statCurr);
	// 특성보정치를 곱한다.
	float adjVal = GetAdjByTech(m_Unit, XGAME::xADJ_DEFENSE, statCurr.def);
	return adjVal;
}

float XHero::GetSquadMaxHp( int levelSquad)
{
	xSquadStat statCurr;
	GetSquadStat(levelSquad, &statCurr);
	// 특성보정치를 곱한다.
	float adjVal = GetAdjByTech(m_Unit, XGAME::xADJ_MAX_HP, (float)statCurr.hp);
	return adjVal;
}

float XHero::GetSquadSpeedAtk( int levelSquad)
{
	xSquadStat statCurr;
	GetSquadStat(levelSquad, &statCurr);
	// 특성보정치를 곱한다.
	float adjVal 
		= GetAdjByTech(m_Unit, XGAME::xADJ_ATTACK_SPEED, statCurr.speedAtk);
	return adjVal;
}

float XHero::GetSquadSpeedMove( int levelSquad)
{
	xSquadStat statCurr;
	GetSquadStat(levelSquad, &statCurr);
	// 특성보정치를 곱한다.
	float adjVal 
		= GetAdjByTech(m_Unit, XGAME::xADJ_MOVE_SPEED, statCurr.speedMoveForMeter);
	return adjVal;
}

int XHero::GetLevelAbil(XGAME::xtUnit unit, LPCTSTR sid)
{
	auto pNode = XPropTech::sGet()->GetpNodeBySkill(unit, sid);
	if (XASSERT(pNode)) {
		auto abil = GetAbilNode(unit, pNode->idNode);
		return abil.point;
	}
	return 0;
}
/**
 @brief unit의 특성트리에서 잠금해제한 특성이 몇개가 있는지 센다.
*/
int XHero::GetNumUnlockAbil( /*XGAME::xtUnit unit*/ )
{
	int num = 0;
	XBREAK( m_aryUnitsAbil.size() == 0 );
	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
		auto unit = (XGAME::xtUnit)i;
		auto& mapAbil = m_aryUnitsAbil[ unit ];
		XBREAK( mapAbil.size() > 0 && GetTypeAtk() != XGAME::GetAtkType(unit) );
		num += mapAbil.size();
	}
	return num;
// 	return mapAbil.size();
// 	for( auto& itor : mapAbil ) {
// 		ID idNode = itor.first;
// 		auto& abil = itor.second;
// 		if( abil.point >= 0 )
// 			++num;
// 	}
// 	return num;
}
/**
 @brief unit의 특성트리에서 현재 잠금해제 비용을 얻는다.
*/
int XHero::GetGoldUnlockAbilCurr( /*XGAME::xtUnit unit*/ )
{
	int numUnlock = GetNumUnlockAbil( /*unit*/ );
	return XPropTech::sGet()->GetGoldUnlockWithNum( numUnlock );
}

#endif // defined(_CLIENT) || defined(_GAME_SERVER)

/**
@brief 특성찍은게 총 몇포인트인지 얻는다.
*/
int XHero::GetNumSetAbilPoint()
{
	int num = 0;
	XBREAK( m_aryUnitsAbil.size() == 0 );
	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
		XGAME::xtUnit unit = ( XGAME::xtUnit ) i;
		num += GetNumSetAbilPointByUnit( unit );
	}
	return num;
}
/**
@brief 해당 유닛의 특성포인트 찍은게 총 몇포인트인지 얻는다.
*/
int XHero::GetNumSetAbilPointByUnit( XGAME::xtUnit unit )
{
	int num = 0;
	XBREAK( m_aryUnitsAbil.size() == 0 );
	auto& mapAbil = m_aryUnitsAbil[ unit ];
	for( auto itor : mapAbil ) {
		ID idNode = itor.first;
		auto& abil = itor.second;
		if( abil.point > 0 )
			num += abil.point;
	}
	if( num > 0 ) {
		// 다른 병과유닛인데 포인트가 있으면 안됨.
#ifdef _DUMMY_GENERATOR
		if( XBREAK(GetTypeAtk() != XGAME::GetAtkType(unit)) ) {
			mapAbil.clear();
			num = 0;
		}
#else
		if( XBREAK( GetTypeAtk() != XGAME::GetAtkType(unit) ) ) {
			mapAbil.clear();
			num = 0;
		}
#endif
	}
	return num;
}

/**
 @brief unit의 idAbil특성을 몇포인트나 찍었는지.
*/
int XHero::GetNumAbilPoint( XGAME::xtUnit unit, ID idAbil )
{
	auto abil = GetAbilNode( unit, idAbil );
	return abil.point;
}

#if defined(_GAME_SERVER) && defined(_DEV)
/**
@brief pRoot에 있는 특성들을 랜덤으로 찍는다.
*/
bool XHero::RecursiveAbilPointRandom(XGAME::xtUnit unit
																	, XPropTech::xNodeAbil *pRoot)
{
	if (pRoot->listChild.size() == 0)
		return false;
	// 루트가 가진 차일드중에 잠금해제가 가능한 노드들만 뽑는다.
	XList4<XPropTech::xNodeAbil*> listTops;
	for (auto pNode : pRoot->listChild) {
		// 오픈할수 있는 레벨이 된것만.
		if (GetLevel() >= pNode->GetLvOpanable())
			listTops.Add(pNode);
	}
	if (listTops.size() == 0)
		return false;
	int idxRandom = xRandom(listTops.size());
	auto ppNode = listTops.GetpByIndex(idxRandom);
	if (XBREAK(ppNode == nullptr))
		return false;
	auto pNode = *(ppNode);
	const auto abil = GetAbilNode(unit, pNode->idNode);
	// 일단 이 노드가 잠겨있다면 이것부터 찍는다.
	if( abil.IsLock() ) {
		SetUnlockAbil( unit, pNode->idNode );
		AddAbilPoint( unit, pNode->idNode );
		return true;
	}
// 		pAbil->SetUnlock();
	// 포인트가 하나도 없으면 무조건 하나는 찍음.
	if( abil.point == 0 ) {
		AddAbilPoint( unit, pNode->idNode );
		return true;
	}
	// 포인트가 다 찍혔으면 하위노드로 넘어가야 한다.
	if( abil.point >= 5 ) {
		bool bOk = RecursiveAbilPointRandom( unit, pNode );
		if( bOk == false ) {
			// 더이상 차일드가 없거나
			// 더이상 오픈할수 있는 노드가 없거나
			// 기타 다른 이유로 포인트를 못찍음.
			return false;	
		} else {
			// 포인트를 찍는데 성공함
			return true;
		}
	} else {
		// 이노드에서 찍을건지 다음노드로 넘어갈건지 정해야 한다.
		// 최소 3포인트는 무조건 찍어야 다음단계로 넘어가도록.
		if( abil.point < 3 ) {
			//++pAbil->point;
			AddAbilPoint( unit, pNode->idNode );
			return true;
		}
		// 확률로 이노드를 찍을건지 다음 노드를 찍을건지 결정.
		if( XE::IsTakeChance(0.5f) ) {
			XBREAK( abil.point >= 5 );
			//++pAbil->point;
			AddAbilPoint( unit, pNode->idNode );
			return true;
		} else {
			// 다음 노드로 판정 넘김.
			bool bOk = RecursiveAbilPointRandom( unit, pNode );
			if( bOk == false ) {
				// 더이상 차일드가 없거나
				// 더이상 오픈할수 있는 노드가 없거나
				// 기타 이유로 포인트를 못찍음.
				// 그냥 이 노드를 찍고 끝냄.
				XBREAK( abil.point >= 5 );
				//++pAbil->point;
				AddAbilPoint( unit, pNode->idNode );
			}
			return true;
		}
	}
	XBREAK(1);
	return false;
}
/**
@brief 더미 계정을 위한 특성포인트를 설정한다.
*/
void XHero::GenerateAbilityForDummy( XGAME::xtUnit unit, int lvAcc )
{
	InitAbilMap();
	XBREAK(m_aryUnitsAbil.size() == 0);
	// 레벨에 따라 각 유닛별 특성포인트를 계산한다.
	// 각 유닛의 최상단 더미루트를 얻는다.
	auto pRoot = XPropTech::sGet()->GetpNodeRoot(unit);
	auto& tbl = XGC->GetDummyTbl( lvAcc );
	int pointTech = tbl.m_numAbilPerUnit;
	int cntMax = pointTech * 2;	
	// 포인트는 남았는데 더이상 찍을수 없는 상태라면 무한루프에 빠질수 있기때문에.
	while( cntMax-- ) {
		bool bOk = RecursiveAbilPointRandom( unit, pRoot );
		if( bOk ) {
			// 포인트를 찍는데 성공함.
				if( --pointTech <= 0 )
					// 포인트 다 썼으면 끝냄
					break;
		} else {
			// 포인트를 찍는데 실패함.
			// 확률로 선택한 가지가 모두 full상태였을때 이런경우가 생김.
		}
	}
	if( cntMax == 0 ) {
		// 우연히 못찍는 가지만 확률이 나올경우 이렇게 될 수 있음.
		CONSOLE("경고:특성포인트 찍는데 실패함.:%s", XGAME::GetStrUnit(unit) );
		// 이런경우가 자주 나오면 가지선택전 하위가지가 모두 풀상태인지 보고 골라야 한다.
	}
}

#endif // defined(_GAME_SERVER) && defined(_DEV)

/**
@brief
*/
BOOL XHero::IsEmptyAbilMap()
{
	if( m_aryUnitsAbil.size() == 0 )
		return TRUE;
	return m_aryUnitsAbil[ 1 ].size() == 0;
}

void XHero::InitAbilMap()
{
#if defined(_DB_SERVER) || defined(_LOGIN_SERVER)
	InitAryAbil();
#else
#pragma message("실제 특성찍은것만 가지고 있도록 메모리 최적화 할것.")
	// 다른서버에서 serialize로 만들어 보내기때문에 0이면 안됨.
	XBREAK(m_aryUnitsAbil.size() == 0);
#endif
}
/**
@brief 특성맵 시리얼라이즈
*/
int XHero::SerializeAbil(XArchive& ar)
{
#if defined(_CLIENT) || defined(_GAME_SERVER)
	if (XBREAK(m_aryUnitsAbil.size() == 0))
		return 0;
#else
	if (m_aryUnitsAbil.size() == 0)
		InitAbilMap();
#endif
// 	ar << VER_ABILITY_SERIALIZE;
	for (int i = 1; i < XGAME::xUNIT_MAX; ++i) {
		auto unit = (XGAME::xtUnit) i;
		auto& mapAbil = m_aryUnitsAbil[unit];
		// unit특성에 포인트가 있고 이 영웅의 병과와 다르면 안됨.
		XBREAK( mapAbil.size() > 0 && GetTypeAtk() != XGAME::GetAtkType(unit) );
		ar << (int)mapAbil.size();
		for ( auto itor : mapAbil) {
			ID idNode = itor.first;
			auto& abil = itor.second;
			XBREAK(idNode >= 0xffff);
			ar << (char)abil.point;
			ar << (BYTE)0;
			ar << (WORD)idNode;
		}
	}
	return 1;
}

int XHero::DeserializeAbil(XArchive& ar, int verHero )
{
// 	int ver = 0;
// 	ar >> ver;
	for (int i = 1; i < XGAME::xUNIT_MAX; ++i) {
		auto unit = (XGAME::xtUnit) i;
		auto& mapAbil = m_aryUnitsAbil[unit];
		int size = 0;
		ar >> size;
		for (int i = 0; i < size; ++i) {
			char c0;
			WORD w0;
			XGAME::xAbil abil;
			ID idNode;
			ar >> c0;		abil.point = c0;
			XBREAK(abil.point > 5);
			if (abil.point == 255) {
				abil.point = -1;
				XBREAK(abil.point != -1);
			}
			ar >> c0;
			ar >> w0;	idNode = w0;
			mapAbil[idNode] = abil;
		}
		// 영웅병과와 다른 유닛에 특성정보가 있으면 지움.
		if( mapAbil.size() > 0 )
			if( GetTypeAtk() != XGAME::GetAtkType(unit) ) {
				mapAbil.clear();
		}
	}
	return 1;
}
/**
 @brief this영웅의 현재 unit트리를 연구하기 위한 비용을 얻는다.
*/
#if defined(_CLIENT) || defined(_GAME_SERVER)
XPropTech::xtResearch& XHero::GetCostAbilCurr()
{
	int numPoint = GetNumSetAbilPoint() + 1;
	return XPropTech::sGet()->GetResearchWithNum( numPoint );
}
#endif // defined(_CLIENT) || defined(_GAME_SERVER)
/**
 @brief this영웅이 unit을 연구가능한가.
*/
bool XHero::IsResearbleUnit( XGAME::xtUnit unit )
{
	return GetpProp()->typeAtk == XGAME::GetAtkType( unit );
}

/**
 @brief 영웅의 특성트리를 초기화하는 비용을 계산함.
*/
int XHero::GetCostInitAbil()
{
	return m_numInitAbil * 50000;
}

/**
 @brief 영웅의 특성트리를 초기화한다.
*/
#if defined(_CLIENT) || defined(_GAME_SERVER)
bool XHero::DoInitAbil()
{
	bool bOk = true;
	// 이 영웅이 찍은 특성포인트 전체개수
	int numPoint = GetNumSetAbilPoint();
	// 이 영웅이 언락한 횟수.
	int numUnlock = GetNumUnlockAbil();
	do {
		// 더이상 초기화 못시킴. 초기화 시키려면 이 카운트자체를 날리는 템을 사야함.
		if( m_numInitAbil >= 255 ) {
			bOk = false;
			break;
		}
		// 특성 포인트 어레이 초기화
		InitAryAbil();
		m_numInitAbil++;
	} while (0);
	//
	if( bOk ) {
		// 되돌려 받은 포인트를 현재값에 더한다.(포인트를 선물등으로 받을수도 있기때문)
		m_numRemainAbilPoint += numPoint;
		m_numRemainAbilUnlock += numUnlock;
	}
	return bOk;
}
#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

void XHero::AddRemainAbilPoint( int add ) 
{
	if( XBREAK( m_numRemainAbilPoint <= 0 ) )
		return;
	if( XBREAK( add > 1 || add == 0 ) )
		return;
	m_numRemainAbilPoint += add;
}

void XHero::AddRemainAbilUnlock( int add )
{
	if( XBREAK( m_numRemainAbilUnlock <= 0 ) )
		return;
	if( XBREAK( add > 1 || add == 0 ) )
		return;
	m_numRemainAbilUnlock += add;
}

void XHero::AddcntInitAbil( int add )
{
	if( m_numInitAbil >= 255 )	// 더이상 초기화 못시킴. 초기화 시키려면 이 카운트자체를 날리는 템을 사야함.
		return;
	if( XBREAK( add > 1 || add == 0 ) )
		return;
	m_numInitAbil += add;
}

