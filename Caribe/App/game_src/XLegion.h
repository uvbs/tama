﻿#pragma once
#include "XPropHero.h"
#include "XLegionH.h"
namespace XGAME {
struct xSquad;
struct xLegion;
}
class XArchive;
class XAccount;
class XSquadron;
class XHero;
// #define RATE_ATK_DEFAULT_ELITE		0.2f
// #define RATE_HP_DEFAULT_ELITE		5.f
// #define RATE_ATK_DEFAULT_RAID		0.5f
// #define RATE_HP_DEFAULT_RAID		100.f
////////////////////////////////////////////////////////////////
class XLegion
{
public:
	enum {
		xMAX_NOT_FOG = 3,	// 안개로가려지지 않는 부대수
	};
	static void sSerialize( XSPLegionConst spLegion, XArchive* pOut );
	static void sDeSerializeUpdate( XSPLegion spLegion, XSPAcc spAcc, XArchive& ar );
	static int sSerializeFull( XArchive& ar, XSPLegion spLegion );
	static XLegion* sCreateDeserializeFull( XArchive& ar );
//	static XLegion* sCreateDeserializeForGuildRaid( XArchive& ar );
	static XLegion* sCreateLegionForNPC( int lvAcc, int adjDiff, 
																		XGAME::xLegionParam& legionInfo );
	static XSPLegion sCreateLegionForNPC2( XGAME::xLegion& legion, int lvExtern, bool bAdjustLegion );
//	static XSPLegion sCreatespLegion( const XGAME::xLegion& legion, int lvExtern, bool bReArrangeSquad, XVector<XSPHero>* pOutAry );
#if defined(_XSINGLE) || !defined(_CLIENT)
	static XSPSquadron sCreateSquadronForNPC( int levelLegion, int adjDiff, XGAME::xtUnit unit, ID idHero=0, int _levelHero=0, int levelSkill=0, int _levelSquad=0, XGAME::xtGradeLegion gradeLegion = XGAME::xGL_NORMAL, const XGAME::xLegionParam *pLegionInfo = nullptr );
	static XSPSquadron sCreateSquadronForNPC2( const int levelLegion
																					, XPropHero::xPROP *pPropHero
																					, const XGAME::xSquad& sqParam );
	static XSPSquadron sCreateSquadronForNPC2( const int levelLegion
																					, const XGAME::xSquad& sqParam );
#endif // defined(_XSINGLE) || !defined(_CLIENT)
	// 앞으로 이 시리즈들 사용금지.
	static XPropHero::xPROP* sGetpPropHeroByInfo( const XGAME::xSquad& squad, int lvLegion );
	static XGAME::xtUnit sGetUnitByInfo( const XGAME::xSquad& squad, XPropHero::xPROP *pPropHero, int lvLegion );
	static int sGetLvHeroByInfo( const XGAME::xSquad& squad, int lvLegion );
	static int sGetLvSquadByInfo( const XGAME::xSquad& squad, int lvLegion );
	static int sGetLvSkillByInfo( const XGAME::xSquad& squad, int lvLegion );
	static XGAME::xtGrade sGetGradeHeroByInfo( const XGAME::xSquad& squad, int lvLegion );
	// 앞으로 이 시리즈들을 쓸것.
	static XPropHero::xPROP* sGetpPropHeroByInfo( ID idHero, XGAME::xtAttack atkType, XGAME::xtUnit unit, int lvLegion );
	static XGAME::xtUnit sGetUnitByInfo( XGAME::xtUnit unit, XGAME::xtSize sizeUnit, XGAME::xtAttack typeAtk, int lvLegion );
	static int sGetLvHeroByInfo( int lvHero, int lvLegion );
	static int sGetLvSquadByInfo( int lvSquad, int lvLegion );
	static int sGetLvSkillByInfo( int lvSkill, int lvLegion );
	static XGAME::xtGrade sGetGradeHeroByInfo( XGAME::xtGrade gradeHero, int lvLegion );
	//
	static void sSetLeaderByInfo( const XGAME::xLegion* pxLegion, XSPLegion spLegion, XSPSquadron pSquad );
	static XGAME::xtUnit sCreateUnitType( int spawnType );
//	static int sCreateUnitNum( XGAME::xtUnit unit, BOOL bRandom );
	static int sGetNumUnitByLevel( XGAME::xtUnit unit, int lvUnit );
	static int sGetSquadMaxLevelByHeroLevel( int lvHero );
	static ID sGetHeroByUnit( XGAME::xtUnit unit );
	static XGAME::xtGrade sGetRandomGradeHeroByTable( int levelUser, XGAME::xtGradeLegion gradeLegion );
	static int sGetAvailableUnitByLevel( int level, XArrayLinearN<XGAME::xtUnit, XGAME::xUNIT_MAX> *pOutAry );
	static int sGetAvailableUnitByLevel( int level, XVector<XGAME::xtUnit> *pOutAry );
	static int sGetMilitaryPower( XSPLegion spLegion );
	static int sGetMilitaryPower( XSPHero pHero );
	static int sGetMilitaryPowerMax( int lvAcc );
//	static XSPLegion sCreateLegionWithDat( const xnLegion::xLegionDat& legionDat, XSPAccConst spAcc );
private:
	ID m_snLegion;
	XSPHero m_pLeader = nullptr;		// 널이 될수 있음.
	XList4<XSPSquadron> m_listSquadrons;		// 부대 리스트
	XList4<XSPSquadron> m_listFogs;
	XGAME::xtGradeLegion m_gradeLegion = XGAME::xGL_NORMAL;
	float m_RateHp = 1.f;	// 인게임전투시 유닛들에게 곱해질 hp배율
	float m_RateAtk = 1.f;	// 인게임전투시 유닛들에게 곱해질 공격력배율
	std::vector<ID> m_aryResourceHero;	// 자원부대영웅의 sn
	void Init() {
		m_snLegion = 0;
		m_snLegion = XE::GenerateID();
	}
	void Destroy();
	void Clear();
public:
	XLegion();
	XLegion( ID snLegion ) {
		Init();
		m_snLegion = snLegion;
	}
	~XLegion() { Destroy(); }
	//
// 	GET_ACCESSOR( const XVector<XSPSquadron>&, arySquadrons );
	GET_ACCESSOR_CONST( const XList4<XSPSquadron>&, listSquadrons );
	GET_ACCESSOR_CONST( ID, snLegion );
	GET_SET_ACCESSOR( XSPHero, pLeader );
	GET_ACCESSOR_CONST( XGAME::xtGradeLegion, gradeLegion );
	GET_ACCESSOR_CONST( const std::vector<ID>&, aryResourceHero );
	int GetPower() const;
	void SetAryResource( std::vector<ID>& arySrc ) {
		m_aryResourceHero = arySrc;
	}
	void SetgradeLegion( XGAME::xtGradeLegion gradeLegion ) {
		m_gradeLegion = gradeLegion;
		XBREAK( XGAME::IsInvalidGradeLegion(gradeLegion) );
	}
	GET_SET_ACCESSOR_CONST( float, RateHp );
	GET_SET_ACCESSOR_CONST( float, RateAtk );
	void Serialize( XArchive& ar ) const;
	void SerializeFull( XArchive& ar );
	BOOL DeSerialize( XArchive& ar, XSPAccConst spAcc, int verLegion );
private:
	BOOL DeSerializeFull( XArchive& ar, int verLegion );
	int SerializeFogs( XArchive& ar ) const;
	int DeSerializeFogs( XArchive& ar, int verLegion );
public:
	XLegion* CreateLegionForLink( XSPAcc spAcc );
	void SetAutoLeader();
	// 부대 수를 얻는다.
	inline int GetNumSquadrons() {
		return m_listSquadrons.size();
	}
	XSPSquadron GetpmSquadronByidxPos( int idxSquad );
	const XSPSquadron GetpSquadronByidxPos( int idxSquad ) const;
	const XSPSquadron GetpSquadronBySN( ID snSquad ) const;
	XSPSquadron GetpmSquadronBySN( ID snSquad );
	void AddSquadron( int idxPos, XSPSquadron pSq, bool bCreateHero );
	void DestroySquadron( ID idSquad );
	XSPSquadron GetSquadronByHeroSN( ID snHero ) const;
	int _GetIdxSquadByHeroSN( ID snHero ) const;
	void SwapSlotSquad( XSPHero pHeroSrc, XSPHero pHeroDst );
	void SwapSlotSquad( int idxSrc, int idxDst );
	void DestroySquadBysnHero( ID snHero );
	void DestroySquadByIdxPos( int idx );
	int GetHerosToAry( XVector<XSPHero> *pOutAry );
	int GetHeroesToList( XList4<XSPHero>* pOutList );
	void AdjustLegion();
	int DeserializeForGuildRaid( XArchive& ar );
	void SetResourceSquad( int numSquad );
	void ClearResourceSquad();
	int GetNumDeadSquadByResourceSquad();
	bool IsSettedResourceSquad();
	bool IsResourceSquad( ID snHeroSrc ) const;
	void MakeFogs( int numVisible = 3 );
	bool IsFog( ID snHero ) const;
	void ClearFogs() {
		m_listFogs.clear();
	}
	int GetUnFogList( std::vector<ID> *pOutAry );
	void SetUnFogList( const std::vector<ID>& aryUnFogHeroSN );
	int GetNumFogs();
	int GetMaxFogs( int numVisible );
	bool DelFogSquad( ID snHero );
	XSPHero GetpHeroBySN( ID snHero ) const;
	XSPHero GetpHeroByIdxPos( int idxSquad );
	int GetIdxSquadByLeader();
	bool ChangeHeroInSquad( XSPHero pHeroFrom, XSPHero pHeroTo );
	/// 영웅이 군단에 속해있는지.
	inline bool IsEnteredHero( ID snHero ) const {
		return GetpHeroBySN( snHero ) != nullptr;
	}
	inline int GetMaxSquadSlot() const {
		return XGAME::MAX_SQUAD;
//		return m_arySquadrons.GetMax();
	}
	float GethpMaxEach( ID snSquadHero, bool bHero ) const;
//	bool IsValid() const;
	bool IsNpc() const;
	XSPSquadron CreateAddSquadron( int idxSquad, XSPHeroConst pHero, bool bCreateHero );
	void DestroySquadronAll();
	void sDeserialize( XSPLegion spLegionTarget, XSPAccConst spAcc, XArchive& ar );
private:
};

