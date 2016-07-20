#pragma once
#include "XHeroH.h"
#include "XFramework/Game/XFLevelH.h"
#include "XPropHero.h"
//#include "skill/XSkillDat.h"
//#include "skill/XAdjParam.h"
#include "XPropSquad.h"
#include "XPropUpgrade.h"
#include "XStruct.h"
#if defined(_CLIENT) || defined(_GAME_SERVER)
#include "XPropTech.h"
#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

class XArchive;
class XBaseItem;
class XAccount;
namespace XGAME {
	struct xAbil;
};
namespace XSKILL {
class XSkillDat;
class XAdjParam;
}

////////////////////////////////////////////////////////////////
// class XHero : public XSKILL::XAdjParam
class XHero : public XDelegateLevel
{
public:
	static int sSerialize( XArchive& ar, XHero *pHero );
	static XHero* sCreateDeSerialize( XArchive& ar, XSPAcc spAcc );
	static int sGetMaxExpWithLevel( XGAME::xtTrain type, int level );
#if defined(_XSINGLE) || !defined(_CLIENT)
	static XHero* sCreateHero( XPropHero::xPROP *pProp, int levelSquad, XGAME::xtUnit unit );
//	static XHero* sCreateHero( XPropHero::xPROP *pProp, XGAME::xtUnit unit, int numUnit );
//	static XHero* sCreateHero( ID idProp, XGAME::xtUnit unit, int numUnit );
#endif // defined(_XSINGLE) || !defined(_CLIENT)
	struct xItem {
	private:
		ID snItem = 0;
		XBaseItem *pItem = nullptr;
	public:
		void Set( XBaseItem *_pItem );
		XBaseItem* GetpItem();
		ID GetsnItem() {
			return snItem;
		}
	};
#ifdef _CLIENT
//	static volatile float s_fMultiply;				// 메모리 치팅방지
#endif // _CLIENT
private:
//	std::shared_ptr<XSKILL::XAdjParam> m_spAdjParam;
	//
	ID m_idProp = 0;      // m_pProp을 교체해야할때 필요함.
	ID m_keyPropHero = 0;
	XPropHero::xPROP *_m_pProp = nullptr;
	ID m_snHero;
	std::vector<xnHero::xUpgrade> m_aryUpgrade;		// 업글3종세트의 정보가 담김
	XGAME::xtGrade m_Grade = XGAME::xGD_COMMON;		// 생성되면 1성부터 시작함.
	XArrayN<xItem, XGAME::xPARTS_MAX> m_aryEquip;
	XGAME::xtUnit m_Unit;		// 인솔중인 유닛
#if defined(_CLIENT) || defined(_GAME_SERVER)
	ID m_keySkillMng = 0;
	XSKILL::XSkillDat *m_pPassive = nullptr;
	XSKILL::XSkillDat *m_pActive = nullptr;
#endif
	bool m_bLive = true;			///< 레이드등에서 영웅의 사망여부를 알수있도록.
// 	XArrayLinearN<std::map<ID, XGAME::xAbil>, XGAME::xUNIT_MAX> m_aryUnitsAbil;		///< 모든 유닛들의 특성치 포인트(특성아이디로 특성포인트를 검색한다
	XVector<std::map<ID, XGAME::xAbil>> m_aryUnitsAbil;		///< 모든 유닛들의 특성치 포인트(특성아이디로 특성포인트를 검색한다
	int m_numInitAbil = 0;			// 특성초기화 한 횟수
	int m_numRemainAbilPoint = 0;		// 남은 특성포인트 수
	int m_numRemainAbilUnlock = 0;	// 남은 특성언락기회 횟수
	void Init() {
		m_snHero = 0;
		m_Unit = XGAME::xUNIT_NONE;
		InitAryAbil();
 		for (int i = 0; i < XGAME::xPARTS_MAX; ++i)
 			m_aryEquip[i].Set( nullptr );
		for( int i = 0; i < XGAME::xTR_MAX; ++i ) {
			if( i > 0 ) {
				m_aryUpgrade[ i ].SetpDelegateLevel( this, i );
			}
		}
	}
	void Destroy();
public:
	XHero();
	XHero( XPropHero::xPROP *pProp, int levelSquad, XGAME::xtUnit unit );
	virtual ~XHero() { Destroy(); }
	//
//	GET_ACCESSOR( std::shared_ptr<XSKILL::XAdjParam>, spAdjParam );
	GET_SET_ACCESSOR( XGAME::xtUnit, Unit );
	int GetnumUnit();
	GET_SET_ACCESSOR_CONST( bool, bLive );
	inline int GetLevel( XGAME::xtTrain type ) const {
		if( XASSERT(XGAME::IsValidTrainType( type )) )
			return m_aryUpgrade[ type ].GetLevel();
		return 0;
	}
	inline int GetLevel() const {
		return m_aryUpgrade[ XGAME::xTR_LEVEL_UP ].GetLevel();
	}
	inline int GetLevelHero() const {
		return m_aryUpgrade[ XGAME::xTR_LEVEL_UP ].GetLevel();
	}
	inline void SetLevel( XGAME::xtTrain type, int level ) {
		m_aryUpgrade[ type ].SetLevel( level );
	}
	inline void SetLevel( int level ) {
		SetLevel( XGAME::xTR_LEVEL_UP, level );
	}
	inline void SetlevelSquad( int level ) {
		SetLevel( XGAME::xTR_SQUAD_UP, level );
	}
	ID GetsnHero( void ) {
		return m_snHero;
	}
	ID getid() {
		return m_snHero;
	}
  XPropHero::xPROP* GetpProp();
  void SetpProp( XPropHero::xPROP* pProp, ID idKey );
  void SetpProp( ID idProp );
  GET_ACCESSOR( ID, idProp );
  GET_SET_ACCESSOR( XGAME::xtGrade, Grade );
	/// 영웅등급
	const _tstring& GetstrIdentifer() {
		return GetpProp()->strIdentifier;
	}
	const _tstring& GetstrName() {
		return TEXT_TBL->GetstrText( GetpProp()->idName );
	}
	XGAME::xtSize GetSizeUnit() {
		return XGAME::GetSizeUnit( m_Unit );
	}
#ifdef _CLIENT
	GET_SET_ACCESSOR_CONST( int, numRemainAbilPoint );
	GET_SET_ACCESSOR_CONST( int, numRemainAbilUnlock );
	GET_SET_ACCESSOR_CONST( int, numInitAbil );
#else
	GET_ACCESSOR_CONST( int, numRemainAbilPoint );
	GET_ACCESSOR_CONST( int, numRemainAbilUnlock );
	GET_ACCESSOR_CONST( int, numInitAbil );
#endif // _CLIENT
	void AddRemainAbilPoint( int add );
	void AddRemainAbilUnlock( int add );
	void AddcntInitAbil( int add );
	//
	float GetAttackMeleeRatio( int lvHero, bool bForShow = false );
	float GetAttackRangeRatio( int lvHero, bool bForShow = false );
	float GetDefenseRatio( int lvHero, bool bForShow = false );
	float GetHpMaxRatio( int lvHero, bool bForShow = false );
	float GetAttackSpeed( int lvHero, bool bForShow = false );
	float GetMoveSpeed( int lvHero, bool bForShow = false ); 
	/// 장비의 효과가 적용된 스탯치
	float GetAttackMeleeRatio( bool bForShow = false ) {
		return GetAttackMeleeRatio( GetLevelHero(), bForShow );
	}
	float GetAttackRangeRatio(  bool bForShow = false ) {
		return GetAttackRangeRatio( GetLevelHero(), bForShow );
	}
	float GetDefenseRatio( bool bForShow = false ) {
		return GetDefenseRatio( GetLevelHero(), bForShow );
	}
	float GetHpMaxRatio( bool bForShow = false ) {
		return GetHpMaxRatio( GetLevelHero(), bForShow );
	}
	/// 몇초에 한번씩 공격하는가
	float GetAttackSpeed( bool bForShow = false ) {
		return GetAttackSpeed( GetLevelHero(), bForShow );
	}
	float GetMoveSpeed( bool bForShow = false ) {
		return GetMoveSpeed( GetLevelHero(), bForShow );
	}
	//
	float GetAttackRadiusRatio( void ) {
//		return GetpProp()->radiusAtk;	// 사거리는 레벨증가에 따른 증가치 없음.
		return 1.f;		// 사거리는 일단 증폭이 안되는걸로 함.
	}
	/// 일단 지휘력은 보정값이 없는걸로 하고 추가 패치하자
	BOOL IsRange( void ) {
		return GetpProp()->IsRange();
	}
	/**
	 @brief 스킬 포인터를 꺼냄
	*/
#if defined(_CLIENT) || defined(_GAME_SERVER)
	XSKILL::XSkillDat* GetSkillDatPassive( void );
	XSKILL::XSkillDat* GetSkillDatActive( void );
	XSKILL::XSkillDat* GetSkillDat( XGAME::xtTrain type );
	void AssignSkillPtr( void );
	int GetTrainExpRemain( XGAME::xtTrain typeTrain );
#endif 
	//
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar, XSPAcc spAcc, int verHero );
	int SerializeUpgrade( XArchive& ar ) const;
	int DeSerializeUpgrade( XArchive& ar );
	//
	void SetUnequipAll( void );
	BOOL SetUnequip(XBaseItem *pItem);
	BOOL SetEquip(XBaseItem *pItem);
	XBaseItem* GetEquipItem(XGAME::xtParts parts);

	///< level에서의 최대 exp의 값을 돌려줘야 한다.
	virtual DWORD OnDelegateGetMaxExp( XFLevel *pLevel, int level, DWORD param1, DWORD param2 );
	///< 최대 레벨값을 돌려줘야 한다.
	virtual int OnDelegateGetMaxLevel( XFLevel *pLevel, DWORD param1, DWORD param2 );
//	float GetSquadAttackMeleePower();
	void GetSquadStat( int levelSquad, xSquadStat *pOut );
	float GetAdjParamByItem( XGAME::xtParameter adjParam );
	void UpdateEquipItem( ID snItem, XBaseItem *pNewItem );
	bool IsEquip( ID snItem );
	const XPropUpgrade::xPropSkill* GetpPropSkillupNext( XGAME::xtTrain type ) const;
	const XPropUpgrade::xPropSkill* GetpPropSkillupNextNext( XGAME::xtTrain type ) const;
	const XPropSquad::xPROP* GetpPropSquadupNext() const;
	const XPropSquad::xPROP* GetpPropSquadupNextNext() const;
	bool IsFullMedal();
	bool IsFullScroll( XGAME::xtTrain type );
	// 렙업이 가능한 상태인가
	bool IsAbleLevelup( XGAME::xtTrain typeTrain ) {
		auto& upgrade = m_aryUpgrade[ typeTrain ];
		return upgrade.GetLevel() < upgrade.GetMaxLevel();
	}
	/// 부대렙업이 가능한 상태인지
	bool IsMaxLevel( XGAME::xtTrain type ) const;
	inline XGAME::xtAttack GetType() {
		return XGAME::GetTypeUnit( m_Unit );
	}
	inline XGAME::xtAttack GetTypeAtk() {
		return XGAME::GetTypeUnit( m_Unit );
	}
	bool IsHaveSkill( XGAME::xtTrain type ) {
		if( XBREAK(XGAME::IsInvalidSkillTrain(type)) )
			return false;
		if( type == XGAME::xTR_SKILL_ACTIVE_UP )
			return !GetpProp()->strActive.empty();
		else
			return !GetpProp()->strPassive.empty();
	}
	int IsMaxLevelLevel() {
		return IsMaxLevel( XGAME::xTR_LEVEL_UP );
	}
	XGAME::xtError DoPromotion();
	int GetMaxLevel() const;
	int GetMaxLevel( XGAME::xtTrain type ) const;
	_tstring GetsidSkill( XGAME::xtIdxSkilltype idxType );
	int GetMaxExp( XGAME::xtTrain type );
	int GetMaxExpWithLevel( XGAME::xtTrain type, int level ) const {
		return sGetMaxExpWithLevel( type, level );
	}
// 	void SetbAutoLevelup( XGAME::xtTrain type, bool flag ) {
// 		m_aryUpgrade[ type ].m_Level.SetbAutoLevelup( flag );
// 	}
	bool AddExp( XGAME::xtTrain type, int add ) {
		int lvOld = m_aryUpgrade[ type ].m_Level.GetLevel();
		m_aryUpgrade[ type ].m_Level.AddExp( add );
		// 렙업했으면  true
		return m_aryUpgrade[ type ].m_Level.GetLevel() != lvOld;
	}
	bool IsFullExp( XGAME::xtTrain type ) {
		return m_aryUpgrade[ type ].m_Level.IsFullExp();
	}
	inline int GetlvPassive() {
		return GetLevel( XGAME::xTR_SKILL_PASSIVE_UP );
	}
	inline int GetlvActive() {
		return GetLevel( XGAME::xTR_SKILL_ACTIVE_UP );
	}
	bool GetbLevelUpAndClear( XGAME::xtTrain type ) {
		return m_aryUpgrade[ type ].m_Level.GetbLevelUpAndClear();
	}
	int GetlevelSquad() {
		return GetLevel( XGAME::xTR_SQUAD_UP );
	}
	int GetExp() {
		return m_aryUpgrade[ XGAME::xTR_LEVEL_UP ].GetExp();
	}
	int GetMaxExpCurrLevel() {
		return m_aryUpgrade[ XGAME::xTR_LEVEL_UP ].GetMaxExpCurrLevel();
	}
	int GetExp( XGAME::xtTrain type ) {
		return m_aryUpgrade[ type ].GetExp();
	}
	int GetMaxExpCurrLevel( XGAME::xtTrain type ) {
		return m_aryUpgrade[ type ].GetMaxExpCurrLevel();
	}
	int GetExpRemain( XGAME::xtTrain type ) {
		return GetMaxExpCurrLevel( type ) - GetExp( type );
	}
	XINT64 GetExpSum() {
		return m_aryUpgrade[ XGAME::xTR_LEVEL_UP ].GetExpSum();
	}
	XFLevel GetLevelObj( XGAME::xtTrain type ) const {
		return m_aryUpgrade[ type ].m_Level;
	}
	int GetExpWithLevel( XGAME::xtTrain typeTrain, int lvHero ) {
//		return m_aryUpgrade[ typeTrain ].m_Level.getexp
		return 0;
	}
	int GetLvLimitByAccLv( int lvAcc, XGAME::xtTrain type );
	bool IsActivationSkill( XGAME::xtTrain type );
	XGAME::xtError _IsPromotionForXAccount();
	// 특성
	const XGAME::xAbil GetAbilNode(XGAME::xtUnit unit, ID idNode);
	inline std::map<ID, XGAME::xAbil>& GetTechTree( XGAME::xtUnit unit ) {
		return m_aryUnitsAbil[ unit ];
	}
	inline const std::map<ID, XGAME::xAbil>& GetmapAbil( XGAME::xtUnit unit ) {
		return m_aryUnitsAbil[ unit ];
	}
	bool SetUnlockAbil( XGAME::xtUnit unit, ID idNode );
	void SetAbilPoint( XGAME::xtUnit unit, ID idNode, int point );
	int GetLevelAbil( XGAME::xtUnit unit, ID idNode ) {
		auto abil = GetAbilNode( unit, idNode );
		return abil.point;
	}
#if defined(_CLIENT) || defined(_GAME_SERVER)
	// abil, PropTech
	void SetAbilPoint( const _tstring& idsAbil, int point );
	int AddAbilPoint( XGAME::xtUnit unit, ID idAbil );
	float GetAdjByTech(XGAME::xtUnit unt, XGAME::xtParameter adjParam, float baseStat);
	float GetSquadAttackMeleePower( int levelSquad);
	float GetSquadAttackRangePower( int levelSquad);
	float GetSquadDefensePower( int levelSquad);
	float GetSquadMaxHp( int levelSquad);
	float GetSquadSpeedAtk( int levelSquad);
	float GetSquadSpeedMove( int levelSquad);
	void GetSquadStatWithTech( int levelSquad, xSquadStat *pOut);
	/// unit의 특성 idNode의 현재 특성레벨을 얻는다.
 	int GetLevelAbil(XGAME::xtUnit unit, LPCTSTR sid);
	int GetNumUnlockAbil();
 	int GetGoldUnlockAbilCurr();
	bool IsEnableAbil( XGAME::xtUnit unit, XPropTech::xNodeAbil *pProp );
	XPropTech::xtResearch& GetCostAbilCurr();
	bool DoInitAbil();
private:
public:
#endif // defined(_CLIENT) || defined(_GAME_SERVER)
	int GetNumSetAbilPoint();
	int GetNumSetAbilPointByUnit( XGAME::xtUnit unit );
	void InitAbilMap();
#if defined(_GAME_SERVER) && defined(_DEV)
	bool RecursiveAbilPointRandom( XGAME::xtUnit unit, XPropTech::xNodeAbil *pRoot );
	void GenerateAbilityForDummy( XGAME::xtUnit unit, int lvAcc );
#endif // defined(_GAME_SERVER) && defined(_DEV)
	int SerializeAbil( XArchive& ar );
	int DeserializeAbil( XArchive& ar, int verHero );
	bool IsResearbleUnit( XGAME::xtUnit unit );
	int GetCostInitAbil();
	int GetNumAbilPoint( XGAME::xtUnit unit, ID idAbil );
	float GetSquadPower( float stat, XGAME::xtSize size, int levelSquad );
	float GetAttackMeleePowerSquad( int levelSuqad );
	float GetAttackRangePowerSquad( int levelSquad );
	int GetMaxHpSquad( int levelSquad );
	float GetDefensePowerSquad( int levelSquad );
private:
	void InitAryAbil();
	BOOL IsEmptyAbilMap();
};

