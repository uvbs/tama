/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XFramework/XEBaseProp.h"

const int VER_PROP_UPGRADE = 1;

class XPropUpgrade : public XEBaseProp
{
public:
	static std::shared_ptr<XPropUpgrade> sGet();
  static void sDestroy() {
    s_spSingleton.reset();
  }
public:
	static std::shared_ptr<XPropUpgrade> s_spSingleton;
	struct xPropSkill {
		int levelLimitByHero = 0;	// 영웅렙 제한
		int expMax = 0;				// 다음레벨로 가기위한 경험치
		void Serialize( XArchive& ar ) const {
			ar << levelLimitByHero;
			ar << expMax;
		}
		void DeSerialize( XArchive& ar, int ) {
			ar >> levelLimitByHero;
			ar >> expMax;
		}
	};
private:
	std::vector<xPropSkill> m_aryTableSkill;				///< 스킬업글 테이블
	int m_lvLast = 0;
	float m_goldPerExp = 0;		// 경험치당 금화
	float m_secPerExp = 0;		// 경험치당 훈련시간
	void Init() {}
	void Destroy();
public:
	XPropUpgrade();
	virtual ~XPropUpgrade() { Destroy(); }
	BOOL ReadProp( CToken& token, DWORD dwParam ) override;					///< txt 파싱
// 	const xPropLevel* GetpTableForLevelup( int level );
	const xPropSkill* GetpTableForSkillup( int level );
	const xPropSkill* GetpTableForSkillupNext( int levelCurr ) const;
// 	virtual int GetSize( void ) { return m_aryTableLevel.size(); }
	int GetSize( void ) override { return m_aryTableSkill.size(); }
	void OnDidBeforeReadProp( CToken& token ) override;
// 	BOOL ReadPropUpgradeLevel( CToken& token );
	BOOL ReadPropUpgradeSkill( CToken& token );
// 	void AddLevelTbl( int level, int wood, int iron, XGAME::xtClan );
//	void AddSkillTbl( int level, xPropSkill& prop );
	int GetLevelSkillMaxByHero( int lvHero );
	int GetMaxExp( int lvUpgrade ) {
		return m_aryTableSkill[ lvUpgrade ].expMax;
	}
	// gold로 얻을 수 있는 exp양
	int GetExpTrainByGold( int gold ) {
		return (int)(gold / m_goldPerExp);
	}
	// exp만큼 훈련을 시키는데 필요한 금액을 알려준다.
	int GetGoldByExp( int exp ) {
		return (int)(exp * m_goldPerExp);
	}
	// exp만큼 훈련을 시키는데 필요한 시간을 알려준다.
	int GetSecByExp( int exp ) {
		return (int)(exp * m_secPerExp);
	}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
};

