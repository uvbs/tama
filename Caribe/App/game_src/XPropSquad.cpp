#include "stdafx.h"
#include "XPropSquad.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XPropSquad *PROP_SQUAD = NULL;

XPropSquad::XPropSquad() 
{ 
	Init(); 
	xPROP prop;
	m_aryProp.Add( prop );		// index 0에 빈거 하나 넣어야함.
}

void XPropSquad::Destroy() 
{
}

/**
 @brief 반복읽기를 하기전에 불려짐
*/
void XPropSquad::OnDidBeforeReadProp( CToken& token )
{
	m_goldPerExp = token.GetNumberF();
	m_secPerExp = token.GetNumberF();
}

BOOL XPropSquad::ReadProp( CToken& token, DWORD dwParam )
{
	xPROP prop;
	const int level = token.GetNumber();
	if( level == TOKEN_EOF )
		return FALSE;		// false를 리턴해야 루프를 빠져나옴
	if( level != m_lvLast + 1 ) {
		XERROR( "%s: 레벨이 순차적이지 않습니다. %d", GetstrFilename(), level );
		return FALSE;
	}
	XBREAK( level > XGAME::MAX_SQUAD_LEVEL );
	prop.level = level;
	prop.levelLimitByHero = token.GetNumber();
	prop.maxSmall = token.GetNumber();
	prop.maxMiddle = token.GetNumber();
	prop.aryMultiplyRatioMiddle[ 0 ] = token.GetNumberF();
	prop.aryMultiplyRatioMiddle[ 1 ] = token.GetNumberF();
	prop.aryMultiplyRatioMiddle[ 2 ] = token.GetNumberF();
	prop.aryMultiplyRatioMiddle[ 3 ] = token.GetNumberF();
	float totalMul = 0.f;
	for( auto mul : prop.aryMultiplyRatioMiddle ) {
		totalMul += mul;
	}
	prop.avgMultiplyByMiddle = totalMul / prop.maxMiddle;		// 평균치로 갖고 있음.
	token.GetNumberF();		// 합산능력치. 게임에선 사용하지 않음.
	prop.multiplyRatioBig = token.GetNumberF();
	prop.expMax = token.GetNumber();
	// 추가
	Add( prop );
	m_lvLast = level;

	return TRUE;
}

void XPropSquad::Add( const xPROP& prop )
{
	m_aryProp.Add( prop );
}

/**
 @brief lvHero가 가질수 있는 최대 부대레벨
*/
int XPropSquad::GetLevelSquadMaxByHero( int lvHero )
{
	int level = 0;
	XARRAYLINEARN_LOOP_AUTO( m_aryProp, &prop ) {
		if( lvHero < prop.levelLimitByHero )
			return level-1;
		++level;
	} END_LOOP;
	// 나머지레벨은 부대최대값
	return m_aryProp.size() - 1;
}

/**
 @brief 부대레벨에 따른 
 @param idxUnit 부대내 유닛의 인덱스, 중형유닛이라면 0~3까지 있다.
*/
float XPropSquad::GetMulByLvSquad( XGAME::xtSize size, int idxUnit, int lvSquad )
{
	auto& propSquad = GetTable( lvSquad );
	float mulByLvSquad = 1.f;
	if( size == XGAME::xSIZE_MIDDLE ) {
		XBREAK( idxUnit >= XGAME::MAX_UNIT_MIDDLE );
		mulByLvSquad = propSquad.aryMultiplyRatioMiddle[ idxUnit ];
		if( mulByLvSquad <= 0.f )		// 싱글테스트모드에선 부대레벨이 1인데 유닛이 4마리가 나오게 되는수가 있어서 그런경우 강제로 1로 맞춰지게 함.
			mulByLvSquad = 1.f;
// 		pUnit->SetmultipleAbility( multiplyAbil );
	} else 
	if( size == XGAME::xSIZE_BIG ) {
		XBREAK( idxUnit >= XGAME::MAX_UNIT_BIG );
		mulByLvSquad = propSquad.multiplyRatioBig;
//		pUnit->SetmultipleAbility( propSquad.multiplyRatioBig );
// 		XBREAK( pUnit->GetmultipleAbility() == 0.f );
	} else
	if( size == XGAME::xSIZE_SMALL ) {
		return 1.f;
	} else {
		XBREAK(1);
	}
	XBREAK( mulByLvSquad <= 0.f );
	return mulByLvSquad;
}

float XPropSquad::GetAvgMulByLvSquad( XGAME::xtSize size, int lvSquad )
{
	auto& propSquad = GetTable( lvSquad );
	float mulByLvSquad = 1.f;
	if( size == XGAME::xSIZE_MIDDLE ) {
		mulByLvSquad = propSquad.avgMultiplyByMiddle;
		if( mulByLvSquad <= 0.f )		// 싱글테스트모드에선 부대레벨이 1인데 유닛이 4마리가 나오게 되는수가 있어서 그런경우 강제로 1로 맞춰지게 함.
			mulByLvSquad = 1.f;
	} else 
	if( size == XGAME::xSIZE_BIG ) {
		mulByLvSquad = propSquad.multiplyRatioBig;
	} else
	if( size == XGAME::xSIZE_SMALL ) {
		return 1.f;
	} else {
		XBREAK(1);
	}
	XBREAK( mulByLvSquad <= 0.f );
	return mulByLvSquad;
}

void XPropSquad::xPROP::Serialize( XArchive& ar ) const {
	ar << level << levelLimitByHero << maxSmall << maxMiddle;
	ar << aryMultiplyRatioMiddle;
	ar << avgMultiplyByMiddle;
	ar << maxBig << multiplyRatioBig << expMax;
}
void XPropSquad::xPROP::DeSerialize( XArchive& ar, int ) {
	aryMultiplyRatioMiddle.clear();
	aryMultiplyRatioMiddle.clear();
	ar >> level >> levelLimitByHero >> maxSmall >> maxMiddle;
	ar >> aryMultiplyRatioMiddle;
	ar >> avgMultiplyByMiddle;
	ar >> maxBig >> multiplyRatioBig >> expMax;
}

void XPropSquad::Serialize( XArchive& ar ) const
{
	ar << VER_PROP_SQUAD;
	ar << m_goldPerExp << m_secPerExp << m_lvLast;
	ar << m_aryProp;
}
void XPropSquad::DeSerialize( XArchive& ar, int )
{
	m_aryProp.Clear();
	int ver;
	ar >> ver;
	ar >> m_goldPerExp >> m_secPerExp >> m_lvLast;
	ar >> m_aryProp;
}

