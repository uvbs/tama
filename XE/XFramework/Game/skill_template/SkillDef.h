#pragma once
#include <list>
using namespace std;
// 2011.3.2 성정우
// 이 파일은 게임프로젝트마다 복사본을 만들어 사용합니다.
// 추가하고 싶은 타입이 있을경우 enum타입에 추가하면 됩니다.
// define으로 하지 않은이유는 디버깅시에 enum은 타입을 바로 확인할수 있기 때문입니다.
// enum {} 에 타입추가가 가능하다면 이렇게 안해도 되는데, 추가방법 아시는분은 제보바랍니다. ㅠㅠ;
#define NAMESPACE_XSKILL_START	namespace XSKILL {
#define NAMESPACE_XSKILL_END	}

NAMESPACE_XSKILL_START
// 시전방식 타입
	enum xCastMethod { xNONE_CAST=0, 
							xACTIVE,	// ##액티브
							xPASSIVE, 
							xTOGGLE, 
							xSCRIPT, 
							xETC 
	};	
	// 시전대상 종류
	enum xtCastTarget {
						xCST_NONE,
						// 앞번호는 발동대상의 enum과 값을 맞출것. 같은 한글식별자를 쓰기 위함.
						xCST_SELF,		// 자신(시전자 자신)
						xCST_PARTY,			// 파티
						xCST_OTHER = 100,		// 타인
						xCST_OBJECT,		// 객체(자신을 포함한 아군적군 모두)
						xCST_GROUND,		// 바닥(특정오브젝트가 아닌 바닥좌표)
						xCST_GROUND_RADIUS_OBJ,	// 바닥반경개체
						xCST_TARGET_GROUND,	// 타겟바닥(타겟이 서있는 바닥좌표)
//						xCST_FAN,			// 부채꼴
//						xCST_LINE,			// 직선(일직선)
//						xCST_RADIUS,		// 반경(시전자를 중심으로),타겟반경개체로 대체됨
						xCST_TARGET_RADIUS_OBJ,	// 타겟반경개체(스크립터는 이걸 직접 쓰진 않는다. 타겟+시전범위만으로 같은효과를 낼수 있다.)
//						xCST_SURROUND,		// 주변(시전자는 제외)
						xCST_DEAD,			// 사망자
						xCST_ALL_TARGET,	// 전체
						//
						xCST_NEAREST_TARGET,	// 가장가까운타겟(정식 타겟에 올려도 될듯. D&D의 매직미사일 같은게 좋은예)
						xCST_LOWER_HP,		// 체력이낮은타겟
						xCST_HIGHER_HP,		// 체력이높은타겟
						xCST_MASS_AREA,		// 뭉쳐있는지역(적들이 모여있는좌표)
						xCST_MASS_OBJS,		// 뭉쳐있는객체(모여있는 적들)
						xCST_AREA,			// 지역
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
	enum xtTargetRange {
		xTR_NONE,
		xTR_ONE,		///< 단일타겟
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
	enum xtInvokeTarget { xIVT_NONE,
						// 앞번호는 시전대상의 enum과 값을 맞출것. 같은 한글식별자를 쓰기 위함.
						xIVT_SELF,	// 대상자(시전된 대상자신)
						xIVT_PARTY,			// 대상파티
//						xIVT_CASTED_AREA,	// 시전영역(시전영역안의 개체들,이게 직관적이긴 한데)
						xIVT_RADIUS = 200,	// 대상반경(시전대상(발동자)을 중심으로)
						xIVT_SURROUND,	// 대상주변(시전대상은 제외)
						xIVT_FAN,			// 부채꼴
						xIVT_LINE,			// 직선(일직선)
						xIVT_ATTACKER,		// 공격자, 시전대상을 때리는놈
						xIVT_DEFENDER,		// 피격자, 시전대상이 때리는놈
//						xIVT_DEAD,			// 사망자(전체 오브젝트중에 죽은넘)
						// XT3확장
						xIVT_ALL_AREA,		// 지역전체
	};
	
	enum xtUseType {
					xUST_NONE,
					xUST_TARGETTING,	// 타겟지정, 타겟을 직접 지정
					xUST_GROUND,	// 바닥지정, 바닥을 직접 지정
					xUST_IMMEDIATELY,	// 즉시사용(스킬버튼 누르면 바로사용)
	};
/*
	타겟: 단일타겟
	타겟반경개체: 캐스팅타겟을 타겟A를 중심으로 반경내 모든 타겟들로 바꿔준다.
	            시전대상을 타겟으로 하고 시전범위를 주면 자동으로 타겟반경개체로 된다.
	바닥반경개체: 파라메터는 좌표로 주지만 캐스팅타겟은 바닥이 아닌 반경내 개체들로 바꿔준다.
	            광역으로 타겟들에게 버프를 걸필요가 있을때 사용한다.
	바닥: 캐스팅 타겟이 바닥이 된다.
		 시전범위가 있다면 바닥중심광역이 된다.
		 지속시간이 있다면 바닥에 일정시간 놓이는 광역효과가 된다.
	타겟바닥: 타겟의 현재 좌표만 취한다.
	// 바닥과 바닥반경개체를 따로 두지 않으려면 지속시간을 그 스킬자체의 지속시간과
	// 발동효과의 지속시간으로 나누어야 한다.
*/
	/**
	 @brief 우호 필터
	 우리편이냐 적편이냐 중립이냐만 판단함.
	*/
	enum xtFriendshipFilt { xfNONESHIP=0, 
							xfALL_FRIENDSHIP=0xffffffff,	//모두,전체
							xfALLY=0x01,				// 아군,우군
							xfHOSTILE=0x02,				// 적군,적
							xfNEUTRALITY=0x04			// 중립,중립군
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
							xfHUMAN=0x01, 
							xfAI=0x02 
	};
	enum xtPlayerType { xNONE_PLAYER_TYPE=0, xHUMAN, xAI };	// 플레이어가 인간인가 AI인가
	// 발동조건(효과의 발동조건인가, SFX의 발동조건인가)
	enum xtCondition { xNONE_COND,	// 없음
						xDIE,		// 사망, (발동대상이 사망상태여야한다)
						xLIVE,		// 생존, (발동대상이 생존상태여야한다)
						xATTACK };	// 공격시(사용되지 않음)	
	// 발동시점
	enum xtJuncture { xJC_NONE,
						xJC_FIRST,		// 최초발동(최초한번만 효과가 발동됨)
						xJC_PERSIST,	// 지속발동(지속시간내내 효과가 발동됨)
						xJC_ATTACK,		// 공격시
						xJC_CLOSE_ATTACK,	// 근접공격시
						xJC_RANGE_ATTACK,	// 원거리공격시
//						xJC_HIT,		// 피격시(근접,원거리 모두)
						xJC_CLOSE_HIT,	// 근접피격시
//						xJC_RANGE_HIT,	// 원거리피격시
						xJC_DEFENSE,	// 방어시
						xJC_EVADE,		// 회피시
						xJC_DEAD,		// 사망시
						xJC_KILL_ENEMY,	// 적사살시
						xJC_LAST,		// 마지막, 지속시간의 마지막
						//////////////////////////////////////////////////////////////////////////
						xJC_ATTACK_COUNTER,	// 공격카운터
						xJC_DAMAGE_ACCU,	// 데미지누적
	};
#if 0
	// 발동파라메터(주의: ADJ가 없는 파라메터는 보정치가 아니다)
	// 이것도 예제템플릿만 제공해주고 사용자정의로 쓰는것이 나을듯.
	enum xtParameter {	xNONE_PARAM, 
						xADJ_ATTACK,		// 공격력보정
						xADJ_DEFENSE,		// 방어력보정
						xADJ_MAX_HP,		// 최대체력보정
						xADJ_HP,				// 체력보정
						xADJ_HP_REGEN,	// 체력리젠양보정
						xADJ_HP_REGEN_SEC,	// 체력리젠속도보정
						xADJ_MAX_MP,		// 최대마나보정
						xADJ_MP,				// 마나보정
						xADJ_MP_REGEN,	// 마나리젠양보정
						xADJ_MP_REGEN_SEC,	// 마나리젠속도보정
						xADJ_MP_REDUCE,	// 마나소모량보정
						xADJ_ACQUIRE_MANA,		// 마나획득량보정
						xADJ_MOVE_SPEED,	// 이속보정
						xADJ_ATTACK_SPEED,	// 공속보정
						xADJ_ATTACK_RANGE,	// 공격사거리보정
						xADJ_PHY_DAMAGE,		// 물리데미지보정
						xADJ_MAG_DAMAGE,		// 마법데미지보정
						xADJ_PHY_DAMAGE_RECV,		// 받는물리데미지보정
						xADJ_MAG_DAMAGE_RECV,	// 받는마법데미지보정
						xADJ_DAMAGE_RECV,		// 받는데미지보정
						xADJ_HEAL,				// 힐량보정
						xADJ_HEAL_RECV,			// 받는힐량보정
						xADJ_SKILL_POWER,		// 스킬파워보정
						xADJ_SIGHT,				// 시야보정
						xADJ_PENETRATION_RATE,	// 관통율보정
						xADJ_PENETRATION_PROB,	// 관통확률보정
						xADJ_EVADE_RATE,		// 회피율보정
						xADJ_BLOCKING_RATE,		// 완전방어율보정
						// TODO: 추가하려면 여기서부터 하시오.
						xADJ_RESIST_POISON,		// 중독내성보정
						xADJ_RESIST_ICE,		// 아이스내성보정
						xADJ_RESIST_KNOCKBACK,	// 넉백내성보정
						xADJ_RESIST_CHAOS,		// 혼란내성보정
						xADJ_RESIST_STUN,		// 스턴내성보정
						xADJ_RESIST_FIRE,		// 화염내성보정
						xADJ_RESIST_NERF,		// 너프내성보정
						xADJ_RESIST_ALL,		// 모든내성보정
						xADJ_RECV_SP,			// SP획득보정
						xADJ_LUCKY,				// 레어획득보정
						xADJ_SKILL_DAMAGE,		// 스킬데미지보정(데미지류스킬만...)
						xADJ_ATTACK_COUNTER,	// 공격카운터보정
						xADJ_GET_NUM_ITEM_STAT_UP,	// 획득아이템수스탯보정
						xADJ_NUM_MEMBER_STAT_UP,	// 아군수스탯보정
						// 이곳까지 추가 하시오
						xMAX_PARAM, 
						// TODO: 보정치가 아닌 파라메터는 이곳에 추가하시오
						xHP,				// 체력(주의:보정아님)
						xMP,				// 마나(주의:보정아님)
						xVAMPIRIC_HP,				// 체력흡혈
						xVAMPIRIC_MP,			// 마나흡혈
	};
#endif	
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
		xPT_TARGET_BOTTOM,	// 타겟바닥(타겟의 발밑)
		xPT_TARGET_TOP,		// 타겟머리(타겟의 머리위)
		xPT_TARGET_CENTER,	// 타겟중심
		xPT_TARGET_POS,		// 타겟좌표(타겟의 좌표땅바닥에 생성.타겟을 따라다니지 않음)
		xPT_TARGET_POS_CENTER,	// 타겟중심좌표(타겟의 중심좌표에 생성.타겟을 따라다니지 않음)
		xPT_ACTION_EVENT,	// 액션이벤트(애니메이션 이벤트 지점)
		xPT_WINDOW_CENTER,	// 윈도우 중앙
		xPT_WINDOW_CENTER_BOTTOM,	// 윈도우중앙하단
	};
	// 대상생존
	enum xtTargetLive {
		xTL_NONE,
		xTL_LIVE,	// 생존자, 생존
		xTL_DEAD,	// 사망자, 사망
	};
	enum xtTargetFilter {
		xTF_NONE = 0,
		xTF_LIVE = 0x0001,		// 생존자만
		xTF_DEAD = 0x0002,		// 사망자만
	};
	//////////////////////////////////////////////////////////////////////////
	// XT3확장
	// XT3 확장
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
				};

//	#define XMAX_SPR_NAME		64			// spr파일명 길이
	/**
	 @brief 스킬효과
	 @todo 각 프로젝트별 확장효과는 struct상속으로 구현해볼것.
	*/
	struct EFFECT
	{
		// 타겟
// 		xtTargetRange		targetRange;		///< 타겟범위
// 		xtFriendshipFilt	targetFriendship;	///< 타겟우호
// 		xtEffectPosition	effectPosition;		///< 효과위치
		_tstring			strTargetEffect;	// 타겟이펙트, 기준타겟에 생성되는 이펙트
		ID					idTargetEffect;		// 타겟이펙트id
		xtAniLoop			targetEffectPlayMode;		// 타겟이펙트반복
		xtPoint				targetEffectPoint;		// 타겟이펙트생성지점
		// 시전
		xtCastTarget		castTarget;			// 시전대상-자신,타겟,지역,반경내,부채꼴,일직선,주변,파티,유닛전체,장군
		xtTargetRange		castTargetRange;	// 시전범위
		xtTargetCond		castTargetCond;		// 시전조건
		float				castRange;			// 시전거리(원형일때는 반지름으로 사용)
		float				castWidth;			// 시전폭
		xtFriendshipFilt	castfiltFriendship;		// 시전대상우호-모두,아군,적군,중립군
		xtPlayerTypeFilt	castfiltPlayerType;	// 시전대상플레이어-인간/AI
		float				castTime;				// 시전시간(캐스팅시간)
		_tstring			strCasterEffect;		// 시전자이펙트
		ID					idCasterEffect;			// 시전자이펙트id
		xtAniLoop			casterEffectPlayMode;		// 루핑같은거
		xtPoint				casterEffectPoint;		// 시전자이펙트생성지점
		_tstring			strCastTargetEffect;			// 시전대상이펙트, 시전대상에 하나씩 이펙트가 생긴다.
		ID					idCastTargetEffect;			// 시전대상이펙트id(spr로 쓸땐 액션번호로, 이펙트자체를 아이디타입으로 쓸땐 이펙트id로)
		xtAniLoop			castTargetEffectPlayMode;		// 루핑같은거
		xtPoint				castTargetEffectPoint;		// 시전대상이펙트생성지점
//		float				castRadius;				// 시전반경(바닥을 타겟으로할땐 시전범위를 사용하지 않는다.)
//		XE::VEC2			castSize;				// 시전길이/시전폭(직선일경우)
		float				secDuration;			// 버프의(지속시간, 0:즉시1회성  -1(무한대), 
		ID					idCastSound;			// 시전사운드
		// 발동
		xtInvokeTarget		invokeTarget;		// 발동대상-시전대상과 같음. 발동대상이 광역인데 지속시간까지 있는경우는 "발동스킬"을 써서 스킬하나를 더 만들것
		xtFriendshipFilt	invokefiltFriendship;	// 발동대상우호
		xtPlayerTypeFilt	invokefiltPlayerType;	// 발동대상플레이어-인간/AI
		xtJuncture			invokeJuncture;			// 발동시점
		xtCondition			invokeCondition;	// 발동조건-없음,공격시 
		ID					idInvokeSkill;			// 발동스킬id, 아이디
		_tstring			strInvokeSkill;			// 발동스킬, 식별자
		float				invokeRatio;			// 발동확률
//		TCHAR				szInvokeSkill[XMAX_SPR_NAME];		// 발동스킬, 이름(스트링으로도 스크립팅 할수 있게)
		int					invokeParameter;	// 발동파라메터
		///< @todo 파라메터는 1개만 하고 만약 최소/최대 보정이 필요하다면 각각 EFFECT를 따로 할당하도록 하는게 나을듯..
		float				invokeAbilityMin;	// 능력치(최소). 
		float				invokeAbilityMax;	// 능력치(최대)
		xtValType			valtypeInvokeAbility;	// #%! 능력치값 연산타입
		int					invokeState;		// 상태발동
		float				secState;			// 상태시간(상태가 걸리면 얼마나 지속되는가)
		union {
			float	invokeStateParam[4];		// 상태변수1234
			// XT3확장
			struct {
				float	secIceTime;
				float	secFreezeTime;	
				float	reservedIce[2];
			};
			struct {
				float	meterKnockback;
			};
		};
		float				invokeRadius;		// 발동반경
		XE::VEC2			invokeSize;			// 발동길이/발동폭(직선일경우)
		float				secInvokeDOT;			// 발동간격(DOT뎀)
		int					invokeNumApply;		// 적용대상수, 시전/발동효과가 적용될 개체수(0은 제한없음)
		_tstring			strInvokeEffect;	// 발동이펙트
		ID					idInvokeEffect;		// 발동이펙트id
		xtAniLoop			invokeEffectPlayMode;		// 발동이펙트반복(루핑정보)
		xtPoint				invokeEffectPoint;		// 발동이펙트생성지점
		ID					idInvokeSound;			// 발동사운드
		//
		BOOL				bDuplicate;			// 중복가능, 같은 버프/디버프가 두개이상 걸릴수 있는가. 물론 자기가 두개거는건 무조건 안되고 여러사람이 한타겟에 같은 스킬을 쓸때의 가능여부다
		int					numOverlap;			// 버프중첩, 버프가 몇개까지 쌓일수 있는가. 0이면 쌓이지않음. 디폴트는 1 지속시간이 있을때 쓰인다
		xtTargetLive		liveTarget;			// 대상생존, 대상이 살아있어야 하는가 죽어있어야 하는가.
		// 발사체
		_tstring			strShootObj;				// 발사체
		ID							idShootObj;			// 발사체id
		float						shootObjSpeed;		// 발사체속도
		_tstring			strCreateObj;				// 소환
		ID					idCreateObj;
		float				createObjParam[4];
		string				scriptUse;				// 사용
		string				scriptCast;		// "대상시전"스크립트
		string				scriptInit;			// "발동시작"스크립트
		string				scriptProcess;	// "발동중"
		string				scriptDOT;		// "도트"
		string				scriptUninit;		// "발동끝"
		BOOL				bImmunity;			// 면역여부, 발동스킬의 면역가능여부
		//////////////////////////////////////////////////////////////////////////
		// XT3확장
		// 스트럭트 상속을 사용해볼것.
//		XList<XMW::xtJob>	listInvokeJobFilter;	// 발동대상직업, 발동효과가 적용될 직업들
//		float				damageAccu;				// 데미지누적량 (버프가 걸린시점부터)
//		int					levelRequire;			// 필요레벨
		EFFECT() {
			idTargetEffect = 0;
			castTarget = xCST_SELF;
			castTargetRange = xTR_ONE;
			castTargetCond = xTC_NONE;
			castfiltFriendship = xfALL_FRIENDSHIP;
			castfiltPlayerType = xfALL_PLAYER_TYPE;
			castRange = 0;
			castWidth = 1.f;
			castTime = 0;
			idCasterEffect = 0;
			idCastTargetEffect = 0;
			casterEffectPoint = xPT_TARGET_BOTTOM;
			casterEffectPlayMode = xAL_NONE;
			castTargetEffectPlayMode = xAL_NONE;
			castTargetEffectPoint = xPT_NONE;
			secDuration = 0;
			idCastSound = 0;
			invokeTarget = xIVT_SELF;
			invokefiltFriendship = xfNONESHIP;	// 따로지정하지 않으면 시전대상우호값으로 사툥하도록 바뀜
			invokefiltPlayerType = xfALL_PLAYER_TYPE;
			invokeJuncture = xJC_FIRST;
			invokeCondition = xNONE_COND;
			idInvokeSkill = 0;
			invokeRatio = 1.0f;
			invokeParameter = 0;
			invokeAbilityMin = invokeAbilityMax = 0;
			valtypeInvokeAbility = xVAL;
			invokeState = 0;
			secState = 1.0f;
			XCLEAR_ARRAY( invokeStateParam );
			invokeRadius = 0;
			invokeSize = XE::VEC2(10, 1);		// 디폴트 10미터, 폭1미터
			secInvokeDOT = 0;
			invokeNumApply = 0;
			idInvokeEffect = 0;
			invokeEffectPlayMode = xAL_NONE;
			invokeEffectPoint = xPT_NONE;
			idInvokeSound = 0;
			bDuplicate = FALSE;
			numOverlap = 1;		// 기본적으로 1개는 걸린다
			liveTarget = xTL_LIVE;
			idShootObj = 0;
			shootObjSpeed = 0.6f;
			idCreateObj = 0;
			XCLEAR_ARRAY( createObjParam );
			bImmunity = FALSE;
			//////////////////////////////////////////////////////////////////////////
//			invokeCondMember = XMW::JOB_NONE;
//			damageAccu = 0;
//			levelRequire = 1;
		}
		~EFFECT() {}
	};
	typedef std::list<EFFECT *>				EFFECT_LIST;
	typedef std::list<EFFECT *>::iterator	EFFECT_ITOR;

NAMESPACE_XSKILL_END

