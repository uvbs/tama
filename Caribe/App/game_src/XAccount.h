#pragma once

#include <stdlib.h>

#include "XFramework/XDBAccount.h"
#include "XArchive.h"
#include "XFramework/Game/XFLevel.h"

#include "XPropItem.h"
#include "XPostInfo.h"
#include "XPropCloud.h"
#include "XPropWorld.h"
#include "XPropTech.h"
#include "XGuild.h"
#include "../Resource/VerPacket.h"
#include "XPropUpgrade.h"
#include "XFramework/Game/XEComponents.h"
#include "XPropSquad.h"
#include "XStruct.h"
#include "XQuest.h"
#include "XQuestCon.h"


/*
ver2: szHello추가
ver3: 무역상 타이머 교체
ver4: 월드데이터 압축
ver5: postInfo(로 하려고 했으나 VER_POST로 대체)
ver6: 월정액관련 값 바뀜
*/
#define VER_SERIALIZE			6

/*
ver 2: ver_hero_serialize추가
ver 3: 훈련 완료 추가
ver 5: 렙업대기 플래그
ver 7: 사망여부
ver 8: 여분 특성포인트&언락포인트.
ver 9: 특성초기화 횟수
*/
#define VER_HERO_SERIALIZE		9

/*
ver 3: 리더 아이디 추가.
ver 4: full serialize 표시 추가
ver 5: 리소스 부대
ver 6: 안개정보
ver 7: rateHP, rateAtk
ver 8,9: 부대 mulAtk, mulHp적용
ver 10: XLegion의 squadron저장방식 변경(고정배열->가변배열)
*/
#define VER_LEGION_SERIALIZE	10
#define VER_RESOURCE_SERIALIZE	1
/*
	ver 2: 용맹포인트 추가.
	ver 3: 스페셜스팟 데이타 바뀜.
	ver 4: 훈련소 슬롯 추가
	ver 5: 행동력 추가
	ver 6: 영웅훈련 시간바뀜
	ver 7: maxap
	ver 8: snSlot
	ver 10: 훈련슬롯 param추가
	ver 11: 배틀세션
	ver 12: 총 전적
	ver 13: 기능 잠금해제
	ver 14: 시퀀스 저장
	ver 16: ptGuild
	ver 17: 테크포인트 삭제
	ver 18: 스팟 별개수
	ver 19: secLastSaved
	ver 20: 월드좌표 저장.
	ver 21: numBuyCash, amountBuyCash추가
*/
#define VER_ETC_SERIALIZE		21
/*
ver 2: 연구중 정보 추가
ver 3: 시간단위가 바뀜.
*/
#define VER_ABILITY_SERIALIZE	6		// 특성 바이너리 버전
#define VER_BATTLE_LOG			4		// 전투기록 버전
#define VER_ENCOUNTER			1			// 유황 encounter 버전

#if VER_SERIALIZE > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif
#if VER_HERO_SERIALIZE > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif
#if VER_LEGION_SERIALIZE > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif
#if VER_RESOURCE_SERIALIZE > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif
#if VER_ETC_SERIALIZE > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif
#if VER_ABILITY_SERIALIZE > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif
#if VER_LOG_ATTACK > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif
#if VER_LOG_DEFENSE > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif
#if VER_ENCOUNTER > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif

class XHero;
class XWorld;
class XLegion;
class XSquadron;
class XBaseItem;
class XPostInfo;
class XSpot;
class XSpotCastle;
class XSpotJewel;
class XSpotSulfur;
class XSpotMandrake;
class XSpotNpc;
class XSpotDaily;
//class XSpotSpecial;
class XSpotCampaign;
class XSpotVisit;
class XSpotCash;
class XDelegateSpot;
class XQuestMng;
class XDelegateQuestMng;
class XGuild;
class XAccount;

struct xSquadStat;

namespace XGAME {
	enum xtCashSkill {
		xCS_NONE,
		xCS_REGEN_SPOT,		// 스팟 리젠
		xCS_CHANGE_PLAYER,	// 상대 교체
		xCS_SULFUR_RETREAT,	// 유황지대 도주
	};
	// 계정단위 버프 효과
	struct xBuff {
		ID idBuff = 0;
		_tstring sid;
		_tstring strIcon;
		ID idName = 0;		// 버프 이름
		ID idDesc = 0;		// 버프 설명
		XTimer2 timer;
		ID getid() {
			return idBuff;
		}
		// 버프 남은 시간
		float GetRemainSec() {
			return timer.GetsecRemainTime();
		}
		// 버프 지나간 시간
		float GetPassSec() {
			return timer.GetsecPassTime();
		}
		// 버프 최대 시간
		float GetBuffSec() {
			return timer.GetsecWaitTime();
		}
		int Serialize(XArchive& ar);
		int DeSerialize(XArchive& ar, int ver);
	};
	// 전투기록
	struct xBattleLog;
	// 보유한 자원으로 영웅x를 훈련시킬경우 필요한 정보의 공통 구조체
	struct xTrainInfo {
		int numItemWill = 0;	// 훈련후 받게된 아이템 수
		int numRemain = 0;		// 채워야할 남은 아이템 수
		int secTotal = 0;		// 총 훈련시간
		int needRes1 = 0;   // 필요한 자원양
		int needRes2 = 0;
		int useRes1 = 0;		// 소모될 첫번째 자원수
		int useRes2 = 0;		// 소모될 두번째 자원수(있다면)
	};
}

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/02/17 14:24
*****************************************************************/
class XDelegateBuff
{
public:
	XDelegateBuff() {}
	virtual ~XDelegateBuff() {}
	//
	virtual void DelegateFinishBuff(XSPAcc spAcc, const XGAME::xBuff& buff) {}
}; // class XDelegateBuff
//////////////////////////////////////////////////////////////////////////
/*!
 * \class XAccount
 *
 * \brief 
 *
 * \author xuzhu
 * \date 5월 2016
 */
class XAccount : public XDBAccount
{
public:
#ifdef _CLIENT
	static XSPAcc s_spInstance;
	static XSPAcc sGetPlayer() { return s_spInstance; }
	static XSPAccConst sGetPlayerConst() {
		return s_spInstance;
	}
	static void sSetPlayer( XSPAcc spAcc ) { 
#ifdef _DEBUG
		XBREAK( s_spInstance != nullptr );
#endif // _DEBUG
		s_spInstance = spAcc; 
	}
	static void sDestroyPlayer() {
		s_spInstance.reset();
	}
#endif // _CLIENT
	_tstring m_strPayload;			// 임시
	static void sDeserializeResource(XArchive& ar, xResourceAry& ary) {
		int ver;
		ar >> ver;
		ar >> ary;
	}
	static bool sDeserializeResource2(XArchive& ar, xResourceAry& ary) {
		int ver;
		ar >> ver;
		if( ver == 0 )
			return false;
		ar >> ary;
		return true;
	}
	static void sSerializeResource(XArchive& ar, const xResourceAry& ary) {
		ar << VER_RESOURCE_SERIALIZE;
		ar << ary;
	}
	enum {
		LV_KILL = 7,		// 소탕버튼 나오는 레벨
	};
	static int s_secOfflineForSimul;		// 스팟침공 시뮬레이션시 오프라인 시간.
	static int s_bTraderArrived;			// 무역상이 도착함.
	// 헬퍼 함수
	static XBaseItem* sCreateItem( const XPropItem::xPROP *pProp, int num);
	static XBaseItem* sCreateItem(ID idItem, int num);
	static int sGetMaxSquadByLevelForPlayer(int level);
	static int sGetLevelByUnlockUnit(int numUnits);
	// 길드
	static void sSerializeGuildJoinReq( XSPAcc spAcc, XArchive* pOut );
	static void sSerializeGuildInfo( XSPAcc spAcc, XArchive* pOut );
	static void sSerializeGuildInfo( ID idGuild, XGAME::xtGuildGrade grade, const XList4<ID>& listJoinReq, XArchive* pOut );
	static void sDeSerializeGuildJoinReq( XArchive& ar, XSPAcc spAcc );
	static void sDeSerializeGuildInfo( XArchive& ar, XSPAcc spAcc );
	static void sUpdateByGuildEvent( XSPAcc spAcc, XGAME::xtGuildEvent event, const XGuild* pGuild, XArchive& arParam );
public:
	//////////////////////////////////////////////////////////////////////////
	// 훈련소 슬롯
	struct xTrainSlot {
	private:
		DWORD _param[2];
	public:
		ID snSlot = 0;
		XTimerTiny timerStart;
		XGAME::xtTrain type = XGAME::xTR_NONE;			// 훈련종류
		ID snHero = 0;						// 훈련받고 있는 영웅
		int secTotal = 0;		// 훈련에 필요한 총 시간
		xTrainSlot() {
			snSlot = XE::GenerateID();
			XCLEAR_ARRAY(_param);
		}
		ID getid() {
			return snSlot;
		}
		// 렙업훈련시 받게될 경험치.
		DWORD GetAddExp() {
//			XASSERT(type == XGAME::xTR_LEVEL_UP);
			return _param[0];
		}
		void SetAddExp(DWORD expAdd) {
			_param[0] = expAdd;
		}
		void Clear() {
			timerStart.Off();
			type = XGAME::xTR_NONE;
			snHero = 0;
			secTotal = 0;
			_param[0] = _param[1] = 0;
		}
		bool IsActive() const {
			return type != XGAME::xTR_NONE;
		}
		bool IsLevelup() {
			return type == XGAME::xTR_LEVEL_UP;
		}
		bool IsSquadup() {
			return type == XGAME::xTR_SQUAD_UP;
		}
		bool IsSkillAnyup() {
			return type == XGAME::xTR_SKILL_ACTIVE_UP || type == XGAME::xTR_SKILL_PASSIVE_UP;
		}
		bool IsSkillActiveup() {
			return type == XGAME::xTR_SKILL_ACTIVE_UP;
		}
		bool IsSkillPassiveup() {
			return type == XGAME::xTR_SKILL_PASSIVE_UP;
		}
		bool IsComplete() const {
			if (!IsActive())
				return false;
			if (timerStart.IsOver(secTotal))
				return true;
			return false;
		}
#ifdef _SERVER
		xSec DoStartTrain(XGAME::xtTrain _type, ID _snHero, float secTotalTrain) {
			XBREAK(type != XGAME::xTR_NONE);		// 초기화가 안되어있으면 에러
			timerStart.DoStart();
			type = _type;
			snHero = _snHero;
			secTotal = (int)secTotalTrain;
			return timerStart.GetsecStart();
		}
		xSec DoStartLevelup(ID _snHero, float secTotalTrain) {
			return DoStartTrain(XGAME::xTR_LEVEL_UP, _snHero, secTotalTrain);
		}
		xSec DoStartSquadup(ID _snHero, float secTotalTrain) {
			return DoStartTrain(XGAME::xTR_SQUAD_UP, _snHero, secTotalTrain);
		}
		xSec DoStartSkillActiveup(ID _snHero, float secTotalTrain) {
			return DoStartTrain(XGAME::xTR_SKILL_ACTIVE_UP, _snHero, secTotalTrain);
		}
		xSec DoStartSkillPassiveup(ID _snHero, float secTotalTrain) {
			return DoStartTrain(XGAME::xTR_SKILL_PASSIVE_UP, _snHero, secTotalTrain);
		}
		xSec DoStartSkillup(XGAME::xtTrain type, ID _snHero, float secTotalTrain) {
			return DoStartTrain(type, _snHero, secTotalTrain);
		}
#endif // SERVER
		int Serialize(XArchive& ar);
		int DeSerialize(XArchive& ar, int);
		// 훈련 남은 시간을 얻는다.
		int GetsecRemain() {
			int sec = secTotal - (int)timerStart.GetsecPass();
			if( sec < 0 )
				sec = 0;
			return sec;
		}
		_tstring GetstrSecRemain() {
			return XGAME::GetstrResearchTime( GetsecRemain() );
		}
		// 지나간 훈련시간을 얻는다.
		int GetsecPass() {
			return (int)timerStart.GetsecPass();
		}
		// 훈련시간 감소
		void DoDecreaseTrainingSec(int sec) {
			secTotal -= sec;		// 총 훈련시간에서도 빼줌.
			if (secTotal < 0)
				secTotal = 0;
		}
	}; // xtTrainSlot
	struct bitfield {
		BYTE bReadyBattle : 1;	// 최초 전투배치씬 튜토리얼을 마치면 1
		BYTE bSummonHero : 1;	// 최초 영웅소환을 마치면 1
		BYTE bTouchSquadInReady : 1; // 전투준비씬에서 부대를 선택해봄.
		BYTE bControlSquadInBattle : 1;   // 전투씬에서 부대를 수동조작해봄.
		BYTE b4 : 1; BYTE b5 : 1; BYTE b6 : 1; BYTE b7 : 1;
		BYTE b8 : 1; BYTE b9 : 1; BYTE b10 : 1; BYTE b11 : 1; BYTE b12 : 1; BYTE b13 : 1; BYTE b14 : 1; BYTE b15 : 1;
		BYTE b16 : 1; BYTE b17 : 1; BYTE b18 : 1; BYTE b19 : 1; BYTE b20 : 1; BYTE b21 : 1; BYTE b22 : 1; BYTE b23 : 1;
		BYTE b24 : 1; BYTE b25 : 1;	BYTE b26 : 1; BYTE b27 : 1; BYTE b28 : 1; BYTE  : 1; BYTE bUnlockTreant : 1; BYTE bUnlockAngel : 1;
	} m_bitFlagTutorial;
	//////////////////////////////////////////////////////////////////////////
private:
  // 전투시작시 담겨질 전투정보(DB저장)
  struct xBattleSession {
    ID snSession = 0;
    ID idAccEnemy = 0;      // 상대중인 적의 계정아이디
    ID idSpot = 0;          // 어떤 스팟의 전투인가.
    DWORD param = 0;       // 만약 캠페인형태라면 스테이지 인덱스나 스테이지 아이디등의 부가정보.
    XSPLegion spEnemy;      // 적 부대 정보(이것을 DB에 저장하는 이유는 전투중 잠깐 끊겼다가 다시 붙었을때 부대 정보가 없어서 무효가 되는일이 없도록하기위함)
    XTimerTiny timerStart;      // 전투 시작시간
    void Clear() {
      snSession = 0;
      idAccEnemy = 0;
      idSpot = 0;
      param = 0;
      Release();
    }
    void Release() {
      spEnemy.reset();
    }
    int Serialize( XArchive& ar );
    int DeSerialize( XArchive& ar, int );
  };
	DWORD m_Ver;					// 계정 시리얼라이즈 버전
	_tstring m_strID;				// 영문아이디(이메일)
	_tstring m_strPassword;			// 비밀번호
	_tstring m_strUUID;
	_tstring m_strFbUserId;			// facebook userid
	_tstring m_StrSessionKey;
	_tstring m_strName;				// 닉네임
	_tstring m_strHello;			// 성스팟 인사말.
	CTimer m_timerPlay;				///< 접속후 총 플레이 시간.
	xSec m_secPlay = 0;			///< 계정생성부터 총 플레이 시간.
	XList4<XGAME::xBuff> m_listBuff;	///< 계정단위 버프리스트.
	XList4<XGAME::xBattleLog> m_listBattleLogAttack;	// 전투기록(공격)
	XList4<XGAME::xBattleLog> m_listBattleLogDefense;	// 전투기록(방어)
	BYTE m_numAttaccked = 0;		// 오프라인동안 공격받은 횟수.
	XArrayN<BYTE, 16> m_aryUnlockedUnit;	// 유닛 잠김상태(0=lock 1=unlock and noCheck 2=unlock
//	int m_dummy = 0;
	xBattleSession m_BattleSession;     // 전투정보 세션
	std::map<ID,int> m_mapSpotStar;		// 스팟들의 별개수.idSpot/별개수. 맵에 없는건 3별로 졸업했다는 뜻.
	int m_numStar = 0;					// 스팟으로 부터 얻은 별점을 누적한다. 퀘스트로 사용이 되면 그만큼은 차감된다.
	XE::VEC2 m_vFocusWorld;		// 현재 보고있는 월드맵좌표(가장 마지막으로 전투했을때의 좌표)
	int m_numBuyCash = 0;			// 캐쉬 구입 횟수
	int m_amountBuyCash = 0;	// 구입한 캐쉬 총 개수
	int m_numHirePremium = 0;	// 고급영웅소환 횟수.
protected:

	ID	m_CurrSN;											// ?
	XFLevel m_Level;										// 레벨/경험치 객체
	DWORD m_Gold;											// 게임머니
	DWORD m_Cashtem;										// 캐쉬 아이템(보석같은..)
	xResourceAry m_aryResource;								// 자원 보유량
	int m_PowerExcludeEmpty = 0;	            // 현재 군단의 전투력(빈슬롯을 포함하지않음.). 이값은 DB에 저장되지 않는다. 근데 전투력 랭킹같은거 하려면 저장해야할듯. 
	int m_powerTotal = 0;                 // 전체 군사력
	int m_PowerIncludeEmpty = 0;							// 현재 군단의 전투력(빈슬롯을 포함함)
	int m_Ladder = 0;                      // 
	XList4<XHero*> m_listHero;								// 영웅 리스트
//	XArrayN<LegionPtr, XGAME::MAX_LEGION> m_aryLegion;		// 군단 배열
	XVector<XSPLegion> m_aryLegion;
	XList4<XBaseItem*> m_listItem;							// 아이템 인벤
	XList4<XPostInfo*> m_listPost;							// 우편함 인벤
	std::vector<ID> m_listShopSell;							// 상점 판매 목록
	int m_maxItems;		///< 인벤 최대개수
//	int m_ptBrave = 0;										// 용맹포인트.
	int m_ptGuild = 0;										// 길드포인트.
	int m_numBattleByPvp = 0;                  // 총 pvp횟수
	int m_numWinsByPvp = 0;                     // pvp전에서의 총 승리횟수
	
	XTimerTiny m_timerTrader;			// 무역상인 귀환타이머
	XTimerTiny m_timerShop;				// 무기상인
	int m_numSubscribe = 0;				// (메일을)지급받은 횟수
	xSec m_secNextSubscribe = 0;		// 월정액상품. 다음에 지급해야할 시간
	int	 m_cntSubscribeOffline;	// 월정액 아이템 모프라인이라 못받은거..

	XWorld* m_pWorld;										// 월드맵 정보
	xSec m_secLastSaved = 0;		// 마지막 저장시간.(온라인중에도 저장될수 있음)
	XGAME::xResearch m_Researching;		// 현재 연구중인 정보.
	XList4<xTrainSlot> m_listTrainSlot;	// 훈련소 슬롯
	int m_numFreeSlot = 1;			// 잠금이 풀린 슬롯 개수
	XQuestMng *m_pQuestMng;
	int m_AP = 0;					// 현재 행동력
	int m_maxAP = 0;
	XTimerTiny m_timerAP;			// 행동력 타이머
	ID m_GuildIndex;										// 
	XList4< ID > m_listGuildJoinReq;				// 가입 길드 리스트?
	XGAME::xtGuildGrade m_Guildgrade;						// 길드 등급
	int	m_GMLevel;											// 1이 가장높은 등급. 
	XECompBit m_bitUnlockMenu;			// 잠금해제된 기능들(xBM_XXXX)
	bool m_bUnlockTicketForPaladin = false;		// 기사 언락 포인트
	int m_numUnlockTicketForMiddleOrBig = 0;	// 중대형유닛중 하나를 언락시킬수 있는 포인트 개수.
	int m_lvOverSulfur = 0;		// 마지막으로 스폰된 유황스팟의 레벨오버값.
	std::map<std::string,bool> m_mapCompletedSeq;		// 실행완료된 시퀀스 아이디
	std::string m_idsLastSeq;			// 마지막으로 실행완료되었던 시퀀스
	XVector<XGAME::xEncounter> m_aryEncounter;		// 유황기습 정보.
	//////////////////////////////////////////////////////////////////////////
	// DB저장하지 않는 데이타
	// XLegion *m_pEnemyLegion;	
	// DB저장하지 않는 데이타
	//////////////////////////////////////////////////////////////////////////
private:
	void Init() {
		m_CurrSN = 0;
		m_Ver = VER_SERIALIZE;
		m_Gold = 0;
		m_Cashtem = 0;
		m_pWorld = nullptr;
		m_aryResource.assign( m_aryResource.size(), 0 );
		m_GMLevel = 0;
		m_maxItems = XGAME::ITEM_INIT_COUNT;
		m_pQuestMng = nullptr;
		m_timerPlay.Set(0);
		m_GuildIndex = 0;
		m_listGuildJoinReq.clear();
		m_Guildgrade = XGAME::xtGuildGrade::xGGL_NONE;
		m_cntSubscribeOffline = 0;
		*((DWORD*)(&m_bitFlagTutorial)) = 0;
		m_aryUnlockedUnit.Clear(0);
		m_aryUnlockedUnit[XGAME::xUNIT_SPEARMAN] = 2;
		m_aryUnlockedUnit[XGAME::xUNIT_ARCHER] = 2;
		XBREAK( m_aryUnlockedUnit.GetMax() < XGAME::xUNIT_MAX );
		SetlvLimit( 5 );		// 기본 5부터 시작
	}
	void Destroy();

	SET_ACCESSOR(XWorld*, pWorld);
	SET_ACCESSOR(DWORD, Gold);
	XBaseItem* AddItem(XBaseItem *pBaseItem);		// 아이템을 인벤에 추가

public:
	bool m_bDebugMode = false;
	XAccount();
	XAccount(ID idAccont);
	XAccount(ID idAccount, LPCTSTR szID);
	XAccount( ID idAcc, const _tstring& strUUID );
	virtual ~XAccount() { Destroy(); }

	// 액세서
	virtual int GetverCGPK() {
		return VER_CGPK;
	}
	GET_SET_TSTRING_ACCESSOR(strID);
	GET_ACCESSOR(const _tstring&, strPassword);
	GET_ACCESSOR(CTimer, timerPlay);
	GET_ACCESSOR(xSec, secPlay);
	GET_ACCESSOR( const XList4<xTrainSlot>&, listTrainSlot );
	GET_SET_ACCESSOR( int, numFreeSlot );
	GET_SET_ACCESSOR( int, AP );
	GET_ACCESSOR( XTimerTiny&, timerAP );
	GET_SET_ACCESSOR( int, maxAP );
	GET_ACCESSOR( XECompBit&, bitUnlockMenu );
	GET_SET_ACCESSOR( const XE::VEC2&, vFocusWorld );
	GET_ACCESSOR_CONST( const _tstring&, strUUID );
	GET_SET_ACCESSOR_CONST( const _tstring&, strHello );
#if defined(_CLIENT) || defined(_GAME_SERVER)
	int GetmaxAP( int level );
#endif
	GET_SET_ACCESSOR( bool, bUnlockTicketForPaladin );
	GET_SET_ACCESSOR( int, numUnlockTicketForMiddleOrBig );
	GET_SET_ACCESSOR( int, numBattleByPvp );
	GET_SET_ACCESSOR( int, numWinsByPvp );
	GET_SET_ACCESSOR( int, lvOverSulfur );
	GET_SET_ACCESSOR( int, numStar );
	GET_ACCESSOR( xSec, secLastSaved );
	GET_ACCESSOR( XGAME::xResearch&, Researching );
	GET_SET_ADD_ACCESSOR( int, numBuyCash );
	GET_SET_ADD_ACCESSOR( int, amountBuyCash );
	GET_SET_ADD_ACCESSOR( int, numHirePremium );
	GET_SET_ACCESSOR_CONST( const _tstring&, strFbUserId );
	GET_SET_ACCESSOR_CONST( int, GMLevel );
	void SetsecLastSave() {
		m_secLastSaved = XTimer2::sGetTime();
	}
	/// pvp전 승리율을 얻는다.
	float GetRatioWinsByPvp() {
		if( m_numBattleByPvp == 0 )
			return 0.5f;
		return (float)m_numWinsByPvp / m_numBattleByPvp;
	}
	// 훈련가능한 남은 슬롯수
	int GetNumRemainFreeSlot() {
		return m_numFreeSlot - GetNumActiveSlot();	// 총 풀린슬롯수에서 훈련중인 슬롯수를 뺀다.
	}
private:
	void SetCompleteResearch() {
		m_Researching.DoComplete();
	}
public:
	GET_SET_ACCESSOR(BYTE, numAttaccked);
	GET_SET_TSTRING_ACCESSOR(strName);	// LPCTSTR GetstrName(),	SetstrName( LPCTSTR )
	XFLEVEL_ACCESSOR(m_Level);
	GET_ACCESSOR_CONST(DWORD, Gold);
	GET_SET_ACCESSOR_CONST(DWORD, Cashtem);
	int GetWood(void) const { return m_aryResource[XGAME::xRES_WOOD]; }
	int GetIron(void) const { return m_aryResource[XGAME::xRES_IRON]; }
	int GetJewel(void) const { return m_aryResource[XGAME::xRES_JEWEL]; }
	int GetSulphur(void) const { return m_aryResource[XGAME::xRES_SULFUR]; }
	int GetMandrake(void) const { return m_aryResource[XGAME::xRES_MANDRAKE]; }
	void SetWood(int val) { m_aryResource[XGAME::xRES_WOOD] = val; }
	void SetIron(int val) { m_aryResource[XGAME::xRES_IRON] = val; }
	void SetJewel(int val) { m_aryResource[XGAME::xRES_JEWEL] = val; }
	void SetSulphur(int val) { m_aryResource[XGAME::xRES_SULFUR] = val; }
	void SetMandrake(int val) { m_aryResource[XGAME::xRES_MANDRAKE] = val; }
	const xResourceAry& GetaryResource() const {
		return m_aryResource;
	}
	SET_ACCESSOR( xResourceAry&, aryResource );
	GET_SET_ACCESSOR_CONST(int, Ladder);
	int GetPowerExcludeEmpty();
	int GetPowerIncludeEmpty();
#ifdef _CLIENT
	SET_ACCESSOR( int, PowerExcludeEmpty );
	SET_ACCESSOR( int, PowerIncludeEmpty );
#endif // CLIENT
	GET_SET_ACCESSOR_CONST(int, maxItems); //MAX Item Count 를 DB에 저장 시킨다.

	GET_ACCESSOR_CONST( const XTimerTiny&, timerTrader );
	GET_ACCESSOR_CONST( const XTimerTiny&, timerShop );
	void SerializeTimerByTrader( XArchive& ar );
	void DeserializeTimerByTrader( XArchive& ar );
	void DeserializeTimerByArmory( XArchive& ar );
	void AddcntSubscribeOffline(int val) { 
		m_cntSubscribeOffline += val; 
	}
	GET_SET_ACCESSOR_CONST( int, cntSubscribeOffline );
	GET_SET_ACCESSOR_CONST( int, numSubscribe );
	// 현재 월정액 상품 기간인가
	bool IsSubscribing() const {
		return m_numSubscribe > 0;
	}
	int AddnumSubscribe( int add ) {
		return (m_numSubscribe += add);
	}
	GET_SET_ACCESSOR_CONST( xSec, secNextSubscribe );
	GET_ACCESSOR(XWorld*, pWorld);
	GET_ACCESSOR(XQuestMng*, pQuestMng);

	GET_SET_ACCESSOR_CONST(ID, GuildIndex);
	ID GetidGuild() const { return m_GuildIndex;	}
	GET_SET_ACCESSOR_CONST(XGAME::xtGuildGrade, Guildgrade);
	void SetGuild( ID idGuild, XGAME::xtGuildGrade grade ) {
		m_GuildIndex = idGuild;
		m_Guildgrade = grade;
		m_listGuildJoinReq.clear();		// 길드가입하면 이건 필요가 없다는 가정
	}
	void ClearGuild() {
		m_GuildIndex = 0;
		m_Guildgrade = XGAME::xGGL_NONE;
//		m_ReqJoinGuildList.clear();
	}
// 	bool IsActiveSpecialSpotTimer() const {
// 		return m_secSpecialSpotReleased != 0;
// 	}
	//////////////////////////////////////////////////////////////////////////
	// 계정
	BOOL IsHavePassword() const {
		return (m_strPassword.empty() == FALSE) ? TRUE : FALSE;
	}
	BOOL  IsSamePassword(LPCTSTR szPassword) const {
		return (m_strPassword == szPassword) ? TRUE : FALSE;
	}
	void SetPassword(LPCTSTR szPassword) {
		m_strPassword = szPassword;
	}
	BOOL IsSameID(LPCTSTR szID) const {
		return m_strID == szID;
	}
// 	void SetSessionkey(LPCTSTR Sessionkey){ m_StrSessionKey = Sessionkey; }
// 	LPCTSTR  GetSessionKey(){ return m_StrSessionKey.c_str(); }

	void SetCurrSN(ID snnum){ m_CurrSN = snnum; }
	ID GetCurrSN()const { return m_CurrSN; }
	ID GenerateSN() {
		m_CurrSN++;
		return m_CurrSN;
	}
	void AddScore(int add) {
		m_Ladder += add;
		if (m_Ladder < 1)
			m_Ladder = 1;
	}
	BOOL AddGuildJoinReq(ID joinreqguildidx);
	int GetReqGuildJoinListSize() const { 
		return (int)m_listGuildJoinReq.size(); 
	}
	const XList4<ID>& GetlistGuildJoinReq() const {
		return m_listGuildJoinReq;
	}
	BOOL IsReqJoinGuild(ID joinreqguildidx) const;	//strName이 가입신청을 해둔 길드인지
	void ClearlistGuildJoinReq()	{
		m_listGuildJoinReq.clear();
	}
	void DelGuildJoinReq( ID idGuild ) {
		m_listGuildJoinReq.Del( idGuild );
	}
	// 스페셜 & 데이 스팟
// 	void SetSpecialSpotTimer(int dayAdjustWait = 0);		// 스페셜 스팟 스폰용 타이머를 작동
// 	int GetRemainDaySpecialSpot(void);
// 	int GetPassDaySpecialSpot(void);
	/**
	@brief 총플레이시간을 업데이트 한다.
	*/
	void UpdatePlayTimer(xSec secUpdate = 0) {
		if (secUpdate == 0) {
			m_secPlay += (xSec)m_timerPlay.GetPassSec();
		}
		else {
			m_secPlay = secUpdate;
		}
		m_timerPlay.Reset();
	}

	// 자원

	int AddResource(XGAME::xtResource type, int amount);		// 자원을 증감시킨다.
	int GetResource(XGAME::xtResource type) const;
#if defined(_CLIENT) || defined(_DEV)
	// 강제 동기화. 클라에서만 사용
	void SetResource(XGAME::xtResource type, int num);
#endif // defined(_CLIENT) || defined(_DEV)

	// m_Gold의 타입을 XDWord 같은걸로 일반화 시켜 오버플로 되지 않는 Add같은걸 만들어 넣을것.
	int AddGold(int add);
	void ClearGold() {
		m_Gold = 0;
	}
	int AddCashtem( int add );

	XGAME::xtSize GetSizeByLevel(int levelAcc);

	bool IsRemainSquad();
#if defined(_CLIENT) || defined(_GAME_SERVER)
	// abil, PropTech
	bool IsUnlockableAbil( XHero *pHero, XGAME::xtUnit unit, XPropTech::xNodeAbil *pProp );
	XGAME::xtError GetUnlockableAbil( XHero *pHero, XGAME::xtUnit unit, XPropTech::xNodeAbil *pProp );
	XGAME::xtError GetUnlockableAbil( XHero *pHero, XGAME::xtUnit unit, ID idAbilNode );
	bool IsEnableAbil( XHero *pHero, XGAME::xtUnit unit, XPropTech::xNodeAbil *pProp );
	///<
	bool IsEnoughResourceForResearch( XHero *pHero/*, XGAME::xtUnit unit*/ );
	bool IsEnoughIdxResourceForResearch( XHero *pHero/*, XGAME::xtUnit unit*/, int idxRes );
	bool IsEnoughIdxResourceForResearchWithPoint( /*XGAME::xtUnit unit,*/ int numPoint, int idxRes );
	XPropTech::xtResearch& GetCostAbilWithNum( int numPoint );
// 	XPropTech::xtResearch& GetCostAbilCurr( XGAME::xtUnit unit );
#endif

	// Hero

	XHero* GetHero(ID snHero);
	inline XHero* GetpHeroBySN( ID snHero ) {
		return GetHero( snHero );
	}
	const XHero* GetpcHeroBySN( ID snHero ) const;
	XHero* GetHeroByidProp( ID idProp );
	XHero* GetpHeroByAtkType( XGAME::xtAttack typeAtk );
	XHero* GetpHeroByUnit( XGAME::xtUnit unit );
	XHero* GetpHeroByIndex( int idx );
	// 앞으로 GetlistpHeroByInven을 사용할것.
	void _GetInvenHero(XList4<XHero*> &listHero) {
		listHero = m_listHero;
	}
	inline void GetlistpHeroByInven( XList4<XHero*> *plistOut ) {
		*plistOut = m_listHero;
	}
	inline const XList4<XHero*>& GetlistHeroByInvenConst() const {
		return m_listHero;
	}
	void GetarypHeroByInven( XVector<XHero*> *pAryOut );
	void GetaryidPropHeroByInven( XVector<ID> *pAryOut );
	int GetHerosListExceptLegion(XArrayLinearN<XHero*, 1024> *pOutAry, XLegion *pLegion);
	void DestroyHeros(void);
	XHero* AddHero(XHero *pHero);

	void AddExpToHeros(int add, XLegion *pLegion, XVector<ID>* pOutAryLevelup = nullptr);
	void DeleteHeroInLegion(ID snHero);
	void DestroyHero(ID snHero);
	/// 현재 pHero영웅의 스킬 레벨업이 가능한 조건인가
#if defined(_CLIENT) || defined(_GAME_SERVER)
	bool IsAbleLevelupSkill(XHero *pHero, XGAME::xtTrain type) {
		return GetAbleLevelupSkill(pHero, type) == XGAME::xES_OK;
	}
	XGAME::xtSkillLevelUp GetAbleLevelupSkill(XHero *pHero, XGAME::xtTrain type);		// 영웅의 스킬이 레벨업 가능한지 조사
	//	void GetNeedSkillUpItem(int lvSkill, ID *pOutId, int *pOutNum);
#if defined(_XSINGLE) || defined(_GAME_SERVER)
	void CreateFakeAccount(void);
#endif // defined(_XSINGLE) || defined(_GAME_SERVER)
//	BOOL IsEmptyAbilMap();
	bool IsAbleUpgradeHero();
	bool IsAbleUpgradeHeroAny(XHero *pHero);
	bool IsAbleLevelUpHero();
	bool IsAbleLevelUpHero(XHero *pHero);
	bool IsAbleLevelUpSquad( XHero *pHero );
	bool IsUpdateHero();
	bool IsAbleLevelUpSquad();
	bool GetAbleLevelupSkill();
#endif // CLIENT or GAME_SERVER
#ifdef _GAME_SERVER
	int DoCompleteCurrResearch();
#endif // _GAME_SERVER

	// Legion
#ifdef _XSINGLE
	void SetspLegion( int idxLegion, XSPLegion spLegion );
//	XSPLegion CreatespLegion( const XGAME::xLegion& infoLegion );
#endif // _XSINGLE
	XSPLegion& GetCurrLegion(void) {
		return m_aryLegion[GetCurrLegionIdx()];
	}
	int GetCurrLegionIdx(void) {
		return 0;
	}
	// 	void SetCurrLegion( XLegion *pLegion ) {
	// 		m_aryLegion[0] = pLegion;
	// 	}
	XSPLegion GetLegionByIdx(int idx) {
		if (idx < 0 || idx >= m_aryLegion.Size())
			return nullptr;
		return m_aryLegion[idx];
	}
	void DestroyLegion(void);
	// 군단 최대수를 얻는다.
	int GetMaxLegions(void) {
		return m_aryLegion.Size();
	}
// 	XArrayN<LegionPtr, XGAME::MAX_LEGION>& GetaryLegion(void) {
// 		return m_aryLegion;
// 	}
	bool IsHaveLegion(int idx) const {
		if (idx < 0 || idx >= m_aryLegion.Size())
			return FALSE;
		return m_aryLegion[idx] != nullptr;
	}
	bool IsEmptyLegion(int idx) const {
		return !IsHaveLegion(idx);
	}

	XGAME::xtGrade GetRandomGradeHeroByTable(int levelUser) const;
	XSquadron* CreateSquadronByRandom(XLegion *pLegion, int idxSquad, int levelUser, LPCTSTR szHero, XGAME::xtGrade grade, XGAME::xtUnit unit);
#if defined(_XSINGLE) || !defined(_CLIENT)
	XSquadron* CreateSquadron(XLegion *pLegion, int idxSquad, LPCTSTR szHeroIdentifier, int levelSquad, int tierUnit);
	XSquadron* CreateSquadron(XLegion *pLegion,
														int idxSquad,
														int lvHero,
														int lvSquad,
														int lvActive,
														int lvPassive,
														LPCTSTR idsHero,
														XGAME::xtGrade grade,
														XGAME::xtUnit unit );
	static XSquadron* sCreateSquadron( XLegion *pLegion, int idxSquad, const _tstring& idsHero, int levelSquad, int tierUnit, XSPAccConst spAcc );
#endif // defined(_XSINGLE) || !defined(_CLIENT)
	// 스팟

	XSpot* CreateNewSpot( ID idSpot, XDelegateSpot *pDelegate );
private:
	XSpot* CreateNewSpot( XPropWorld::xBASESPOT *pBaseProp, XDelegateSpot *pDelegate );
public:
	void CreateSpotsByCloud( XPropCloud::xCloud *pPropCloud, XDelegateSpot *pDelegate, XArrayLinearN<XSpot*, 1024> *pOutAry = nullptr, bool bDummyMode = false );
// 	XSpotSpecial* DoCheckSpecialSpotActivateNow();
	void AddSpot( XSpot *pBaseSpot );
  
	// 시리얼라이즈

	virtual int Serialize(XArchive& ar) override;
	virtual int DeSerialize(XArchive& ar) override;
	int SerializeResource(XArchive& ar);
	int SerializeResource2( XArchive& ar );
	int DeSerializeResource(XArchive& ar);
	int DeSerializeResource2( XArchive& ar );
	int SerializeEtcData(XArchive& ar);
	int DeSerializeEtcData(XArchive& ar);
	int SerializeItems(XArchive& ar);
	int DeSerializeItems(XArchive& ar);
	int DeSerializeItems2(XArchive& ar);
	int SerializeAbil(XArchive& ar);
	int DeserializeAbil(XArchive& ar);
	int DeSerializeQuest(XArchive& ar);
	int SerializeQuest(XArchive& ar);
	int SerializePostInfo(XArchive& ar) const;
	int DeSerializePostInfo(XArchive& ar, int verPost);
	XBaseItem* DeserializeAddItem(XArchive& ar, int ver);
	XBaseItem* DeserializeUpdateItem(XArchive& ar, int verItem);
	int SerializeShopList(XArchive& ar);
	int DeSerializeShopList(XArchive& ar);
	int SerializeJoinReqGuild(XArchive& ar);
	int DeSerializeJoinReqGuild(XArchive& ar);
	int SerializeSubscribe(XArchive& ar);
	int DeSerializeSubscribe(XArchive& ar);
	int SerializeTrainSlot(XArchive& ar);
	int DeSerializeTrainSlot(XArchive& ar, int verEtc);
	int SerializeUnlockPoint( XArchive& ar );
	int DeSerializeUnlockPoint( XArchive& ar, int verEtc );
	int SerializeEncounter( XArchive& ar );
	int DeSerializeEncounter( XArchive& ar );


	// 우편함
	XPostInfo* AddPostInfo(XPostInfo* pPostInfo);
	XPostInfo* GetPostInfo( ID snPost );
	void DestroyPostInfo( ID snPost );
// 	XList<XPostInfo*>* GetPostInfo(){ return &m_listPost; }
// 	XList<XPostInfo*>& GetlistPostInfo(){ 
// 		return m_listPost; 
// 	}
	GET_ACCESSOR_CONST( const XList4<XPostInfo*>&, listPost );
	/// 인벤토리의 아이템 개수를 얻는다.	
	int GetNumItems(void) const {
		return m_listItem.size();
	}
	//우편함 개수를 얻는다.
	int GetNumPostInfo(void) {
		return m_listPost.size();
	}

#ifndef _CLIENT
private:
#endif // not CLIENT
#if defined(_XSINGLE) || !defined(_CLIENT)
	XLegion* CreateLegionByRandom(int numSquad, int byLevel = 0);
#endif // defined(_XSINGLE) || !defined(_CLIENT)
public:

	// 인벤토리

	void GetInvenItem(XList4<XBaseItem*> &listItem) {
		listItem = m_listItem;
	}
	bool IsInvenOver() { return m_listItem.size() > (unsigned int)m_maxItems; }		// 인벤 넘침 체크

	void GetListShopSell(std::vector<ID> &listItem) { 
		listItem = m_listShopSell; 
	}
	void RemoveListShopSell(ID idProp) { 
		m_listShopSell.erase(std::find(m_listShopSell.begin(), m_listShopSell.end(), idProp)); 
	}
	bool GetIdxShopItem( int idxSlot, ID _idItem );

	void ChangeShopItemList();
	BOOL IsSaleItemidProp(ID idItemProp);
//	ID ChangeScalpToBook(XGAME::xtClan clan, XArrayLinearN<XBaseItem*, 256> *pOutAry = nullptr);
	int GetNumItems(LPCTSTR szIdentifier);
	int GetNumItems(ID idProp);
	int DestroyItem(LPCTSTR szIdentifier, int num);
	int DestroyItem(ID idProp, int num);
	int DestroyItemBySN(ID snProp, const int num = 1);
	//	LPCTSTR GetClanScalpIdentifier( XGAME::xtClan clan ) const;
// 	int GetNumScalp(XGAME::xtClan clan, int grade = XGAME::xGD_NONE);
// 	int GetNumClanBook(XGAME::xtClan clan, int grade = XGAME::xGD_NONE);
// 	XINT64 GetExpClanBooks(XGAME::xtClan clan, int grade = XGAME::xGD_NONE);
	int CreateItemToInven( const XPropItem::xPROP *pProp, int num, XArrayLinearN<XBaseItem*, 256> *pAryOut = nullptr);
	int CreateItemToInven( ID idProp, int num, XArrayLinearN<XBaseItem*, 256> *pAryOut = nullptr );
	int CreateItemToInven( const _tstring& idsItem, int num, XArrayLinearN<XBaseItem*, 256> *pAryOut = nullptr );
	XBaseItem* CreateItemToInvenForNoStack( const XPropItem::xPROP *pProp);
	XBaseItem* CreatePieceItemByidHero( ID idHero, int num );
	XBaseItem* CreatePieceItem( ID idPropItem, int num );
	XBaseItem* GetItem(ID snItem);
	inline XBaseItem* GetpItemBySN( ID snItem ) {
		return GetItem( snItem );
	}
	const XBaseItem* GetpcItemBySN( ID snItem ) const;
	XBaseItem* GetItem( LPCTSTR idsItem );
	XBaseItem* GetItem( _tstring& idsItem ) {
		return GetItem( idsItem.c_str() );
	}
	bool IsExistItem(ID snItem) {
		return GetItem(snItem) != nullptr;
	}
	XBaseItem* GetItemByEquip(XGAME::xtParts parts, bool bExcludeEquiped = false);
// 	bool IsEnoughResourceForAbil(xAbil *pAbil, XPropTech::xNodeAbil* pProp);
//	bool IsEnoughGoldForAbil(xAbil *pAbil, XPropTech::xNodeAbil* pProp);
// 	bool IsEnoughSubResourceForAbil(xAbil *pAbil, XPropTech::xNodeAbil* pProp, int idx);
	bool IsCompleteResearch() const {
		return m_Researching.IsComplete();
	}
	// 퀘스트
	void CreateQuestMng(void);
	void SetQuestDelegate(XDelegateQuestMng *pDelegate);
	//옵션 조정. 일단 사용이 확실시되는 옵션만 구현해둠
// 	void SetbSound(BOOL bOption) {
// 		m_xOption.bSound = bOption;
// 		m_xOption.Save();
// 	}
// 	BOOL GetbSound(void) {
// 		return m_xOption.bSound;
// 	}
// 	void SetLanguage(int nLang) {
// 		m_xOption.nLang = nLang;
// 		m_xOption.Save();
// 	}
// 	int GetLanguage(void) {
// 		return m_xOption.nLang;
// 	}
#if defined(_DB_SERVER) || defined(_LOGIN_SERVER)
	friend class CUserDB;
	friend class XDatabase;

#elif defined(_CLIENT)
	friend class XSockGameSvr;
	friend class XSockLoginSvr;
#endif
	int GetCostCashSkill( XGAME::xtCashSkill typeCashSkill );
	int GetCashFromRes( XGAME::xtResource typeRes, int num );
	int GetCashFromAP( int ap );
  int GetCashFromGold( int numGold );
	bool IsEnoughCash( int cost ) {
		XBREAK( cost < 0 );
		return GetCashtem() >= (DWORD)cost;
	}
	bool IsEnoughCash(DWORD cost) {
		return GetCashtem() >= cost;
	}
	bool IsNotEnoughCash(DWORD cost) {
		return !IsEnoughCash(cost);
	}
	bool IsEnoughCash(XGAME::xtCashSkill typeCost) {
		return GetCashtem() >= (DWORD)GetCostCashSkill(typeCost);
	}
	bool IsNotEnoughCash(XGAME::xtCashSkill typeCost) {
		return !IsEnoughCash(typeCost);
	}
	bool IsEnoughGold( int cost ) {
		XBREAK( cost < 0 );
		return GetGold() >= (DWORD)cost;
	}
	bool IsNotEnoughGold( int cost ) {
		return !IsEnoughGold( cost );
	}
	int GetGoldNormalSummon();
	void Process( float dt, XDelegateBuff *pDelegate = nullptr );
#ifdef _CLIENT
	void ProcessClientPerSec(float dt);
#endif // _CLIENT
	XGAME::xBuff* AddBuff(const XGAME::xBuff& buff);
	void DelBuff(ID idBuff);
	// 버프를 몇개나 가지고 있나.
	int GetNumBuff() {
		return m_listBuff.size();
	}
	GET_ACCESSOR(XList4<XGAME::xBuff>&, listBuff);
	XGAME::xBuff* GetBuff(ID idBuff);
	XGAME::xBuff* GetBuff(LPCTSTR szIdentifier);
	GET_ACCESSOR(XList4<XGAME::xBattleLog>&, listBattleLogAttack);
	GET_ACCESSOR(XList4<XGAME::xBattleLog>&, listBattleLogDefense);
	XGAME::xBattleLog* AddBattleLog(bool bAttack, XGAME::xBattleLog& log);
	int SerializeAttackLog(XArchive& ar);
	int DeSerializeAttackLog(XArchive& ar);
	int SerializeDefenseLog(XArchive& ar);
	int DeSerializeDefenseLog(XArchive& ar);
	void UpdateNewItemForHeros(ID snItem, XBaseItem *pReallocItem);
	int GetCashRemainResearch();
	int GetCashResearch(int sec);
	int GetsecRemainResearch() const {
		return m_Researching.GetsecRemain();
	}
//	int GetCostHeroLevelup(XHero *pHero, XINT64 expAdd);
	bool IsEquip(ID snItem);
	XHero* GetHeroByEquip(ID snItem);
	//	std::pair<int,int> GetCostHeroLevelUpByRes( XHero *pHero );
	ID AddTrainSlot(const xTrainSlot& slot);
	bool IsTrainingHero(ID snHero, XGAME::xtTrain type);
	bool IsTrainingLevelupHero(ID snHero) {
		return IsTrainingHero(snHero, XGAME::xTR_LEVEL_UP);
	}
	bool IsTrainingSquadupHero(ID snHero) {
		return IsTrainingHero(snHero, XGAME::xTR_SQUAD_UP);
	}
	bool IsTrainingAnySkillupHero(ID snHero) {
		return IsTrainingHero(snHero, XGAME::xTR_SKILL_ACTIVE_UP) ||
			IsTrainingHero(snHero, XGAME::xTR_SKILL_PASSIVE_UP);
	}
	bool IsTrainingSkillActiveupHero(ID snHero) {
		return IsTrainingHero(snHero, XGAME::xTR_SKILL_ACTIVE_UP);
	}
	bool IsTrainingSkillPassiveupHero(ID snHero) {
		return IsTrainingHero(snHero, XGAME::xTR_SKILL_PASSIVE_UP);
	}
	bool IsTrainingSkillupHero(ID snHero, XGAME::xtIdxSkilltype type) {
		if (type == XGAME::xPASSIVE)
			return IsTrainingHero(snHero, XGAME::xTR_SKILL_PASSIVE_UP);
		return IsTrainingHero(snHero, XGAME::xTR_SKILL_ACTIVE_UP);
	}
	bool IsTrainingSkillupHero(ID snHero, XGAME::xtTrain type) {
		return IsTrainingHero(snHero, type);
	}
	bool IsTrainingHeroByAny(ID snHero);
	bool IsTrainingAnyHero();

	xTrainSlot* GetTrainingHero(ID snHero, XGAME::xtTrain type);
	xTrainSlot* GetTrainingLevelupHero(ID snHero) {
		return GetTrainingHero(snHero, XGAME::xTR_LEVEL_UP);
	}
	xTrainSlot* GetTrainingSquadupHero(ID snHero) {
		return GetTrainingHero(snHero, XGAME::xTR_SQUAD_UP);
	}
	xTrainSlot* GetTrainingSkillActiveupHero(ID snHero) {
		return GetTrainingHero(snHero, XGAME::xTR_SKILL_ACTIVE_UP);
	}
	xTrainSlot* GetTrainingSkillPassiveupHero(ID snHero) {
		return GetTrainingHero(snHero, XGAME::xTR_SKILL_PASSIVE_UP);
	}
	xTrainSlot* GetTrainingSkillupHero(ID snHero, XGAME::xtTrain type) {
		return GetTrainingHero(snHero, type);
	}
	bool IsTrainingComplete(ID snSlot);
	void IfCompleteThenDoCompleteAndInvokeHandler(void(*func)());
	// 현재훈련중인 슬롯의 개수가 아니고 최대가능한 슬롯의 개수임에 주의
	int GetMaxTrainSlot() {
		return XGAME::MAX_TRAIN_SLOT;
	}
	int GetNumActiveSlot() {
		return m_listTrainSlot.size();
	}
	/// 해당슬롯이 현재 훈련중인가.
	bool IsTrainingSlot(ID snSlot) {
		if (XBREAK(snSlot == 0))
			return nullptr;
		auto pProp = m_listTrainSlot.FindByIDNonPtr(snSlot);
		if (pProp == nullptr)
			return false;
		if (pProp->type)
		{
			XBREAK(pProp->timerStart.IsOff());
			XBREAK(pProp->secTotal == 0);
			XBREAK(pProp->snHero == 0);
			return true;
		}
		return false;
	}
	xTrainSlot* GetpTrainingSlot(ID snSlot) {
		if (XBREAK(snSlot == 0))
			return nullptr;
		auto pProp = m_listTrainSlot.FindByIDNonPtr(snSlot);
		if (pProp) {
			XBREAK(!pProp->type);
			XBREAK(pProp->timerStart.IsOff());
			XBREAK(pProp->secTotal == 0);
			XBREAK(pProp->snHero == 0);
		}
		return pProp;
	}
	int DoCompleteTraining(ID snSlot, XHero *pHero, int expAdd);
	int AddAP(int add) {
		m_AP += add;
		if (m_AP > GetmaxAP())
			m_AP = GetmaxAP();
		if (m_AP < 0)
			m_AP = 0;
		return m_AP;
	}
	bool IsAPTimeOver() const;
	// 몇분이 지났는가.(왜 이딴 함수를 만들었지?)
	int GetMinTimeOver() const {
		xSec secPass = m_timerAP.GetsecPass();
		return secPass / 60;
	}
	// ap타이머가 지나간 시간을 얻음.
	int GetsecPassAPTime() const {
		return m_timerAP.GetsecPass();
	}
	// 
	void ResetAPTimer( int secOver = 0 ) {
		m_timerAP.ResetTimer();
		if( secOver )
			m_timerAP.AddSec( secOver );
	}
	bool IsResearching() {
		return m_Researching.IsResearching();
	}
	void DelTrainSlotByHero(ID snHero);
	int GetTrainCompleteSlots(XArrayLinearN<ID, 64> *pOutAry);
	bitfield& GetFlagTutorial() {
		return m_bitFlagTutorial;
	}
	DWORD GetdwFlagTutorial() {
		return *((DWORD*)(&m_bitFlagTutorial));
	}
	bool IsLockUnit(XGAME::xtUnit unit) {
		return !IsUnlockUnit( unit );
	}
	bool IsUnlockUnit(XGAME::xtUnit unit) {
		if( XBREAK( XGAME::IsInvalidUnit( unit ) ) )
			return false;
		return m_aryUnlockedUnit[ unit ] != 0;
	}
	void SetUnlockUnit(XGAME::xtUnit unit);
	void GetUnlockUnitsToAry( XArrayLinearN<XGAME::xtUnit, XGAME::xUNIT_MAX> *pOutAry );
	void GetLockUnitsToAry( XVector<XGAME::xtUnit> *pOutAry );
	// 현재 잠긴 유닛이 몇개인지 센다.
	int GetNumLockUnit() {
		int num = 0;
		for (int i = 1; i < XGAME::xUNIT_MAX; ++i) {
			if (m_aryUnlockedUnit[i] == 0 )
				++num;
		}
		return num;
	}
	/// 현재 잠긴 유닛들을 풀려면 몇레벨이 되어야 하는지 알려준다.
//	int GetLevelUnlockUnit();
	int GetCostRemainFullAP();
	int GetCashRemainTrain(xTrainSlot *pSlot);
	int GetCashRemainTrain(ID snSlot) {
		auto pSlot = GetpTrainingSlot(snSlot);
		if (pSlot == nullptr)
			return 0;
		return GetCashRemainTrain(pSlot);
	}
	static int sGetNumUnitByLevel(int lvAcc);
//	bool IsUnlockableUnitByLevel()
	bool IsUnlockableAnyUnit();
	// unit을 언락할수 있는 상태인가.
	bool IsUnlockableUnit( XGAME::xtUnit unit );
	int GetLevelUnlockableUnit( XGAME::xtUnit unit );
	bool IsAbleEquipAnyHero();
	bool IsHaveBetterEquipItem( const XBaseItem* pItemEquip);
	bool IsHaveBetterThanParts(XHero *pHero);
	bool IsHaveBetterThanPartsEnteredHero();
#if defined(_GAME_SERVER) && defined(_DEV)
	int CreateDummyAccount( int lvExtern = 0 );
	int CreateDummyAccountLegion( int level );
// 	bool RecursiveAbilPointRandom( XGAME::xtUnit unit, XPropTech::xNodeAbil *pRoot );
// 	void GenerateAbilityForDummy( int lvAcc, XArrayLinearN<char, XGAME::xUNIT_MAX>& aryTechPoint );
	void GenerateAbilityForDummy( XHero *pHero, int lvAcc );
	void UnlockUnitForDummy( /*XArrayLinearN<char, XGAME::xUNIT_MAX>& aryTechPoint*/ );
#endif // _GAME_SERVER && _DEV
	int GetMilitaryPowerCurrLegion();
	int GetMilitaryPower();
	static int sGetMaxTechPoint( int lvAcc );
	bool IsMaxLevel() {
		return GetLevel() >= XGAME::GetLevelMaxAccount();
	}
	void UpdatePower();
	XGAME::xtError IsOpenableCloud( XPropCloud::xCloud *pCloud );
	XGAME::xtError IsOpenableCloudWithHaveGold( XPropCloud::xCloud *pProp, int goldHave );
	bool IsLockArea( XPropCloud::xCloud *pProp, XGAME::xtError *pOut = nullptr );
	bool IsLockArea( ID idArea, XGAME::xtError *pOut = nullptr );
	bool IsHaveHero( LPCTSTR szIdentifier );
	bool IsHaveHero( const _tstring& idsHero ) {
		return IsHaveHero( idsHero.c_str() );
	}
	XHero* GetpHeroByIdentifier( LPCTSTR szIdentifier );
	int SerializeHeroUpdate( XArchive& ar, XHero *pHero );
	int DeserializeHeroUpdate( XArchive& ar );
	void AddCompleteSeq( const std::string& idsSeq ) {
		if( XBREAK( IsCompletedSeq( idsSeq ) ) )
			return;
		m_mapCompletedSeq[ idsSeq ] = true;
		m_idsLastSeq = idsSeq;
	}
	bool IsCompletedSeq( const std::string& idsSeq );
	bool IsCompletedSeq( const char *idsSeq ) {
		return IsCompletedSeq( std::string(idsSeq) );
	}
	void _ClearCompletedSeq( const std::string& idsSeq );
	void _ClearAllCompletedSeq();
	void _SetidsLastSeq( const char *idsSeq) {
		m_idsLastSeq = idsSeq;
	}
	int GetNumCompletedSeq() {
		return m_mapCompletedSeq.size();
	}
	int SerializeSeq( XArchive& ar );
	int DeserializeSeq( XArchive& ar, int verEtc );
	GET_ACCESSOR( const std::string&, idsLastSeq );
	// xuzhu end
	ID SetBattleSession( ID snSession, const XSPLegion& spLegion, ID idAccEnemy, ID idSpot, DWORD param = 0 );
	void ClearBattleSession() {
		m_BattleSession.Clear();
	}
	ID GetsnSession() {
		return m_BattleSession.snSession;
	}
	// 전투시간이 오버됬는지 검사한다.
	bool IsTimeOverBattle( xSec secMax ) {
		return m_BattleSession.timerStart.IsOver( secMax );
	}
	// 지나간 전투시간을 얻는다.
	xSec GetsecPassBattle() {
		return m_BattleSession.timerStart.GetsecPass();
	}
	const xBattleSession& GetBattleSession() {
		return m_BattleSession;
	}
	GET_SET_ACCESSOR_CONST( int, ptGuild );
	inline int GetGuildCoin() const {
		return m_ptGuild;
	}
	int AddGuildPoint( int add );
	inline int AddGuildCoin( int add ) {
		return AddGuildPoint( add );
	}

	//영혼석
	void GetsnlistSoulStone( XList4<ID> &listSoulStone );
	int GetListSoulStoneExcludeHaveHero( XList4<XBaseItem*> *pOutList );
	int GetNumSoulStone( const _tstring& strId );
	int GetNumSoulStoneWithidPropHero( ID idPropHero );
	int GetNumSoulStone( XHero *pHero );
	ID GetsnSoulStone( const _tstring& strId );
	XGAME::xtError DoPromotionHero( ID snHero );
#ifndef _CLIENT
	XGAME::xtError DoSummonHeroByPiece( ID idPropHero, XHero **ppOut = nullptr );
#endif // not _CLIENT
	//  bool IsPromotionHero(_tstring strIdHero);
	XGAME::xtError IsPromotionHero( XHero *pHero );
	XGAME::xtError IsAbleSummonHeroBySoulStone( const _tstring& strIdHero );
	XBaseItem* GetSoulStoneByHero( LPCTSTR idsHero );
	XBaseItem* GetSoulStoneByHero( const _tstring& strHero ) {
		return GetSoulStoneByHero( strHero.c_str() );
	}
	int GetNeedSoulPromotion( XHero *pHero );
#if defined(_XSINGLE) || !defined(_CLIENT)
	XHero* CreateAddHero( ID idHero, XGAME::xtUnit unitExtern = XGAME::xUNIT_NONE );
	XHero* CreateAddHero( const _tstring& idsHero, XGAME::xtUnit unitExtern = XGAME::xUNIT_NONE );
#endif // defined(_XSINGLE) || !defined(_CLIENT)
	ID GetidItemPieceByidHero( ID idHero );
	inline bool IsLockBarrack() {
		return GetbitUnlockMenu().IsNotBit( XGAME::xBM_BARRACK );
	}
	inline bool IsLockLaboratory() {
		return GetbitUnlockMenu().IsNotBit( XGAME::xBM_LABORATORY );
	}
	inline bool IsLockCathedral() {
		return GetbitUnlockMenu().IsNotBit( XGAME::xBM_CATHEDRAL );
	}
	inline bool IsLockTavern() {
		return GetbitUnlockMenu().IsNotBit( XGAME::xBM_TAVERN );
	}
	inline bool IsLockAcademy() {
		return GetbitUnlockMenu().IsNotBit( XGAME::xBM_ACADEMY );
	}
	inline bool IsLockHangout() {
		return GetbitUnlockMenu().IsNotBit( XGAME::XBM_UNIT_HANGOUT );
	}
	inline bool IsLockEmbassy() {
		return GetbitUnlockMenu().IsNotBit( XGAME::xBM_EMBASSY );
	}
	inline bool IsLockMarket() {
		return GetbitUnlockMenu().IsNotBit( XGAME::xBM_MARKET );
	}
	inline bool IsLockMenu( XGAME::xtMenus bitMenu ) {
		return GetbitUnlockMenu().IsNotBit( bitMenu );
	}
	inline bool IsUnlockMenu( XGAME::xtMenus bitMenu ) {
		return !IsLockMenu( bitMenu );
	}
// 	bool IsLockTraingCenter() {
// 		return GetbitUnlockMenu().IsNotBit( XGAME::xBM_TRAING_CENTER );
// 	}
	void UpdateSpots( XDelegateSpot *pDelegate );
	void UpdateCloud();
#ifdef WIN32
	bool IsCompletedQuest( const std::string& idsQuest );
#endif // WIN32
	bool IsCompletedQuest( const _tstring& idsQuest );
	bool IsFinishedQuest( const _tstring& idsQuest );
	bool IsSameOverThanMe( XSpot *pBaseSpot );
	bool IsGreenOver( XSpot *pBaseSpot );
	bool IsGreenOver( int powerEnemy );
	int GetGradeLevel( int powerEnemy );
	int GetTrainExpByGold( int lvHero, int gold, XGAME::xtTrain typeTrain );
	void GetTrainExpByGoldCurrLv( XHero *pHero, int goldBase, XGAME::xtTrain typeTrain, int *pOutExp, int *pOutSec, int *pOutGold );
	int GetGoldByExp( int lvHero, int exp, XGAME::xtTrain typeTrain );
	static int sGetGoldByMaxExp( int lvHero, XGAME::xtTrain typeTrain );
	int GetsecTrainHero( int lvHero, int exp, XGAME::xtTrain typeTrain );
	bool IsAbleGetStar() const;
	int GetNumSpotStar( ID idSpot );
	int AddSpotStar( ID idSpot );
	int SetNumStarBySpot( ID idSpot, XGAME::xtSpot type, int numStar );
	int SetNumStarBySpot( XSpot *pBaseSpot, int numStar );
	int SerializeNumStarSpot( XArchive& ar );
	int DeSerializeNumStarSpot( XArchive& ar, int verEtc );
	int GetGoldRemainResearch();
	int GetGoldResearch( int sec );
	int GetGoldRemainTrain( xTrainSlot *pSlot );
	int GetCostOpenFog( XSPLegion spLegion );
	int GetAPPerBattle();
	bool IsAbleResearch();
	bool IsAbleResearchUnit( XHero *pHero );
	bool IsAbleSummonHero();
	bool IsAblePromotionHero();
	bool IsNoCheckUnlockUnit();
	bool IsNoCheckUnlockUnitEach( XGAME::xtUnit unit );
	bool IsHaveHeroWithAtkType( XGAME::xtAttack typeAtk );
	bool IsNoCheckUnlockUnitWithHero( XHero *pHero );
	void SetCheckUnlockUnit( XGAME::xtUnit unit );
	bool IsAblePvP();
	bool IsDummyUser() {
		return true;	// 지금은 더미유저를 판단할수 없어 모두 더미로 간주.
	}
	bool AddEncounter( const XGAME::xEncounter& enc );
	void ClearEncounter() {
		m_aryEncounter.Clear();
	}
	void DoStackEncounter( xSec secOffline );
	int GetNumHeroes();
	int GetNumExtraHeroes();
	void DispatchQuestEventCurrState( XGAME::xtQuestCond typeCond );
	void DispatchQuestEventCurrState( XQuestObj *pQuestObj, XGAME::xtQuestCond typeCond );
	bool DispatchQuestEventCurrState( XQuestObj *pQuestObj, XQuestCon *pQuestCon, XGAME::xtQuestCond typeCond );
	/// 로그인후 아직 클라에게 계정보내주기 전인가.
	bool IsLoginBefore() const {
		return m_Level.GetpDelegate() == nullptr;
	}
	void StartTimerByTrader();
	void OffTimerByTrader() {
		m_timerTrader.Off();
	}
	void ResetTimerByArmory() {
		m_timerShop.ClearTimer();
		m_timerShop.DoStart();
	}
	inline bool IsOverTraderRecall() const {
		return m_timerTrader.IsOn() && m_timerTrader.IsOver();
	}
	bool IsReadyTrader() const;
	bool IsOverArmoryItemReset() const;
	int GetCashForTraderRecall() const;
	int GetsecRemainByTraderRecall() const;
	bool IsCallableTraderByGem();
	int GetCashUnlockTrainingSlot();
	int GetLvHeroAfterAddExp( XHero *pHero, XGAME::xtTrain type, int expAdd, bool bAccLvLimit, int *pOutExp );
	bool ReceivePostItemsAll( ID snPost );
	void ReceivePostItemsAll( XPostInfo* pPostInfo );
	XGAME::xtError IsAbleKill( XSpot* pBaseSpot );
	XGAME::xtError IsAbleKill( ID idSpot );
	void SerializeCheat( XArchive* pOutAr );
	void DeSerializeCheat( XArchive& ar );
	int ProcessCheatCmd( const _tstring& strCmd );
	bool IsEnoughResource( const XGAME::xRES_NUM& res );
	bool IsEnoughResourceWithAry( const XVector<XGAME::xRES_NUM>& aryRes );
	void SerializeHeros( XArchive& ar ) const;
	bool DeSerializeHeros( XArchive& ar );
	const XBaseItem* GetpEquipItemWithHero( XHero* pHero, XGAME::xtParts parts ) const;
//////////////////////////////////////////////////////////////////////////
private:
	int GetPowerMaxInHeroes();
	void OnHeroLevelup( XGAME::xtTrain type, XHero *pHero );
	XSPAcc GetThis() {
		// 객체 정적생성(스택)금지
		return std::static_pointer_cast<XAccount>( XDBAccount::GetThis() );
	}
	void DestroyPostInfoAll();
}; // class XAccount


#ifdef _CLIENT
#define ACCOUNT		XAccount::sGetPlayer()
#endif // _CLIENT
