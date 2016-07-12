/********************************************************************
	@date:	2014/11/07 19:10
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XPropSquad.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XFramework/XEBaseProp.h"
#include "XPropUpgrade.h"

const int VER_PROP_SQUAD = 1;

class XPropSquad : public XEBaseProp
{
public:
	struct xPROP {
		int level = 0;					///< 부대레벨
		int levelLimitByHero = 0;		///< 영웅에의한 레벨제한
		int maxSmall = 0;				///< 소형유닛 수
		int maxMiddle = 0;				///< 중형유닛수
		XVector<float> aryMultiplyRatioMiddle;		///< 중형유닛의 능력치 배수
		float avgMultiplyByMiddle = 0.f;		// 중형유닛의 평균 능력치 배수.
		int maxBig = 1;					///< 대형 유닛수
		float multiplyRatioBig = 0;		///< 대형유닛 능력치 배수
		int expMax = 0;					///< 레벨업에 필요한 exp max
		xPROP() : aryMultiplyRatioMiddle(4) {}
		int GetMaxUnit( XGAME::xtUnit unit ) const {
			switch( XGAME::GetSizeUnit( unit ) ) {
			case XGAME::xSIZE_SMALL:	return maxSmall;
			case XGAME::xSIZE_MIDDLE:	return maxMiddle;
			case XGAME::xSIZE_BIG:		return maxBig;
			default:
				XBREAK(1);
				break;
			}
			return 1;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
private:
	XArrayLinearN<xPROP, 17> m_aryProp;				///< 전체 프로퍼티 배열
	float m_goldPerExp = 0;		// 경험치당 금화
	float m_secPerExp = 0;		// 경험치당 훈련시간
	int m_lvLast = 0;

	void Init() {}
	void Destroy();

public:
	XPropSquad();
	virtual ~XPropSquad() { Destroy(); }

//	GET_ACCESSOR( XArrayLinearN<xPROP*, 17>&, aryProp );
	
	BOOL ReadProp( CToken& token, DWORD dwParam=0 ) override;					///< txt 파싱

	/**
	 @brief level에 해당하는 부대테이블을 꺼내준다.
	*/
	const xPROP& GetTable( int level ) {
		if( XBREAK(level <= 0 || level >= m_aryProp.size()) )
			return m_aryProp[0];
		return m_aryProp[level];
	}
	// null이면 범위 초과
	const xPROP* GetpTable( int level ) {
		if( level <= 0 || level >= m_aryProp.size() )
			return nullptr;
		return &m_aryProp[ level ];
	}
	const xPROP* GetpTableNextLevel( int levelCurr ) {
		int levelNext = levelCurr + 1;
		if( levelNext <= 0 || levelNext >= m_aryProp.size() )
			return nullptr;
		return &m_aryProp[ levelNext ];
	}
	const xPROP* GetpTableNextNextLevel( int levelCurr ) {
		int levelNext = levelCurr + 2;
		if( levelNext <= 0 || levelNext >= m_aryProp.size() )
			return nullptr;
		return &m_aryProp[ levelNext ];
	}
	void Add( const xPROP& prop );
	
	virtual int GetSize( void ) { return m_aryProp.size(); }
	int GetMaxLevel( void ) {
		return m_aryProp.size() - 1;
	}
	int GetLevelSquadMaxByHero( int lvHero );
	int GetMaxExp( int lvCurr ) {
		int levelNext = lvCurr + 1;
		if( XBREAK(levelNext <= 0 || levelNext >= m_aryProp.size()) )
			return 0;
		return m_aryProp[ levelNext ].expMax;
	}
	void OnDidBeforeReadProp( CToken& token ) override;
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
	float GetMulByLvSquad( XGAME::xtSize size, int idxUnit, int lvSquad );
	float GetAvgMulByLvSquad( XGAME::xtSize size, int lvSquad );
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
};

extern XPropSquad *PROP_SQUAD;