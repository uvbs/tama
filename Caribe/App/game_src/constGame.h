#pragma once
#include "../Resource/defineAct.h"
#include "../Resource/defineGame.h"
//#include "XPropItem.h"

#ifdef _CLIENT
#include "XFramework/XESceneMng.h"
//#include "client/XAppMain.h"	// 잘안바뀌는 파일이라 넣었음.
#endif

class XSquadron;
class XBaseUnit;
class XSquadObj;
class XLegion;
class XLegionObj;
class XHero;
class XBaseItem;
class XAccount;
class XSpot;
class XPostItem;
class XPostInfo;
namespace xnUnit {
	class XMsgBase;
}
typedef std::shared_ptr<xnUnit::XMsgBase> XSPMsgBase;

typedef std::shared_ptr<XLegion> XSPLegion;
typedef std::weak_ptr<XLegionObj> XSPLegionObjW;
typedef std::shared_ptr<const XLegion> XSPLegionConst;

typedef std::shared_ptr<XSquadron> XSPSquadron;
typedef std::shared_ptr<const XSquadron> XSPSquadronConst;
typedef std::weak_ptr<XSquadron> XSPSquadronW;
typedef std::weak_ptr<const XSquadron> XSPSquadronConswtW;

typedef std::shared_ptr<XLegionObj> XSPLegionObj;
typedef std::shared_ptr<const XLegionObj> XSPLegionObjConst;

typedef std::shared_ptr<XBaseUnit> XSPUnit;
typedef std::shared_ptr<const XBaseUnit> XSPUnitConst;
typedef std::weak_ptr<XBaseUnit> XSPUnitW;

typedef std::shared_ptr<XSquadObj> XSPSquadObj;
typedef std::shared_ptr<const XSquadObj> XSPSquadObjConst;
typedef std::weak_ptr<XSquadObj> XSPSquadObjW;

typedef std::shared_ptr<XHero> XSPHero;
typedef std::shared_ptr<const XHero> XSPHeroConst;

typedef std::shared_ptr<XBaseItem> XSPBaseItem;

typedef std::shared_ptr<XSpot> XSPSpot;
typedef std::shared_ptr<const XSpot> XSPSpotConst;

typedef std::shared_ptr<XAccount> XSPAcc;
typedef std::shared_ptr<const XAccount> XSPAccConst;
typedef std::weak_ptr<XAccount> XSPAccW;
typedef std::weak_ptr<const XAccount> XSPAccConstW;

typedef std::shared_ptr<XPostItem> XSPPostItem;
typedef std::shared_ptr<const XPostItem> XSPPostItemConst;
typedef std::shared_ptr<XPostInfo> XSPPostInfo;
typedef std::shared_ptr<const XPostInfo> XSPPostInfoConst;

//#define XSPLegion LegionPtr
// #define XSPSquad SquadPtr
// #define XSPUnit UnitPtr
//#define XSPHero HeroPtr
// #define XSPSpot SpotPtr


namespace xCampaign {
  class XStageObj;
  class XCampObj;
  class XStageObjHero;
	class XStageObjHero2;
  class XCampObjHero;
	class XCampObjHero2;
  typedef std::shared_ptr<XStageObj> StageObjPtr;
  typedef std::shared_ptr<XCampObj> CampObjPtr;
  typedef std::shared_ptr<XStageObjHero> StageObjHeroPtr;
  typedef std::shared_ptr<XCampObjHero> CampObjHeroPtr;
};

// typedef으로 하면 혹시나 내부에 레퍼런스카운트를 따로 쓸까봐 define으로 함.
#define XSPStageObj			xCampaign::StageObjPtr
#define XSPCampObj			xCampaign::CampObjPtr				
#define XSPStageObjHero			xCampaign::StageObjHeroPtr
#define XSPCampObjHero			xCampaign::CampObjHeroPtr				
typedef std::shared_ptr<xCampaign::XCampObjHero2> XSPCampObjHero2;
typedef std::shared_ptr<xCampaign::XStageObjHero2> XSPStageObjHero2;

namespace xHelp {
	class XOrder;
	class XPropSeq;
	class XPropOrder;
	typedef std::shared_ptr<XOrder> OrderPtr;
	typedef std::shared_ptr<XPropSeq> PropSeqPtr;
	typedef std::shared_ptr<XPropOrder> PropOrderPtr;
	enum { DEFAULT = 0x7fffffff };
};

typedef std::vector<int> xResourceAry;

#define XVALID_UNIT(U)		(U > XGAME::xUNIT_NONE && U < XGAME::xUNIT_MAX)


#define BUTT_MID				_T("common_butt_mid.png")
#define BUTT_SMALL				_T("common_butt_small.png")
#define BUTT_X			_T("common_butt_x.png")

#define SPR_MY_CASTLE		_T("spot_castle.spr"), 1
#define SPR_CASTLE		_T("spot_castle.spr"), 2
#define SPR_JEWEL		_T("spot_jewel.spr"), 1
#define SPR_SULFUR		_T("spot_sulfur.spr"), 1
#define SPR_MANDRAKE	_T("spot_mandrake.spr"), 1
#define SPR_NPC			_T( "spot_npc.spr" ), 1
#define SPR_NPC3			_T( "spot_npc3.spr" ), 1		// 3명버전
#define SPR_NPC4			_T( "spot_npc4.spr" ), 1		// 4명버전
#define SPR_NPC_CROW	_T( "spot_npc_crow.spr" ), 1
#define SPR_NPC_IRONLORD	_T( "spot_npc_ironlord.spr" ), 1
#define SPR_NPC_FREEDOM	_T( "spot_npc_freedom.spr" ), 1
#define SPR_NPC_ANCIENT	_T( "spot_npc_ancient.spr" ), 1
#define SPR_NPC_FLAME	_T( "spot_npc_flame.spr" ), 1
#define SPR_DAILY		_T( "spot_daily.spr" ), 1
#define SPR_SPECIAL		_T( "spot_special.spr" ), 1
#define SPR_CAMPAIGN	_T( "spot_campaign.spr" ), 1
#define SPR_VISIT		_T( "spot_visit.spr" ), 1
#define SPR_CASH		_T("spot_cash.spr"), 1
#define SPR_COMMON		_T( "spot_npc.spr" ), 1

#define FONT_RESNUM		_T("res_num.ttf")
//#define FONT_HAND			_T("hand.ttf")


#define xMETER_TO_PIXEL(M)		((float)XGAME::ONE_METER * ((float)M))
#define xPIXEL_TO_METER(P)		(((float)P) / (float)XGAME::ONE_METER)

#define HEXA_CENTER_OFFSET		XE::VEC2(48, 21)
//#define ICON_SIZE				XE::VEC2(86/2.f,84/2.f)
#define ICON_SIZE				XE::VEC2(37,37)

#define XCOLOR_XRED			XCOLOR_RGBA( 188, 63, 57, 255 );		// 빨간색 글자 쓸거 있으면 이 색으로.

#ifdef _CLIENT
//	#define COST_LOW_GATHA				(XGAME::GetCostLowGatcha(ACCOUNT->GetLevel()))
//	#define COST_HIGH_GOLD_GATHA		(XGAME::GetCostHighGoldGatcha(ACCOUNT->GetLevel()))
//	#define COST_HIGH_CASH_GATHA		(XGC->GetcashHighSummon())
#else
//	#define COST_LOW_GATHA				(XGAME::GetCostLowGatcha(m_pAccount->GetLevel()))
//	#define COST_HIGH_GOLD_GATHA		(XGAME::GetCostHighGoldGatcha(m_pAccount->GetLevel()))
//	#define COST_HIGH_CASH_GATHA		(XGAME::GetCostHighCashGatcha(m_pAccount->GetLevel()))
#endif

// 각 자원들의 스트링
#define STR_WOOD		XTEXT(2007)
#define STR_IRON		XTEXT(2008)
#define STR_JEWEL		XTEXT(2009)
#define STR_SULFUR		XTEXT(2010)
#define STR_MANDRAKE	XTEXT(2011)

#define SCROLL_GRAY		10010
#define SCROLL_GREEN	10011
#define SCROLL_BLUE		10012
#define SCROLL_PURPLE	10013

#define DUMMY_NAME	_T("TEST_USER")
namespace XGAME {
	extern const XE::VEC2 x_sizeHexa;
	inline float GetRadiusHexa() {
		float r = x_sizeHexa.w / 2.f;
		return r + (r * 0.1f);	// 10%더크게
	}

	const int MAX_ID_LEN = 20;
	const int INCREASE_INVEN_SIZE = 5;
	const int MAX_SALE_GRADE_COUNT	= 3;
	const int MAX_SALE_TOTAL_COUNT	= 15;
	const int MAX_GUILD_MEMBER_COUNT = 30;
	const int TRADE_CALL_INTERVAL = 60 * 60 * 12;
	const int SHOP_CALL_INTERVAL = 60 * 60 * 12;
	const int SUBSCRIBE_PERIOD = 60 * 60 * 24 * 30; //1개월
	const int SUBSCRIBE_INTERVAL = 60 * 60 * 24; //1시간에 1개씩..
	const int MAX_TRAIN_SLOT = 10;		// 최대 훈련소 슬롯개수

	const int ITEM_INIT_COUNT = 30;			// 인벤토리 초기 갯수
	const int ITEM_MAX_COUNT = 1000;		// 인벤토리 한계 갯수. 테스트용

	const int MAX_SEARCH_COUNT = 3;
	//const int ITEM_MAX_COUNT = 1000;		// 인벤토리 한계 갯수

	enum {
//		xNUM_RESOURCE_SPOT=15,
//		xNUM_USER_SPOT=20,
//		xNUM_NPC_SPOT=30,
		SEC_REGEN=60,							// 자원지 생산시간 단위(초)
		SEC_REGEN_CASTLE=200,					// 유저스팟 스폰시간 기본 상수
		MAX_LEGION=3,							// 계정당 군단수
		MAX_SQUAD=15,							// 군단내 최대 부대수
		WIN=1,
		LOSE=0,
		ONE_METER=12,							///< 1미터에 해당하는 좌표
		MAX_UNIT_SMALL=20,
		MAX_UNIT_MIDDLE=4,
		MAX_UNIT_BIG=1,							///< 대형유닛은 인원수 늘어나는거 없이 레벨만 증가하는걸로 한다.
		MAX_UNIT_PER_TYPE = 3,			///< 병과당 유닛수
		DIST_MELEE_ATTACK = ONE_METER * 6,		// 미터
		NUM_SCALP_FOR_BOOK=10,					///< 책교환을 위해 필요한 징표 개수
		MAX_SKILL_LEVEL=11,						// 1~10번 인덱스 사용
		MAX_STAGE_IN_CAMP = 64,					///< 캠페인 스테이지 최대수
		LEVEL_UNLOCK_UNIT = 9,					///< 중형급유닛 언락할수 있는 레벨
		LEVEL_UNLOCK_PALADIN = 3,				///< 기사를 언락시킬수 있는 레벨
		UNLOCK_LV_ACADEMY = 5,
		UNLOCK_LV_UNIT_HANGOUT = 6,
		UNLOCK_LV_USER_CASTLE = 8,					///< 최초의 유저성이 등장하는 시기
// 		LEVEL_MIDDLE_UNIT = 12,					///< 중형유닛 등장 레벨
// 		LEVEL_BIG_UNIT = 25,					///< 대형유닛 등장레벨 
		MAX_HERO_LEVEL = 50,
		MAX_SQUAD_LEVEL = 16,
		MAX_ACC_LEVEL = 50,
		MAX_AREA_LEVEL = 50,
		MAX_NPC_LEVEL = 50,
		LEVEL_JEWEL = 10,		// 보석자원이 나오기 시작하는 레벨
		LEVEL_SULFUR = 15,		// 유황 자원이 나오기 시작하는 레벨
		LEVEL_MANDRAKE = 19,	//
		MAX_ABIL_UNIT = 15,		// 유닛당 특성개수
		
	};
	// 다용도 에러. 0이면 성공 0이 아니면 실패.
	enum xtError {
		xE_OK = 0,
		xE_SUCCESS = 0,
		xE_UNKNOWN,
		xE_TIMEOUT,
		xE_ERROR_CRITICAL,
		xE_NOT_ENOUGH_LEVEL,
		xE_NOT_ENOUGH_GOLD,
		xE_NOT_ENOUGH_CASH,
		xE_NOT_ENOUGH_RESOURCE,
		xE_NOT_ENOUGH_ITEM,				// 필요아이템이 부족
		xE_NOT_ENOUGH_NUM_TRY,			// 도전횟수를 다씀
		xE_NOT_ENOUGH_NUM_CLEAR,		// 클리어횟수를 다 채움.
		xE_NOT_ENOUGH_AP,				// AP부족
		xE_MUST_PREV_STAGE_CLEAR,		// 선행 스테이지를 먼저 깨야함.
		xE_LOCK,						// 잠겨있음.
		xE_MUST_PRECEDE_AREA_OPEN,		// 선행지역을 먼저 오픈해야 함.
		xE_MUST_OPEN_AREA,				// 어떤 지역을 오픈해야 함.,
		xE_MUST_NEED_KEY_ITEM,			// 키 아이템이 필요.
		xE_NOT_FOUND,					// (xxx)를 찾을 수 없음.
		xE_NO_MORE,						// 더이상 xxx할수 없다.
		xE_CAN_NOT,						// xx할수없다.
		xE_MUST_PRECEDE_QUEST,			// 선행퀘스트를 먼저 깨야함.
		xE_ALREADY_RECV_REWARD,			// 이미 보상받음.
		xE_MUST_UNLOCK_UNIT,			// 유닛을 언락해야함.
		xE_MUST_PRECEDE_UNLOCK,			// 선행요소를 언락해야함.
		xE_INVALID_GCM_REGIST_ID,		
		xE_NOT_ENOUGH_AUTHORITY,		// 권한이 없음.
	
	};
	// 훈련 타입
	enum xtTrain {
		xTR_NONE,
		xTR_LEVEL_UP,	// 영웅렙업
		xTR_SQUAD_UP,	// 영웅부대업
		xTR_SKILL_ACTIVE_UP,	// 영웅스킬업
		xTR_SKILL_PASSIVE_UP,	// 영웅스킬업
		xTR_MAX,
		xTR_EMPTY,		// 비어있음(UI에만 사용)
		xTR_LOCK,			// 잠겨있음(UI에만 사용)
	};

	// 요일별 스팟의 드랍타입
// 	enum xtDailySpot {
// 		xDS_NONE,
// 		xDS_SCALP,		// 징표스팟
// 		xDS_SKILL,		// 스킬템
// 		xDS_GENERAL,	// 책사드랍
// 	};

	// 스페셜 스팟의 타입
	enum xtSpecialSpot {
		xSS_NONE,
		xSS_HIGH,		// 상급
		xSS_LOW,		// 하급
	};

	// DelegateConnection()에 넘길 파라메터 종류
	enum xtConnectParam {
		xCP_NONE,
		xCP_LOGIN_UUID,		// UUID로 로그인할 예정
		xCP_LOGIN_IDPW,		// ID/PW로 로그인할 예정
		xCP_LOGIN_FACEBOOK,	// facebook으로 로그인
		xCP_LOGIN_NEW_ACC,	// 새계정 생성 요청
		xCP_LOGIN_NEW_ACC_BY_FACEBOOK,	// 새계정 생성 요청
		xCP_LOGIN_MAX,
	};

	inline bool IsValidConnectParam( xtConnectParam param ) {
		return param > 0 || param < xCP_LOGIN_MAX;
	}
	/// 배열에 사용하는 인덱스
	enum xtSideIndex {
		xSI_PLAYER=0,		///< 플레이어측
		xSI_OTHER=1,		///< 상대측
	};

	/**
	 @brief 어느진영인지 비트단위로 각자 할당받는다.
	 카리브에서는 나아니면 적밖에 없기때문에 2개만 할당함.
	 비트 할당 규칙에 대한 더 자세한 사항은 XECompCamp 클래스 설명을 참조
	 @see XECompCamp
	*/
	enum xtSide {
		xSIDE_NONE = 0,				///< 초기화 값
		xSIDE_PLAYER = 0x01,		///< 플레이어
		xSIDE_OTHER = 0x02,			///< 상대측
		xSIDE_ALL = 0xFFFFFFFF,		///< 모두선택
	};

	/// 피아식별 상수. 이것은 개별오브젝트간에 상대적이다.(적입장에서 아군은 xFR_FRIEND를 써야함)
	enum xtFriend {
		xFR_FRIEND,		///< 아군
		xFR_ENEMY,		///< 적군
		xFR_ALL,		///< 모두
	};

	/// 군단 인덱스
	enum xtLegionIdx {
		xLI_PLAYER = 0,		// 플레이어측
		xLI_OTHER = 1,
	};

	/**
	 @brief 오브젝트의 대분류
	*/
	enum xtObjType {
		xOT_NONE,
		xOT_UNIT,			///< 유닛
		xOT_SFX,			///< sfx
		xOT_FLOOR_OBJ,		///< 바닥에 딱붙어 찍히는류의 오브젝트
		xOT_DAMAGE,			///< 주변에 데미지를 주는 오브젝트.
		xOT_ETC,			///< 기타
		xOT_SKILL_EFFECT,	///< 스킬로 인해 좌표에 생성된 더미객체
		xOT_UI,		///< 가장상위에 찍혀야하는 오브젝트레이어
		xOT_MAX,
	};

	enum xtMelee {
		xMT_NONE,
		xMT_MELEE,		// 붙어서 공격(유닛타입의 근접/스피드는 모두 xMT_MELEE)
		xMT_RANGE,		// 원거리 공격
	};

	/**
	 @brief 스킬레벨업 에러코드
	*/
	enum xtSkillLevelUp {
		xES_NONE = 0,					///< 초기화값
		xES_OK = 1,						///< 레벨업 가능
		xES_FAIL = 0x8000,				///< 그냥 실패
		xES_NO_HAVE_SKILL,			///< 스킬이 없음
		xES_FULL_LEVEL,				///< 이미 만렙
		xES_NOT_ENOUGH_ITEM,		///< 아이템 부족
		xES_INVALID_CALL,			// 잘못된 함수호출
		xES_LIMITED_BY_HERO_LEVEL,	/// 영웅렙제한에 걸림
	};

	enum xtGatha {
		xGA_NONE=0,
		xGA_LOW_GOLD,		///< 일반소환
//		xGA_HIGH_GOLD,		///< 고급소환(금화)
		xGA_HIGH_CASH,		///< 고급소환(캐쉬)
		xGA_MAX,
	};
	enum xtSpentCall
	{	
		xSC_SPENT_NONE = 0,
		xSC_SPENT_ITEM,
		xSC_SPENT_GEM,
		xSC_MAX,
	};
	enum xtPostType
	{	
		xPOSTTYPE_NONE = 0,
		xPOSTTYPE_NORMAL,
		xPOSTTYPE_NOTIFY,
	//	xPOSTTYPE_VECTOR,
//		xPOSTTYPE_TABLE,
	//	xPOSTTYPE_SUBSCRIBE,
	};
	enum xtGuildGrade{
		//xGGL_READY = -1,
		xGGL_NONE = 0,
		xGGL_LEVEL1 = 1,
		xGGL_LEVEL2,
		xGGL_LEVEL3,
		xGGL_LEVEL4,
		xGGL_LEVEL5,
		xGGL_MAX,
	};
	enum xtGuildAcceptFlag{
		xGAF_ACCEPT = 0,
		xGAF_REFUSE,
	};
// 	enum xtGuildUpdate{
// 		xGU_UPDATE_NONE,	
// 		xGU_UPDATE_CREATE,
// 		xGU_UPDATE_MEMBER_INFO,				// 특정 멤버의 정보 갱신
// 		xGU_UPDATE_MEMBER_LIST,		// 멤버 목록 갱신
// 		xGU_UPDATE_JOIN_ACCEPT,
// 		xGU_UPDATE_JOIN_REQ,
// 		xGU_UPDATE_CONTEXT,
// 		xGU_UPDATE_OPTION,
// 		xGU_UPDATE_MEMBER_MASTER,
// 		xGU_UPDATE_DELETE,
// 		xGU_UPDATE_DEL_JOIN_REQ,
// 	};
	enum xtGuildEvent {
		xGEV_NONE,
//		xGEV_GUILD_INFO,		// 길드가입정보와 가입신청목록
		xGEV_CREATE,		// 길드생성
		xGEV_JOIN_ACCEPT,		// 가입승인
		xGEV_JOIN_REJECT,		// 가입거절
		xGEV_CHANGE_MEMBER_INFO,	// 유저의 멤버정보 변경
		xGEV_KICK,					// 쫒겨남
		xGEV_OUT,						// 탈퇴
		xGEV_DESTROY,				// 길드해체
		xGEV_UPDATE,				// 각 항목(xtBitGuildUpdate)별 업데이트
// 		xGEV_UPDATE_MEMBERS,	// 멤버목록 업데이트
//  		xGEV_UPDATE_REQERS,		// 가입신청자 목록 업데이트
//  		xGEV_UPDATE_MEMBERS_AND_REQERS,		// 멤버목록과 가입신청자목록 업데이트
// 		xGEV_UPDATE_GUILD_INFO,		// 길드기본정보 업데이트(길마같은..)
	};
	enum xtBitGuildUpdate {
		xBGU_NONE = 0,
		xBGU_BASIC_INFO = 0x00ff,
		xBGU_MASTER			= 0x0001,
		xBGU_OPTION			= 0x0002,
		xBGU_CONTEXT		= 0x0004,
		xBGU_REQERS			= 0x0100,
		xBGU_MEMBERS		= 0x0200,
	};
	template<typename T>
	inline T xBitOr( T b1, T b2 ) {
		return (T)((DWORD)b1 | (DWORD)b2);
	}
	template<typename T>
	inline T xBitOr( T b1, T b2, T b3 ) {
		return (T)( (DWORD)b1 | (DWORD)b2 | (DWORD)b3);
	}
	template<typename T>
	inline T xBitOr( T b1, T b2, T b3, T b4 ) {
		return (T)( (DWORD)b1 | (DWORD)b2 | (DWORD)b3 | (DWORD)b4 );
	}
	template<typename T>
	inline T xBitOr( T b1, T b2, T b3, T b4, T b5 ) {
		return (T)( (DWORD)b1 | (DWORD)b2 | (DWORD)b3 | (DWORD)b4 | (DWORD)b5 );
	}

	enum xtGuildError{
		xGE_NO_SEND = -1,						// 클라에게 전송하지 말것.
		xGE_SUCCESS = 0,
		xGE_ERROR_UNKNOWN,          // 기타등등 에러
		xGE_ERROR_CRITICAL,          // 있어서는 안되는 상황이 생겼을때.
		XGE_ERROR_GUILD_NOT_FOUND,
		XGE_ERROR_MEMBER_NOT_FOUND,
		XGE_ERROR_GUILD_INFO,
		XGE_ERROR_GUILD_CREATE,
		xGE_ERROR_ACCEPT_MEMBER,			// 멤버 승인 실패.
		xGE_ERROR_JOIN_MEMBER,				// 가입멤버 실패.
		xGE_ERROR_NAME_DUPLICATE,			// 이름 중복.
		xGE_ERROR_ALREADY_HAVE_GUILD,		// 이미 길드 가입 되어 있음..
		xGE_ERROR_COST_NOT_ENOUGH,			// 소모 비용 모자람.
		xGE_ERROR_MAX_MEMBER_OVER,			// 최대인원수 초과.
		xGE_ERROR_NO_AUTHORITY,				// 권한 없음.
		xGE_ERROR_BLOCK_JOIN_REQ,			// 가입 신청 제한
		xGE_ERROR_MASTER_OUT,				// 연방장이 탈퇴하려할때
		xGE_ERROR_STILL_TRYING_RAID,  // 이미 누가 레이드 trying중임.
		xGE_ERROR_NOT_FOUND_RAID,     // 레이드 정보를 찾을 수 없음.
		xGE_ERROR_ALREADY_EXIST_RAID,	// 레이드캠페인을 생성하려고 했으나 이미 생성되어있다.
		xGE_ERROR_IS_NOT_LOCKER,		// 레이드 Leave를 하려고 했을때 락건사람이 자기가 아니었음.
		xGE_ERROR_ALREADY_CLEAR_RAID,	// 이미 레이드가 클리어됨.
		xGE_ERROR_CLOSED_RAID,			// 시간오버로 레이드가 닫혔다.
		xGE_ERROR_ALREADY_MEMBER,		// 이미 가입된 멤버
		xGE_ERROR_ALREADY_JOIN_REQ,	// 이미 가입요청이 되어있음.
		xGE_ERROR_NAME_IS_TOO_LONG,		// 길드이름이 너무 김
		xGE_ERROR_CONTEXT_IS_TOO_LONG,		// 길드설명이 너무 김
		xGE_ERROR_NOT_FOUND_USER,					// 해당 유저를 찾을 수 없음.
		xGE_ERROR_NOT_FOUND_REQER,				// 가입신청자가 아님
		xGE_ERROR_DO_NOT_YOURSELF,				// 자기자신을 어찌하려는 류의 에러
		xGE_ERROR_INVALID_PARAMETER,			// 잘못된 파라메터
		xGE_ERROR_DB,											// DB처리 에러
	};
	enum xtCashType{
		xCT_NONE = 0,
		xCT_NORMAL,
		xCT_SUBSCRIBE,
	};
	enum xPushNotify {
		xPN_SYSTEM = 0,
		xPN_GAME = xPN_SYSTEM + 1000,
		xPN_GAME_TRADE_TIME_OVER,
		xPN_GAME_SHOP_LIST_RESET,
		xPN_GAME_GEM_SUBSCRIBE,
		xPN_GAME_GEM_SUBSCRIBE_END,
		xPN_GAME_TECH_RESEARCH_COMPLETE,
		xPN_EVENT = xPN_GAME + 1000,
	};
	enum xtUserLog{
		xULog_NONE = 0,		
		xULog_Account_Create,
		xULog_Account_Login,
		xULog_Account_Save,
		xULog_Account_Logout,
		xULog_Account_Duplicate_connect,
		xULog_Account_Level_Up,
		xULog_Quest_Reward,
		xULog_Cloud_Open,
		xULog_User_Spot_Recon,
		xULog_User_Spot_Combat,		
		xULog_Use_Trade,
		xULog_Trade_Call_Gem,
		xULog_ShopList_Call_Gem,
		xULog_Trade_Call_Time,
		xULog_ShopList_Call_Time,
		xULog_Skill_Tree_Research,
		xULog_Hero_Level_Up,
		xULog_Hero_Skill_Level_Up,
		xULog_Hero_Legion_Level_Up,
		xULog_Hero_Gatcha_Normal,
		xULog_Hero_Gatcha_Royal,
		xULog_Buy_Items,
		xULog_Sell_Items,
		xULog_Buy_Gem,
		xULog_Buy_Gem_Subscribe,
		xULog_Use_Gem,
		xULog_Hero_Train,

		
		xULog_System = 100,
		xULog_System_Unkown_idpacket,
		xULog_System_Use_Cheat,
		xULog_Unkown_idpacket,
		xULog_MAX = 200,
	};
	enum xtIdxSkilltype {
		xPASSIVE = 0,
		xACTIVE = 1,
	};
	/// 전투 종료 코드
	enum xtExitBattle {
		xEB_NONE = 0,
		xEB_FINISH,     // 정상 전투종료
		xEB_CANCEL,     // 전투 취소
		xEB_RETREAT     // 전투중 도주
	};
	// 성스팟 드랍타입
	enum xtDropCastle {
		xDC_NONE,   // 초기화값
		xDC_NO_DROP,    // 아무것도 드랍하지 않음
		xDC_MEDAL,      // 
		xDC_SCROLL,
	};
	// 군단생성 옵션
	enum xtLegionOption {
		xLO_DEFAULT = 0,
		xLO_NO_CREATE_DEAD = 0x01,		// 사망부대는 생성하지 않는다.
	};
	// 전투종류
	enum xtBattle {
		xBT_NONE = 0,
		xBT_NORMAL,		// 일반 전투
		xBT_GUILD_RAID,	// 길드레이드
		xBT_PRIVATE_RAID,		// 개인레이드
	};
	// 전투 승패결과
	enum xtWin {
		xWT_NONE,		// 전투를 하지 않음.
		xWT_WIN,		
		xWT_LOSE,
		xWT_DRAW,		// 무승부(결판이 나지 않음)
	};
	// 월드맵 동적 오브젝트
	enum xtWorldObj {
		xWO_NONE,
		xWO_BIRD,
	};
	// UI용 인덱스
	enum xtIndexUI {
		xIU_GOLD = xRES_MAX,
		xIU_CASH,
		xIU_GUILD_COIN,
		xIU_MAX,
	};

	// 보상타입
	enum class xtReward {
		xRW_NONE = 0,
		xRW_ITEM,
		xRW_GOLD,
		xRW_CASH,		// 캐쉬
		xRW_RESOURCE,
		xRW_HERO,
		xRW_GUILD_POINT,
		xRW_MAX,
	};

	inline bool IsValidxtReward( xtReward type ) {
		return type > xtReward::xRW_NONE && type < xtReward::xRW_MAX;
	}
	inline bool IsInvalidxtReward( xtReward type ) {
		return !IsValidxtReward( type );
	}

	// 즉시완료 타입
	enum xtQuickComplete {
		xQCT_NONE,		// 즉시완료아님
		xQCT_CASH,		// 캐시로 즉시완료
		xQCT_GOLD,		// 금화로 즉시완료
	};
	// 피해속성
	enum xtDamageAttr {
		xDA_NONE = 0,
		xDA_FIRE,
	};
	// 파라메터의 타입(여러가지 xml스크립트에서 쓰는 파라메터들의 타입)
	enum xtParam {
		xPT_NONE,
		xPT_NUMBER,		// 일반숫자(레벨,개수,횟수,기타등등 숫자형태)
		xPT_AREA,			// 지역아이디
		xPT_SPOT,			// 스팟아이디
		xPT_SPOT_CODE,	// 스팟코드
		xPT_SPOT_TYPE,	// 스팟타입
		xPT_HERO,			// 영웅아이디
		xPT_ITEM,			// 아이템아이디
		xPT_QUEST,		// 퀘스트아이디
		xPT_RESOURCE,	// 리소스 아이디
		xPT_ABIL,			// 특성아이디
		xPT_UNIT,			// 유닛아이디
		xPT_ETC,			// 기타
		xPT_MAX,
	};
	// enum end
// 	inline int GetNeedSoulSummon(XGAME::xtGrade grade) {
// 		switch (grade) {
// // 		case XGAME::xGD_RARE: return 30; break;
// // 		case XGAME::xGD_EPIC: return 40; break;
// 		case XGAME::xGD_NONE:	return 10;	break;
// 		case XGAME::xGD_COMMON: return 10; break;
// 		case XGAME::xGD_VETERAN: return 50; break;
// 		case XGAME::xGD_RARE: return 100; break;
// 		case XGAME::xGD_EPIC: return 200; break;
// 		case XGAME::xGD_LEGENDARY: return 0;	break;
// 		}
// 		return 0;
// 	}

	int GetNeedSoulPromotion(XGAME::xtGrade gradeCurr);
	int GetNumTotalSoulSummon( XGAME::xtGrade grade );
	//영혼석을 이용한 소환에 필요한 영혼석 수
	inline int GetNeedSoulSummon() {
		return GetNeedSoulPromotion( XGAME::xGD_NONE );
	}

	inline LPCTSTR GetStrUnit( XGAME::xtUnit unit ) {
		switch( unit )
		{
		case XGAME::xUNIT_SPEARMAN:	return XTEXT(30000);
		case XGAME::xUNIT_ARCHER:	return XTEXT(30001);
		case XGAME::xUNIT_PALADIN:	return XTEXT(30002);
		case XGAME::xUNIT_MINOTAUR:	return XTEXT(30003);
		case XGAME::xUNIT_CYCLOPS:	return XTEXT(30004);
		case XGAME::xUNIT_LYCAN:	return XTEXT(30005);
		case XGAME::xUNIT_GOLEM:	return XTEXT(30006);
		case XGAME::xUNIT_TREANT:		return XTEXT(30007);
		case XGAME::xUNIT_FALLEN_ANGEL:	return XTEXT(30008);
		}
		return _T("");
	}

	/// 유닛의 사이즈를 알려준다.
	inline XGAME::xtSize GetSizeUnit( XGAME::xtUnit unit ) {
		switch(unit)
		{
		case XGAME::xUNIT_SPEARMAN:	
		case XGAME::xUNIT_ARCHER:	
		case XGAME::xUNIT_PALADIN:	
			return XGAME::xSIZE_SMALL;
		case XGAME::xUNIT_MINOTAUR:	
		case XGAME::xUNIT_CYCLOPS:	
		case XGAME::xUNIT_LYCAN:	
			return XGAME::xSIZE_MIDDLE;
		case XGAME::xUNIT_GOLEM:	
		case XGAME::xUNIT_TREANT:	
		case XGAME::xUNIT_FALLEN_ANGEL:	
			return XGAME::xSIZE_BIG;
		}
		XBREAK(1);
		return XGAME::xSIZE_NONE;
	}

	inline XGAME::xtAttack GetTypeUnit( XGAME::xtUnit unit ) {
		switch( unit )
		{
		case XGAME::xUNIT_SPEARMAN:
		case XGAME::xUNIT_MINOTAUR:
		case XGAME::xUNIT_GOLEM:
			return XGAME::xAT_TANKER;
		case XGAME::xUNIT_ARCHER:
		case XGAME::xUNIT_CYCLOPS:
		case XGAME::xUNIT_TREANT:
			return XGAME::xAT_RANGE;
		case XGAME::xUNIT_FALLEN_ANGEL:
		case XGAME::xUNIT_LYCAN:
		case XGAME::xUNIT_PALADIN:
			return XGAME::xAT_SPEED;
		}
		XBREAK( 1 );
		return XGAME::xAT_NONE;
	}
	_tstring GetResUnitSmall( XGAME::xtUnit unit );
	inline XGAME::xtAttack GetAtkType( XGAME::xtUnit unit ) {
		return GetTypeUnit( unit );
	}

	inline XCOLOR GetGradeColor( XGAME::xtGrade grade ) {
		switch( grade )
		{
// 		case XGAME::xGD_COMMON:		return XCOLOR_WHITE;	break;
// 		case XGAME::xGD_VETERAN:	return XCOLOR_RGBA(66,208,62,255);	break;
// 		case XGAME::xGD_RARE:		return XCOLOR_RGBA(50,100,163,255);	break;
// 		case XGAME::xGD_EPIC:		return XCOLOR_RGBA(216,0,255,255);	break;
// 		case XGAME::xGD_LEGENDARY:	return XCOLOR_RGBA(249,162,1,255);	break;
		case XGAME::xGD_COMMON:		return XCOLOR_WHITE;	break;
		case XGAME::xGD_VETERAN:	return XCOLOR_GREEN;	break;
		case XGAME::xGD_RARE:		return XCOLOR_RGBA(0,114,227,255);	break;
		case XGAME::xGD_EPIC:		return XCOLOR_RGBA(155,55,233,255);	break;
		case XGAME::xGD_LEGENDARY:	return XCOLOR_RGBA(255,129,6,255);	break;
		default:
			XBREAK(1);
		}
		return XCOLOR_WHITE;
	}
	inline LPCTSTR GetStrGrade( XGAME::xtGrade grade ) {
		switch( grade )
		{
		case XGAME::xGD_COMMON:		return XTEXT(2073);
		case XGAME::xGD_VETERAN:	return XTEXT(2074);
		case XGAME::xGD_RARE:		return XTEXT(2075);
		case XGAME::xGD_EPIC:		return XTEXT(2076);
		case XGAME::xGD_LEGENDARY:	return XTEXT(2077);
		default:
			XBREAK( 1 );
		}
		return _T("알수없는");
	}
	const char* GetIdsRes( xtResource res );
	const char* GetIdsRes( xtIndexUI idxUI );
	// 하드코딩용
	inline LPCTSTR GetResourceIcon( XGAME::xtResource res ) {
		switch( res )
		{
		case XGAME::xRES_WOOD:	return _T("world_top_wood.png");	break;
		case XGAME::xRES_IRON:	return _T("world_top_iron.png");	break;
		case XGAME::xRES_JEWEL:	return _T("world_top_jewel.png");	break;
		case XGAME::xRES_SULFUR:	return _T("world_top_sulfur.png"); break;
		case XGAME::xRES_MANDRAKE:	return _T("world_top_mandrake.png");	break;
    case XGAME::xRES_GOLD:	return _T( "world_top_gold.png" );	break;
		case XGAME::xRES_CASH:	return _T( "world_top_cash.png" );	break;
		case XGAME::xRES_GUILD_COIN: return _T( "guild_coin_small.png" ); break;
		default:
			XBREAK(1);
			break;
		}
		return _T("");
	}
	/// 각 자원들의 spr 파일명을 돌려준다.
	LPCTSTR GetResourceSpr( XGAME::xtResource res );
	// 하드코딩용(패스까지 붙여서 리턴한다.
	inline LPCTSTR GetResourceIcon2( XGAME::xtResource res ) {
		switch( res )
		{
		case XGAME::xRES_WOOD:	return PATH_UI( "world_top_wood.png");	break;
		case XGAME::xRES_IRON:	return PATH_UI( "world_top_iron.png");	break;
		case XGAME::xRES_JEWEL:	return PATH_UI( "world_top_jewel.png" );	break;
		case XGAME::xRES_SULFUR:	return PATH_UI( "world_top_sulfur.png" ); break;
		case XGAME::xRES_MANDRAKE:	return PATH_UI( "world_top_mandrake.png" );	break;
		case XGAME::xRES_NONE:	return PATH_UI( "world_top_question.png" );	break;
		case XGAME::xRES_GOLD:	return PATH_UI( "world_top_gold.png" );	break;
		case XGAME::xRES_CASH:	return PATH_UI( "world_top_cash.png" );	break;
		case XGAME::xRES_GUILD_COIN: return PATH_UI("guild_coin_small.png"); break;
		default:
			XBREAK( 1 );
			break;
		}
		return _T( "" );
	}
	inline LPCTSTR GetResourceIconBig( XGAME::xtResource res ) {
		switch( res )
		{
		case XGAME::xRES_WOOD:	return _T("icon_res_wood.png");	break;
		case XGAME::xRES_IRON:	return _T("icon_res_iron.png");	break;
		case XGAME::xRES_JEWEL:	return _T("icon_res_jewel.png");	break;
		case XGAME::xRES_SULFUR:	return _T("icon_res_sulfur.png"); break;
		case XGAME::xRES_MANDRAKE:	return _T("icon_res_mandrake.png");	break;
    case XGAME::xRES_GOLD:	return _T( "icon_res_gold.png" );	break;
		case XGAME::xRES_CASH:	return _T( "icon_res_cash.png" );	break;
		case XGAME::xRES_GUILD_COIN:	return _T( "guild_coin.png" );	break;
		default:
			XBREAK(1);
			break;
		}
		return _T("");
	}
	LPCTSTR GetEnumResource( XGAME::xtResource res );
	//
	inline LPCTSTR GetStrResource( XGAME::xtResource res ) {
		switch( res )
		{
		case XGAME::xRES_WOOD:	return XTEXT(2007);
		case XGAME::xRES_IRON:	return XTEXT(2008);
		case XGAME::xRES_JEWEL:	return XTEXT(2009);
		case XGAME::xRES_SULFUR:	return XTEXT(2010);
		case XGAME::xRES_MANDRAKE:	return XTEXT(2011);
		case XGAME::xRES_GOLD:		return XTEXT(80055);
		case XGAME::xRES_CASH:		return XTEXT(80012);
		case XGAME::xRES_GUILD_COIN:		return XTEXT( 2286 );
		default:
			XBREAK(1);
			break;
		}
		return _T("");
	}
	inline LPCTSTR GetStrResourceForDebug( XGAME::xtResource res ) {
		switch( res )
		{
		case XGAME::xRES_WOOD:	return _T("W");
		case XGAME::xRES_IRON:	return _T("I");
		case XGAME::xRES_JEWEL:	return _T("J");
		case XGAME::xRES_SULFUR:	return _T("S");
		case XGAME::xRES_MANDRAKE:	return _T("M");
		default:
			XBREAK( 1 );
			break;
		}
		return _T( "" );
	}

	/// unit부대의 업글을 위해 필요한 아이템
	inline ID GetSquadLvupItem( XGAME::xtAttack typeAtk, XGAME::xtGrade grade = XGAME::xGD_NONE ) {
		int idx = (!grade)? 0 : (grade-1);
		switch( typeAtk )
		{
		case XGAME::xAT_TANKER:	return 10100 + idx;
		case XGAME::xAT_RANGE:	return 10120 + idx;
		case XGAME::xAT_SPEED:	return 10110 + idx;
		default:
			XBREAK(1);
			break;
		}
		return 0;
	}
	/// 스킬업을 위해 필요한 아이템 얻기
	inline ID GetSkillLvupItem( XGAME::xtGrade grade = XGAME::xGD_NONE ) {
		if( grade == XGAME::xGD_NONE )
			return SCROLL_GRAY;
		return SCROLL_GRAY + (grade-1);
	}

//	XPropItem::xPROP* GetClanBookProp( XGAME::xtClan clan );

	///< tier: 1~3
	inline XGAME::xtUnit GetRandomTankerUnit( int tier ) {
		XGAME::xtUnit unit[3] = { XGAME::xUNIT_SPEARMAN, XGAME::xUNIT_MINOTAUR, XGAME::xUNIT_GOLEM };
		return unit[xRandom(tier) ];
	}

	///< tier: 1~3
	inline XGAME::xtUnit GetRandomRangeUnit( int tier ) {
		XGAME::xtUnit unit[ 3 ] = {XGAME::xUNIT_ARCHER, XGAME::xUNIT_CYCLOPS, XGAME::xUNIT_TREANT};
		return unit[xRandom(tier) ];
	}

	///< tier: 1~3
	inline XGAME::xtUnit GetRandomSpeedUnit( int tier ) {
		XGAME::xtUnit unit[ 3 ] = {XGAME::xUNIT_PALADIN, XGAME::xUNIT_LYCAN, XGAME::xUNIT_FALLEN_ANGEL};
		return unit[xRandom( tier ) ];
	}

	/// tier: 1~3
	inline XGAME::xtUnit GetRandomUnit( XGAME::xtAttack typeAtk, XGAME::xtSize sizeTier ) {
		int tier = sizeTier;
		switch( typeAtk )
		{
		case XGAME::xAT_TANKER:	return GetRandomTankerUnit( tier );
		case XGAME::xAT_RANGE:	return GetRandomRangeUnit( tier );
		case XGAME::xAT_SPEED:	return GetRandomSpeedUnit( tier );
		default:
			XBREAK(1);
			break;
		}
		return XGAME::xUNIT_NONE;
	}
	xtUnit GetUnitRandomByFilter( XGAME::xtUnitFilter filter );
	bool IsUnitFilter( XGAME::xtUnit unit, xtUnitFilter filter );
	XGAME::xtUnit GetUnitBySizeAndAtkType( XGAME::xtSize size, XGAME::xtAttack atkType );
	/**
	 @brief 유닛 식별자. 모든 코드에 각 유닛들의 식별자는 이것을 공통으로 쓴다.
	*/
	inline const char* GetUnitIdentifier( XGAME::xtUnit unit )	{
		switch( unit )
		{
		case XGAME::xUNIT_SPEARMAN:	return "spearman";
		case XGAME::xUNIT_ARCHER:	return "archer";
		case XGAME::xUNIT_PALADIN:	return "paladin";
		case XGAME::xUNIT_MINOTAUR:	return "minotaur";
		case XGAME::xUNIT_CYCLOPS:	return "cyclops";
		case XGAME::xUNIT_LYCAN:	return "lycan";
		case XGAME::xUNIT_GOLEM:	return "golem";
		case XGAME::xUNIT_TREANT:	return "treant";
		case XGAME::xUNIT_FALLEN_ANGEL:	return "fallen_angel";
		default:
			return "";
		}
		return "";
	}
	inline const char* GetstrEnumUnit( XGAME::xtUnit unit ) {
		switch( unit )
		{
		case XGAME::xUNIT_SPEARMAN:	return "xUNIT_SPEARMAN";
		case XGAME::xUNIT_ARCHER:	return "xUNIT_ARCHER";
		case XGAME::xUNIT_PALADIN:	return "xUNIT_PALADIN";
		case XGAME::xUNIT_MINOTAUR:	return "xUNIT_MINOTAUR";
		case XGAME::xUNIT_CYCLOPS:	return "xUNIT_CYCLOPS";
		case XGAME::xUNIT_LYCAN:	return "xUNIT_LYCAN";
		case XGAME::xUNIT_GOLEM:	return "xUNIT_GOLEM";
		case XGAME::xUNIT_TREANT:	return "xUNIT_TREANT";
		case XGAME::xUNIT_FALLEN_ANGEL:	return "xUNIT_FALLEN_ANGEL";
		case XGAME::xUNIT_NONE:	return "xUNIT_NONE";
		case XGAME::xUNIT_ANY:	return "xUNIT_ANY";
		default:
			return "";
		}
		return "";
	}
	const char* GetstrEnumGrade( XGAME::xtGrade grade );
	const char* GetstrEnumGradeLegion( XGAME::xtGradeLegion grade );
	const char* GetstrEnumTypeAtk( XGAME::xtAttack typeAtk );
	const char* GetstrEnumSizeUnit( XGAME::xtSize sizeUnit );
	/**
	 @brief 각파츠 식별자
	*/
	inline const char* GetPartsIdentifier( XGAME::xtParts parts ) {
		switch( parts )
		{
		case XGAME::xPARTS_HEAD:	return "helm";
		case XGAME::xPARTS_CHEST:	return "chest";
		case XGAME::xPARTS_HAND:	return "hand";
		case XGAME::xPARTS_FOOT:	return "foot";
		case XGAME::xPARTS_ACCESSORY:	return "accessory";
		default:
			XBREAK(1);
			break;
		}
		return "";
	}
	////////////////////////////////////////////////////////////////
	inline LPCTSTR GetStrState( XGAME::xtState state ) {
		switch( state )
		{
		case XGAME::xST_SLEEP:	return XTEXT(2314);
		case XGAME::xST_STUN:	return XTEXT(2315);
		case XGAME::xST_HOLD:	return XTEXT(2317);
		case XGAME::xST_BLEEDING:	return XTEXT( 2318 );
		case XGAME::xST_POISON:	return XTEXT( 2319 );
		case XGAME::xST_BLIND:	return XTEXT( 2320 );
		case XGAME::xST_SLOW:	return XTEXT( 2321 );
		case XGAME::xST_CHAOS:	return XTEXT( 2322 );
		case XGAME::xST_FEAR:	return XTEXT( 2323 );
		case XGAME::xST_SILENCE:	return XTEXT( 2324 );
		case XGAME::xST_INVISIBLE:	return XTEXT( 2325 );
		case XGAME::xST_ENTANGLE:	return XTEXT( 2326 );
		case XGAME::xST_FROZEN:		return XTEXT( 2327 );
		case XGAME::xST_ICE:		return XTEXT( 2328 );
		case XGAME::xST_TAUNT:		return XTEXT( 2329 );
		case XGAME::xST_BURN:		return XTEXT( 2330 );
		case XGAME::xST_FURY:		return XTEXT( 2331 );
		case XGAME::xST_PARALYSIS:	return XTEXT( 2332 );
		default:
			break;
		}
		return _T("state????");
	}

	/**
	 @brief clan의 클랜징표 identifier를 얻는다.,
	*/
// 	inline LPCTSTR GetIdsClanScalp( XGAME::xtClan clan ) {
// 		switch( clan ) {
// 		case XGAME::xCL_CROW:		return _T( "scalp_crow" );	break;
// 		case XGAME::xCL_IRONLORD:	return _T( "scalp_ironlord" );	break;
// 		case XGAME::xCL_FREEDOM:	return _T( "scalp_freedom" );	break;
// 		case XGAME::xCL_ANCIENT:	return _T( "scalp_ancient" );	break;
// 		case XGAME::xCL_FLAME:		return _T( "scalp_flame" );	break;
// 		default:
// 			XBREAK( 1 );
// 			return 0;
// 		}
// 		return _T("");
// 	}
// 
// 	inline LPCTSTR GetIdsClanBook( XGAME::xtClan clan ) {
// 		switch( clan ) {
// 		case XGAME::xCL_CROW:		return _T( "book01_crow" );	break;
// 		case XGAME::xCL_IRONLORD:	return _T( "book01_ironlord" );	break;
// 		case XGAME::xCL_FREEDOM:	return _T( "book01_freedom" );	break;
// 		case XGAME::xCL_ANCIENT:	return _T( "book01_ancient" );	break;
// 		case XGAME::xCL_FLAME:		return _T( "book01_flame" );	break;
// 		default:
// 			XBREAK( 1 );
// 			return 0;
// 		}
// 		return _T( "" );
// 	}
// 	inline LPCTSTR GetIdsClan( XGAME::xtClan clan ) {
// 		switch( clan ) {
// 		case XGAME::xCL_NONE:		return _T( "xCL_NONE" );	break;
// 		case XGAME::xCL_CROW:		return _T( "xCL_CROW" );	break;
// 		case XGAME::xCL_IRONLORD:	return _T( "xCL_IRONLORD" );	break;
// 		case XGAME::xCL_FREEDOM:	return _T( "xCL_FREEDOM" );	break;
// 		case XGAME::xCL_ANCIENT:	return _T( "xCL_ANCIENT" );	break;
// 		case XGAME::xCL_FLAME:		return _T( "xCL_FLAME" );	break;
// 		default:
// 			XBREAK( 1 );
// 			return 0;
// 		}
// 		return _T( "" );
// 	}
	LPCTSTR GetIdsMedal( xtAttack atkType, int grade );
// 	int GetCostGatcha( XGAME::xtGatha type, int level );
// 	inline int GetCostLowGatcha( int level ) {
// 		return GetCostGatcha( XGAME::xGA_LOW_GOLD, level );
// 	}
// 
// 	inline int GetCostHighGoldGatcha( int level ) {
// 		return GetCostGatcha( XGAME::xGA_HIGH_GOLD, level );
// 	}
// 
// 	inline int GetCostHighCashGatcha( int level ) {
// 		return GetCostGatcha( XGAME::xGA_HIGH_CASH, level );
// 	}
	/**
	 @brief 스팟 타입의 이름을 돌려준다.
	*/
	inline LPCTSTR GetStrSpotType( XGAME::xtSpot type ) {
		switch( type ) {
		case XGAME::xSPOT_CASTLE:	return XTEXT(5070);
		case XGAME::xSPOT_JEWEL:	return XTEXT(5071);
		case XGAME::xSPOT_SULFUR:	return XTEXT(5072);
		case XGAME::xSPOT_MANDRAKE:	return XTEXT(5073);
		case XGAME::xSPOT_NPC:		return XTEXT(5074);
		case XGAME::xSPOT_DAILY:	return _T("요일스팟");
		case XGAME::xSPOT_SPECIAL:	return _T("특별 스팟");
		case XGAME::xSPOT_CAMPAIGN:	return XTEXT(5075);
		case XGAME::xSPOT_VISIT:	return XTEXT(5076);
		default:
			XBREAK(1);
			break;
		}
		return _T("");
	}
  /**
   @brief 각 스팟의 enum 문자열을 돌려준다. 로그용
  */
  inline LPCTSTR GetStrSpotEnum( XGAME::xtSpot type ) {
    switch( type )
    {
    case XGAME::xSPOT_CASTLE:	return _T("xSPOT_CASTLE");
    case XGAME::xSPOT_JEWEL:	return _T("xSPOT_JEWEL");
    case XGAME::xSPOT_SULFUR:	return _T("xSPOT_SULFUR");
    case XGAME::xSPOT_MANDRAKE:	return _T("xSPOT_MANDRAKE");
    case XGAME::xSPOT_NPC:		return _T("xSPOT_NPC");
    case XGAME::xSPOT_DAILY:	return _T("xSPOT_DAILY");
    case XGAME::xSPOT_SPECIAL:	return _T("xSPOT_SPECIAL");
    case XGAME::xSPOT_CAMPAIGN:	return _T("xSPOT_CAMPAIGN");
    case XGAME::xSPOT_VISIT:	return _T("xSPOT_VISIT");
    case XGAME::xSPOT_CASH: return _T("xSPOT_CASH");
//     case XGAME::xSPOT_GUILD_RAID: return _T("xSPOT_GUILD_RAID");
//     case XGAME::xSPOT_PRIVATE_RAID: return _T("xSPOT_PRIVATE_RAID");
    case XGAME::xSPOT_COMMON: return _T( "xSPOT_COMMON" );
    default:
      XBREAK( 1 );
      break;
    }
    return _T( "unknown" );
  }
// 	inline LPCTSTR GetstrClan( XGAME::xtClan clan ) {
// 		switch( clan )
// 		{
// 		case XGAME::xCL_CROW:		return XTEXT(5000);
// 		case XGAME::xCL_IRONLORD:	return XTEXT(5001);
// 		case XGAME::xCL_FREEDOM:	return XTEXT(5002);
// 		case XGAME::xCL_ANCIENT:	return XTEXT(5003);
// 		case XGAME::xCL_FLAME:		return XTEXT(5004);
// 		default:
// 			XBREAK(1);
// 			break;
// 		}
// 		return _T("");
// 	}
	// 보정파라메터의 아이콘
	LPCTSTR GetIconAdjParam( XGAME::xtParameter adjParam );
	// 보정파라메터의 텍스트
	LPCTSTR GetStrAdjParam( XGAME::xtParameter adjParam );
	// 각 기능의 스트링을 돌려준다.
	inline LPCTSTR GetstrLockMenu( XGAME::xtMenus menu ) {
		switch( menu )
		{
		case XGAME::xBM_BARRACK:	return XTEXT(80004);
		case XGAME::xBM_LABORATORY:	return XTEXT(2130);
		case XGAME::xBM_CATHEDRAL:	return XTEXT(2131);
		case XGAME::xBM_TAVERN:		return XTEXT(2132);
		case XGAME::xBM_ACADEMY:	return XTEXT(2133);
		case XGAME::XBM_UNIT_HANGOUT:	return XTEXT(2134);
		case XGAME::xBM_EMBASSY:	return XTEXT(2135);
		case XGAME::xBM_MARKET:		return XTEXT(80052);
//		case XGAME::xBM_TRAING_CENTER:	return XTEXT(2097);
		default:
			XBREAK(1);
			break;
		}
		return _T("");
	}
	inline bool IsSmall( XGAME::xtUnit unit ) {
		return GetSizeUnit(unit) == XGAME::xSIZE_SMALL;
	}
	inline bool IsMiddle( XGAME::xtUnit unit ) {
		return GetSizeUnit( unit ) == XGAME::xSIZE_MIDDLE;
	}
	inline bool IsBig( XGAME::xtUnit unit ) {
		return GetSizeUnit( unit ) == XGAME::xSIZE_BIG;
	}
	float GetMultiplyPower( int levelHard );
	// 전투력 powerEnemy의 난이도등급을 리턴한다 0이면 동급이며 양수가 어려움, 음수가 쉬움이다. 기준은 powerBase가 된다.
	int GetHardLevel( int powerEnemy, int powerBase );
	int GetPowerByColor( int powerBase, int lvColor );
	// 전투력 powerEnemy가 무슨색인지 얻는다. 기준은 powerBase
	XCOLOR xGetColorPower( int powerEnemy, int powerBase );
	inline int GetMaxSkillLevel(){
		return MAX_SKILL_LEVEL - 1;
	}
	inline int GetMaxSkillLevelTable(){
		return MAX_SKILL_LEVEL;
	}
	int GetLevelMaxAccount();
	inline bool IsInvalidParts( XGAME::xtParts parts ) {
		if( parts <= XGAME::xPARTS_NONE || parts >= XGAME::xPARTS_MAX )
			return true;
		return false;
	}
	inline bool IsValidParts( XGAME::xtParts parts ){
		return !IsInvalidParts( parts );
	}
	inline bool IsInvalidSpotType( ID typeSpot ) {
		if( typeSpot <= XGAME::xSPOT_NONE || typeSpot >= XGAME::xSPOT_MAX )
			return true;
		return false;
	}
	inline bool IsValidAdjParam( XGAME::xtParameter adjParam ) {
		if( adjParam <= XGAME::xNONE_PARAM || adjParam >= XGAME::xMAX_PARAM )
			return false;
		return true;
	}
	inline bool IsInvalidAdjParam( XGAME::xtParameter adjParam ){
		return !IsValidAdjParam( adjParam );
	}
	inline bool IsInvalidUnit( XGAME::xtUnit unit ){
		return unit <= XGAME::xUNIT_NONE || unit >= XGAME::xUNIT_MAX;
	}
	inline bool IsInvalidTrainType( XGAME::xtTrain typeTrain ){
		return typeTrain <= XGAME::xTR_NONE || typeTrain >= XGAME::xTR_MAX;
	}
	inline bool IsValidTrainType( XGAME::xtTrain typeTrain ){
		return !IsInvalidTrainType( typeTrain );
	}
	inline bool IsInvalidSkillTrain( XGAME::xtTrain typeTrain ){
		return typeTrain != XGAME::xTR_SKILL_ACTIVE_UP && typeTrain != XGAME::xTR_SKILL_PASSIVE_UP;
	}
	inline bool IsInvalidGrade( XGAME::xtGrade grade ){
		return grade <= xGD_NONE || grade >= xGD_MAX;
	}
	inline bool IsInvalidResource( XGAME::xtResource type ){
		return type < XGAME::xRES_WOOD || type >= XGAME::xRES_MAX;
	}
	inline bool IsValidResource( XGAME::xtResource type ){
		return !IsInvalidResource( type );
	}
	inline bool IsInvalidResourceAll( XGAME::xtResource type ){
		return type < XGAME::xRES_WOOD || type >= XGAME::xRES_ALL;
	}
	inline bool IsValidResourceAll( XGAME::xtResource type ){
		return !IsInvalidResourceAll( type );
	}
	inline bool IsInvalidGradeLegion( XGAME::xtGradeLegion grade ){
		return (grade <= XGAME::xGL_NONE || grade >= XGAME::xGL_MAX );
	}
	inline bool IsInvalidAtkType( XGAME::xtAttack atkType ){
		return atkType <= xAT_NONE || atkType >= xAT_MAX;
	}
	inline bool IsInvalidSizeUnit( xtSize sizeUnit ) {
		return sizeUnit <= xSIZE_NONE || sizeUnit >= xSIZE_MAX;
	}
	inline bool IsInvalidBgObj( XGAME::xtBgObj type ){
		return type <= xBOT_NONE || type >= xBOT_MAX;
	}
#ifdef _CLIENT
// 	struct xSPM_BATTLE : public xSPM_BASE {
// 		ID idSpot = 0;
// 		int level = 0;
// 		_tstring strName;
// 		LegionPtr spLegion[2];	// 0:아군 1:적군
// 		int defense = 0;
// 		BOOL bvsNPC = false;		// NPC를 상대하는가.
// 		int idxStage = 0;		///< 캠페인의 경우 스테이지 인덱스
// 		ID m_idEnemy = 0;			// 상대가 pc의 경우.
// //		XAccount *pEnemy = nullptr;
// 	};
#endif // _CLIENT
	struct xLegionParam {
		xtUnit unit = xUNIT_NONE;
 		xtUnitFilter x_filterUnit = xUF_NONE;	// 유닛 필터
		xtGradeLegion x_gradeLegion = xGL_NORMAL;
		bool bRandom = false;	// 싱글에서만 사용
		int m_numVisible = 3;	// 안개로 덮혀있지 않는 부대수.
		//
		xtUnitFilter SetUnitFilter( xtSize size );
		xtUnitFilter SetUnitFilter( xtAttack atkType );
	};
	xtUnitFilter AddUnitFilter( xtAttack atkType, xtUnitFilter filterSrc = xUF_NONE );
	xtUnitFilter AddUnitFilter( xtSize size, xtUnitFilter filterSrc = xUF_NONE );
	void sReplaceToken( _tstring& str, LPCTSTR szToken, LPCTSTR szReplace );
	inline void sReplaceToken( _tstring& str, const _tstring& strToken, LPCTSTR szReplace ) {
		sReplaceToken( str, strToken.c_str(), szReplace );
	}
	inline void sReplaceToken( _tstring& str, const _tstring& strToken, const _tstring& strReplace ) {
		sReplaceToken( str, strToken.c_str(), strReplace.c_str() );
	}
	inline void sReplaceToken( _tstring& str, LPCTSTR szToken, const _tstring& strReplace ) {
		sReplaceToken( str, szToken, strReplace.c_str() );
	}
	//
	struct xDropItem {
		ID idDropItem = 0;		// 드랍시킬 아이템
		int num = 1;			// 드랍시킬 개수
		float chance = 0.f;		// 확률. 1.0은 100%
		xDropItem() {}
		xDropItem( ID idDrop, int num, float chance = 1.f ) :
			idDropItem( idDrop ), num( num ), chance( chance ) {}
		int Serialize( XArchive& ar );
		int DeSerialize( XArchive& ar, int );
	};
	const char* GetstrEnumActionEvent( xtActionEvent event );
#ifdef _CLIENT
	void UpdateSkillTooltip( ID idSkill, XWnd *pRoot );
#endif // _CLIENT
	_tstring GetstrResearchTime( int sec );
	bool IsPenaltyMOS( XGAME::xtAttack typeAttacker, XGAME::xtAttack typeDefender );
	bool IsPenaltySize( XGAME::xtSize sizeAttacker, XGAME::xtSize sizeDefender );
	bool IsSuperiorMOS( XGAME::xtAttack typeAttacker, XGAME::xtAttack typeDefender );
	bool IsSuperiorSize( XGAME::xtSize sizeAttacker, XGAME::xtSize sizeDefender );
	int SerializeCrashDump( XArchive& ar, ID idAcc );
	ID DeSerializeCrashDump( XArchive& ar, ID idAccOrig );
	// db에 저장됨. 순서바꾸지 말것.
	enum xtPlatform {
		xPL_NONE,
		xPL_GOOGLE_STORE,
		xPL_APPLE_STORE,
		xPL_WINDOWS,
		xPL_SOFTNYX,
		xPL_MAX,
	};
#ifdef _CLIENT
	xtPlatform GetPlatform();
#endif // _CLIENT

	enum xtErrorIAP {
		xIAPP_NONE = -1,
		xIAPP_SUCCESS = 0,
		xIAPP_ERROR_UNKNOWN,					/// 기타등등 에러
		xIAPP_ERROR_CRITICAL, 				/// 있어서는 안되는 상황이 생겼을때.
		xIAPP_ERROR_VERIFY_SERVER_OFFLINE,	/// 결제 검증 서버 오프라인
		xIAPP_ERROR_VERIFY_FAILED, 			// 검증 실패
		xIAPP_ERROR_RECEIPT_WRONG, 			// 결제 영수증 에러
		xIAPP_ERROR_NOT_FOUND_PRODUCT,		// 존재 하지 않는 상품
		xIAPP_ERROR_NOT_FOUND_PAYLOAD,		// PAYLOAD 다름.
		xIAPP_ERROR_DUPLICATE_RECEIPT,		// 이미 사용된 영수증.
		xIAPP_ERROR_INVALID_PAYLOAD,			// 유효하지 않은 payload값
		xIAPP_ERROR_INVALID_RECEIPT,			// 유효하지 않은 영수증
		xIAPP_ERROR_CANCELED_PURCHASE,		// 거래취소?
		xIAPP_ERROR_REFUNDED_PURCHASE,		// 환불?
		xIAPP_ERROR_DIFF_PAYLOAD_WITH_PRODUCT,	// DB페이로드에 저장되어있던 상품명과 다름
		xIAPP_ERROR_EXCEPTION,						// 예외 발생
		xIAPP_CANT_BUY_ANYMORE,						// 더이상 구매할 수 없음.
	};
	LPCTSTR GetstrDEV_LEVEL();
#ifdef _CLIENT
#if defined(_SOFTNYX)
	const xtPlatform PLATFORM_STORE = xPL_SOFTNYX;
#elif defined(_VER_ANDROID)
	const xtPlatform PLATFORM_STORE = xPL_GOOGLE_STORE;
#elif defined(_VER_IOS)
	const xtPlatform PLATFORM_STORE = xPL_APPLE_STORE;
#else
	const xtPlatform PLATFORM_STORE = xPL_GOOGLE_STORE;
#endif
#endif // _CLIENT
	/// 지불방법
	enum xtTermsPayment {
		xTP_NONE,
		xTP_GOLD,		// 금화로 지불
		xTP_GOLD_AND_CASH,		// 금화와 모자르는 금액은 캐시로 지불
		xTP_CASH,							// 캐시로 모두 지불
	};
}; // namespace XGAME

