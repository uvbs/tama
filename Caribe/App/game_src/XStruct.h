/********************************************************************
	@date:	2015/07/16 22:13
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XStruct.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
//#include "XAccount.h"
#include "XPropItem.h"

class XAccount;
class XEXmlNode;
class XWorld;
typedef std::pair<ID, int> ItemBox2;

XE_NAMESPACE_START( XGAME )

// 리소스와 개수의 구조를 사용할 다용도 구조체
struct xRES_NUM {
	XGAME::xtResource type;
	float num;
	xRES_NUM() {
		type = XGAME::xRES_NONE;
		num = 0;
	}
	xRES_NUM( XGAME::xtResource _typeRes, int _num ) {
		type = _typeRes;
		num = (float)_num;
	}
	xRES_NUM( XGAME::xtResource _typeRes, float _num ) {
		type = _typeRes;
		num = _num;
	}
	bool IsValid() {
		return type != XGAME::xRES_NONE && num > 0;
	}
	bool IsInvalid() {
		return !IsValid();
	}
// 	template<typename T>
// 	xRES_NUM( XGAME::xtResource _typeRes, T _num ) {
// 		type = _typeRes;
// 		num = (float)_num;
// 	}
// 	void Set( XGAME::xtResource _type, int _num ) {
// 		type = _type;
// 		num = (float)_num;
// 	}
	void Serialize( XArchive& ar ) const {
		ar << (DWORD)type;
		ar << num;
	}
	void DeSerialize( XArchive& ar, int ) {
		DWORD dw0;
		ar >> dw0;	type = ( XGAME::xtResource )dw0;
		ar >> num;
	}
	bool LoadFromXML( XEXmlNode& node, LPCTSTR szTag );
};
int LoadResFromXMLToAry( XEXmlNode& nodeRoot, LPCTSTR szNodeName, LPCTSTR szTag, std::vector<xRES_NUM> *pOutAry );
//////////////////////////////////////////////////////////////////////////
/// "부대"를 정의한다. xml에선 "squad" element로 표현된다.
struct xSquad {
	int idxPos = -1;	///< 부대위치. 분대번호
	ID idHero = 0;
	xtAttack atkType = xAT_NONE;		// 영웅병과
	xtUnit unit = XGAME::xUNIT_NONE;
	xtSize sizeUnit = XGAME::xSIZE_NONE;
	int lvSquad = 0;		///< 부대레벨
	int lvSkill = 0;		///< 영웅의 스킬레벨
	int lvHero = 0;		///< 영웅레벨
	int adjLvHero = 0x7f;	// 영웅레벨 보정
	XGAME::xtGrade grade = XGAME::xGD_NONE;
//	int numUnit = 0;			// 유닛수
	float mulAtk = 1.f;	// 공격력 보정
	float mulHp = 1.f;		// 체력보정
#ifdef _XSINGLE
	bool m_bShow = true;	// 부대hp정보 보이기
#endif // _XSINGLE
#ifdef _DEBUG
	int debug = 0;			// 디버깅용
#endif // _DEBUG
	//
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int ver );
	bool LoadFromXML( XEXmlNode& node, LPCTSTR szTag );
	bool SaveXML( XEXmlNode& nodeLegion );
};
//////////////////////////////////////////////////////////////////////////
struct xLegion {
private:
	ID m_idProp = 0;			///< "legion"블럭이 있었으면 고유번호가 들어간다. 이것은 save에 활용된다.
public:
	std::string strIds;		// 식별자
//	ID snLegion = 0;	///< "legion"블럭이 있었으면 고유번호가 들어간다. 이것은 save에 활용된다.
	ID idName = 0;		///< 군단이름
	int lvLegion = 0;	///< 군단레벨(제네레이트 목적)
	XGAME::xtGradeLegion gradeLegion = XGAME::xGL_NORMAL;	///< 정예인가
	int lvLimit = 0;	///< 최소 도전레벨
	int adjLvLegion = 0x7f;	///< 기준레벨대비 보정값. levelLegion이 0이어야 한다.
	int numSquad = 0;	///< 만들어야하는 부대수. 따로지정하지 않으면 levelLegion에 의해 자동계산된다.
	ID idBoss = 0;		///< 군단장 영웅 아이디
	float mulAtk = 1.f;	// 공격력 보정
	float mulHp = 1.f;		// 체력보정
	xSquad squadDefault;	// 디폴트 분대
	XVector<xSquad> arySquads;		///< 수동으로 지정해야 하는 부대리스트
	xLegion() {}		// Deserial용
	xLegion( const char* cIdentifier ) : strIds(cIdentifier) {}
	xLegion( const std::string& strIdentifier ) : xLegion( strIdentifier.c_str() ) {}
	inline void _SetidProp( const ID idProp ) {
		m_idProp = idProp;
	}
	GET_ACCESSOR_CONST( ID, idProp );
	// 수동으로 지정해야할 idxPos의 수동데이타가 있는가.
	const XGAME::xSquad* GetManualSquad( const int idxPos ) const {
		for( auto& squad : arySquads ) {
			if( squad.idxPos == idxPos )
				return &squad;
		}
//		int size = arySquads.size();
//		for( int i = 0; i < size; ++i ) {
// 			const XGAME::xSquad* pSquad = &(arySquads[i]);
// 			if( pSquad->idxPos == idxPos )
// 				return pSquad;
//		} 
		return nullptr;
	}
	// idxPos부대가 메뉴얼 지정이라면 부대프로퍼티를 꺼낸다.
	const xSquad* GetPropSquadWhenManual( const int idxPos ) const {
		return GetManualSquad( idxPos );
	}
	inline int GetNumSquad() {
		return numSquad;
	}
	bool IsEmpty() const {
		return m_idProp == 0;
	}
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int ver );
	bool LoadFromXML( XEXmlNode& node, LPCTSTR szTag );
	inline bool LoadFromXML( XEXmlNode& node, const _tstring& strTag ) {
		return LoadFromXML( node, strTag.c_str() );
	}
	bool SaveXML( XEXmlNode& nodeRoot );
};
//////////////////////////////////////////////////////////////////////////
struct xBattleLog {
	ID idEnemy = 0;    // 상대 계정아이디
	_tstring strName;	// 상대 이름
	ID snLog = 0;
	int m_lvAcc = 0;
	WORD score = 0;		// 상대 점수
	short addScore = 0;	// 얻은/잃은 점수
	std::vector<xRES_NUM> aryLoot;		// 뺏거나 뺏은 자원 목록
	int powerBattle = 0;		// 상대군사력
	bool bRevenge = false;	// 복수전인가 아닌가
	bool bWin = false;		// 승패결과
	bool m_bKill = false;		// 소탕권으로 승리
	xSec sec = 0;         // 공격이나 방어가 이뤄진 시간.
	xBattleLog() {
		snLog = XE::GenerateID();
	}
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int /*ver*/ );
	int AddLootRes( XGAME::xtResource type, int add );
}; // BattleLog
//////////////////////////////////////////////////////////////////////////
/**
 @brief 전투 시작시 필요한 정보들.
*/
struct xBattleStart {
	ID m_idEnemy = 0;			// 상대가 pc의 경우.
#ifndef _XSINGLE
	XGAME::xtSpot m_typeSpot = XGAME::xSPOT_NONE;	// 전투가벌어지는 스팟의 타입(에러확인용)
	ID m_idSpot = 0;					// 전투가 벌어지는 스팟(0인경우도 있음)
#endif // not _XSINGLE
	int m_Level = 0;					// 상대의 레벨
	_tstring m_strName;				// 상대이름
	LegionPtr m_spLegion[2];	// 0:아군 1:적군
	xtBattle m_typeBattle = XGAME::xBT_NONE;
	int m_Defense = 0;				// 방어도(보석광산용)
	int m_idxStage = -1;				///< 캠페인의 경우 스테이지 인덱스
	int m_idxFloor = 0;
	// npc와 전투하는것인가.
	bool IsVsNpc() const {
		return m_idEnemy == 0;
	}
	bool IsValid() const ;
	inline bool IsInvalid() const {
		return !IsValid();
	}
};
//////////////////////////////////////////////////////////////////////////
/**
 @brief 전투후 결과전송을 위해 사용된다.
 DB저장용으로 사용해선 안된다.
*/
struct xBattleResult {
	ID idSpot = 0;
	XGAME::xtSpot typeSpot = XGAME::xSPOT_NONE;
	int numStar = 0;		// 이번전투의 별점.
	int m_totalStar = 0;		// 계정이 가진 총 별개수.
	XArrayLinearN<XBaseItem*, 256> aryUpdated;
	XArrayLinearN<ItemBox, 256> aryDrops;
	XArrayLinearN<XHero*, XGAME::MAX_SQUAD> aryHeroes;
	XVector<ID> m_aryLevelUpHeroes;		// 레벨업한 영웅들의 sn
	xBattleLog logForAttacker;
	int ladder = 0;
	int idxStage = 0;
	XArchive arUpdated;
	XArchive arHeroes;
	XArchive arSpot;			// 동기화용
	xResourceAry aryAccRes;		// 계정에 들어있는 자원총량(동기화용)
	int m_numCashtem = -1;			// 캐시템개수.
	bool bLevelupAcc = false;
	bool bClearSpot = false;	// 스팟을 클리어했는지
	bool bVsPVP = false;		// pvp전이었는지
	bool bFake = false;			// 내부개발용
	bool bEncounter = false;		// 인카운터가 발생함.
	int m_mulByStar = 100;	// 스팟 정복도(별점)에 따른 루팅양 획득률(100분율)
	int m_powerAccIncludeEmpty = 0;			// 전투후 렙업등으로 인해 계정전투력이 바낄수 있으므로/
	int m_powerAccExcludeEmpty = 0;			// 전투후 렙업등으로 인해 계정전투력이 바낄수 있으므로/
	inline bool IsWin() const {
		return logForAttacker.bWin;
	}
	inline bool IsLose() const {
		return !IsWin();
	}
	inline void SetWin( bool bWin ) {
		logForAttacker.bWin = bWin;
	}
	inline void SetidEnemy( ID _idEnemy ) {
		logForAttacker.idEnemy = _idEnemy;
	}
	inline ID GetidEnemy() {
		return logForAttacker.idEnemy;
	}
	inline bool IsVsNpc() {
		return !bVsPVP;
	}
	bool IsLevelupHero( ID _snHero ) const {
		for( auto snHero : m_aryLevelUpHeroes ) {
			if( snHero == _snHero )
				return true;
		}
		return false;
	}
//	xBattleResult() : aryLoots( XGAME::xRES_MAX ) {}
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int );
	int DeSerializeHeroes( XArchive& ar, XSPAcc spAcc );
};
//////////////////////////////////////////////////////////////////////////
/**
 @brief 전투종료시 보내지는 패킷구조체
*/
struct xBattleFinish {
	xtExitBattle ebCode = xEB_NONE;
	ID idSpot = 0;
//	ID idEnemy = 0;	// 상대가 플레이어일때 아이디(해킹될수 있어서 서버에서 직접 읽어서 하도록 바꿈)
	ID snSession = 0;
	xtSide bitWinner = XGAME::xSIDE_NONE;
	int idxStage = 0;
	int m_idxFloor = 0;
	int secPlay = 0;
//	int ptBrave = 0;
	XArchive arLegion[2];
	bool bRunAwaySulfur = false;
	bool bCheatKill = false;
	float m_rateHpAlly = 0.f;
	float m_rateHpEnemy = 0.f;
	// 전투중 후퇴한것인가.
	bool IsRetreat() {
		return ebCode == xEB_RETREAT;
 	}
	//
	void Serialize( XArchive& ar );
	void DeSerialize( XArchive& ar, int );
};
//////////////////////////////////////////////////////////////////////////
/**
 @brief 전투시작시 보내지는 기본정보들(서버->클라)
*/
struct xBattleStartInfo {
	XGAME::xtSpot m_typeSpot = XGAME::xSPOT_NONE;	// 전투가벌어지는 스팟의 타입(에러확인용)
	ID m_idSpot = 0;					// 전투가 벌어지는 스팟(0인경우도 있음)
	ID m_snSession = 0;
	ID m_idEnemy = 0;			// 상대가 pc의 경우.
	int m_Level = 0;					// 상대의 레벨
	_tstring m_strName;				// 상대이름
	LegionPtr m_spLegion;			// 스팟에 군단정보가 없을경우 여기에 들어감.
	xtBattle m_typeBattle = XGAME::xBT_NORMAL;
// 	int m_Defense = 0;				// 방어도(보석광산용)
	int m_idxStage = -1;				///< 캠페인의 경우 스테이지 인덱스
	int m_idxFloor = 0;
	bool m_bRecon = false;			///< 정찰인가 전투인가.
	bool m_bInitSpot = false;		//예외적인 에러로 인해 클라측 스팟정보를 클리어해야할때.
private:
	int m_AP = 0;
	int m_apMax = 0;
public:
	int m_Power = 0;		// 전투력
	int m_Ladder = 0;		// 레더점수.
	XArchive m_arParam;		// 그외 커스텀 파라메터
	xBattleStartInfo( bool bRecon, XSpot *pBaseSpot ) 
		: m_bRecon(bRecon) {
		SetSpotData( pBaseSpot );
	}
#ifdef _CLIENT
	xBattleStartInfo() {}
#endif // _CLIENT
	// npc와 전투하는것인가.
	bool IsVsNpc() const {
		return m_idEnemy == 0;
	}
	bool IsValid() const ;
	inline bool IsInvalid() const {
		return !IsValid();
	}
	// SendBattleInfo에서만 사용함.
	void _SetAPs( int ap, int apMax ) {
		m_AP = ap;
		m_apMax = apMax;
	}
	GET_ACCESSOR( int, AP );
	GET_ACCESSOR( int, apMax );
 	void Serialize( XArchive& ar, XSpot *pBaseSpot ) const;
	void DeSerialize( XArchive& ar, XWorld *pWorld, int ver = 0 );
	void SetSpotData( XSpot *pBaseSpot );
//  	void DeSerialize( XArchive& ar, XSpot *pBaseSpot, int ver = 0 );
};
//////////////////////////////////////////////////////////////////////////
struct xReward;
struct xReward {
	xtReward rewardType;		///< 보상타입
	ID idReward;			///< 보상의 아이디
	int num;				///< 보상개수
	DWORD dwParam = 0;		/// 다용도 파라메터
	float rateDrop = 1.f;	/// 드랍일경우 드랍 확률
	int GetExpBook() const {
		return (int)dwParam;
	}
	bool LoadFromXML( XEXmlNode& nodeReward, LPCTSTR szTag );
	bool SaveXML( XEXmlNode& nodeRoot, LPCTSTR szTag, const std::string& strcNodeName = "reward" ) const;
#ifdef WIN32
	bool SaveXML( XEXmlNode& nodeRoot, const char* cTag, const std::string& strcNodeName = "reward" ) const {
		const _tstring strTag = C2SZ(cTag);
		return SaveXML( nodeRoot, strTag.c_str(), strcNodeName );
	}
#endif // WIN32
	static int sLoadResFromXMLToAry( XEXmlNode& nodeRoot, LPCTSTR szNodeName, LPCTSTR szTag, std::vector<xReward> *pOutAry );
	xReward() : rewardType( xtReward::xRW_NONE ), idReward( 0 ), num( 1 ) {}
	xReward( XHero* pHero );
	inline void SetItem( ID idProp, int _num = 1 ) {
		rewardType = xtReward::xRW_ITEM;
		idReward = idProp;
		num = _num;
	}
	inline void SetGold( int gold ) {
		rewardType = xtReward::xRW_GOLD;
		num = gold;
	}
	inline void SetCash( int cash ) {
		rewardType = xtReward::xRW_CASH;
		num = cash;
	}
	inline void SetGuildCoin( int _num ) {
		rewardType = xtReward::xRW_GUILD_POINT;
		num = _num;
	}
	inline void SetResource( XGAME::xtResource typeRes, int numRes ) {
		rewardType = xtReward::xRW_RESOURCE;
		idReward = typeRes;
		num = numRes;
	}
	inline XGAME::xtResource GetResourceType() const {
		XBREAK( rewardType !=  XGAME::xtReward::xRW_RESOURCE );
		return (XGAME::xtResource)idReward;
	}
	inline void SetHero( ID idProp, int _num = 1 ) {
		rewardType = xtReward::xRW_HERO;
		idReward = idProp;			// empty한 컨트롤을 만들때 0이올수 있음.
		num = _num;
	}
	ID GetidHero() const {
		if( rewardType == xtReward::xRW_HERO )
			return idReward;
		return 0;
	}
	ID GetidItem() const {
		if( rewardType == xtReward::xRW_ITEM )
			return idReward;
		return 0;
	}
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int );
	bool IsValid() const {
		return IsValidxtReward( rewardType );
	}
	bool IsTypeResource() const {
		return rewardType == xtReward::xRW_RESOURCE;
	}
};
//////////////////////////////////////////////////////////////////////////
struct xParam {
#ifdef _DEBUG
	_tstring _strParamForDebug;	// 디버깅용. 보통 dwParam에 숫자형아이디가 들어있을때 원래 문자열을 저장함.
#endif // _DEBUG
	_tstring strParam;	// 스트링형 파라메터
	union {
		struct {
			BYTE b[ 4 ];
		};
		struct {
			WORD w[ 2 ];
		};
		DWORD dwParam;
		int nParam;
		float fParam;
	};
	xParam() : dwParam( 0 ) {}
	void SetDebugStr( LPCTSTR szStrDebug ) {
#ifdef _DEBUG
		_strParamForDebug = szStrDebug;
#endif // _DEBUG
	}
	void SetDebugStr( const _tstring& strDebug ) {
#ifdef _DEBUG
		_strParamForDebug = strDebug;
#endif // _DEBUG
	}
	void Serialize( XArchive& ar ) const {
		ar << strParam;
		ar << dwParam;
	}
	void DeSerialize( XArchive& ar, int ) {
		ar >> strParam;
		ar >> dwParam;
	}
};
//////////////////////////////////////////////////////////////////////////
struct xExpTable {
	DWORD m_dwMaxExp = 0;		// 다음레벨로 가기위한 경험치
	float m_secTrainPerExp = 0;	// 경험치당 훈련시간
	float m_goldPerExp = 0;		// 경험치당 금화
};
//////////////////////////////////////////////////////////////////////////
struct xAbil {
	int point = -1;		// -1:락상태 
	bool IsLock() const {
		return point == -1;
	}
	bool IsUnlock() const {
		return !IsLock();
	}
	void SetUnlock() {
		point = 0;
	}
};
//////////////////////////////////////////////////////////////////////////
// 현재 특성연구중인 정보.
struct xResearch {
private:
	ID m_snHero = 0;
	XGAME::xtUnit m_unit = XGAME::xUNIT_NONE;
	ID m_idAbil = 0;			// 현재 연구중인 특성
	XTimerTiny m_timer;			// 연구 타이머
	xSec m_secTotal = 0;		// 연구에 필요한 총 시간
	void Init() {
		m_snHero = 0;
		m_unit = XGAME::xUNIT_NONE;
		m_idAbil = 0;
		m_timer.Off();
		m_secTotal = 0;
	}
public:
	GET_ACCESSOR_CONST( ID, snHero );
	GET_ACCESSOR_CONST( XGAME::xtUnit, unit );
	GET_ACCESSOR_CONST( ID, idAbil );
	GET_ACCESSOR_CONST( xSec, secTotal );
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int );
	// 연구를 시작한다.
#ifdef _CLIENT
// 	void Start();
#else
	void Start( ID snHero, XGAME::xtUnit unit, ID idAbil, xSec secResearchComplete );
#endif // not _CLIENT
	void DoComplete() {
		Init();
	}
	///< 연구시간이 끝났는가.
	bool IsComplete() const {
		if( XBREAK( m_idAbil == 0 ) )
			return false;
		//	XBREAK( m_secResearchStart == 0 );
		XBREAK( m_timer.IsOff() );
		XBREAK( m_secTotal == 0 );
		return m_timer.IsOver( m_secTotal );
	}
	///< 남은 연구시간
	int GetsecRemain() const {
		if( XBREAK( m_idAbil == 0 ) )
			return -1;
		XBREAK( m_timer.IsOff() );
		XBREAK( m_secTotal == 0 );
		int secRemain = (int)( (xSec)m_secTotal - m_timer.GetsecPass() );
		if( secRemain < 0 )
			secRemain = 0;
		return secRemain;
	}
	xSec GetsecPass() const {
		XBREAK( m_timer.IsOff() );
		return m_timer.GetsecPass();
	}
	xSec GetsecStart() const {
		XBREAK( m_timer.IsOff() );
		return m_timer.GetsecStart();
	}
	///< 현재 연구중인가
	bool IsResearching() const {
		if( m_idAbil != 0 ) {
			XBREAK( m_timer.IsOff() );
			return true;
		}
		return false;
	}
}; // struct xResearch {

struct xEncounter {
	enum { VER = 1 };
	_tstring m_strDefender;	// 유황기습당한 사람이름.
	ID m_idSpot = 0;				// 기습한 스팟 아이디
	ID m_idDefender = 0;		// 유황기습당한 사람.
	int m_numSulfur = 0;		// 얻은 유황양.
	xSec m_secEncount = 0;	// 기습한 시간.
	xEncounter() {}
	xEncounter( ID idSpot, ID idDefender, int numSulfur
						, xSec secEncount, LPCTSTR szName ) {
		m_strDefender = szName;
		m_idSpot = idSpot;
		m_idDefender = idDefender;
		m_numSulfur = numSulfur;
		m_secEncount = secEncount;
	}
	void Serialize(XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
	bool IsValid() const {
		if( m_strDefender.empty() )
			return false;
		if( m_idSpot == 0 )
			return false;
		if( m_idDefender == 0 )
			return false;
		if( m_numSulfur == 0 )
			return false;
		if( m_secEncount == 0 )
			return false;
		return true;
	}
}; // struct xEncounter {

struct xJewelInfo {
	int m_idxJewel = 0;		// 광산 인덱스 번호.
	ID m_idOwner = 0;			// 현재 광산소유주.
	ID m_idMatchEnemy = 0;	// 현재 매치된 상대
	_tstring m_strName;	// 광산소유주 이름.
	int m_LevelMine = 0;	// 광산레벨
	int m_Defense = 0;		// 광산 방어도
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int ver = 0 );
};

// 매치된 상대의 정보
struct xJewelMatchEnemy {
//	int m_idxMine = 0;
	int m_idAcc = 0;
	_tstring m_strName;
	_tstring m_strFbUserId;
	_tstring m_strHello;
	int m_lvAcc = 0;
	int m_LvMine = 0;
	int m_Defense = 0;
	int m_Power = 0;
	int m_Ladder = 0;
	int m_idxLegion = -1;
	int m_lootJewel = 0;
	bool m_bDummy = false;
	void Serialize( XArchive& ar );
	void DeSerialize( XArchive& ar, int ver = 0 );
//#ifdef _DEV
	bool IsDummyUser() const {
		return m_bDummy;
//		return m_strName == _T("TEST_USER");
	}
//#endif // DEV
};
//////////////////////////////////////////////////////////////////////////
/**
 @brief 카메라 이동 관리자.
*/
#ifdef _CLIENT
struct xFocusMng {
	CTimer m_Timer;
	XE::VEC2 vStart;	// 시작 위치
	XE::VEC2 vEnd;		// 끝위치
	ID m_snOrder = 0;
	bool IsScrolling() {
		return m_Timer.IsOn() == TRUE;
	}
	XE::VEC2 GetCurrFocus();
	// _sec==0이면 자동시간계산.
	void DoMove( const XE::VEC2& _vStart, const XE::VEC2& _vEnd, float _sec = 0 );
	void Stop() {
		m_Timer.Off();
	}
};
#endif // _CLIENT

//////////////////////////////////////////////////////////////////////////
/**
 @brief 인앱결제시 서버끼리 주고받는 구조체
*/
struct xInApp {
	xtPlatform m_Platform;
	xtCashType m_typeProduct;
	_tstring m_idsProduct;		// global.xml에 정의되어있는 제품 아이디
	std::string m_strcSignature;	
	std::string m_strcJsonReceipt;
	std::string m_strcPurchaseTime;		// 거래시간
	std::string m_strcOrderId;				// 거래아이디
	std::string m_strcPayload;
	xInApp() : m_Platform(XGAME::xPL_NONE), m_typeProduct(XGAME::xCT_NONE) {}
	xInApp( xtPlatform platform
				, xtCashType typeProduct
				, const _tstring& idsProduct
				, const std::string& strcSignature
				, const std::string& strcJsonReceipt
				, const std::string& strcPurchaseTime
				, const std::string& strcOrderId 
				, const std::string& strcPayload )
		: m_Platform(platform), m_typeProduct(typeProduct)
		, m_idsProduct(idsProduct), m_strcSignature(strcSignature)
		, m_strcJsonReceipt(strcJsonReceipt), m_strcPurchaseTime(strcPurchaseTime)
		, m_strcOrderId(strcOrderId), m_strcPayload(strcPayload) {
	}
	bool IsSubscribe() const {
		return m_typeProduct == XGAME::xtCashType::xCT_SUBSCRIBE;
	}
	bool IsNormal() const {
		return m_typeProduct == XGAME::xtCashType::xCT_NORMAL;
	}
	void Serialize( XArchive& ar ) const {
		ar << (BYTE)m_Platform;
		ar << (BYTE)m_typeProduct;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << m_idsProduct;
		ar << m_strcSignature << m_strcJsonReceipt;
		ar << m_strcPurchaseTime << m_strcOrderId;
		ar << m_strcPayload;
	}
	void DeSerialize( XArchive& ar, int ver ) {
		BYTE b0;
		_tstring str;
		ar >> b0;		m_Platform = (XGAME::xtPlatform)b0;
		ar >> b0;		m_typeProduct = (XGAME::xtCashType)b0;
		ar >> b0 >> b0;
		ar >> m_idsProduct;
		ar >> str;	m_strcSignature = SZ2C(str);
		ar >> str;	m_strcJsonReceipt = SZ2C(str);
		ar >> str;	m_strcPurchaseTime = SZ2C(str);
		ar >> str;	m_strcOrderId = SZ2C(str);
		ar >> str;	m_strcPayload = SZ2C(str);
	}
};

XE_NAMESPACE_END;	// XGAME
