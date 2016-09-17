#pragma once
//#include "XHero.h"
#include "XPropHero.h"
//#include "XPropLegionH.h"
namespace XGAME {
struct xSquad;
struct xLegion;
}
class XArchive;
class XAccount;
class XSquadron;
class XHero;
#define RATE_ATK_DEFAULT_ELITE		0.2f
#define RATE_HP_DEFAULT_ELITE		5.f
#define RATE_ATK_DEFAULT_RAID		0.5f
#define RATE_HP_DEFAULT_RAID		100.f
////////////////////////////////////////////////////////////////
class XLegion
{
public:
	enum {
		xMAX_NOT_FOG = 3,	// 안개로가려지지 않는 부대수
	};
	static int sSerializeFull( XArchive& ar, XSPLegion spLegion );
	static XLegion* sCreateDeserializeFull( XArchive& ar );
//	static XLegion* sCreateDeserializeForGuildRaid( XArchive& ar );
	static XLegion* sCreateLegionForNPC( int lvAcc, int adjDiff, 
																		XGAME::xLegionParam& legionInfo );
	static XSPLegion sCreateLegionForNPC2( XGAME::xLegion& legion, int lvExtern, bool bAdjustLegion );
//	static XSPLegion sCreatespLegion( const XGAME::xLegion& legion, int lvExtern, bool bReArrangeSquad, XVector<XHero*>* pOutAry );
#if defined(_XSINGLE) || !defined(_CLIENT)
	static XSquadron* sCreateSquadronForNPC( int levelLegion, int adjDiff, XGAME::xtUnit unit, ID idHero=0, int _levelHero=0, int levelSkill=0, int _levelSquad=0, XGAME::xtGradeLegion gradeLegion = XGAME::xGL_NORMAL, const XGAME::xLegionParam *pLegionInfo = nullptr );
	static XSquadron* sCreateSquadronForNPC2( const int levelLegion
																					, XPropHero::xPROP *pPropHero
																					, const XGAME::xSquad& sqParam );
	static XSquadron* sCreateSquadronForNPC2( const int levelLegion
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
	static void sSetLeaderByInfo( const XGAME::xLegion* pxLegion, XSPLegion spLegion, XSquadron *pSquad );
	static XGAME::xtUnit sCreateUnitType( int spawnType );
//	static int sCreateUnitNum( XGAME::xtUnit unit, BOOL bRandom );
	static int sGetNumUnitByLevel( XGAME::xtUnit unit, int lvUnit );
	static int sGetSquadMaxLevelByHeroLevel( int lvHero );
	static ID sGetHeroByUnit( XGAME::xtUnit unit );
	static XGAME::xtGrade sGetRandomGradeHeroByTable( int levelUser, XGAME::xtGradeLegion gradeLegion );
	static int sGetAvailableUnitByLevel( int level, XArrayLinearN<XGAME::xtUnit, XGAME::xUNIT_MAX> *pOutAry );
	static int sGetAvailableUnitByLevel( int level, XVector<XGAME::xtUnit> *pOutAry );
	static int sGetMilitaryPower( XSPLegion spLegion );
	static int sGetMilitaryPower( XHero *pHero );
	static int sGetMilitaryPowerMax( int lvAcc );
private:
	ID m_snLegion;
	XHero *m_pLeader = nullptr;		// 널이 될수 있음.
	XVector< XSquadron*> m_arySquadrons;	// 중대 리스트
	XList4<XSquadron*> m_listFogs;
	XGAME::xtGradeLegion m_gradeLegion = XGAME::xGL_NORMAL;
	float m_RateHp = 1.f;	// 인게임전투시 유닛들에게 곱해질 hp배율
	float m_RateAtk = 1.f;	// 인게임전투시 유닛들에게 곱해질 공격력배율
	std::vector<ID> m_aryResourceHero;	// 자원부대영웅의 sn
	void Init() {
		m_snLegion = 0;
		m_snLegion = XE::GenerateID();
	}
	void Destroy();
public:
	XLegion();
	XLegion( ID snLegion )
		: m_arySquadrons( XGAME::MAX_SQUAD ) {
		Init();
		m_snLegion = snLegion;
	}
	~XLegion() { Destroy(); }
	//
	GET_ACCESSOR( const XVector<XSquadron*>&, arySquadrons );
//	int GetSquadronToAry( XVector<XSquadron*>* pOut ) const;
	GET_ACCESSOR_CONST( ID, snLegion );
	GET_SET_ACCESSOR( XHero*, pLeader );
	GET_ACCESSOR_CONST( XGAME::xtGradeLegion, gradeLegion );
	GET_ACCESSOR_CONST( const std::vector<ID>&, aryResourceHero );
	int GetPower() const;
	void SetAryResource( std::vector<ID>& arySrc ) {
		m_aryResourceHero = arySrc;
	}
	void SetgradeLegion( XGAME::xtGradeLegion gradeLegion ) {
		m_gradeLegion = gradeLegion;
		XBREAK( XGAME::IsInvalidGradeLegion(gradeLegion) );
		// 이제 이값은 외부에서 직접 입력해야하도록 바뀜
// 		if( gradeLegion == XGAME::xGL_ELITE )
// 			m_RateHp = 10.f;
// 		else if( gradeLegion == XGAME::xGL_RAID )
// 			m_RateHp = 100.f;
	}
	GET_SET_ACCESSOR_CONST( float, RateHp );
	GET_SET_ACCESSOR_CONST( float, RateAtk );
	void Serialize( XArchive& ar );
	void SerializeFull( XArchive& ar );
	BOOL DeSerialize( XArchive& ar, XSPAcc spAcc, int verLegion );
private:
	BOOL DeSerializeFull( XArchive& ar, int verLegion );
	int SerializeFogs( XArchive& ar );
	int DeSerializeFogs( XArchive& ar, int verLegion );
public:
	XLegion* CreateLegionForLink( XSPAcc spAcc );
	void SetAutoLeader();
	// 부대 수를 얻는다.
	int GetNumSquadrons( void );
	XSquadron* GetSquadron( int idx ) {
		if( idx == -1 )
			return nullptr;
		if( idx < 0 || idx >= m_arySquadrons.GetMax() )
			return nullptr;
		return m_arySquadrons[idx];
	}
	inline XSquadron* GetpSquadronByIdx( int idx ) {
		return GetSquadron( idx );
	}
	BOOL SetSquadron( int idx, XSquadron *pSq, BOOL bCreateHero );
	bool SetSquadron( int idx, XSquadron *pSq, bool bCreateHero ) {
		return SetSquadron( idx, pSq, xboolToBOOL(bCreateHero) ) != FALSE;
	}
	XSquadron* GetSquadronByHeroSN( ID snHero ) const;
	int GetSquadronIdxByHeroSN( ID snHero ) const;
	inline int GetIdxSquadByHeroSN( ID snHero ) const {
		return GetSquadronIdxByHeroSN( snHero );
	}
	BOOL SwapSlotSquad( XHero *pHeroSrc, XHero *pHeroDst );
	BOOL SwapSlotSquad( int idxSrc, int idxDst );
//	void MoveSlotSquad( XHero *pHero, int idx )
	BOOL RemoveSquad( XHero *pHero );
	BOOL RemoveSquad( ID snHero );
	BOOL RemoveSquad( int idx );
	int GetHerosToAry( XArrayLinearN<XHero*, XGAME::MAX_SQUAD>& aryOut );
	int GetHerosToAry( XVector<XHero*> *pOutAry );
	int GetLevelUpHerosToAry( XArrayLinearN<XHero*, XGAME::MAX_SQUAD> *pAryOut );
	void AdjustLegion();
	void DummyDataRechange( int levelAcc, XAccount *pAcc );
	//BOOL SetSquadAll(int idx, XLegion *pLegion);
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
	XHero* GetpHeroBySN( ID snHero ) const;
	XHero* GetpHeroByIdxSquad( int idxSquad );
	int GetIdxSquadByLeader();
	bool ChangeHeroInSquad( XHero *pHeroFrom, XHero *pHeroTo );
	/// 영웅이 군단에 속해있는지.
	inline bool IsEnteredHero( ID snHero ) const {
		return GetpHeroBySN( snHero ) != nullptr;
	}
	inline int GetMaxSquadSlot() const {
		return m_arySquadrons.GetMax();
	}
	float GethpMaxEach( ID snSquadHero, bool bHero ) const;
//	bool IsValid() const;
	bool IsNpc() const;
};

