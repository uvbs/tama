#pragma once
#include "XPropWorld.h"
//#include "XPropCamp.h"
#include "XArchive.h"
#include "etc/XTimer2.h"
//#include "XLegion.h"
//#include "XCampObj.h"
#include "XPropCloud.h"
#include "XPropItem.h"
#include "XStruct.h"


namespace XGAME {
};	// XGAME

class XSpot;
class XAccount;
class XWorld;
class XGuild;
#ifdef _GAME_SERVER
class XGameUser;
#endif
////////////////////////////////////////////////////////////////
class XDelegateSpot
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateSpot() { Init(); }
	virtual ~XDelegateSpot() { Destroy(); }
	// 스팟에 뭔가 스폰이 되었을때 발생하는 델리게이트
	virtual void DelegateOnSpawnTimer( XSpot *pSpot, xSec secAdjustOffline ) {}
	// 스팟이 누군가의 소유가 되었을때 발생하는 델리게이트
	virtual void DelegateOnCalcTimer( XSpot *pSpot, const XTimer2& timerCalc ) {}
	// 스팟이 해제(지도상에서 사라질때)될때 발생
	virtual void DelegateReleaseSpotBefore( XSpot *pSpot ) {}
	// 스팟이 해제(지도상에서 사라질때)될때 발생
	virtual void DelegateReleaseSpotAfter( XSpot *pSpot ) {}
	// 스팟이 생성된 후 추가 초기화 
	virtual void DelegateAfterCreateSpotAdditionalInit( XSPAcc spAcc, XSpot *pBaseSpot ) {}
};
////////////////////////////////////////////////////////////////
class XSpot
{
public:
	static void sSerialize( XArchive& ar, XSpot *pSpot );
	static XSpot* sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	static int sDeSerialize( XArchive& ar, XSpot *pSpot );
	static XSpot* sCreate( XWorld *pWorld, XPropWorld::xBASESPOT *pBaseProp, XSPAcc spAcc, XDelegateSpot *pDelegate );
	static int sDeserializeUpdate( XWorld *pWorld, ID idSpot, XArchive& ar );
	static ItemBox sDoDropScalp( XSPAcc spAcc );
	static ItemBox2 sDoDropScalp2( XSPAcc spAcc );
	static XGAME::xDropItem sDoDropScalpWithDropItem( XSPAcc spAcc );
	static ItemBox sDoDropMedal( XSPAcc spAcc, int lvSpot );
	static ItemBox2 sDoDropMedal2( XSPAcc spAcc, int lvSpot );
	static XGAME::xDropItem sDoDropMedalWithDropItem( XSPAcc spAcc, int lvSpot );
	static ItemBox sDoDropScroll( XSPAcc spAcc, int lvSpot );
	static ItemBox2 sDoDropScroll2( XSPAcc spAcc, int lvSpot );
	static XGAME::xDropItem sDoDropScrollWithDropItem( XSPAcc spAcc, int lvSpot );
	static XPropItem::xPROP* sDoDropEquip( int level );
//	static XGAME::xReward sGetRewardDailyToday( XPropWorld::xDaily* pProp, int lvAcc );
// 	static XGAME::xReward sGetRewardDaily( XPropWorld::xDaily* pProp, XE::xtDOW dow, int lvAcc );
	static bool sGetRewardDailyToday( XPropWorld::xDaily* pProp, int lvAcc, XVector<XGAME::xReward>* pOutAry );
	static bool sGetRewardDaily( XPropWorld::xDaily* pProp, XE::xtDOW dow, int lvAcc, XVector<XGAME::xReward>* pOutAry );
	// 지역창고
	struct xLOCAL_STORAGE {
		XGAME::xtResource m_Type = XGAME::xRES_NONE;	// 새로 추가됨
		float numCurr = 0;	// 현재 쌓인양
		float maxSize = 0;	// 최대 크기
		// bFullCheck가 true면 맥시멈을 넘어가지 않도록 끊는다.
		void Add( float num, bool bFullCheck ) {
			if( maxSize == 0 )
				return;
			numCurr += num;
			if( bFullCheck && numCurr > maxSize )		// 각종보상을 지역창고에 쌓을수 있게 하기위해 일시적으로 풀음.
				numCurr = maxSize;
		}
		void Clear() {
			numCurr = 0;
		}
		// 손실을 입힌다.
		float Loss( float rateLoss ) {
			XBREAK( rateLoss > 1.0f );
			auto loss = (numCurr * rateLoss);
			numCurr -= loss;
			return loss;
		}
		void Serialize( XArchive& ar ) {
			ar << (BYTE)m_Type;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << numCurr;
			ar << maxSize;
			XBREAK( maxSize == 0 );
		}
		void DeSerialize( XArchive& ar, int ver = 0 ) {
			BYTE b0;
			ar >> b0;	m_Type = (XGAME::xtResource)b0;
			ar >> b0 >> b0 >> b0;
			ar >> numCurr;
			ar >> maxSize;
			XBREAK( maxSize == 0 );
		}
	};
// 	enum xtSpotFlag {
// 		xSF_NONE,
// 		xSF_NO_ATTACK = 0x0001,		// 공격금지
// 	};
private:
  ID m_idProp = 0;
	ID m_snSpot;		// 스팟 시리얼 번호
	XGAME::xtSpot m_typeSpot;	// 스팟 타입
	XPropWorld::xBASESPOT *_m_pBaseProp;	// 프로퍼티
//	ID m_keyProp = 0;
	XWorld *m_pWorld =nullptr;
	_tstring m_strName;	// 현재 점유자 닉네임
	int m_Level = 0;		// 소유자 레벨
	int m_Difficult = 0;	// -10~+10범위. 같은 레벨이라도 난이도 보정치에 따라 조금씩 더 차이가 난다. +10이면 레벨+1과 비슷하다.
//	int m_LevelArea = 0;	// 미리 계산된 지역레벨
	XTimer2 m_timerSpawn;	// 공통 스폰타이머.
	XDelegateSpot *m_pDelegate;
	LegionPtr m_spLegion;		// 스팟에 있던 군대 객체. 이것은 스팟의 종류마다 메모리 할당의 성격이 다르다
	int m_Score;
	int m_Power = 0;
	bool m_bDestroy = false;		///< 파괴될 스팟
	std::vector<XGAME::xDropItem> m_aryDropItems;		// 드랍가능성이 있는 아이템 목록
	//ID m_idDropItem = 0;		///< 드랍될 아이템
	int m_numLose = 0;      ///< 연패 횟수
	XSPAccW m_spOwner;	// 스팟주인?의 포인터
	DWORD m_seedRandom = 0;		// 군단생성용 랜덤시드
//	int m_numStar = 0;			// 각 스팟의 별클리어
protected:
	// 일단 여기다 때려넣고 나중에 자원스팟 전용 클래스를 만들어 옮긴다.
	xResourceAry m_aryLoots;	// 여기서 루팅가능한 자원양(중앙창고 및 지역창고 합산). -1값이면 ?상태가 된다.
public:
	/// 군대를 삭제하고 데이타들을 초기화한다.(초기화하는거 자꾸까먹어서 여기다 둠)
	virtual void ClearSpot();
private:
//	xtSpotFlag m_bitFlag = xSF_NONE;
	struct bitfield {
		BYTE noAttack : 1;
		BYTE bReconed : 1;
		BYTE b2 : 1;
		BYTE b3 : 1;
		BYTE b4 : 1;
		BYTE b5 : 1;
		BYTE b6 : 1;
		BYTE b7 : 1;
	} m_bitFlag;
	//
	void Init() {
		m_snSpot = 0;
		m_typeSpot = XGAME::xSPOT_NONE;
		m_pDelegate = nullptr;
		m_Score = 0;
		*((BYTE*)(&m_bitFlag)) = 0;
	}
	void Destroy() {
		m_spLegion.reset();
	}
protected:
	SET_TSTRING_ACCESSOR( strName );
	GET_SET_ACCESSOR_CONST( DWORD, seedRandom );
//	SET_ACCESSOR( XPropWorld::xBASESPOT*, pBaseProp );
//	GET_ACCESSOR( XWorld*, pWorld );
	int GetNumSpots() const;
	// 정산타이머를 리셋시킨다.
	virtual void ResetTimerCalc( void ) {}		
public:
// 	XSpot( XGAME::xtSpot typeSpot, XWorld *pWorld ) { 
// 		Init(); 
// 		m_typeSpot = typeSpot;
// 		m_pWorld = pWorld;
// 	} 
protected:
	XSpot( XWorld *pWorld, XGAME::xtSpot typeSpot );
public:
	XSpot( XPropWorld::xBASESPOT *pProp, XGAME::xtSpot typeSpot, XWorld *pWorld, XDelegateSpot *pDelegate = nullptr );
	virtual ~XSpot() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( ID, snSpot );
	GET_ACCESSOR_CONST( XGAME::xtSpot, typeSpot );
	GET_ACCESSOR_CONST( XSPAccW, spOwner );
	GET_SET_ACCESSOR_CONST( int, numLose );
	GET_SET_ACCESSOR_CONST( int, Difficult );
//	GET_SET_ACCESSOR( int, numStar );
	/// 프로퍼티가 재로딩되어 _m_pBaseProp이 바꼈을경우 다시 세팅함.
	void ReloadProp();
	/// 연패중이냐
	bool IsSuccessiveDefeat() const {
		return m_numLose >= 3;
	}
	XPropWorld::xBASESPOT* GetpBaseProp() const;
	void SetpBaseProp( ID idProp );
private:
	void SetpBaseProp( XPropWorld::xBASESPOT* pProp );
public:
	LPCTSTR GetszName( void ) const {
// 		if( m_pBaseProp->idName )
// 			return XTEXT(m_pBaseProp->idName);
		return m_strName.c_str();
	}
	const _tstring& GetstrName() const {
		return m_strName;
	}
//	GET_TSTRING_ACCESSOR( strName );
	GET_ACCESSOR_CONST( const XTimer2&, timerSpawn );
	XTimer2& GettimerSpawnMutable() {
		return m_timerSpawn;
	}
#ifdef _CLIENT
	GET_SET_ACCESSOR_CONST( XDelegateSpot*, pDelegate );
#else
	GET_ACCESSOR_CONST( XDelegateSpot*, pDelegate );
#endif // _CLIENT
//	GET_ACCESSOR( XPropWorld::xBASESPOT*, pBaseProp );
	GET_SET_ACCESSOR_CONST( int, Level );
//	GET_ACCESSOR( XLegion*, pLegion );
// 	virtual GET_ACCESSOR( LegionPtr&, spLegion );
	LegionPtr GetspLegion() const {
		return m_spLegion;
	}
//	SET_ACCESSOR( LegionPtr&, spLegion );
	void SetspLegion( LegionPtr spLegion );
	GET_SET_ACCESSOR_CONST( int, Score );
	GET_SET_ACCESSOR_CONST( int, Power );
	GET_ACCESSOR_CONST( const struct bitfield&, bitFlag );
	void SetbitFlagByReconed( bool bFlag ) {
		m_bitFlag.bReconed = xboolToByte(bFlag);
	}
	void SetbitFlagByNoAttack( bool bFlag ) {
		m_bitFlag.noAttack = xboolToByte( bFlag );
	}
//	GET_SET_ACCESSOR( ID, idDropItem );
	void SetpLegion( XLegion *pLegion );
	virtual void Release() {
		m_spLegion.reset();
	}
	// 이 스팟이 npc인가 pc인가의 여부. 현재 전투대상이 pc인가 npc와는 다른것임.
	virtual bool IsNpc() const = 0;
	virtual bool IsPC() const = 0;
//	virtual bool IsPC() { return !IsNpc(); }
	// 현재 대전중인 상대가 npc인가
	virtual bool IsVsNPC() const {
		return IsNpc();			// 일반적인 상황에선 스팟의 현재 주인을 보고 판단하면 된다.
	}
	bool IsVsPC() const {
		return !IsVsNPC();
	}
	virtual bool IsEnemy() const {
		return true;
	}
	virtual bool IsEmpty() const {
		return m_spLegion == nullptr;
	}
	ID GetidSpot( void ) const {
		return m_idProp;
	}
	inline ID getid() const {
		return GetidSpot();
	}
	XE::VEC2 GetPosWorld( void ) const {
		return GetpBaseProp()->vWorld;
	}
	bool IsDestroy() const {
		return m_bDestroy;
	}
	GET_SET_ACCESSOR_CONST( bool, bDestroy );
	bool IsNoAttack() const {
		return m_bitFlag.noAttack != 0;
	}
	bool IsReconed() const {
		return m_bitFlag.bReconed != 0;
	}
	virtual void OnCreate( XSPAcc spAcc );
	// 스팟이 월드에 최초 등장하면 호출된다. Deserialize로 생성된 스팟은 호출되지 않는다.
	virtual void OnCreateNewOnServer( XSPAcc spAcc ) {}
	virtual void AdjustTimer( xSec secAdjust ) {
		m_timerSpawn.AddTime( secAdjust );
	}
//  virtual void ResetProp( XPropWorld::xBASESPOT *pBaseProp ) = 0;
	virtual void Serialize( XArchive& ar );
// 	virtual void SerializeUpdate( XArchive& ar );
// 	virtual void DeSerializeUpdate( XArchive& ar );
protected:
	virtual BOOL DeSerialize( XArchive& ar, DWORD verWorld );
public:
	virtual void SerializeLocalStorage( XArchive& ar ) {}
	virtual void Process( float dt );
	// spAccount가 DeSerialize된후 최초 호출
	virtual void OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pUser, ID idAcc, xSec secLastCalc );
	//
	// 각 스팟의 자원을 지나간시간을 기준으로 정산한다.
	virtual void CalculateResource( ID idAcc, xSec secAdjustOffline ) {} //=0;
	// 지역창고에 자원을 쌓는다.
	virtual void DoStackToLocal( float num, bool bFullCheck ) {}	//=0;
	// 지역창고에 쌓여있던 자원을 계정창고로 옮긴다.
	virtual void MoveResourceLocalToMain( XSPAcc spAccount, XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pOutAry=nullptr ) {} //=0;
	virtual void ClearLocalStorage() {}		//=0;
	// 각 지역창고의 디버깅용 스트링을 돌려준다.
	virtual _tstring GetStrLocalStorage( void ) const { return _tstring(); }
	// 지역창고에 자원이 쌓여있는가.
//	virtual BOOL IsHaveLocalStorage( void ) { return FALSE; }	//=0;
	// 
	/**
	 @brief 지역창고에 자원을 rateLoss율 만큼 소실시킨다.
	 @param pOutAry 널이 아닐경우 소실된 자원양을 ary에 누적시킨다.
	*/
	virtual void LossLocalStorage( float rateLoss, std::vector<int> *pOutAry ) {} //=0;
	// 지역창고에 쌓여있는 자원양을 돌려준다. 단 유저스팟은 목재/철 두가지 이므로 이것으로 알수 없다.
	virtual float GetLocalStorageAmount( XGAME::xtResource typeRes = XGAME::xRES_NONE ) const { return 0; } //= 0;
	// 지역창고에 쌓여있는 자원들을 pOutAry에 더해준다.
	virtual void AddLocalStorageResources( xResourceAry *pOutAry ) {}
	// 지역창고에 쌓여있는 자원양의 퍼센테이지를 돌려준다. 
	virtual float GetLocalStoragePercent( void ) const { return 0; } //= 0;
	// 지역창고에 수거가능한 양이 모였는가.
	BOOL IsAbleGetLocalStorage( void ) const {
		return GetLocalStoragePercent() > 0.05f;
	}
	// 루팅할수 있는 자원양을 넣는다.
	void ClearLootRes() {
		m_aryLoots.assign( m_aryLoots.size(), 0 );
	}
	// 루팅할수 있는 자원수를 얻는다.
	int GetnumLoots() const {
		int num = 0;
		for( auto numRes : m_aryLoots ) {
			if( numRes > 0 )
				++num;
		}
		return num;
	}
	void SetLootRes( XGAME::xtResource type, int loot ) {
		if( XBREAK( XGAME::IsInvalidResource( type ) ) )
			return;	// 이제 NONE같은거 허용안함.
		if( XBREAK( loot < 0 ) ) 
			loot = 0;
		m_aryLoots[ type ] = loot;
	}
	void SetLootRes( const XVector<XGAME::xReward>& aryReward );
	void AddLootRes( XGAME::xtResource type, int loot ) {
		if( XBREAK( XGAME::IsInvalidResource( type ) ) )
			return;
		m_aryLoots[ type ] += loot;
	}
	// type자원의 루팅할수 있는 양을 얻늗다.
	int GetLootAmount( XGAME::xtResource type ) const {
		if( XASSERT( XGAME::IsValidResource( type ) ) ) {
			const auto numLootAmount = m_aryLoots[ type ];
			XBREAK( numLootAmount < 0 );
			return numLootAmount;
		}
		return 0;
	}
	virtual BOOL IsActive( void ) const { return TRUE; }
	/// 스팟이 맵에 있어도 실제 지역이 열려서 쓸수 있는 스팟인지 검사.
	bool IsOpenedSpot( XWorld *pWorld );
	/// 스팟을 초기화 시킨다. DB데이터의 에러등등의 이유로 최초 상태로 복구시켜야 한다.
	virtual void Initialize( XSPAcc spAcc ) {};
	ID GetidArea() const {
		return GetpBaseProp()->idArea;
	}
	// 비정상스팟인가
	bool IsInvalidSpot() const {
		return GetpBaseProp()->IsInvalidSpot();
	}
// 	ID GetidAreaConst() const;
// 	bool IsInvalidSpotConst() const;
	/**
	 @brief 스팟이 다시 작동할수 있도록 준비시킨다.
	 @note Reset과 Spawn의 차이.
		Reset: 스팟이 정상작동할수 있도록 초기상태로 만들어준다. 예를들어 npc스팟의 초기상태라는 것은 모든것이 클리어 되어있고 일정시간후에 스폰이 일어날수 있도록 타이머를 다시 가동시키는것이다.
		Spawn: 스팟이 화면상에 등장한 시점을 의미한다. npc스팟의 경우 스폰시간이 되어 화면상에 나타날때가 Spawn시점이다.
	*/
	virtual void ResetSpot() {}
	/// 스팟을 강제로 스폰스킨다.
	virtual void DoSpawnSpot() {}
	virtual void OnSpawn( XSPAcc spAcc );
	bool IsEventSpot() {
		return GetpBaseProp()->strWhen.empty() == false;
	}
	// 드랍할 아이템을 등록한다.
//	virtual void RegisterDrops() {}
	int DoDropList( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, float multiplyDropNum = 1.f ) const;
	virtual int DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum ) const;
	virtual void GetLootInfo( XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pAry ) const;
	void GetLootInfo( XVector<XGAME::xRES_NUM>* pOutAry ) const;
#if defined(_CLIENT) || defined(_GAME_SERVER)
	virtual int GetNeedAP( XSPAcc spAcc );
#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

	virtual void CreateLegion( XSPAcc spAccount );
public:
	// 레벨등에 의하여 스팟이름이 달라져야한다면 따로 처리한다.
	virtual void ResetName( XSPAcc spAcc ) {}
	virtual XGAME::xtBattle GetBattleType() { return XGAME::xBT_NORMAL;	}
	virtual bool IsGuildRaid() const { return false; }
	virtual bool IsQuestion() const { return false; }
	virtual bool IsCampaignType() const { return false; }
	virtual xCampaign::CampObjPtr GetspCampObj() { return xCampaign::CampObjPtr(); }
	XPropCloud::xCloud* GetpAreaProp() const;
	bool GetLevelRangeByArea( int *pOutLvMin, int *pOutLvMax, int lvAcc ) const;
	int GetLevelSpawn( int lvAcc ) const;
//	int GetLevelSpawn( int lvAcc );
	int GetLevelSpawn() const;
	int GetAreaLevel() const {
		if( XASSERT(GetpAreaProp()) )
			return GetpAreaProp()->lvArea;
		return 0;
	}
	void UpdateLevelArea( XSPAcc spAcc );
	/// 전투후 스팟에 대한 처리
	virtual void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, bool bRetreat );
	/// 전투직전 호출되는 스팟핸들러.
	virtual void OnBeforeBattle( XSPAcc spAcc ) {}
	/// 스팟을 터치했을때 업데이트할것이 있다면 업데이트 한다. 
	virtual bool Update( XSPAcc spAcc ) { return true; }
//	void DoDropRegisterBooty( XSPAcc spAcc, float multiply, int power, int lvSpot );
	virtual int GetDropItems( std::vector<XGAME::xDropItem> *pOutAry );
	virtual bool IsElite() { return false; }
	void SetDropItems( const std::vector<XGAME::xDropItem>& aryDropItem );
	void UpdatePower( /*XSPAcc spAccEnemy, */LegionPtr spLegion = LegionPtr() );
	bool IsSulfur() {
		return GettypeSpot() == XGAME::xSPOT_SULFUR;
	}
	float GetsecRemainRegen() {
		return GettimerSpawn().GetsecRemainTime();
	}
	// 스팟
//	virtual ID GetidEnemy() { return 0; }
	// 연구소,아카데미 등등의 기능성 스팟인가.
	bool IsOpened() {
		return GetpBaseProp()->IsOpened();
	}
	// pPropArea가 열리면 그 아래에 있던 모든 스팟들에게 이벤트가 발생한다.
	virtual void OnOpenedArea( XPropCloud::xCloud* pPropArea ) {}
	bool IsHomeSpot() const {
		return GetpBaseProp()->strIdentifier == _T("spot.home");
	}
	// 시장/성당같은 기능성 스팟.
	bool IsOurBuilding() const;
	void Restore();
	/// 성/보석/만드레이크 스팟의 경우 현재 소유자아이디를 얻는다. 원래 여기 넣으면 안되는건데 간단하게 하기위해 그냥 함. 나중에 이 세스팟은 별로의 루트클래스가 필요함.
	virtual ID GetidOwner() const { return 0; }
	virtual _tstring GetstrHello() const { return _tstring(); }		// 일단 이렇게 하고 나중에 리팩토링
	virtual void SetstrHello( const _tstring& strHello ) {}
#ifdef _DEV
	bool IsDummy() const {
		return m_strName == _T("TEST_USER");
	}
#endif // _DEV
protected:
	void AddDropItem( ID idDrop, int num, float chance );
	void AddDropItem( XGAME::xDropItem& dropItem );
	void AddDropItem( XGAME::xDropItem&& dropItem );
	void AddDropItem( const _tstring& strIds, int num, float chance );
	// 아직 외부에서 써야할일이 없어서 숨김.
	void ClearDropItems() {
		m_aryDropItems.clear();
	}
	virtual int GetDropableList( std::vector<ID> *pOutAry );
	void DoDropRegisterEquip( int level );
	// 모든종류의 드랍예약을 한다.
	virtual void DoDropRegisterItem( int level );
	/// 자원드랍이 이뤄지는 스팟이라면 드랍될 자원을 등록시키는 처리를 한다.
	virtual void DoDropRegisterRes( XSPAcc spAcc ) {}
	void DestroyLegion( void ) {
//		m_Level = 0;		// 스팟자체에 레벨이 필요없는 스팟들은 레벨을 0으로 두는걸 원칙으로 한다.
		m_spLegion.reset();
		m_Power = 0;
	}
	int CalcLevel( XPropWorld::xBASESPOT *pProp, int lvAcc ) const;
private:
	virtual void ResetPower( int lvSpot ) {}
	virtual void ResetLevel( XSPAcc spAcc ) {}

friend class CUserDB;

};	// class XSpot

typedef std::shared_ptr<XSpot> SpotPtr;



