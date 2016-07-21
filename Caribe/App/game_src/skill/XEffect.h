﻿#pragma once
#include <list>
#include "SkillDef.h"
#include "Sprite/Sprdef.h"
// #define XE_NAMESPACE_START( XSKILL )	namespace XSKILL {
// #define XE_NAMESPACE_END	}

XE_NAMESPACE_START( XSKILL )
/**
	@brief 스킬효과
	@todo 각 프로젝트별 확장효과는 struct상속으로 구현해볼것.
*/
struct EFFECT	{
	ID m_snEffect = 0;
	// 시전
	xtCastTarget		castTarget;			// 시전대상-자신,타겟,지역,반경내,부채꼴,일직선,주변,파티,유닛전체,장군
	xtTargetRangeType	castTargetRange;	// 시전범위
	xtTargetCond		castTargetCond;		// 시전조건
	xtFriendshipFilt	castfiltFriendship;		// 시전대상우호-모두,아군,적군,중립군
	xtPlayerTypeFilt	castfiltPlayerType;	// 시전대상플레이어-인간/AI
	xEffSfx m_CasterEff;							// 시전자 이펙트
// 	_tstring			m_CasterEff.m_strSpr;		// 시전자이펙트
// 	ID					m_CasterEff.m_idAct;			// 시전자이펙트id
// 	xtPoint				m_CasterEff.m_Point;		// 시전자이펙트생성지점
	xEffSfx m_CastTargetEff;						// 시전대상이펙트, 시전대상에 하나씩 이펙트가 생긴다.
// 	_tstring			m_CastTargetEff.m_strSpr;			// 시전대상이펙트, 시전대상에 하나씩 이펙트가 생긴다.
// 	ID					m_CastTargetEff.m_idAct;			// 시전대상이펙트id(spr로 쓸땐 액션번호로, 이펙트자체를 아이디타입으로 쓸땐 이펙트id로)
// 	xtPoint				m_CastTargetEff.m_Point;		// 시전대상이펙트생성지점
	XE::VEC2			castSize;				// 시전범위(반지름)/시전길이/시전폭
	XArrayLinearN<float, XGAME::MAX_SKILL_LEVEL> arySecDuration;	// (시전)지속시간
	xEffSfx m_PersistEff;					// 지속이펙트
// 	_tstring			m_PersistEff.m_strSpr;		// 지속이펙트
// 	ID					m_PersistEff.m_idAct;			// 지속이펙트id
// 	xtPoint				m_PersistEff.m_Point;		// 지속이펙트생성지점
	ID					idCastSound;			// 시전사운드
	// 발동
	xtInvokeTarget		invokeTarget;		// 발동대상-시전대상과 같음. 발동대상이 광역인데 지속시간까지 있는경우는 "발동스킬"을 써서 스킬하나를 더 만들것
	xtFriendshipFilt	invokefiltFriendship;	// 발동대상우호
	xtPlayerTypeFilt	invokefiltPlayerType;	// 발동대상플레이어-인간/AI
	xtJuncture			invokeJuncture;			// 발동시점
	bool IsInvokeByHit() const {
		// 이 이펙트는 맞았을때 발동되는 류이다.
		return invokeJuncture == xJC_CLOSE_HIT || invokeJuncture == xJC_ALL_HIT || invokeJuncture == xJC_RANGE_HIT;
	}
	xtCondition			invokeTargetCondition;	// 발동대상조건-없음,공격시 
	XArrayLinearN<xCOND,4> aryInvokeCondition;		// 발동조건(발동대상조건의 확장판) 배열만큼 or조건을 쓸수 있다.
	float				secDurationInvoke = 0;		// 발동지속시간
	ID					idInvokeSkill;			// 발동스킬id, 아이디
	_tstring			strInvokeSkill;			// 발동스킬, 식별자
	_tstring			strInvokeTimeSkill;		///< 발동시점스킬(여기에 지정된 스킬이 발동되면 이스킬이 함께 발동된다)
	_tstring			strInvokeIfHaveBuff;	///< 발동조건스킬(여기에 스킬(버프)이 지정되어있다면 이 버프를 갖고 있을때만 이스킬이 발동된다)
	XArrayLinearN<float, XGAME::MAX_SKILL_LEVEL> aryInvokeRatio;	// 발동확률
	XVector<float> m_aryInvokeApplyRatio;		// 발동적용확률
	int					invokeParameter;	// 발동파라메터
	///< @todo 파라메터는 1개만 하고 만약 최소/최대 보정이 필요하다면 각각 EFFECT를 따로 할당하도록 하는게 나을듯..
	XArrayLinearN<float, XGAME::MAX_SKILL_LEVEL> invokeAbilityMin;	// 능력치(최소). 
	xtValType			valtypeInvokeAbility;	// #%! 능력치값 연산타입
	int					invokeState;		// 상태발동
	XE::VEC2			_invokeSize;			// 발동길이/발동폭(직선일경우)
	XArrayLinearN<float, XGAME::MAX_SKILL_LEVEL> aryInvokeSize;		// 발동범위
	float				secInvokeDOT;			// 발동주기(DOT뎀)
	int					invokeNumApply;		// 적용대상수, 시전/발동효과가 적용될 개체수(0은 제한없음)
	xEffSfx m_invokerEff;					// 발동자이펙트
	xEffSfx m_invokeTargetEff;		// 발동대상이펙트
// 	_tstring			m_invokeTargetEff.m_strSpr;	// 발동이펙트
// 	ID					m_invokeTargetEff.m_idAct;		// 발동이펙트id
// 	xtPoint				m_invokeTargetEff.m_Point;		// 발동이펙트생성지점
	ID					idInvokeSound;			// 발동사운드
	//
	BOOL				bDuplicate;			// 중복가능, 같은 버프/디버프가 두개이상 걸릴수 있는가. 물론 자기가 두개거는건 무조건 안되고 여러사람이 한타겟에 같은 스킬을 쓸때의 가능여부다
	int					numOverlap;			// 버프중첩, 버프가 몇개까지 쌓일수 있는가. 0이면 쌓이지않음. 디폴트는 1 지속시간이 있을때 쓰인다
	xtTargetLive		liveTarget;			// 대상생존, 대상이 살아있어야 하는가 죽어있어야 하는가.
	xtEffectAttr		attrAmplify = xEA_NONE;	///< 증폭파라메터
	// 발사체
	_tstring			strCreateObj;				// 소환
	ID					idCreateObj;
	float				createObjParam[4];
	std::string				scriptUse;				// 사용
	std::string				scriptCast;		// "대상시전"스크립트
	std::string				scriptInit;			// "발동시작"스크립트
	std::string				scriptProcess;	// "발동중"
	std::string				scriptDOT;		// "도트"
	std::string				scriptUninit;		// "발동끝"
	BOOL				bImmunity;			// 면역여부, 발동스킬의 면역가능여부
	float				invokeAddAbility;	// 추가능력치
	ID					idAddAbilityToClass;		// 추가능력치직업
	ID					idAddAbilityToTribe;		// 추가능력치종족
	DWORD dwParam[4];								///< 다용도 파라메터
	int m_Debug = 0;
	//////////////////////////////////////////////////////////////////////////
	// Caribe추가

	EFFECT() {
		m_snEffect = XE::GenerateID();
		castTarget = xCST_BASE_TARGET;
		castTargetRange = xTR_ONE;
		castTargetCond = xTC_NONE;
		castfiltFriendship = xfALL_FRIENDSHIP;
		castfiltPlayerType = xfALL_PLAYER_TYPE;
// 		m_CasterEff.m_idAct = 0;
// 		m_CastTargetEff.m_idAct = 0;
// 		m_CasterEff.m_Point = xPT_TARGET_POS;
//			casterEffectPlayMode = xAL_NONE;
//			castTargetEffectPlayMode = xAL_NONE;
// 		m_CastTargetEff.m_Point = xPT_TARGET_POS;
// 		m_PersistEff.m_idAct = 0;
// 		m_PersistEff.m_Point = xPT_TARGET_POS;
		idCastSound = 0;
		invokeTarget = xIVT_CAST_TARGET;
		invokefiltFriendship = xfNONESHIP;	// 따로지정하지 않으면 시전대상우호값으로 사툥하도록 바뀜
		invokefiltPlayerType = xfALL_PLAYER_TYPE;
		invokeJuncture = xJC_FIRST;
		invokeTargetCondition = xNONE_COND;
		idInvokeSkill = 0;
		invokeParameter = 0;
		valtypeInvokeAbility = xVAL;
		invokeState = 0;
		secInvokeDOT = 0;
		invokeNumApply = 0;
// 		m_invokeTargetEff.m_idAct = 0;
// 		m_invokeTargetEff.m_Point = xPT_NONE;
		idInvokeSound = 0;
		bDuplicate = FALSE;
		numOverlap = 1;		// 기본적으로 1개는 걸린다
		liveTarget = xTL_LIVE;
		idCreateObj = 0;
		XCLEAR_ARRAY( createObjParam );
		bImmunity = FALSE;
		invokeAddAbility = 0.f;
		idAddAbilityToClass = 0;
		idAddAbilityToTribe = 0;
		XCLEAR_ARRAY( dwParam );
	}
	~EFFECT() {}
	// 지속시간이 있는가.
	bool IsDuration() const {
		if( arySecDuration.size() > 1 )
			return arySecDuration[1] != 0;
		else if( arySecDuration.size() == 0 )
			return false;
		return arySecDuration[0] != 0;
	}
	float GetDuration( int level ) const {
		if( arySecDuration.size() == 0 )
			return 0;
		else if( arySecDuration.size() > 1 )
			return arySecDuration[ level ];
		return arySecDuration[ 0 ];
	}
	float GetAbilityMin( int level ) const {
		if( invokeAbilityMin.size() == 0 )
			return 0;
		else if( invokeAbilityMin.size() > 1 )
			return invokeAbilityMin[ level ];
		return invokeAbilityMin[0];
	}
	float GetInvokeSize( int level ) const {
		if( aryInvokeSize.size() == 0 )
			return 0;
		else if( aryInvokeSize.size() > 1 )
			return aryInvokeSize[ level ];
		return aryInvokeSize[0];
	}
	// 발동범위를 갖고 있는가.
	bool IsHaveInvokeSize() {
		if( aryInvokeSize.size() == 0 )
			return false;
		else if( aryInvokeSize.size() == 1 )
			return aryInvokeSize[ 0 ] != 0;
		return aryInvokeSize[1] != 0;
	}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
};
typedef std::list<EFFECT *>				EFFECT_LIST;
typedef std::list<EFFECT *>::iterator	EFFECT_ITOR;

XE_NAMESPACE_END
