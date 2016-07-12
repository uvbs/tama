#pragma once
#include "XStruct.h"

class XExpTableHero
{
	XVector<XGAME::xExpTable> m_Table;
	void Init() {
	}
	void Destroy() {}
public:
	XExpTableHero() 
		: m_Table( XGAME::MAX_HERO_LEVEL + 1) { 
		Init(); 
	}
	virtual ~XExpTableHero() { Destroy(); }
	//
	/**
	 @brief level레벨의 최대경험치를 구한다.
	*/
	DWORD GetMaxExp( int level ) {
		return m_Table[ level ].m_dwMaxExp;
	}
	/**
	 @brief 최대가능 레벨을 얻는다
	*/
	int GetMaxLevel( void ) {
		return m_Table.size() - 1;
	}
	//
	BOOL Load( LPCTSTR szFile );
	// gold로 얻을 수 있는 exp양
	int GetExpTrainByGold( int gold, int lvHero ) {
		const auto goldPerExp = m_Table[ lvHero ].m_goldPerExp;
		float v = gold / goldPerExp;
		if( v < 1.f )
			return 0;	// 이경우는 훈련이 안되게 해야함.
		if( v )
			v = (float)ROUND_FLOAT( v, 0 );
		return (int)v;
	}
	// exp만큼 훈련을 시키는데 필요한 금액을 알려준다.
	int GetGoldByExp( int exp, int lvHero ) {
		const auto goldPerExp = m_Table[ lvHero ].m_goldPerExp;
		float v = exp * goldPerExp;
		if( v < 1.f )	// 금액이 100원이하면 최소 100원으로 한다.
			return 0;
		if( v )
			v = (float)ROUND_FLOAT( v, 0 );
		return (int)(v);
	}
	// exp만큼 훈련을 시키는데 필요한 시간을 알려준다.
	int GetSecByExp( int exp, int lvHero ) {
		const auto secPerExp = m_Table[ lvHero ].m_secTrainPerExp;
		float sec = exp * secPerExp;
		if( sec < 1.f )	// 훈련시간이 10초미만이면 최소 10가 되도록 한다.
			sec = 0;	// 0초는 즉시 완료
		return (int)sec;
	}
	int GetGoldByLvUp( int lv ) const;
};

extern XExpTableHero *EXP_TABLE_HERO;
