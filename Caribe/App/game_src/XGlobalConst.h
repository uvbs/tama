
/********************************************************************
	@date:	2016/06/08 18:52
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XGlobalConst.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

////////////////////////////////////////////////////////////////
// 글로벌 상수모음.
class XGlobalConst;
class XGameCommon;
//extern XGlobalConst *XGC;
#include "XFramework/XGlobal.h"
#include "XFramework/XParamObj.h"

/**
 @brief 
*/
class XGlobalConst : public XGlobalVal
{
public:
private:
//	XGameCommon *m_pGameCommon;
	static XGlobalConst *s_pSingleton;
public:
//	static void sCreateSingleton( XGameCommon *pGameCommon );
	static const XGlobalConst* sGet();
	static XGlobalConst* sGetMutable();
	static void sDestroySingleton( void );
	// 자원을 금화로 환산.
	static int sGetGoldByTradeRes( XGAME::xtResource typeRes, int num );

	//////////////////////////////////////////////////////////////////////////
	// cheat
	_tstring m_strHero1;		// 1:1대전 왼쪽("random"은 랜덤, 비워두면 안됨.)
	_tstring m_strHero2;		// 오른쪽
	XGAME::xtUnit m_unitLeft = XGAME::xUNIT_NONE;	// none이면 랜덤.
	XGAME::xtUnit m_unitRight = XGAME::xUNIT_NONE;
// 	int m_numLeft;
// 	int m_numRight;
	int m_lvSquadLeft = 0;					// 0이면 디폴트값
	int m_lvSquadRight = 0;
	int m_lvHero1 = 0;
	int m_lvHero2 = 0;

	// cheat
	//////////////////////////////////////////////////////////////////////////
	//
	int m_gemFillDailyTry = 0;		// 요일스팟 도전횟수 리필시 젬개수.
	float m_rateFillDailyTry = 1.f;	// 구매시마다 증가되는 비율
	int m_numEnterDaily = 5;				// 요일스팟 1일 도전횟수.
	float m_rateShopSell;			//상점에 되팔때 얻는 골드양.
	std::string m_strSampleLua;
private:
	std::vector<float> m_aryRateMainLoss;		// 중앙창고 손실율
	std::vector<float> m_aryRateLocalLoss;		// 지역창고 손실율
public:
	float m_rateJewelStorageLoss;	// 보석광산 지역창고 손실율
	float m_rateLootResourceSquad = 0.25f;	// 루팅할수 있는 자원중 자원부대가 보유하고 있는 비율
	int m_expBattle;				///< 1회전투시 획득 경험치
	int m_expBattlePerHero;			///< 전투후 영웅들이 받게될 경험치
	XArrayLinearN<int,XGAME::xSIZE_MAX> m_aryAddBravePoint;
	float m_rateEquipDrop = 0.1f;	///< 장비아이템 드롭율
	int m_frEquipDropMax = 100000;	///< 확률 정밀도(10만분의 1)
	float m_rateKillDrop = 0.3f;		// 소탕권 드랍 확률
	float m_rateScalpDrop = 0.1f;		// 장비구입용 징표 드랍률
	int m_bravePointPerTech = 200;	///< 특성레벨당 필요 용맹포인트
	float m_rateDropLastChance = 0.5f;	///< 스팟에 드랍아이템을 가질수 있는 확률
	float m_cashPerAP = 1.f;			// ap1당 젬개수
	struct xEquipDrop {
		int lvStart = 0;	// 시작레벨
		int lvEnd = 0;		// 끝레벨
		XArrayLinearN<int, XGAME::xGD_MAX> aryDropRate;	// lvStart~lvEnd레벨대에서의 장비아이템 등급별 드롭율
	};
	XArrayLinearN<xEquipDrop ,5> m_aryDropRatePerLevel;	// 레벨대별 장비아이템 드롭율
	int m_apPerOnce = 1;				// 1회(m_secByAP)당 차오르는 ap양
	int m_secByAP = 60;					// ap가 차오르는 주기
	int m_expPerBook = 5000;			// 책당 경험치
private:
	int m_goldChangePlayer = 10000;   // 플레이어 교체 비용
public:
	int m_cashChangePlayer = 10;      // 플레이어 교체비용(캐쉬)
	xSec m_secMaxBattleSession = 180;   // 전투세션 최대 생존가능시간
	xSec m_secMaxBattle = 120;          // 유저의 최대 전투시간
	xSec m_secMaxBattleReady = 30;      // 전투대기씬 최대 시간
#if defined(_CLIENT) || defined(_GAME_SERVER)
	std::vector<int> m_aryMaxPowerPerLevel;     // 계정레벨별 가능한 최대 전투력
#endif
#ifdef _GAME_SERVER
	std::vector<int> m_aryNpcPowerPerLevel;		// npc레벨별 전투력테이블
	int GetNpcPowerWithLevel( int lvNpc ) const {
		XBREAK( lvNpc <= 0 || lvNpc > (int)( m_aryNpcPowerPerLevel.size() - 1 ) );
		return m_aryNpcPowerPerLevel[ lvNpc ];
	}
	int GetMaxNpcLevel() const {
		return m_aryNpcPowerPerLevel.size() - 1;
	}
#endif 
	int m_costMedalForArmoryHero = 50;		// 무기상영혼석 메달가격
private:
	// 가격얻기는 XGAME::GetCostGatcha()를 사용할것.
	int m_cashHighSummon;			///< 상급소환 캐쉬가격
	std::vector<_tstring> m_aryBgsBattle;		 // 전투배경 이미지
	XParamObj2 m_Param;		// key/val 방식의 다이나믹 파라메터
public:
	int m_cashShopReload;			//상점 목록 다시 부르기
	int m_armoryCallInterval;		// 무기상점 갱신 주기
	std::vector<float> m_aryGachaChanceNormal;
	std::vector<float> m_aryGachaChancePremium;
	std::vector<float> m_resourceRate;							// 무역상 자원 시세
	XArrayLinearN<int, 6> m_aryLevelsUnlockUnitForNotSmall;		// 중/대형유닛들의 언락포인트 받는 레벨들
	std::vector<int> m_aryAreaCost;	// 각 지역오픈 비용
	float m_rateDropSoulByCastle = 0;							// 성스팟 영혼석 드랍률
	struct xLegionTable {
		int m_lvLegion = 0;
		int m_numSquad = 0;
		int m_lvHero = 0;
		XGAME::xtGrade m_gradeHero = XGAME::xGD_NONE;
		int m_lvSquad = 0;
	};
#if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))
	// 더미계정 생성용 테이블
	struct xDummy {
		int m_lvAcc = 0;
		int m_numSquad = 0;
		int m_lvHero = 0;
		XGAME::xtGrade m_gradeHero = XGAME::xGD_NONE;
		int m_lvSquad = 0;
		int m_lvSkill = 0;
		int m_numAbilPerUnit = 0;	// 유닛당 특성 개수
		int m_numEquip = 0;
		XGAME::xtGrade m_gradeEquip = XGAME::xGD_NONE;
	};
	std::vector<xDummy> m_aryDummy;
	bool LoadDummyTable( CToken& token );
	const xDummy& GetDummyTbl( int lvAcc ) const {
		if( XASSERT( lvAcc > 0 && lvAcc <= (int)( m_aryDummy.size() - 1 ) ) ) {
			return m_aryDummy[ lvAcc ];
		}
		XBREAK( m_aryDummy.size() == 0 );
		return m_aryDummy[ 0 ];
	}
#endif // #if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))
private:
	std::vector<xLegionTable> m_aryLegionTable;		// npc군단 생성용 테이블
public:
	const xLegionTable& GetLegionTable( int lvLegion ) const;
	//
private:
	void Init() {
		m_expBattle = 0;
		m_expBattlePerHero = 0;
		m_cashHighSummon = 0;
		m_cashShopReload = 0;
		m_LogLevel = 0;
		m_rateShopSell = 0.1f;
		m_aryBgsBattle.resize( XGAME::xSPOT_MAX );
	}

	void Destroy() {
	}

public:
	XGlobalConst() 
		: m_aryGachaChanceNormal( XGAME::xGD_MAX )
		, m_aryGachaChancePremium( XGAME::xGD_MAX )
		, m_resourceRate( XGAME::xRES_MAX )
		, m_aryAreaCost( XGAME::MAX_AREA_LEVEL + 1 )	// 렙 50이상을 표현하기 위해 몇개 더 넣음.
		, m_aryLegionTable( XGAME::MAX_NPC_LEVEL + 1 )
		, m_aryRateMainLoss( 5 )
		, m_aryRateLocalLoss( 5 )
#if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))
		, m_aryDummy( XGAME::MAX_ACC_LEVEL + 1 )
#endif // #if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))
#if defined(_GAME_SERVER)
		, m_aryNpcPowerPerLevel( XGAME::MAX_NPC_LEVEL + 1 ) 
#endif
	{ 
		Init(); 
	}
	virtual ~XGlobalConst() { Destroy(); }
	///< 
//	GET_ACCESSOR_CONST( int, goldLowSummon );
//	GET_ACCESSOR_CONST( int, goldHighSummon );
	GET_ACCESSOR_CONST( int, cashHighSummon );
	//
	bool LoadTable( LPCTSTR szTxt );
	void LoadConst();
	void LoadDynaParam( XParamObj2* pOut );
public:
	//////////////////////////////////////////////////////////////////////////
	// jw
	// 귀중품 상점
	int m_secSubscribeCycle = 0;		// 월정액상품 지급 주기(초)
	int m_daysSubscribe = 0;		// 지급해야할 월정액상품 일(day) 수
	// 무역상 & 무기상 & 창고
	int m_traderRecallItem = 0;
//	int m_traderRecallGem = 0;
	int m_armoryRecallItem = 0;
	int m_armoryRecallGem = 0;
	int m_storageLockItem = 0;
	int m_storageLockGem = 0;
	// 무역상 & 귀중품 상점 호출 주기
//	int m_tradeCallInterval = 0;
	int m_shopCallInerval = 0;
	int m_LogLevel = 0;
	//////////////////////////////////////////////////////////////////////////
	// 캐쉬 아이템 정보
	struct cashItem {
		_tstring m_productID;		// 구글이나 애플 상품 ID
		XGAME::xtCashType m_type;					// 상품타입
		int m_gem;					// 젬 갯수(월정액일경우 하루에 지급할 젬개수)
//		int m_day;					// 지급할 날짜			// m_daysSubscribe로 대체함. 아직은 월정액 상품이 한종류뿐이라 편의상 그쪽 변수를 씀.
//		int m_daygem;				// 하루에 지급할 젬 개수
		_tstring m_contry;			// 통화명
		float m_price;				// 가격
		int m_text;					// 텍스트 ID
		_tstring m_img;				// 이미지
		XE::VEC2 m_pos;				// 이미지 위치
		bool IsScribeType() const {
			return m_type == XGAME::xCT_SUBSCRIBE;
		}
	};
	XVector<int> m_arySizeCost;		// 유닛크기에 따른 크기비용
private:
	XVector<cashItem> m_aryCashitems;				// 캐쉬 아이템 목록
	XVector<cashItem> m_aryCashitemsBySoftnyx;		// 캐쉬 상품 목록(소프트닉스)
public:
	std::vector<int> m_arrEtcItems;						// 기타 아이템 목록
	struct xItemGuildShop {
		_tstring idsItem;
		int cost = 0;
	};
	std::vector<xItemGuildShop> m_aryGuildShop;				// 길드샵 아이템 리스트
#ifdef _CLIENT
	const std::vector<cashItem>& GetAryCashItems( XGAME::xtPlatform platform = XGAME::PLATFORM_STORE ) const;
	const cashItem* GetCashItem(const _tstring& productID, XGAME::xtPlatform platform = XGAME::PLATFORM_STORE ) const ;
#else
	const std::vector<cashItem>& GetAryCashItems( XGAME::xtPlatform platform ) const;
	const cashItem* GetCashItem( const _tstring& productID, XGAME::xtPlatform platform ) const;
#endif // _CLIENT
	const std::vector<int>& GetArrEtcItems() const { return m_arrEtcItems; }
	void LoadCashItems( const char* cKey, XVector<cashItem>* pOutAry ) const;		// 캐쉬 아이템 로드
	void LoadEtcItems();		// 기타 아이템 로드
	_tstring GetBgBattle( XGAME::xtSpot type ) const {
		return m_aryBgsBattle[ type ];
	}
	void MakeTablePowerPerLevel();
	void SaveMaxPowerTable() const;
	int GetsecTraderReturn( int lvAcc ) const;
	bool LoadAreaCost( CToken& token );
	bool LoadLegionTable( CToken& token );
	int GetNpcLevelByColor( int powerBase, int diffLv ) const;
	/**
	 @brief 공격자에 대한 방어자색에 따라 손실율을 돌려준다.
	 @param powerGradeByDefender -2~2
	 @note 예를들어 초록인 공격자한테 주황인 내가 졌다면 powerGrade는 2가 된다.
	*/
	float GetRateLossLocal( int powerGradeByDefender ) const {
		powerGradeByDefender += 2;
		if( XBREAK(powerGradeByDefender < 0 || powerGradeByDefender >= 5) )
			powerGradeByDefender = 2;		// 중간값으로..
		return m_aryRateLocalLoss[ powerGradeByDefender ];
	}
	float GetRateLossMain( int powerGradeByDefender ) const {
		powerGradeByDefender += 2;
		if( XBREAK(powerGradeByDefender < 0 || powerGradeByDefender >= 5) )
			powerGradeByDefender = 2;		// 중간값으로..
		return m_aryRateMainLoss[ powerGradeByDefender ];
	}
	int GetGoldChangePlayer( int lvAcc ) const {
		return m_goldChangePlayer * lvAcc;
	}
	/// 총 루팅할수 있는 자원수 num을 각 부대당 얼마나 가져야하는지를 계산한다.
	float GetLootAmountByEach( float num ) const {
		XBREAK( m_rateLootResourceSquad == 0 );
		// 총 루팅양에 xx%를 적용한 후 다시 부대수로 나눈다.
		return (num * m_rateLootResourceSquad) / 3.f;
	}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
}; // class GlobalConstant

#define XGC		XGlobalConst::sGet()
extern XGlobalConst* _XGC;
