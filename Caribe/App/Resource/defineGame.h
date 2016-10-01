#pragma once
// cpp소스에서도 사용되고 리소스로도 사용되는 상수들입니다.
// 여기서 정의한 상수들은 프로퍼티나 스크립트 등에서 사용할 수있습니다.
#ifdef __cplusplus
// 아래에 정의된 상수들은 모두 루아에서 쓸수 있다.
#define DEFINE_SAMPLE		11


namespace XGAME
{
	///< 통화
	enum xCurrency {
		xCUR_NONE,
		xCUR_WON_KOREA,
		xCUR_US_DOLLAR,
	};

// 	///< 8방향
// 	enum xtDir { DIR_E, 
// 				DIR_ES, 
// 				DIR_S, 
// 				DIR_WS, 
// 				DIR_W, 
// 				DIR_WN, 
// 				DIR_N, 
// 				DIR_EN };	
	
	///< 유닛 사이즈
	enum xtSize { 
				xSIZE_NONE,
				xSIZE_SMALL,
				xSIZE_MIDDLE,
				xSIZE_BIG,
				xSIZE_MAX,
				};
	// 유닛 필터
	enum xtUnitFilter {
		xUF_NONE,
		xUF_SMALL   = 0x0001,
		xUF_MIDDLE  = 0x0002,
		xUF_BIG		= 0x0004,
		xUF_SIZE_ALL = 0x0007,
		xUF_TANKER  = 0x0008,
		xUF_RANGE	= 0x0010,
		xUF_SPEED	= 0x0020,
		xUF_ATKTYPE_ALL = 0x0038,
		xUF_ALL = 0xffffffff,
	};
	///< 공격 타입
	enum xtAttack {
					xAT_NONE,
					xAT_TANKER,
					xAT_RANGE,
					xAT_SPEED,
					xAT_MAX,
				};
	enum xtUnitRank {
		xUR_NONE,
		xUR_SOLDIER,		///< 병사
		xUR_HERO,			///< 영웅
	};
	enum xtStat {
		xSTAT_NONE,
		xSTAT_ATK_MELEE,
		xSTAT_ATK_RANGE,
		xSTAT_DEF,
		xSTAT_HP,
		xSTAT_SPEED_ATK,
		xSTAT_SPEED_MOV
	};
// 	enum xtResource {	
// 						xRES_WOOD,
// 						xRES_IRON,
// 						xRES_JEWEL,
// 						xRES_SULFUR,		///< 유황
// 						xRES_MANDRAKE,
// 						xRES_MAX,
// 						xRES_GOLD = -1,						
// 						xRES_WOOD_IRON = -2,
// 						xRES_CASH = -3,
// 						xRES_GUILD_COIN = -4,
// 						xRES_ALL = -0x7f,
// 	};
	// 순서나 숫자 바꾸지 말것.
	enum xtResource {	
		xRES_WOOD,
		xRES_IRON,
		xRES_JEWEL,
		xRES_SULFUR,		///< 유황
		xRES_MANDRAKE,
		xRES_MAX,
		xRES_GOLD = xRES_MAX,
		xRES_CASH,
		xRES_GUILD_COIN,
		xRES_WOOD_IRON,
		xRES_ALL,
		xRES_NONE = xRES_ALL,
	};

	enum xtPOSTResource {
		xPOSTRES_GOLD,
		xPOSTRES_GEM,
		xPOSTRES_ITEMS,
		xPOSTRES_RESOURCE,
		xPOSTRES_GUILD_COIN,
		xPOSTRES_MAX,
		xPOSTRES_INIT = 999,
	};

	enum xtUnit { 
				xUNIT_NONE,
				xUNIT_SPEARMAN,
				xUNIT_ARCHER,
				xUNIT_PALADIN,
				xUNIT_MINOTAUR,
				xUNIT_CYCLOPS,
				xUNIT_LYCAN,
				xUNIT_GOLEM,
				xUNIT_TREANT,
				xUNIT_FALLEN_ANGEL,		///< 타천사
				xUNIT_MAX,
				xUNIT_ANY = -1,
	};

	///< 지휘타입 xtAttack을 함께 쓴다.
// 	enum xtLead { xLD_NONE,
// 				xLD_TANKER,		///< 근접유닛 지휘
// 				xLD_RANGE,		///< 원거리 유닛 지휘
// 				xLD_SPEED,		///< 스피드유닛 지휘
// 				xLD_MAX,
//	};

	///< 클랜
// 	enum xtClan { xCL_NONE,
// 				xCL_CROW,			///< 까마귀 
// 				xCL_IRONLORD,		///< 강철군주
// 				xCL_FREEDOM,		///< 자유동맹
// 				xCL_ANCIENT,		///< 고대문명
// 				xCL_FLAME,			///< 불꽃
// 				xCL_MAX,
// 				xCL_RANDOM=-1,		///< 랜덤결정
// 	};

	//갓차 등급
	enum xtUserGrade{
				xUG_NORMAL,			//일반
				xUG_PRIMIUM,		//고급
				xUG_MAX,			
	};	

	///< 클래스
	enum xtClass { xCLS_NONE,
				xCLS_TANKER,		///< 근접형영웅
				xCLS_RANGE,			///< 원거리형 영웅
				xCLS_HORSE,			///< 기마형
				xCLS_HEAL,			///< 힐러형
				xCLS_WIZARD,		///< 마법사형
	};

	///< 등급
	enum xtGrade {
				xGD_NONE,
				xGD_COMMON,			///< 일반
				xGD_VETERAN,		///< 베테랑
				xGD_RARE,			///< 희귀
				xGD_EPIC,			///< 에픽
				xGD_LEGENDARY,		///< 전설
				xGD_MAX,
	};
	// 군단 등급
	enum xtGradeLegion {
		xGL_NONE,
		xGL_NORMAL,		// 일반 부대
		xGL_ELITE,		// 정예 부대(hp가 일반부대의 2배)
		xGL_RAID,		// 레이드 부대(hp가 일반부대의 4배)
		xGL_MAX,
	};

	///< 아이템 타입
	enum xtItem {
		xIT_NONE,		///< 초기화용
		xIT_EQUIP,		///< 장착아이템
		xIT_SCALP,		///< 클랜 징표
		xIT_SCROLL,		///< 스킬 전리품
		xIT_MEDAL,		///< 부대업글용 전리품
		xIT_BOOK,		///< 책
		xIT_QUEST,		///< 퀘템
		xIT_CASH,		///< 유료템
		xIT_EVENT,		///< 이벤트
		xIT_ETC,		///< 기타 잡템
		xIT_SOUL,
	};

	///< 아이템 사용방식(인벤에서 사용)
	enum xtItemUse {
		xIU_NONE,		///< 사용불가능
		xIU_USE,		///< 인벤에서 사용가능
	};

	///< 장착아이템 부위
	enum xtParts {
		xPARTS_NONE,			///< 초기화용
		xPARTS_HEAD,			///<  머리
		xPARTS_CHEST,			///< 가슴
		xPARTS_HAND,			///< 손
		xPARTS_FOOT,			///< 발
		xPARTS_ACCESSORY,		///< 악세서리
		xPARTS_MAX,
	};

	// 발동파라메터(주의: ADJ가 없는 파라메터는 보정치가 아니다)
	// 이것도 예제템플릿만 제공해주고 사용자정의로 쓰는것이 나을듯.
	enum xtParameter {
		xNONE_PARAM = 0,
		xADJ_NONE = 0,
		xADJ_ATTACK,					// 공격력보정
		xADJ_ATTACK_MELEE_TYPE,			// 근접공격력보정
		xADJ_ATTACK_RANGE_TYPE,			// 원거리공격력보정
		xADJ_DEFENSE,					// 방어력보정
		xADJ_MAX_HP,					// 최대체력보정
		xADJ_HP,						// 체력보정
		xADJ_HP_REGEN,					// 체력리젠양보정
		xADJ_HP_REGEN_SEC,				// 체력리젠속도보정
		xADJ_MOVE_SPEED,				// 이속보정
		xADJ_ATTACK_SPEED,				// 공속보정
		xADJ_ATTACK_RANGE,				// 공격사거리보정
		xADJ_HIT_RATE,					// 적중율보정
		xADJ_DAMAGE,					// 모든피해보정
		xADJ_PHY_DAMAGE,				// 물리피해보정
		xADJ_MAG_DAMAGE,				// 마법피해보정
		xADJ_PHY_DAMAGE_RECV,			// 받는물리피해보정
		xADJ_MAG_DAMAGE_RECV,			// 받는마법피해보정
		xADJ_MELEE_DAMAGE_RECV,			// 받는근접피해보정
		xADJ_RANGE_DAMAGE_RECV,			// 받는원거리피해보정
		xADJ_DAMAGE_RECV,				// 받는모든피해보정
		xADJ_HEAL,						// 힐량보정
		xADJ_HEAL_RECV,					// 받는힐량보정
		xADJ_SKILL_POWER,				// 스킬파워보정
		xADJ_SIGHT,						// 시야보정
		xADJ_PENETRATION_RATE,			// 관통율보정
		xADJ_PENETRATION_PROB,			// 관통확률보정
		xADJ_EVADE_RATE,				// 회피율보정
		xADJ_EVADE_RATE_MELEE,			// 근접회피율보정
		xADJ_EVADE_RATE_RANGE,			// 원거리회피율보정
		xADJ_EVADE_RATE_SMALL,			// 소형회피율보정
		xADJ_EVADE_RATE_MIDDLE,			// 중형회피율보정
		xADJ_EVADE_RATE_BIG,			// 대형회피율보정
		xADJ_BLOCKING_RATE,				// 완전방어율보정
		xADJ_CRITICAL_RATE,				// 치명타율보정
		xADJ_CRITICAL_POWER,			///< 치명타배수보정.
		xADJ_CRITICAL_RATE_RECV,		///< 받는치명타율보정
		xADJ_CRITICAL_RECV,				// 받는모든치명타피해보정
		xADJ_MELEE_CRITICAL_RECV,		// 받는근접치명타피해보정
		xADJ_RANGE_CRITICAL_RECV,		// 받는원거리치명타피해보정
		xADJ_ADD_DAMAGE_TRIBE,			///< 추가피해율보정종족
		xADJ_ADD_DAMAGE_CLASS,			///< 추가피해율보정직업
		xADJ_ATTACK_SPEED_SMALL,		///< 소형공속보정
		xADJ_ATTACK_SPEED_MIDDLE,		///< 중형공속보정
		xADJ_ATTACK_SPEED_BIG,			///< 대형공속보정
		xADJ_DAMAGE_SMALL,				///< 소형피해보정
		xADJ_DAMAGE_MIDDLE,				///< 중형피해보정
		xADJ_DAMAGE_BIG,				///< 대형피해보정
		xADJ_DAMAGE_TANKER,				///< 근접형피해보정
		xADJ_DAMAGE_RANGE,				///< 원거리형피해보정
		xADJ_DAMAGE_SPEED,				///< 스피드형피해보정
		xADJ_DAMAGE_SPEARMAN,			///< 창병피해보정
		xADJ_DAMAGE_ARCHER,			///< 궁수피해보정
		xADJ_DAMAGE_PALADIN,			///< 기사피해보정
		xADJ_DAMAGE_MINOTAUR,			///< 미노피해보정
		xADJ_DAMAGE_CYCLOPS,			///< 사이피해보정
		xADJ_DAMAGE_LYCAN,			///< 라이피해보정
		xADJ_DAMAGE_GOLEM,			///< 골렘피해보정
		xADJ_DAMAGE_TREANT,			///< 앤트피해보정
		xADJ_DAMAGE_FALLEN_ANGEL,			///< 타천피해보정
		xADJ_DAMAGE_FIRE,			///< 화염피해보정
		xADJ_SKILL_COOL,				///< 쿨타임보정/재사용대기시간보정
		xADJ_VAMPIRIC,					///< 흡혈양보정

		// 이곳까지 추가 하시오
		xMAX_PARAM,
		xADJ_HARDCORD,					///< 하드코딩용
		// TODO: 보정치가 아닌 파라메터는 이곳에 추가하시오
		xHP=-1,				// 체력(주의:보정아님)
		xMP=-2,				// 마나(주의:보정아님)
// 		xVAMPIRIC_HP=-3,		// 체력흡혈
// 		xVAMPIRIC_MP=-4,		// 마나흡혈
	};

	enum xtState {
		xST_NONE,
		xST_SLEEP,			///< 수면(
		xST_STUN,			///< 기절
		xST_HOLD,			///< 포박
		xST_BLEEDING,		///< 출혈
		xST_POISON,			///< 중독(독계열 도트데미지이며 해독이 가능하다. 그러나 DOT데미지양은 출혈보다 더 크다.)
		xST_BLIND,			///< 암흑
		xST_SLOW,			///< 둔화
		xST_FROZEN,			///< 빙결
		xST_ICE,			///< 얼음(완전히 동작이 굳어짐)
		xST_PARALYSIS,	///< 마비(현재로선 얼음과 같음)
		xST_BURN,			///< 화상(출혈과 화상의 차이는?)
		xST_CHAOS,			///< 혼란(피아를 식별하지 못하고 공격한다.)
		xST_FEAR,			///< 공포(타겟과 반대방향으로 도망간다)
		xST_SILENCE,		///< 침묵(스킬 및 특성사용불가)
		xST_INVISIBLE,		///< 투명화
		xST_ENTANGLE,		///< 휘감기(이동불가)
		xST_TAUNT,			///< 도발
		xST_DECAY,			///< 부패(부패에 걸린적을 타격하면 추가데미지가 들어간다)
		xST_FURY,			///< 분노(이동속도 방해효과에 걸리지 않는다)
		xST_MAX,
	};

	// 종족
	enum xtTribe {
		xTB_NONE,		///< 초기값 혹은 모두
		xTB_HUMAN,
		xTB_BEAST,		///< 짐승
		xTB_MONSTER,	
		xTB_DEVIL,		///< 악마
	};
	// 영웅 입수방법
	enum xtGet {
		xGET_NONE,
		xGET_GATHA				= 0x001,
		xGET_QUEST				= 0x002,
		xGET_GUILD_RAID		= 0x004,
		xGET_MEDAL_SPOT		= 0x008,
		xGET_EVENT				= 0x010,
		xGET_MAX,
	};
// 	enum xtGetBit {
// 		xGB_NONE = 0,
// 		xGB_GATHA				= 0x001,
// 		xGB_QUEST				= 0x002,
// 		xGB_GUILD_RAID	= 0x004,
// 		xGB_MEDAL_SPOT	= 0x008,
// 		xGB_EVENT				= 0x010,
// 		xGB_MAX,
// 	};
	// 스팟 타입
	enum xtSpot {
		xSPOT_NONE,
		xSPOT_CASTLE,		// 유저스팟
		xSPOT_JEWEL,
		xSPOT_SULFUR,
		xSPOT_MANDRAKE,
		xSPOT_NPC,
		xSPOT_DAILY,		// 요일별 이벤트 스팟
		xSPOT_SPECIAL,		// 스페셜 이벤트 스팟
		xSPOT_CAMPAIGN,		///< 캠페인 스팟
		xSPOT_VISIT,		///< 방문스팟(비전투)
		xSPOT_CASH,			///< 캐쉬리젠 스팟
		xSPOT_GUILD_RAID,     ///< 길드레이드
		xSPOT_PRIVATE_RAID,   ///< 개인 레이드
		xSPOT_COMMON,         ///< 범용
		xSPOT_MAX,
		xSPOT_WHEN = xSPOT_MAX,
	};
	// UI 액션
	enum xtUIAction {
		xUA_NONE = 0,
		xUA_RESEARCH,		// 연구
		xUA_UNLOCK_UNIT,	// 유닛잠금해제
		xUA_TRADE,			// 무역상 거래
		xUA_VISIT_ARMOR,	// 무기상 방문
		xUA_BUY_ARMOR,		// 무기상 거래
		xUA_VISIT_CASH,		// 귀중품상점 방문
		xUA_BUY_CASH,		// 귀중품상점에서 구입
		xUA_SELL_ITEM,		// 아이템 판매
		xUA_HIRE_HERO,		// 영웅고용
		xUA_PROVIDE_BOOK,	// 책 제공
		xUA_PROVIDE_SKILL,	// 보옥 제공
		xUA_PROVIDE_SQUAD,	// 메달 제공
		xUA_REPOSITION,		// 부대 포지션 변경
		xUA_TRAINING_LEVEL,		// 영웅렙업 훈련
		xUA_TRAINING_SKILL,		// 영웅 스킬 훈련
		xUA_TRAINING_SQUAD,		// 영웅 부대 훈련
		xUA_JOIN_GUILD,			// 길드 가입/생성
		xUA_EQUIP,			// 장비장착
		xUA_EXPAND_INVENTORY,	// 창고확장
		xUA_PROMOTION_HERO,		// 영웅 승급
		xUA_SUMMON_HERO,		// 영혼석으로 소환
		xUA_ENTER_SCENE,		// 씬 진입
		xUA_HERO_ORGANIZE,		// 영웅을 군단에 편입
		xUA_CONTROL_SQUAD,		// 부대컨트롤
		xUA_TOUCH_SQUAD_IN_READY,		// 전투배치씬에서 부대 선택.
		xUA_EDIT_SQUAD,				// 진영편집
		xUA_MAX,		// 
	};
	enum xtEvt {
		xEVT_NONE,
		xEVT_TRADE,
		xEVT_SHOP,
	};
	// 상점 타입
	enum xtShopType {
		xSHOP_NONE,
		xSHOP_ARMORY,		// 무기상점
		xSHOP_CASHSHOP,		// 귀중품(캐시)상점.
		xSHOP_GUILD,		// 길드상점
		xSHOP_MAX,
	};
	// 지불타입
	enum xtCoin {
		xCOIN_NONE,
		xCOIN_GOLD,			// 금화
		xCOIN_CASH,			// 캐시
		xCOIN_GUILD,		// 길드코인
		xCOIN_MEDAL,		// 메달
		xCOIN_MAX,

	};
	enum xtPush {
		xHERO_LEVELUP_TRAINING,		// 영웅 렙업 훈련
		xSQUAD_LEVELUP_TRAINING,	// 부대 훈련
		xSKILL_TRAINING_ACTIVE,		// 스킬 훈련(액티브)
		xSKILL_TRAINING_PASSIVE,	// 스킬 훈련(패시브)
		xTECH_TRAINING,				// 특성 연구
		xTRADER_RETURN,				// 무역상 리턴
		xARMORY_RETURN,				// 무기상 리턴
		xATTACT,					// 공격받음
		xEVENT,						// 이벤트
	};
	// 시퀀스 발생 이벤트
// 	enum xtSeqEvent {
// 		xSE_NONE,
// 		xSE_ENTER_SCENE,
// 		xSE_UNLOCK_MENU,		// 기능 잠금해제
// 	};
	// 각종 기능들의 잠금해제용으로 쓰는 비트
	enum xtMenus {
		XBM_NONE = 0,
		xBM_LABORATORY = 0x01,		// 연구소
		xBM_CATHEDRAL = 0x02,		// 성당(기사가 잠금해제된게 아님. 기사의 잠금해제를 할수 있게 된거임)
		xBM_TAVERN = 0x04,			// 선술집
		xBM_ACADEMY = 0x08,			// 아카데미(학교)
		XBM_UNIT_HANGOUT = 0x10,			// 병영
		xBM_EMBASSY = 0x20,			// 대사관
		xBM_MARKET = 0x40,			// 시장
//		xBM_TRAING_CENTER = 0x80,	// 훈련소
		xBM_DISPOSITION = 0x100,	// 부대배치 
		xBM_EQUIP = 0x200,			// 장비슬롯
		xBM_BARRACK = 0x400,		// 부대편성메뉴
		xBM_ALL = 0xffffffff,		// 모든비트
	};
	// 각종이벤트
	enum xtActionEvent {
		xAE_NONE,
		xAE_UNLOCK_MENU,	// 어떤 잠금기능이 해제됨
		xAE_OPEN_AREA,		// 지역이 열림
		xAE_UNLOCK_UNIT,	// 유닛이 잠금해제되었다.
		xAE_ENTER_SCENE,	// 씬에 진입
		xAE_POPUP,			// 팝업이 떴다.
		xAE_CLOSE_POPUP,	// 팝업이 닫힘.
		xAE_END_SEQ,		// 시퀀스하나가 끝남
		xAE_WHERE_QUEST,	// 퀘창의 where누름
		xAE_CONFIRM_QUEST,	// 퀘스트 읽고 확인을 누름
		xAE_QUEST_COMPLETE,	// 퀘스트가 ?상태가 됨
		xAE_QUEST_NEW,		// 새 퀘스트를 받음.
		xAE_QUEST_DESTROY,	// 퀘스트의 보상까지 받고 퀘가 삭제됨.
		xAE_VISIT_SPOT,		// 스팟 방문
		xAE_MAX,
	};
	// 씬
	enum xtScene {
		xSC_NONE,
		xSC_START,
//		xSC_MAIN_LOADING,
		xSC_OPENNING,
		xSC_TITLE,
		xSC_WORLD,
		xSC_UNIT_ORG,
		xSC_STORAGE,
		xSC_SHOP,
		xSC_HERO,
		xSC_TRADER,
		xSC_ARMORY,
		xSC_OPTION,
		xSC_ENDING,
		xSC_LEGION,			// 군단편성
		xSC_TECH_SEL,		///< 특성트리 유닛선택
		xSC_TECH,			///< 특성트리씬
		xSC_INGAME,
		xSC_EQUIP,
		xSC_GUILD,
		xSC_GUILD_SHOP,
		xSC_READY,			///< 인게임 전투배치씬.
		xSC_PATCH,
		xSC_LOADING,		///< 로딩씬
		xSC_TEST,				///< 각종테스트용 empty 씬
		xSC_TEST2,				///< 각종테스트용 empty 씬
		xSC_PRIVATE_RAID,
		xSCENE_MAX,
	};
	// 컷씬 이벤트 파라메터
	enum xtEVentParam {
		xEP_NONE,
		xEP_EXTRA_HERO,
		xEP_LOCK_PALADIN,
	};
	/**
	 현재 when이벤트와 what이벤트를 xQC_EVENT로 함께 쓰고 있다. 그러나 when에서 지원하지 않는 몇몇 이벤트(HAVE_ITEM같은)
	 를 when에 사용할 소지는 충분하다. 따라서 장기적으로 when과 what이벤트를 구분해서 쓰고 event_level처럼 양쪽이벤트에
	 다쓰이는 경우는 what이벤트를 DispatchQuestEvent()할때 자동으로 when으로도 바꿔서 OccurQuest를 하도록 해야한다.
	 너무 큰작업이 될거같아 일단은 이대로 쓴다.
	*/
	// 저장되는것이므로 순서바꾸지 말것.(어디서 저장됨????)
	enum xtQuestCond {
		xQC_NONE = 0,
		// 언제/무엇을(퀘 등장조건)
		xQC_EVENT_LEVEL,				///< 특정레벨이 되면
		xQC_EVENT_OPEN_AREA,			///< 특정 지역이 열림
		xQC_EVENT_CLEAR_SPOT,			///< 특정 스팟을 클리어(when으로 사용못함)
		xQC_EVENT_RECON_SPOT,			///< 스팟을 정찰(when으로 사용못함)
		xQC_EVENT_VISIT_SPOT,			///< 스팟을 방문(터치)(when으로 사용못함)
		xQC_EVENT_CLEAR_SPOT_CODE,		///< 특정스팟들을 클리어(when으로 사용못함)
		xQC_EVENT_CLEAR_SPOT_TYPE,		///< 스팟종류를 클리어(when으로 사용못함)
		xQC_EVENT_RECON_SPOT_TYPE,		///< 스팟종류를 정찰(when으로 사용못함)
		xQC_EVENT_VISIT_SPOT_TYPE,		///< 스팟종류를 방문(터치)(when으로 사용못함)
		xQC_EVENT_GET_HERO,				///< 특정 영웅을 얻음(when으로 사용못함)
		xQC_EVENT_KILL_HERO,			///< 영웅을 사살(이번버전에서 지원하지 않음.)(when으로 사용못함)
		xQC_EVENT_CLEAR_QUEST,			///< 특정 퀘스트를 얻음
		xQC_EVENT_DEFENSE,				///< 방어에 성공(아직안됨)(when으로 사용못함)
		xQC_EVENT_GET_RESOURCE,			///< 자원을 획득(특정시점 이후부터 카운팅)(when으로 사용못함)
		xQC_EVENT_HAVE_RESOURCE,		///< 자원을 보유(현재 보유한 자원양으로만)(현재 지원안함)(when으로 사용못함)
		xQC_EVENT_GET_ITEM,				///< 아이템을 획득( " )(when으로 사용못함)
		xQC_EVENT_HAVE_ITEM,			///< 아이템을 보유(when으로 사용못함)
		xQC_EVENT_CLEAR_STAGE,			///< 캠페인 스테이지를 클리어(가급적이면 사용하지 말것. 퀘스트를 줄이기 위해서 만든게 캠페인임)(when으로 사용못함)
		xQC_EVENT_UI_ACTION,			///< 특정 UI기능을 수행
		xQC_EVENT_END_SEQ,				///< 시퀀스가 끝남
		xQC_EVENT_GET_STAR,				///< 별점 획득.(when으로 사용못함)
		xQC_EVENT_HERO_LEVEL,			///< 아무영웅이나 x레벨에 도달(when으로 사용못함)
		xQC_EVENT_HERO_LEVEL_SQUAD,			///< 아무영웅이나 부대레벨 x에 도달(when으로 사용못함)
		xQC_EVENT_HERO_LEVEL_SKILL,			///< 아무영웅이나 스킬레벨 x에 도달(when으로 사용못함)
		xQC_EVENT_TRAIN_QUICK_COMPLETE,	///< 훈련 즉시 완료(when으로 사용못함)
		xQC_EVENT_RESEARCH_ABIL,				///< 특성 연구.(파라메터로 TIER단계 조건)(when으로 사용못함)
		xQC_EVENT_UNLOCK_UNIT,					///< 유닛 잠금 해제(when으로 사용못함)
		xQC_EVENT_HIRE_HERO,						///< 영웅영입(영웅숙소)(when으로 사용못함)
		xQC_EVENT_HIRE_HERO_NORMAL,		///< 일반영웅영입(영웅숙소)(when으로 사용못함)
		xQC_EVENT_HIRE_HERO_PREMIUM,		///< 고급영웅영입(영웅숙소)(when으로 사용못함)
		xQC_EVENT_SUMMON_HERO,					///< 영혼석으로 소환(when으로 사용못함)
		xQC_EVENT_PROMOTION_HERO,				///< 영웅 승급(파라메터로 등급 지정)(when으로 사용못함)
		xQC_EVENT_BUY_CASH,							///< 캐시 구입(when으로 사용못함)
		xQC_EVENT_BUY_ITEM,							///< 무기상점에서 아이템 구입(when으로 사용못함)
		xQC_EVENT_ATTACK_SPOT,					///< 특정 스팟을 공격시도.(when으로 사용못함)
		xQC_EVENT_CLEAR_GUILD_RAID,			///< 길드레이드를 완전히 클리어 한다.(when으로 사용못함)
		xQC_EVENT_MAX,
		// 어디서
		xQC_WHERE_SPOT = 100,		///< 특정스팟에서
		xQC_WHERE_SPOT_TYPE,		///< 특정스팟종류에서
		xQC_WHERE_AREA,				///< 특정지역에서
		xQC_WHERE_SPOT_CODE,		///< 특정스팟들에서(공통코드를 가진)
		// 어떻게
		xQC_HOW_LIMITED_SQUAD = 200,		///< 한정된 수의 부대로
		xQC_HOW_BAN_UNIT,					///< 특정유닛 사용금지
		xQC_HOW_BAN_ARM,					///< 특정병과 사용금지
		xQC_HOW_BAN_CLAN,					///< 특정클랜 사용금지
		xQC_HOW_BAN_TRIBE,					///< 특정종족 사용금지
		// enum값이 255를 넘지 않도록 할것.
	};
	// 배경오브젝트 타입
	enum xtBgObj {
		xBOT_NONE,
		xBOT_WATER_SHINE,		// 물반짝이
		xBOT_WATER_FALL,		// 폭포
		xBOT_WATER_STEAM,					// 수증기
		xBOT_LAVA_BOIL,			// 용암끓기
		xBOT_LAVA_STEAM,		// 용암증기
		xBOT_LAVA_STREAM,		// 용암증기
		xBOT_JEWEL_SHINE,		// 보석 반짝이
//		xBOT_SOUND,					// 사운드 객체
		xBOT_MAX,
	};

	/**
	 @brief 캐쉬로 지불가능한 요소들.
	*/
	enum xtPaymentRes {
		xPR_NONE,
		xPR_GOLD,
		xPR_RES,
		xPR_TIME,
		xPR_AP,
		xPR_ITEM,
		xPR_GEM,
		xPR_TRY_DAILY,		// 요일스팟 도전횟수
	};
	// 기타 enum들
	enum {
		xQUEST_COMPLETE = 1,
		xQUEST_NOT_COMPLETE = 0,
	};
	

};	// namespace XGAME

#endif