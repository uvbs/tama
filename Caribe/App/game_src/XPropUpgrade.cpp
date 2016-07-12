#include "stdafx.h"
#include "XPropUpgrade.h"
#include "XExpTableHero.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

std::shared_ptr<XPropUpgrade> XPropUpgrade::s_spSingleton;
std::shared_ptr<XPropUpgrade> XPropUpgrade::sGet() 
{
	if( s_spSingleton == nullptr )
		s_spSingleton = std::shared_ptr<XPropUpgrade>( new XPropUpgrade );
	return s_spSingleton;
}

XPropUpgrade::XPropUpgrade() 
	: /*m_aryTableLevel( EXP_TABLE_HERO->GetMaxLevel() + 1 ),*/
	m_aryTableSkill( XGAME::GetMaxSkillLevel() + 1 )
{ 
	Init(); 
//	m_Mode = xMODE_LEVEL;
//  	m_aryTableLevel.resize( EXP_TABLE_HERO->GetMaxLevel() + 1 );
// 	m_aryTableSkill.resize( XGAME::GetMaxSkillLevel() + 1 );
}

void XPropUpgrade::Destroy() 
{
}

/**
 @brief 반복읽기를 하기전에 불려짐
*/
void XPropUpgrade::OnDidBeforeReadProp( CToken& token )
{
	m_goldPerExp = token.GetNumberF();
	m_secPerExp = token.GetNumberF();
}

BOOL XPropUpgrade::ReadProp( CToken& token, DWORD dwParam )
{
// 	if( m_Mode == xMODE_LEVEL )
// 	{
// 		auto bRet = ReadPropUpgradeLevel( token );
// 		if( !bRet )
// 			return bRet;
// 	} else
// 	if( m_Mode == xMODE_SKILL ) {
		auto bRet = ReadPropUpgradeSkill( token );
		if( !bRet )
			return bRet;
//  	}
	return TRUE;
}

/**
 @brief 영웅 렙업 프로퍼티 읽기
*/
// BOOL XPropUpgrade::ReadPropUpgradeLevel( CToken& token )
// {
// 	int level = TokenGetNumber( token );
// 	if( level != m_lvLast + 1 )
// 	{
// 		XERROR( "%s: 레벨이 순차적이지 않습니다. %d", GetstrFilename(), level );
// 		return FALSE;
// 	}
// 	if( token.IsEof() )
// 		return FALSE;		// false를 리턴해야 루프를 빠져나옴
// 	
//  	m_aryTableLevel[ level ].secTrain = (float)token.GetNumber();
// 	for( int i = 1; i < XGAME::xCL_MAX; ++i )
// 	{
// 		int wood = token.GetNumber();
// 		int iron = token.GetNumber();
// 		AddLevelTbl( level, wood, iron, (XGAME::xtClan)i );
// 	}
// 	if( level == EXP_TABLE_HERO->GetMaxLevel() )
// 	{
// 		m_Mode = xMODE_SKILL;
// 		m_lvLast = 0;
// 	} else
// 		m_lvLast = level;
// 	return TRUE;
// }

/**
 @brief 스킬렙업 프로퍼티 읽기
*/
BOOL XPropUpgrade::ReadPropUpgradeSkill( CToken& token )
{
	int level = TokenGetNumber( token );
	if( level != m_lvLast + 1 ) {
		XERROR( "%s: 레벨이 순차적이지 않습니다. %d", GetstrFilename(), level );
		return FALSE;
	}
	if( token.IsEof() )
		return FALSE;		// false를 리턴해야 루프를 빠져나옴

	xPropSkill prop;
	prop.levelLimitByHero = token.GetNumber();
	prop.expMax = token.GetNumber();
// 	prop.gradeNeed = (XGAME::xtGrade)TokenGetNumber( token );
// 	prop.numItem = token.GetNumber();
// 	prop.needTrain.secTrainPerItem = token.GetNumber();
// 	prop.needTrain.aryNum[0] = token.GetNumber();
	m_aryTableSkill[ level ] = prop;

	if( level == XGAME::GetMaxSkillLevel() ) {
//		m_Mode = xMODE_NONE;
		m_lvLast = 0;
		return FALSE;
	} else
		m_lvLast = level;

	return TRUE;
}

// void XPropUpgrade::AddLevelTbl( int level, int wood, int iron, XGAME::xtClan clan )
// {
// 	xNeed res;
// 
// 	auto& prop = m_aryTableLevel[ level ];
// 	if( prop.aryClans.size() == 0 )	// xCL_NONE용
// 		prop.aryClans.Add( res );
// 
// 	res.aryNum.Add( wood );
// 	res.aryNum.Add( iron );
// 	prop.aryClans.Add( res );
// }

// void XPropUpgrade::AddSkillTbl( int level, xPropSkill& prop )
// {
// 	m_aryTableSkill[ level ] = prop;
// }
// 
// const XPropUpgrade::xPropLevel* XPropUpgrade::GetpTableForLevelup( int level )
// {
// 	XBREAK( level <= 0 );
// 	return &m_aryTableLevel[ level ];
// }

const XPropUpgrade::xPropSkill* XPropUpgrade::GetpTableForSkillup( int level )
{
	XBREAK( level <= 0 );
	return &m_aryTableSkill[ level ];
}

const XPropUpgrade::xPropSkill* XPropUpgrade::GetpTableForSkillupNext( int levelCurr ) const
{
	if( levelCurr +1>= XGAME::MAX_SKILL_LEVEL )
		return nullptr;
	return &m_aryTableSkill[ levelCurr + 1 ];
}


/**
 @brief lvHero가 가질수 있는 최대 스킬레벨
*/
int XPropUpgrade::GetLevelSkillMaxByHero( int lvHero )
{
	int level = 0;
	for( auto& prop : m_aryTableSkill )	{
		if( lvHero < prop.levelLimitByHero )
			return level-1;
		++level;
	}
	// 나머지레벨은 레벨최대값
	return m_aryTableSkill.size() - 1;
}

void XPropUpgrade::Serialize( XArchive& ar ) const 
{
	ar << VER_PROP_UPGRADE;
	ar << m_lvLast;
	ar << m_goldPerExp;
	ar << m_secPerExp;
	ar << m_aryTableSkill;
}
void XPropUpgrade::DeSerialize( XArchive& ar, int )
{
	m_aryTableSkill.clear();
	int ver;
	ar >> ver;
	ar >> m_lvLast;
	ar >> m_goldPerExp;
	ar >> m_secPerExp;
	ar >> m_aryTableSkill;
}

