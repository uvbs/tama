#pragma once
#include <list>
//using namespace std;
// 2011.3.2 성정우
// 이 파일은 게임프로젝트마다 복사본을 만들어 사용합니다.
// 추가하고 싶은 타입이 있을경우 enum타입에 추가하면 됩니다.
// define으로 하지 않은이유는 디버깅시에 enum은 타입을 바로 확인할수 있기 때문입니다.
// enum {} 에 타입추가가 가능하다면 이렇게 안해도 되는데, 추가방법 아시는분은 제보바랍니다. ㅠㅠ;
// #define XE_NAMESPACE_START( XSKILL )	namespace XSKILL {
// #define XE_NAMESPACE_END	}

XE_NAMESPACE_START( XSKILL )
// 시전방식 타입
	enum xCastMethod { xNONE_CAST=0, 
							xACTIVE,	// ##액티브
							xPASSIVE,	///< 
							xABILITY,	///< 특성. 특성은 모두 패시브.
							xTOGGLE, 
							xSCRIPT, 
							xETC 
	};	
	/// 기준타겟 파라메터
	enum xtBaseTarget {
		xBST_NONE,
		xBST_SELF,			///< 자신
		xBST_CURR_TARGET,	///< 현재타겟
		xBST_POSITION,		///< 좌표
		xBST_CONDITION,		///< 조건
	};
	enum xtBaseTargetCond {
		xBTC_NONE,
		xBTC_LEAST_HP_PARTY,		///< 체력이가장적은부대
		xBTC_HIGHEST_HP_PARTY,		///< 체력이가장많은부대
		xBTC_CURR_TARGET_LEADER,	///< 기준타겟리더
		xBTC_RANDOM = 99,				///< 랜덤파티
	};
	// 시전시점 (스킬버튼(혹은AI)을 눌렀을때 언제 스킬을 Use하는지)
	enum xtWhenUse {
		xWC_NONE,	
		xWC_EVENT_IMMEDIATELY,			// (이벤트:스킬버튼)즉시
		xWC_BASE_TARGET_NEAR,	// 기준타겟근접(현재 기준타겟은 자신아니면 공격타겟이므로 공격타겟근접과 같다.)
		xWC_ATTACK_TARGET_NEAR,	// 공격타겟근접

	};
	// 시전대상 종류
	enum xtCastTarget {
						xCST_NONE,
						// 앞번호는 발동대상의 enum과 값을 맞출것. 같은 한글식별자를 쓰기 위함.
						xCST_BASE_TARGET,	///< 기준타겟
						xCST_BASE_TARGET_RADIUS, ///< 기준타겟반경
						xCST_BASE_TARGET_SURROUND, ///< 기준타겟주변
						xCST_BASE_TARGET_PARTY,	///< 기준타겟파티
						xCST_BASE_TARGET_POS,	///< 기준타겟좌표
	};
	///< 타겟방법(트리거가 발동되는 순간 타겟을 지정하는 방법)
	enum xtTargetMethod {
		xTM_NONE,
		xTM_AUTO,	///< 트리거가 발동되는 순간 사용자조건에 의해 자동으로 타게팅 된다.
		xTM_SELF,	///< 트리거가 발동되면 오직 자신에게만 타게팅 된다
		xTM_MANUAL,	///< 트리거가 발동되면 대상을 수동으로 지정해야 한다.
	};
	///< 타겟조건
	enum xtTargetCond {
		xTC_NONE,
		xTC_NEAREST,	///< 가장가까운
	};
	///< 타겟종류(타겟은 어떤 종류여야 하는가)
	enum xtTargetType {
		xTT_NONE,
		xTT_OBJECT,		///< 객체
		xTT_GROUND,		///< 바닥
	};
	///< 적용범위 타입(타겟지정 후 어떤 범위로 적용을 시킬것인가)
	enum xtTargetRangeType {
		xTR_NONE,
		xTR_ONE,		///< 단일
		xTR_CIRCLE,		///< 원형
		xTR_LINE,		///< 라인
		xTR_FAN,		///< 부채꼴
		xTR_RECT,		///< 사각형
		xTR_GROUP,		///< 그룹(파티같은)
	};
	// 효과위치
	enum xtEffectPosition {
		xEP_NONE,
		xEP_TARGET,		///< 타겟
		xEP_TARGET_POS,	///< 타겟지점
	};
	///< 적

	// 발동대상
	enum xtInvokeTarget {
		xIVT_NONE,
		// 앞번호는 시전대상의 enum과 값을 맞출것. 같은 한글식별자를 쓰기 위함.
		xIVT_CAST_TARGET,	///< 시전대상
		xIVT_CAST_TARGET_RADIUS,	///< 시전대상반경(시전대상자를 포함)
		xIVT_CAST_TARGET_SURROUND,	///< 시전대상주변(시전대상자를 포함하지 않음)
		xIVT_CAST_TARGET_PARTY,	///< 시전대상파티
		xIVT_CAST_TARGET_POS,	///< 시전대상좌표
		xIVT_CAST_POS_RADIUS,	///< 시전좌표반경
		xIVT_ATTACKER,			///< 타격자
		xIVT_ATTACKED_TARGET,	///< 피격자
		xIVT_ATTACKED_TARGET_RADIUS,	///< 피격자반경
		xIVT_ATTACKED_TARGET_SURROUND,	///< 피격자주변
		xIVT_ATTACKED_TARGET_PARTY,	///< 피격자파티
		xIVT_CURR_TARGET,			///< 현재공격타겟(공격타겟)(시전자의)
		xIVT_CURR_TARGET_PARTY,		///< 현재공격타겟파티
		xIVT_CURR_TARGET_POS,		///< 현재타겟좌표
		xIVT_ALL = 0xffffffff,				///< 모두(아군,적 포함. 발동대상우호로 필터링)
		xIVT_RANDOM_PARTY = 99,			///< 랜덤파티
	};
	enum xtUseType {
		xUST_NONE,
		xUST_TARGETTING,	// 타겟지정, 타겟을 직접 지정
		xUST_GROUND,	// 바닥지정, 바닥을 직접 지정
		xUST_IMMEDIATELY,	// 즉시사용(스킬버튼 누르면 바로사용)
	};
	/**
	 @brief 우호 필터
	 우리편이냐 적편이냐 중립이냐만 판단함.
	*/
	enum xtFriendshipFilt {
		xfNONESHIP = 0,
		xfALL_FRIENDSHIP = 0xffffffff,	//모두,전체
		xfALLY = 0x01,				// 아군,우군
		xfHOSTILE = 0x02,				// 적군,적
		xfNEUTRALITY = 0x04			// 중립,중립군
	};

/*
	// 어느진영인가. 
	이제 진영은 XECompCamp를 사용자 정의해서 쓰도록 바뀜.
	enum xtSide { xSIDE_NONE=0, 
				xSIDE_FRIEND = 0x01, 
				xSIDE_ENEMY = 0x02, 
				xSIDE_NEUTRALITY = 0x04, 
				xSIDE_ALL = 0xffffffff,
	상속받는 하위클래스에서 비트로 정의해서 쓰도록 바꼇다. 진영이 몇개나 나올지 알수없기때문.
	};		
	*/
	enum xtPlayerTypeFilt { xfNONE_PLAYER_TYPE=0, 
							xfALL_PLAYER_TYPE=0xffffffff, 
							xfHUMAN=0x01,			// "인간"이어야 하나 다른곳에서도 이 단어로 상수를 등록하는일이 많아서 원래 이름그대로 xfHUMAN이라고 쓰는걸로 일단 땜빵.
							xfAI=0x02 
	};
	enum xtPlayerType { xNONE_PLAYER_TYPE=0, xHUMAN, xAI };	// 플레이어가 인간인가 AI인가
	///< 발동대상조건. 발동대상에게 효과를 적용할 조건
	enum xtCondition {
		xNONE_COND,	// 없음
		xDIE,		// 사망, (발동대상이 사망상태여야한다)
		xLIVE,		// 생존, (발동대상이 생존상태여야한다)
		xATTACK,	// 공격시(사용되지 않음)	
		xATTACK_TARGET_JOB,	///< 공격대상직업/직업
		xATTACK_TARGET_SIZE,	///< 공격대상크기/크기
		xATTACK_TARGET_TRIBE,	///< 공격대상종족/종족
		xTARGET_ARM,			///< 병과
		xTARGET_RANK,			///< 계급

		xCOND_HARD_CODE = 99,	///< 하드코딩
		xCOND_ALWAYS = 0,		///< 항상
	};
	// 발동시점
	enum xtJuncture : int { 
		xJC_NONE,
		xJC_FIRST,		///< 최초(최초한번만 효과가 발동됨)
		xJC_PERSIST,	///< 지속발동(지속시간내내 매 프레임 효과가 발동됨)
		xJC_HIT_POINT,	///< 슈팅타겟이펙트타점
		xJC_ATTACK,		///< 모든공격시
		xJC_CLOSE_ATTACK,	///< 근접공격시
		xJC_RANGE_ATTACK_START,	// 원거리공격시작시(평타에 발동. 평타화살 발사와 관계없이 별도로 효과가 발동된다.)
		xJC_RANGE_ATTACK_ARRIVE,	// 원거리타격시
		xJC_RANGE_ATK_NORMAL_REPLACEMENT,	// 원거리평타대체(평타에 발동되는건 같으나 효과발동에 성공하면 평타는 나가지 않는다.)
		xJC_ALL_HIT,	// 모든피격시
		xJC_CLOSE_HIT,	// 근접피격시
		xJC_RANGE_HIT,	// 원거리피격시
		xJC_DEFENSE,	// 방어시
		xJC_EVADE,		// 회피시
		xJC_DEAD,		// 사망시
		xJC_KILL_ENEMY,	// 적사살시
		xJC_ARRIVE_TARGET, ///< 목표타겟에 도착시
		xJC_LAST,		// 마지막, 지속시간의 마지막
		xJC_INVOKE_SKILL,	// 스킬발동시(지정된 스킬이 발동되면 이 스킬도 함께 발동된다)
		xJC_HP_UNDER,		// 체력이하
		//////////////////////////////////////////////////////////////////////////
		xJC_START,			// 패시브 발동시킬 타이밍땜에 임시로 만든건데 xJC_FIRST와 겹친다.
		xJC_START_BATTLE,	///< 전투시작시
		xJC_ATTACK_COUNTER,	// 공격카운터
		xJC_DAMAGE_ACCU,	// 피해누적
	};
	// 능력치 연산타입
	enum xtValType { xNONE_VALTYPE=0, 
						xPERCENT, // 퍼센트(비율)
						xVAL,		// 즉치값
						xFIXED_VAL };		// 고정값
	enum xtAniLoop { xAL_NONE, 
					xAL_LOOP,	// 무한반복
					xAL_ONCE,	//한번만 플레이
	};
	enum xtPoint {
		xPT_NONE,
		xPT_TARGET_POS,		// 타겟좌표. 지속시간이 있을경우 목표를 따라다님
		xPT_TARGET_TRACE_CENTER,	// 타겟중심추적. 타겟의 중심에 따라다닌다. 지속시간과 관계없이 무조건 따라다닌다.
		xPT_TARGET_TRACE_POS,			// 타겟좌표추적. 타겟의 좌표를 따라다닌다. 지속시간과 관계없이 무조건 따라다닌다.
		xPT_TARGET_BOTTOM,	// 타겟바닥(완전히 지면에 깔리는류(ex:장판)의 에 사용)
		xPT_TARGET_TRACE_BOTTOM,		// 타겟바닥추적(타겟바닥의 추적버전)
		xPT_TARGET_TOP,		// 타겟머리(타겟의 머리위)
		xPT_TARGET_TRACE_TOP,		// 타겟머리추적(타겟의 머리위)
		xPT_TARGET_CENTER,	// 타겟중심
		xPT_ACTION_EVENT,	// 액션이벤트(애니메이션 이벤트 지점)
		xPT_WINDOW_CENTER,	// 윈도우 중앙
		xPT_WINDOW_CENTER_BOTTOM,	// 윈도우중앙하단
	};
	// 대상생존
	enum xtTargetLive {
		xTL_NONE,
		xTL_LIVE,	// 생존자, 생존
		xTL_DEAD,	// 사망자, 사망
		xTL_ALL=0xffffffff,	// 모두
	};
	enum xtTargetFilter {
		xTF_NONE = 0,
		xTF_LIVE = 0x0001,		// 생존자만
		xTF_DEAD = 0x0002,		// 사망자만
		xTF_ALL = 0x0003,	// 모두(생존/사망)
		xTF_DIFF_SQUAD = 0x0004,	// 각기 다른 부대의 타겟으로
	};
	// 피해타입
	enum xtDamage {
		xDMG_NONE,	// 초기값. 속성없음.
		xDMG_MELEE,		// 근접피해
		xDMG_RANGE,		// 원거리피해(활류)
		xDMG_MAGIC,		// 마법피해(법사나 힐러가 공격하는 속성. 혹은 마법스킬)
	};
	// 효과의 속성
	enum xtEffectAttr {
		xEA_NONE,
		xEA_ABILITY,		///< 능력치
		xEA_CAST_RADIUS,	///< 시전반경
	};
	// 그룹 타입
	enum xtGroup {
		xGT_NONE,
		xGT_ME,							// 나자신의그룹 멤버
		xGT_TARGET_PARTY,				// 공격타겟의 그룹멤버
		xGT_RANDOM_PARTY_FRIENDLY,		// 아군 랜덤파티 멤버
		xGT_RANDOM_PARTY_ENEMY,			// 적군 랜덤파티 멤버
		xGT_FRIENDLY_ALL,				// 아군 부대원 모두.
		xGT_ENEMY_ALL,				// 적 군 부대원 모두.
	};
	//////////////////////////////////////////////////////////////////////////
	// caribe 확장
	//////////////////////////////////////////////////////////////////////////
	// 에러메시지
	enum xtError {	xOK=1,
					xERR_OK=1,
					xERR_MUST_SELECT_TARGET=-1,		// 타겟을 선택해야 합니다
					xERR_MUST_MAKE_AREA=-2,			// 영역을 지정해야 합니다
					xERR_INVALID_CAST_TARGET=-3,		// 잘못된 시전대상
					xERR_INVALID_INVOKE_TARGET=-4,	// 잘못된 발동대상
					xERR_READY_COOLTIME=-5,			// 쿨타임 대기중입니다
					xERR_ALREADY_APPLY_SKILL=-6,		// 이미 적용된 스킬입니다
					xCANCEL=-7,						// 그냥 조용히 취소됨. 에러메시지 처리 하지 말것
					xERR_MUST_CAST_SELF=-8,			// 자신에게 써야합니다
					xERR_MUSTNOT_CAST_SELF=-9,		// 자신에게는 사용할수 없습니다
					xERR_MUST_CAST_FRIENDLY=-10,	// 아군에게 써야 합니다
					xERR_MUST_CAST_ENEMY=-11,		// 적에게만 써야합니다
					xERR_MUST_CAST_NEUTRALITY=-12,	// 중립에게만 써야합니다
					xERR_MUSTNOT_CAST_FRIENDLY=-13,	// 아군에게는 사용할수 없습니다.
					xERR_MUSTNOT_CAST_ENEMY=-14,	// 적에게는 사용할수 없습니다.
					xERR_MUSTNOT_CAST_NEUTRALITY=-15,	// 중립에게는 사용할수 없습니다.
					xERR_NOT_FOUND_CASTING_TARGET=-16,	// 시전대상을 찾지 못함
					xERR_NOT_FOUND_INVOKE_TARGET=-17,	// 발동대상을 찾지 못함
					xERR_CRITICAL_ERROR=-18,				// 여러가지 치명적 에러(포인터가 널 이런거)
					xERR_INVALID_CALL=-19,				// 잘못된 함수 호출
					xERR_GENERAL=-99					///< 그외 에러
				};

//	#define XMAX_SPR_NAME		64			// spr파일명 길이
	struct xCOND_PARAM {
		xtCondition cond = xNONE_COND;	// 조건파라메터
		DWORD val = 0;			// 값
		void Serialize( XArchive& ar ) const {
			ar << (int)cond << val;
		}
		void DeSerialize( XArchive& ar, int ) {
			int i0;
			ar >> i0;		cond = (xtCondition)i0;
			ar >> val;
		}
	};
	struct xCOND {
		XArrayLinearN<xCOND_PARAM, 4> aryParam;		// and파라메터 최대 n개까지 사용가능.
		void Serialize( XArchive& ar ) const {
			ar << aryParam;
		}
		void DeSerialize( XArchive& ar, int ) {
			ar >> aryParam;
		}
	};


struct xEffSfx {
	_tstring m_strSpr;		// 이펙트 스프라이트
	ID m_idAct = 1;				// 스프라이트의 액션아이디
	xtPoint m_Point = xPT_TARGET_POS;		// 이펙트 생성위치
	xtAniLoop m_Loop = xAL_ONCE;
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
	inline bool IsEmpty() const {
		return m_strSpr.empty();
	}
	inline bool IsHave() const {
		return !m_strSpr.empty();
	}
};

// 발사체이동
enum xtMoving {
	xMT_NONE,
	xMT_STRAIGHT,		// 직선
	xMT_ARC,				// 포물선
};

XE_NAMESPACE_END

