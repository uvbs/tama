#pragma once
#include "XSpot.h"
////////////////////////////////////////////////////////////////
/**
 요일별 이벤트 스팟
*/

class XSpotDaily : public XSpot
{
public:
	struct xDay {
		int m_numStar = 0;
		inline void Clear() {
			m_numStar = 0;
		}
	};
	static const int c_maxFloor;			// 최대 단계수
	static const int c_lvLegionStart;		// 시작 군단레벨
// 	enum {
// 		xNUM_ENTER=5,		// 입장 최대 횟수.
// 	};
	enum xtError {
		xERR_OK,
		xERR_TIMEOUT,			// 제한시간이 끝남
		xERR_OVER_ENTER_NUM,	// 입장횟수 초과
	};
private:
	CTimer m_timerCreate;		// 스팟 생성 타이머
	int m_numEnter = 0;
	XE::xtDOW m_dowToday = XE::xDOW_MONDAY;		// 어느 요일꺼냐
	int m_idxFloor = 0;		// 난이도 단계 인덱스
	int m_cntWeekByFloor = 0;		// 현재 단계에서 몇주째가 흘렀는지. floor변경없이 한주가 지나면 ++된다. 다음단계로 넘어가면 0으로 초기화 된다.
	XVector<xDay> m_aryDay;
	int m_cntTouch = 0;		// 궁여지책으로 만든변수. ㅠㅠ
	void Init() {}
	void Destroy() {
	}
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotDaily( XWorld *pWorld, XGAME::xtSpot typeSpot ) 
		: XSpot( pWorld, typeSpot ) 
		, m_aryDay( XE::xDOW_MAX ) {
		Init();
	}
public:
	XSpotDaily( XWorld *pWorld, XPropWorld::xDaily* pProp, XDelegateSpot *pDelegate )
		: XSpot( pProp, XGAME::xSPOT_DAILY, pWorld, pDelegate ) 
		, m_aryDay( XE::xDOW_MAX ) {
		Init();
	}
	// 요일을 넣는 버전
	XSpotDaily( XWorld *pWorld, XE::xtDOW dow, XPropWorld::xDaily* pProp, XDelegateSpot *pDelegate )
		: XSpot( pProp, XGAME::xSPOT_DAILY, pWorld, pDelegate ) 
		, m_aryDay( XE::xDOW_MAX ) {
		Init();
	}
	virtual ~XSpotDaily() { Destroy(); }
	//
	XPropWorld::xDaily* GetpProp() {
		return static_cast<XPropWorld::xDaily*>( GetpBaseProp() );
	}
	GET_ACCESSOR_CONST( const CTimer&, timerCreate );
	GET_SET_ACCESSOR_CONST( int, numEnter );
	GET_ACCESSOR_CONST( XE::xtDOW, dowToday );
	// 치트용
	void _SetdowToday( XSPAcc spAcc, XE::xtDOW dowToday );
	GET_ACCESSOR_CONST( int, idxFloor );
	void IncFloor() {
		++m_idxFloor;
		if( m_idxFloor >= c_maxFloor ) {
			m_idxFloor = c_maxFloor - 1;
		}
	}
	void DecFloor() {
		if( m_idxFloor > 0 )
			--m_idxFloor;
	}
//	GET_ACCESSOR_CONST( int, Day );
	bool IsClearDay( XE::xtDOW dow ) const {
		return m_aryDay[ dow ].m_numStar >= 3;
	}
	int GetnumStar( XE::xtDOW dow ) const {
		return m_aryDay[ dow ].m_numStar;
	}
	// 오늘만 빼고 모두 클리어한걸로 함.
#ifdef _GAME_SERVER
	void ClearCheat() {
		for( int i = 0; i < 7; ++i ) {
			if( i != (int)m_dowToday )
				m_aryDay[ i ].m_numStar = 3;
		}
		m_cntTouch = 2;
	}
#endif // _GAME_SERVER
	int GetRemainEnter() const;
	void AddnumEnter();
	void ClearnumEnter() {
		m_numEnter = 0;
	}
	BOOL IsActive() const override {
		return TRUE;
// 		return m_Type != XGAME::xDS_NONE;
	}
	// hPassHour이미 흘러간 시간
	void SetSpot( XE::xtDOW dow, int secPass, XSPAcc spAccount );
	//
	virtual void OnCreateNewOnServer( XSPAcc spAcc ) override;
	void Serialize( XArchive& ar );
private:
	BOOL DeSerialize( XArchive& ar, DWORD ver );
public:
	//
	void Process( float dt );
	void CreateLegion( XSPAcc spAccount ) override;
//	void ClearLegion();
	// 스팟을 해제시킨다.
// 	void ReleaseSpot() {
// 		m_Type = XGAME::xDS_NONE;	// 제한시간이 끝나 스팟이 사라짐
// 	}
// 	int GetLegionType() const {
// 		return (m_Day % 3) + 1;
// 	}
// 	XGAME::xtAttack GetAtkType() const {
// 		return (XGAME::xtAttack)(( m_Day % 3 ) + 1);
// 	}
	/**
	 스팟에 입장했다.
	*/
	xtError DoEnter();
	int DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum = 1.f ) const override;
	bool IsNpc() const override {
		return true;
	}
	bool IsPC() const override {
		return false;
	}
	bool Update( XSPAcc spAcc ) override;
	bool IsAttackable( XSPAcc spAcc, xtError *pOut = nullptr );
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat ) override;
	// 오늘 나와야할 유닛을 계산한다.
	XGAME::xtUnit GetUnitByDow( XE::xtDOW dow ) const;
	inline XGAME::xtUnit GetUnitByToday() const {
		return GetUnitByDow( GetdowToday() );
	}
	int GetlvLegionDow( XE::xtDOW dow ) const;
	int GetlvLegionCurrFloor() const;
	static bool sGetRewardDailyToday( XPropWorld::xDaily* pProp, int lvLegion, XVector<XGAME::xReward>* pOutAry );
	static bool sGetRewardDaily( XPropWorld::xDaily* pProp, XE::xtDOW dow, int lvLegion, XVector<XGAME::xReward>* pOutAry );
private:
	void ResetLevel( XSPAcc spAcc ) override;
	void ResetPower( int lvSpot ) override;
	void ResetName( XSPAcc spAcc ) override;
	// 현재 층에서 나타나야할 유닛 크기
	XGAME::xtSize GetSizeUnitByFloor() const {
		const int maxSize = XGAME::xSIZE_MAX - 1;
		return ( XGAME::xtSize )( ( m_idxFloor % maxSize ) + 1 );
	}
	void OnTouch( XSPAcc spAcc ) override;
}; // XSpotDaily

