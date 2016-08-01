#include "stdafx.h"
#include "XPropHero.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XPropHero *PROP_HERO = nullptr;

//////////////////////////////////////////////////////////////////////////
XPropHero::XPropHero() 
{ 
	Init(); 
}

void XPropHero::Destroy() 
{
	DestroyProps();
// 	std::map<_tstring, xPROP*>::iterator itor;
// 	for( itor = m_mapData.begin(); itor != m_mapData.end(); itor++ )
// 	{
// 		xPROP *pProp = itor->second;
// 		SAFE_DELETE( pProp );
// 	}
}

void XPropHero::DestroyProps()
{
	for( auto pProp : m_aryProp ) {
		SAFE_DELETE( pProp );
	}
	m_aryProp.clear();
	m_mapID.clear();
	m_mapData.clear();
}

BOOL XPropHero::ReadProp( CToken& token, DWORD dwParam )
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
//	pProp->strName		= XTEXT(idText);								// 이름
	pProp->idName = idText;
//	pProp->gradeMax		= (XGAME::xtGrade)TokenGetNumber( token );
//	pProp->lead			= (XGAME::xtLead)TokenGetNumber( token );
	pProp->typeAtk		= (XGAME::xtAttack)TokenGetNumber( token );
//	pProp->clan			= (XGAME::xtClan)TokenGetNumber( token );
	pProp->classJob		= (XGAME::xtClass)TokenGetNumber( token );
	pProp->tribe		= (XGAME::xtTribe)TokenGetNumber( token );
	pProp->getType		= (XGAME::xtGet)TokenGetNumber( token );
	pProp->priority		= token.GetNumber();
	pProp->lvSpawn		= token.GetNumber();
	// 탱커인데 클래스는 탱커가 아닐때 에러
	XBREAKF( pProp->typeAtk == XGAME::xAT_TANKER && 
			pProp->classJob != XGAME::xCLS_TANKER, 
			"%s:지휘타입은 탱커인데 클래스는 탱크가 아니다.", pProp->strIdentifier.c_str() );
	XBREAKF( pProp->typeAtk == XGAME::xAT_RANGE && 
		pProp->IsRange() == FALSE,
		"%s:지휘타입은 원거리인데 클래스는 원거리가 아니다.", pProp->strIdentifier.c_str() );
	XBREAKF( pProp->typeAtk == XGAME::xAT_SPEED &&
			pProp->classJob != XGAME::xCLS_HORSE,
		"%s:지휘타입은 스피드인데 클래스는 스피드가 아니다.", pProp->strIdentifier.c_str() );
	pProp->atkMelee		= token.GetNumberF();							// 공격력
	pProp->atkRange		= token.GetNumberF();							// 공격력
	pProp->defense		= token.GetNumberF();							// 방어력
	pProp->hpMax		= token.GetNumberF();							// 체력
	pProp->attackSpeed	= token.GetNumberF();							// 공격속도
	pProp->moveSpeed	= token.GetNumberF();							// 이동속도
	pProp->radiusAtk	= token.GetNumberF();
//	pProp->leadPowerMax	= token.GetNumber();
	// 레벨업 증가치
	pProp->atkMeleeAdd		= token.GetNumberF();							// 공격력
	pProp->atkRangeAdd		= token.GetNumberF();							// 공격력
	pProp->defenseAdd		= token.GetNumberF();							// 방어력
	pProp->hpAdd			= token.GetNumberF();							// 체력
	pProp->attackSpeedAdd	= token.GetNumberF();							// 공격속도
	pProp->moveSpeedAdd		= token.GetNumberF();							// 이동속도
//	pProp->leadPowerAdd	= token.GetNumber();
	// 스킬
	pProp->strPassive = GetTokenString( token );
	pProp->strActive = GetTokenString( token );
	
	pProp->strFace = GetTokenString( token );							// 초상화 파일
	if( pProp->strFace.empty() == false )
		pProp->strFace += _T(".png");
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

//	pProp->scale = 1.1f;

	// 추가
	Add( pProp->strIdentifier, pProp );

	return TRUE;
}

void XPropHero::OnDidBeforeReadProp( CToken& token )
{
	int ver = token.GetNumber();
	XBREAK( ver == 0 );
	SetVer( ver );
}

void XPropHero::OnDidFinishReadProp( CToken& token )
{
// 	int size = m_mapData.size();
// 	if( size > 0 )	{
// 		m_aryProp.Create( size );
// // 		std::map<_tstring, xPROP*>::iterator itor;
// 		for( auto itor = m_mapData.begin(); itor != m_mapData.end(); ++itor )		{
// 			xPROP *pProp = itor->second;
// 			XBREAK( pProp == nullptr );
// 			m_aryProp.Add( pProp );
// 		}
// 	}
}

void XPropHero::Add( const _tstring& strIdentifer, xPROP *pProp )
{
	XBREAK( pProp == nullptr );
	XBREAK( strIdentifer.empty() );
	XBREAK( pProp->idProp == 0 );
	m_aryProp.Add( pProp );
	m_mapData[ strIdentifer ] = pProp;		// map에다 넣음.
	m_mapID[ pProp->idProp ] = pProp;		// ID로 검색용 맵에도 넣음.
}

void XPropHero::Add( xPROP *pProp )
{
	XBREAK( pProp == nullptr );
	XBREAK( pProp->strIdentifier.empty() );
	XBREAK( pProp->idProp == 0 );
	m_aryProp.Add( pProp );
	m_mapData[ pProp->strIdentifier ] = pProp;		// map에다 넣음.
	m_mapID[pProp->idProp] = pProp;		// ID로 검색용 맵에도 넣음.
}

XPropHero::xPROP* XPropHero::GetpProp( LPCTSTR szIdentifier ) const
{
	TCHAR szBuff[ 256 ];
	_tcscpy_s( szBuff, szIdentifier );
	_tcslwr_s( szBuff );
	auto itor = m_mapData.find( szBuff );
	if( itor == m_mapData.end() )
		return nullptr;							// 못찾았으면 널 리턴
	auto pProp = (*itor).second;
	XBREAK( pProp == nullptr );			// 위에서 find로 검사했기때문에 nullptr나와선 안됨.
	return pProp;
}

XPropHero::xPROP* XPropHero::GetpProp( ID idProp ) const
{
	if( idProp == 0 )
		return nullptr;
	auto itor = m_mapID.find( idProp );
	if( itor == m_mapID.end() ) {
		return nullptr;							// 못찾았으면 에러 리턴
	}
	auto pProp = (*itor).second;
	XBREAK( pProp == nullptr );			// 위에서 find로 검사했기때문에 nullptr나와선 안됨.
	return pProp; 
}


// szName의 이름을 갖는 프로퍼티를 찾아서 돌려준다.
XPropHero::xPROP* XPropHero::GetpPropFromName( LPCTSTR szName )
{
    XBREAK( szName == nullptr );
    XBREAK( XE::IsEmpty( szName ) );
//     int num = m_aryProp.size();
//     for( int i = 0; i < num; ++i ) {
//         xPROP *pProp = m_aryProp[i];
		for( auto pProp : m_aryProp ) {
				const _tstring strName = XTEXT(pProp->idName);
        if( XE::IsSame( strName.c_str(), szName ) )
            return pProp;
    }
    return nullptr;
}

/**
 @brief grade등급의 영웅중에서 랜덤으로 하나 뽑는다.
*/
// XPropHero::xPROP* XPropHero::GetPropRandomGrade( XGAME::xtGrade gradeMax ) 
// {
// 	XArrayLinearN<XPropHero::xPROP*,1024> ary;
// 	XARRAYLINEAR_LOOP( m_aryProp, xPROP*, pProp ) {
// 		if( pProp->gradeMax == gradeMax &&
// 			pProp->IsEventHero() == false )		// 이벤트용 영웅은 랜덤대상에서 제외
// 			ary.Add( pProp );
// 	} END_LOOP;
// 
// 	return ary.GetFromRandom();
// }

/**
 @brief 지정한 지휘타입의 영웅만 골라준다.
 @param lvLimit lvSpawn상한선. 0이면 제한없이 찾는다.
 @param typeAtk 걸러내야할 atkType. 0,이면 모든타입을 대상으로 찾는다.
*/
int XPropHero::GetpPropBySizeToAry( XArrayLinearN<XPropHero::xPROP*,256>& ary, 
																		XGAME::xtAttack typeAtk,
																		int lvLimitSpawn )
{
// 	int num = m_aryProp.size();
// 	for( int i = 0; i < num; ++i ) {
// 		xPROP *pProp = m_aryProp[ i ];
	for( auto pProp : m_aryProp ) {
		// 이벤트용 영웅은 랜덤대상에서 제외
		if( pProp->IsEventHero() == false ) {
			if( !typeAtk || (typeAtk && pProp->typeAtk == typeAtk) )	{
				if( lvLimitSpawn == 0 || (lvLimitSpawn > 0 && pProp->lvSpawn <= lvLimitSpawn ) ) {
					ary.Add( pProp );
				}
			}
		}
	}
	return ary.size();
}

/**
 @brief lead타입의 영웅 1명을 랜덤으로 고른다.
 @param lvLimit lvSpawn속성의 상한선
*/
XPropHero::xPROP* XPropHero::GetPropRandomByAtkType( XGAME::xtAttack typeAtk
																									, int lvLimitSpawn )
{
	XArrayLinearN<XPropHero::xPROP*, 256> ary;
	GetpPropBySizeToAry( ary, typeAtk, lvLimitSpawn );
	if( ary.size() == 0 )
		return nullptr;
	return ary.GetFromRandom();
}

/**
 @brief 획득방법에 따른 영웅리스트를 배열에 받는다.
*/
int XPropHero::GetpPropByGetToAry( XVector<xPROP*> *pOutAry
								, XGAME::xtGet typeGet
								, XGAME::xtAttack typeAtk/* = XGAME::xAT_NONE*/ )
{
	for( auto pProp : m_aryProp ) {
		if( pProp->getType == typeGet ) {
			if( typeAtk == XGAME::xAT_NONE || typeAtk == pProp->typeAtk )
				pOutAry->push_back( pProp );
		}
	}
	return pOutAry->size();
}

int XPropHero::GetpPropByGetToAryWithExclude( XVector<xPROP*> *pOutAry
																						, XGAME::xtGet typeGet
																						, const XVector<ID>& aryExclude
																						, XGAME::xtAttack typeAtk/* = XGAME::xAT_NONE*/ )
{
	for( auto pProp : m_aryProp ) {
		if( pProp->getType == typeGet ) {
			if( typeAtk == XGAME::xAT_NONE || typeAtk == pProp->typeAtk ){
				// 제외목록에 있는건 제외함.
				bool bExclude = false;
				for( ID idExclude : aryExclude ) {
					if( pProp->idProp == idExclude ) {
						bExclude = true;
						break;
					}
				}
				if( !bExclude )
					pOutAry->push_back( pProp );
			}
		}
	}
	return pOutAry->size();
}

/**
 @brief typeGet타입영웅중에서 하나를 랜덤으로 선택해서 돌려준다.
 @param typeAtk는 디폴트로 none이지만 지정을 하게 되면 이 필터도 함께 적용한다.
*/
XPropHero::xPROP* XPropHero::GetpPropRandomByGetType( XGAME::xtGet typeGet,
													XGAME::xtAttack typeAtk/* = XGAME::xAT_NONE*/ )
{
	// 병과별로 영웅수가 달라서 그냥 랜덤하면 병과별 확률이 균일하지 않다.
	// 병과별로 먼저 확률돌리고 그담에 다시 영웅을 꺼내야한다.
	XVector<xPROP*> ary;
	GetpPropByGetToAry( &ary, typeGet, typeAtk );
	auto ppSelected = ary.GetpFromRandom();
	if( ppSelected )
		return *ppSelected;
	return nullptr;
}
/**
 @brief 영웅의 식별자로 idProp을 돌려준다.
*/
ID XPropHero::GetidPropByIds( LPCTSTR idsHero )
{
	auto pProp = GetpProp( idsHero );
	if( XASSERT(pProp) ) {
		return pProp->idProp;
	}
	return 0;
}

/**
 @brief typeGet 속성이 몇개나 있는지
*/
int XPropHero::GetnumPropByGetType( XGAME::xtGet typeGet )
{
	int num = 0;
	for( auto pProp : m_aryProp ) {
		if( pProp->getType == typeGet )
			++num;
	}
	return num;
}


void XPropHero::xPROP::Serialize( XArchive& ar ) const {
	XBREAK( lvSpawn > 0x7f );
	XBREAK( movSpeedNormal > 255 );
	XBREAK( vHSL.x > 0xffff || vHSL.y > 0xffff || vHSL.z > 0xffff );
	XBREAK( atkMelee < 0 );
	XBREAK( priority > 0x7fff );
	ar << idProp << strIdentifier << idName;
	ar << (BYTE)typeAtk;
	ar << (BYTE)classJob;
	ar << (BYTE)tribe;
	ar << (BYTE)getType;

	ar << (short)priority;
	ar << (char)lvSpawn;
	ar << (BYTE)movSpeedNormal;

	ar << (WORD)( atkMelee * 1000 );
	ar << (WORD)( atkRange * 1000 );
	
	ar << (WORD)( defense * 1000 );
	ar << (WORD)( hpMax * 1000 );
	
	ar << (WORD)( attackSpeed * 1000 );
	ar << (WORD)( moveSpeed * 1000 );
	
	ar << (WORD)( radiusAtk * 1000 );
	ar << (WORD)( atkMeleeAdd * 1000 );
	
	ar << (WORD)( atkRangeAdd * 1000 );
	ar << (WORD)( defenseAdd * 1000 );
	
	ar << (WORD)( hpAdd * 1000 );
	ar << (WORD)( attackSpeedAdd * 1000 );
	
	ar << (WORD)( moveSpeedAdd * 1000 );
	ar << (WORD)( scale * 1000 );
	ar << strPassive << strActive;
	ar << strFace << strSpr;
	ar << (short)vHSL.x;
	ar << (short)vHSL.y;

	ar << (short)vHSL.z;
	ar << (WORD)0;

}
void XPropHero::xPROP::DeSerialize( XArchive& ar, int ) {
	BYTE b0;
	char c0;
	WORD w0;
	short s0;
	ar >> idProp >> strIdentifier >> idName;
	ar >> b0;		typeAtk = (xtAttack)b0;
	ar >> b0;		classJob = (xtClass)b0;
	ar >> b0;		tribe = (xtTribe)b0;
	ar >> b0;		getType = (xtGet)b0;

	ar >> s0;		priority = s0;
	ar >> c0;		lvSpawn = c0;
	ar >> b0;		movSpeedNormal = b0;

	ar >> w0;		atkMelee = w0 / 1000.f;
	ar >> w0;		atkRange = w0 / 1000.f;

	ar >> w0;		defense = w0 / 1000.f;
	ar >> w0;		hpMax = w0 / 1000.f;

	ar >> w0;		attackSpeed = w0 / 1000.f;
	ar >> w0;		moveSpeed = w0 / 1000.f;

	ar >> w0;		radiusAtk = w0 / 1000.f;
	ar >> w0;		atkMeleeAdd = w0 / 1000.f;

	ar >> w0;		atkRangeAdd = w0 / 1000.f;
	ar >> w0;		defenseAdd = w0 / 1000.f;

	ar >> w0;		hpAdd = w0 / 1000.f;
	ar >> w0;		attackSpeedAdd = w0 / 1000.f;

	ar >> w0;		moveSpeedAdd = w0 / 1000.f;
	ar >> w0;		scale = w0 / 1000.f;

	ar >> strPassive >> strActive >> strFace >> strSpr;
	ar >> s0;		vHSL.x = (float)s0;
	ar >> s0;		vHSL.y = (float)s0;
	
	ar >> s0;		vHSL.z = (float)s0;
	ar >> w0;		
}

void XPropHero::Serialize( XArchive& ar ) const 
{
	ar << VER_PROP_HERO;
	ar << (int)m_aryProp.size();
	for( auto pProp : m_aryProp ) {
		ar << (*pProp);
	}
}

void XPropHero::DeSerialize( XArchive& ar, int ) 
{
	int num;
	int ver;
	ar >> ver;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pProp = new xPROP();
		ar >> (*pProp);
		Add( pProp );
	}
}
