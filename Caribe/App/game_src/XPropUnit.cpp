#include "stdafx.h"
#include "XPropUnit.h"
#include "etc/Token.h"
#include "XPropSquad.h"
#include "XHero.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XPropUnit *PROP_UNIT = nullptr;

XPropUnit::XPropUnit() 
{ 
	Init(); 
}

void XPropUnit::Destroy() 
{
// 	std::map<_tstring, xPROP*>::iterator itor;
// 	for( itor = m_mapData.begin(); itor != m_mapData.end(); itor++ )
// 	{
// 		xPROP *pProp = itor->second;
// 		SAFE_DELETE( pProp );
// 	}
	for( auto pProp : m_aryProp ) {
		SAFE_DELETE( pProp );
	}
	m_aryProp.clear();
	m_mapData.clear();
	m_mapID.clear();
}

BOOL XPropUnit::ReadProp( CToken& token, DWORD dwParam )
{

	ID idProp = TokenGetNumber( token );
	if( GetpProp( idProp ) )
	{
		XERROR( "%s: duplication Mob ID. %d", GetstrFilename(), idProp );
		return FALSE;
	}
	if( idProp == TOKEN_EOF )
		return FALSE;		// false를 리턴해야 루프를 빠져나옴
	
	xPROP *pProp = new xPROP;		
	pProp->idProp = idProp;				// ID
	XBREAK( pProp->idProp > 0xffff );		// 이렇게 큰숫자를 쓰는건 권장하지 않음.
	
	pProp->strIdentifier = GetTokenIdentifier( token );					// 식별자
	int idText			= TokenGetNumber( token );						// 텍스트 ID
	pProp->strName		= XTEXT(idText);								// 이름
	pProp->idDesc		= TokenGetNumber(token);
	pProp->typeAtk		= (XGAME::xtAttack)TokenGetNumber( token );		// 공격타입
	pProp->size			= (XGAME::xtSize)TokenGetNumber( token );		// 유닛사이즈
	pProp->atkMelee		= token.GetNumberF();							// 공격력
	pProp->atkRange		= token.GetNumberF();							// 공격력
	pProp->def			= token.GetNumberF();							// 방어력
	pProp->hpMax			= token.GetNumberF();							// 체력
	pProp->atkSpeed		= token.GetNumberF();							// 공격속도
	float speedMeterPerSec	= token.GetNumberF();							// 이동속도
	// 초당 이동미터를 초당 이동픽셀로 변환
	pProp->movSpeedPerSec = speedMeterPerSec * XGAME::ONE_METER;
	float radiusAtkForMeter = token.GetNumberF();
	pProp->radiusAtkByPixel = xMETER_TO_PIXEL( radiusAtkForMeter );
	pProp->tribe = (XGAME::xtTribe)TokenGetNumber( token );
	pProp->resNeed[0]	= (XGAME::xtResource)TokenGetNumber( token );	// 필요자원
	pProp->numRes[0]	= token.GetNumber();
	pProp->resNeed[1]	= (XGAME::xtResource)TokenGetNumber( token );	// 필요자원
	pProp->numRes[1]	= token.GetNumber();
	
	pProp->strFace = GetTokenString( token );							// 아이콘 파일
	if( pProp->strFace.empty() == false )
		pProp->strFace += _T( ".png" );
	pProp->strSpr = GetTokenString( token );							// spr 파일
	if( pProp->strSpr.empty() == false )
		pProp->strSpr += _T(".spr");
	else
		CONSOLE( "warning: identifier=%s, szSpr is empty", pProp->strIdentifier.c_str() );
	pProp->movSpeedNormal = token.GetNumber();
	pProp->scale = token.GetNumberF();
	// HSL값은 포토샵값을 기준으로 하므로 소수점을 사용하지 않아서 일부러 int로 읽음.
	pProp->vHSL.x = (float)token.GetNumber();
	pProp->vHSL.y = (float)token.GetNumber();
	pProp->vHSL.z = (float)token.GetNumber();

	// 레인지 유닛이 아닌데 사거리가 5미터 이상일순 없다.
	XBREAK( pProp->IsRange() == FALSE 
		&& pProp->radiusAtkByPixel > xMETER_TO_PIXEL(5) );
	// 추가
	Add( pProp->strIdentifier, pProp );

	return TRUE;
}

void XPropUnit::OnDidBeforeReadProp( CToken& token )
{
	int ver = token.GetNumber();
	XBREAK( ver == 0 );
	SetVer( ver );
}

void XPropUnit::OnDidFinishReadProp( CToken& token )
{
// 	int size = m_mapData.size();
// 	if( size > 0 ) {
// 		m_aryProp.Create( size );
// 		for( auto itor = m_mapData.begin(); itor != m_mapData.end(); ++itor ) {
// 			xPROP *pProp = itor->second;
// 			XBREAK( pProp == nullptr );
// 			m_aryProp.Add( pProp );
// 		}
// 	}
}

void XPropUnit::Add( const _tstring& strIdentifer, xPROP *pProp )
{
	m_aryProp.Add( pProp );
	m_mapData[ strIdentifer ] = pProp;		// map에다 넣음.
	m_mapID[ pProp->idProp ] = pProp;		// ID로 검색용 맵에도 넣음.
}

void XPropUnit::Add( xPROP *pProp )
{
	XBREAK( pProp->strIdentifier.empty() );
	XBREAK( pProp->idProp == 0 );
	m_aryProp.Add( pProp );
	m_mapData[ pProp->strIdentifier ] = pProp;		// map에다 넣음.
	m_mapID[ pProp->idProp ] = pProp;		// ID로 검색용 맵에도 넣음.
}

XPropUnit::xPROP* XPropUnit::GetpProp( LPCTSTR szIdentifier )
{
	TCHAR szBuff[ 256 ];
	_tcscpy_s( szBuff, szIdentifier );
	_tcslwr_s( szBuff );
	std::map<_tstring, xPROP*>::iterator itor;
	itor = m_mapData.find( szBuff );
	if( itor == m_mapData.end() )
		return nullptr;							// 못찾았으면 널 리턴
	xPROP* pProp = (*itor).second;
	XBREAK( pProp == nullptr );			// 위에서 find로 검사했기때문에 nullptr나와선 안됨.
	return pProp;
}

XPropUnit::xPROP* XPropUnit::GetpProp( ID idProp )
{
	if( idProp == 0 )
		return nullptr;
	std::map<ID, xPROP*>::iterator itor;
	itor = m_mapID.find( idProp );
	if( itor == m_mapID.end() ) {
		return nullptr;							// 못찾았으면 에러 리턴
	}
	xPROP *pProp = (*itor).second;
	XBREAK( pProp == nullptr );			// 위에서 find로 검사했기때문에 nullptr나와선 안됨.
	return pProp; 
}


// szName의 이름을 갖는 프로퍼티를 찾아서 돌려준다.(치트용)
XPropUnit::xPROP* XPropUnit::GetpPropFromName( LPCTSTR szName )
{
    XBREAK( szName == nullptr );
    XBREAK( XE::IsEmpty( szName ) );
//     int num = m_aryProp.size();
//     for( int i = 0; i < num; ++i )
		for( auto pProp : m_aryProp ) {
//         xPROP *pProp = m_aryProp[i];
        if( XE::IsSame( pProp->strName.c_str(), szName ) )
            return pProp;
    }
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// 스탯. 로비등에서 부대공격력등을 표시할때 사용
/**
 @brief 스탯과 사이즈 부대레벨을 받아서 부대총 능력치를 곱해준다.
*/
float XPropUnit::GetSquadPower( XHero *pHero, float statBase, XGAME::xtSize size, int levelSquad )
{
	const XPropSquad::xPROP& propSquad = PROP_SQUAD->GetTable( levelSquad );
	float stat = statBase;
	switch( size )
	{
	case XGAME::xSIZE_SMALL:
		stat *= propSquad.maxSmall;
		break;
	case XGAME::xSIZE_MIDDLE: {
		float power = 0;
		int max = propSquad.maxMiddle;		// 현재레벨에서의 유닛 최대수
		for( int i = 0; i < max; ++i )
			power += stat * propSquad.aryMultiplyRatioMiddle[ i ];
		stat = power;
//		return power;	// 현재 레벨의 최대유닛만큼 
	} break;
	case XGAME::xSIZE_BIG:
		stat *= propSquad.multiplyRatioBig;
		break;
	}
	// 기본능력치에 영웅능력을 곱한다.
	return stat *= pHero->GetAttackMeleeRatio();
}
// 스탯
//////////////////////////////////////////////////////////////////////////
float XPropUnit::GetAttackMeleePowerSquad( XHero *pHero, int levelSuqad ) 
{
	xPROP *pProp = GetpProp( pHero->GetUnit() );
	if( XBREAK( pProp == nullptr ) )
		return 0.f;
	return GetAttackMeleePowerSquad( pHero, pProp, levelSuqad );
}

float XPropUnit::GetAttackRangePowerSquad( XHero *pHero, int levelSquad ) 
{
	xPROP *pProp = GetpProp( pHero->GetUnit() );
	if( XBREAK( pProp == nullptr ) )
		return 0.f;
	return GetAttackRangePowerSquad( pHero, pProp, levelSquad );
}

float XPropUnit::GetDefensePowerSquad( XHero *pHero, int levelSquad ) 
{
	xPROP *pProp = GetpProp( pHero->GetUnit() );
	if( XBREAK( pProp == nullptr ) )
		return 0.f;
	return GetDefensePowerSquad( pHero, pProp, levelSquad );
}

int XPropUnit::GetMaxHpSquad( XHero *pHero, int levelSquad ) 
{
	xPROP *pProp = GetpProp( pHero->GetUnit() );
	if( XBREAK( pProp == nullptr ) )
		return 0;
	return GetMaxHpSquad( pHero, pProp, levelSquad );
}

void XPropUnit::xPROP::Serialize( XArchive& ar ) const {
	ar << idProp << strIdentifier << strName << idDesc;
	ar << (BYTE)typeAtk << (BYTE)size << (BYTE)tribe << (BYTE)0;
	ar << atkMelee << atkRange << def << hpMax << atkSpeed << movSpeedPerSec << radiusAtkByPixel;
	ar << (BYTE)resNeed[0] << (BYTE)resNeed[1] << (WORD)(scale * 1000);
	ar << numRes[0] << numRes[1];
	ar << strFace << strSpr << movSpeedNormal << vHSL;
}
void XPropUnit::xPROP::DeSerialize( XArchive& ar, int ver ) {
	BYTE b0;
	WORD w0;
	ar >> idProp >> strIdentifier >> strName >> idDesc;
	ar >> b0;	typeAtk = (xtAttack)b0;
	ar >> b0;	size = (xtSize)b0;
	ar >> b0;	tribe = (xtTribe)b0;
	ar >> b0;
	ar >> atkMelee >> atkRange >> def >> hpMax >> atkSpeed >> movSpeedPerSec >> radiusAtkByPixel;
	ar >> b0;	resNeed[0] = (xtResource)b0;
	ar >> b0;	resNeed[1] = (xtResource)b0;
	ar >> w0;	scale = ((float)w0 / 1000.f);
	ar >> numRes[0] >> numRes[1];
	ar >> strFace >> strSpr >> movSpeedNormal >> vHSL;
}


void XPropUnit::Serialize( XArchive& ar ) const
{
	ar << (int)m_aryProp.size();
	for( auto pProp : m_aryProp ) {
		ar << (*pProp);
	}
}

void XPropUnit::DeSerialize( XArchive& ar )
{
	Destroy();
	int num;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pProp = new xPROP();
		ar >> (*pProp);
		Add( pProp );
	}
}

