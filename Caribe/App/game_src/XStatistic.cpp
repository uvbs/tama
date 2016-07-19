#include "stdafx.h"
#include "XStatistic.h"
#include "XHero.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
ID XStatistic::xSquad::getid() const {
	return pHero->GetsnHero();
}

XStatistic::XStatistic()
{
	Init();
}

void XStatistic::Destroy()
{
	XLIST4_DESTROY( m_listSquads );
}

void XStatistic::AddSquad( XHero *pHero )
{
	XBREAK( pHero == nullptr );
	xSquad *pSquad = new xSquad;
	pSquad->pHero = pHero;
	m_listSquads.Add( pSquad );
	XBREAK( m_listSquads.size() > 15 );
}

/**
 @brief 데미지딜링양을 누적시킨다.
*/
float XStatistic::AddDamageDeal( ID snHero, float damage )
{
	auto pSquad = m_listSquads.FindpByID( snHero );
	if( XASSERT(pSquad) )
		return (*pSquad)->damageDeal += damage;
	return 0;
}

float XStatistic::AddDamageAttacked( ID snHero, float damage )
{
	auto pSquad = m_listSquads.FindpByID( snHero );
	if( XASSERT( pSquad ) )
		return ( *pSquad )->damageAttacked += damage;
	return 0;
}

float XStatistic::AddDamageDealByCritical( ID snHero, float damage )
{
	auto pSquad = m_listSquads.FindpByID( snHero );
	if( XASSERT( pSquad ) )
		return ( *pSquad )->damageDealByCritical += damage;
	return 0;
}
float XStatistic::AddDamageByEvade( ID snHero, float damage )
{
	auto pSquad = m_listSquads.FindpByID( snHero );
	if( XASSERT( pSquad ) )
		return ( *pSquad )->damageByEvade += damage;
	return 0;
}
float XStatistic::AddHeal( ID snHero, float heal )
{
	auto pSquad = m_listSquads.FindpByID( snHero );
	if( XASSERT( pSquad ) )
		return ( *pSquad )->heal += heal;
	return 0;
}
float XStatistic::AddTreated( ID snHero, float heal )
{
	auto pSquad = m_listSquads.FindpByID( snHero );
	if( XASSERT( pSquad ) )
		return ( *pSquad )->treated += heal;
	return 0;
}
float XStatistic::AddDamageDealBySkill( ID snHero, float damage )
{
	auto pSquad = m_listSquads.FindpByID( snHero );
	if( XASSERT( pSquad ) )
		return ( *pSquad )->damageDealBySkill += damage;
	return 0;
}


/**
 @brief 가장 큰 데미지 딜링값을 얻는다.
*/
float XStatistic::GetMaxDamageDeal()
{
	float max = 0;
	for( auto pSquad : m_listSquads )
	{
		if( pSquad->damageDeal > max )
			max = pSquad->damageDeal;
	}
	return max;
}

float XStatistic::GetMaxDamageAttacked()
{
	float max = 0;
	for( auto pSquad : m_listSquads )
	{
		if( pSquad->damageAttacked > max )
			max = pSquad->damageAttacked;
	}
	return max;
}

float XStatistic::GetMaxDamageDealByCritical()
{
	float max = 0;
	for( auto pSquad : m_listSquads )
	{
		if( pSquad->damageDealByCritical > max )
			max = pSquad->damageDealByCritical;
	}
	return max;
}
float XStatistic::GetMaxDamageByEvade()
{
	float max = 0;
	for( auto pSquad : m_listSquads )
	{
		if( pSquad->damageByEvade > max )
			max = pSquad->damageByEvade;
	}
	return max;
}
float XStatistic::GetMaxHeal()
{
	float max = 0;
	for( auto pSquad : m_listSquads )
	{
		if( pSquad->heal > max )
			max = pSquad->heal;
	}
	return max;
}
float XStatistic::GetMaxTreated()
{
	float max = 0;
	for( auto pSquad : m_listSquads )
	{
		if( pSquad->treated > max )
			max = pSquad->treated;
	}
	return max;
}
float XStatistic::GetMaxDamageDealBySkill()
{
	float max = 0;
	for( auto pSquad : m_listSquads )
	{
		if( pSquad->damageDealBySkill > max )
			max = pSquad->damageDealBySkill;
	}
	return max;
}
