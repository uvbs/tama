#include "stdafx.h"
#include "XSquadron.h"
#include "XLegion.h"
#include "XArchive.h"
#include "XAccount.h"
#include "XPropUnit.h"
#include "XPropSquad.h"
#include "XExpTableHero.h"
#include "XBaseItem.h"
#ifndef _CLIENT
#include "XMain.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
/**
 플레이어 부대를 생성
*/
XSquadron::XSquadron( XHero *pHero )
{
	Init();
	m_bCreateHero = FALSE;
	XBREAK( pHero == NULL );
	m_pHero = pHero;
	XPropUnit::xPROP *pPropUnit = PROP_UNIT->GetpProp( pHero->GetUnit() );
	XBREAK( pPropUnit == NULL );
	XBREAK( m_pHero->IsRange() && pPropUnit->IsRange() == FALSE );
}
#if defined(_XSINGLE) || !defined(_CLIENT)
/**
 @brief 
*/
XSquadron::XSquadron( XPropHero::xPROP *pPropHero,
					int levelHero,
					XGAME::xtUnit unit,
					int levelSquad )
{
	Init();
	XBREAK( pPropHero == nullptr );
	const auto& prop = PROP_SQUAD->GetTable( levelSquad );
	m_bCreateHero = TRUE;
//	int numUnit = prop.GetMaxUnit( unit);
	m_pHero = XHero::sCreateHero( pPropHero, levelSquad, unit );
	XBREAK( m_pHero == nullptr );
	XBREAK( levelHero <= 0 );
	m_pHero->SetLevel(levelHero);
	m_pHero->SetlevelSquad( levelSquad );

	auto pPropUnit = PROP_UNIT->GetpProp( unit );
	XBREAK( pPropUnit == NULL );
	XBREAK( m_pHero->IsRange() && pPropUnit->IsRange() == FALSE );
}
#endif // defined(_XSINGLE) || !defined(_CLIENT)
void XSquadron::Destroy( void )
{
	if( m_bCreateHero )
		SAFE_DELETE( m_pHero );
}

/**
 NPC부대의 경우 영웅의 모든 데이타를 넣고
 PC부대의 경우는 영웅의 시리얼번호만 넣어서 XAccount에서 검색해서 매칭하도록 한다.
*/
void XSquadron::Serialize( XArchive& ar )
{
	if( m_pHero ) {
		ar << (BYTE)11;
		ar << (BYTE)m_bCreateHero;
		ar << (BYTE)VER_HERO_SERIALIZE;
//		ar << xboolToByte( m_bResourceSquad );
		ar << (BYTE)0;
		if( m_bCreateHero )
			m_pHero->Serialize( ar );
		else
			ar << m_pHero->GetsnHero();
	} else {
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
	}
	ar << (WORD)(m_mulAtk * 1000);
	ar << (WORD)(m_mulHp * 1000);
	MAKE_CHECKSUM( ar );
}

/**
 pAccount가 nullptr이면 NPC부대
*/
BOOL XSquadron::DeSerialize( XArchive& ar, XSPAcc spAcc, int verLegion ) 
{
	int isHero;
	BYTE b0;
	ar >> b0;	isHero = b0;
	if( isHero == 11 )
	{
		int verHero = 0;
		ar >> b0;	m_bCreateHero = b0;
		ar >> b0;	verHero = b0;
		ar >> b0;	//m_bResourceSquad = xbyteToBool(b0);
		XBREAK( spAcc && m_bCreateHero == TRUE );	// 계정이 있는데 NPC플랙인경우
		XBREAK( spAcc == nullptr && m_bCreateHero == FALSE );	// 계정이 없는데 PC인경우
		if( m_bCreateHero )
		{
			XBREAK( spAcc != nullptr );
			m_pHero = new XHero;
			m_pHero->DeSerialize( ar, spAcc, verHero );
		} else
		{
			// 플레이어의 부대일경우는 계정에서 영웅을 찾아서 링크만 시킨다.
			ID snHero;
			ar >> snHero;
			if( XBREAK( snHero == 0 ) )
				return FALSE;
			m_pHero = spAcc->GetHero( snHero );
			if( XBREAK( m_pHero == nullptr ) )
				return FALSE;

		}
	} else	{
		ar >> b0 >> b0 >> b0;
		// 11이 아니면 0이어야 하는데 다른수면 시리얼라이즈가 잘못된거임.
		XBREAK( isHero != 0 );
	}
	if( verLegion >= 8 ) {
		WORD w0;
		ar >> w0;		m_mulAtk = (float)w0 / 1000.f;
		ar >> w0;		m_mulHp = (float)w0 / 1000.f;
		XBREAK( m_mulAtk == 0.f );
		XBREAK( m_mulHp == 0.f );
	}
	RESTORE_VERIFY_CHECKSUM( ar );
	return TRUE;
}

/**
 강제로 NPC처럼 모든 영웅데이타를 보낸다.
*/
void XSquadron::SerializeFull( XArchive& ar )
{
	if( m_pHero ) {
		ar << (BYTE)22;
		ar << (BYTE)0;
		ar << (BYTE)VER_HERO_SERIALIZE;
		//ar << xboolToByte(m_bResourceSquad);
		ar << (BYTE)0;
		m_pHero->Serialize( ar );
	} else {
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
	}
	ar << (WORD)( m_mulAtk * 1000 );
	ar << (WORD)( m_mulHp * 1000 );
	MAKE_CHECKSUM( ar );
}


BOOL XSquadron::DeSerializeFull( XArchive& ar, int verLegion ) 
{
	int isHero;
	BYTE b0;
	ar >> b0;	isHero = b0;		// 부대가 있느냐
	if( verLegion >= 2 && isHero == 22 ) {
		int verHero = 0;
		ar >> b0;
		ar >> b0;	verHero = b0;
		ar >> b0;	//m_bResourceSquad = xbyteToBool(b0);
		m_pHero = new XHero;
		m_pHero->DeSerialize( ar, nullptr, verHero );
	} else
	{
		ar >> b0 >> b0 >> b0;
		XBREAK( isHero != 0 );
	}
	if( verLegion >= 9 ) {
		WORD w0;
		ar >> w0;		m_mulAtk = (float)w0 / 1000.f;
		ar >> w0;		m_mulHp = (float)w0 / 1000.f;
		XBREAK( m_mulAtk == 0.f );
		XBREAK( m_mulHp == 0.f );
	}
	RESTORE_VERIFY_CHECKSUM( ar );
	m_bCreateHero = TRUE;
	return TRUE;
}

