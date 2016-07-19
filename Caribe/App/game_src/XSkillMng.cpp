#include "stdafx.h"
#include "XSkillMng.h"
#include "XFramework/XTextTableUTF8.h"
#include "skill/XSkillDat.h"
#include "skill/XEffect.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// 스킬매니저 싱글톤 인스턴스
XSkillMng *SKILL_MNG = nullptr;

////////////////////////////////////////////////////////////////
XSkillMng::XSkillMng()
{
	Init();
	XBREAK( SKILL_MNG != nullptr );
	SKILL_MNG = this;
}

void XSkillMng::Destroy()
{
	SKILL_MNG = nullptr;
}

void XSkillMng::OnAddConstant( XConstant *pConstant ) 
{
	CONSTANT->Add( XSKTEXT( 67 ), XGAME::xADJ_ATTACK );
	CONSTANT->Add( XSKTEXT( 244 ), XGAME::xADJ_ATTACK_MELEE_TYPE );
	CONSTANT->Add( XSKTEXT( 245 ), XGAME::xADJ_ATTACK_RANGE_TYPE );
	CONSTANT->Add( XSKTEXT( 68 ), XGAME::xADJ_DEFENSE );
	CONSTANT->Add( XSKTEXT( 69 ), XGAME::xADJ_MAX_HP );
	CONSTANT->Add( XSKTEXT( 70 ), XGAME::xADJ_HP );
	CONSTANT->Add( XSKTEXT( 71 ), XGAME::xADJ_HP_REGEN_SEC );
	CONSTANT->Add( XSKTEXT( 72 ), XGAME::xADJ_HP_REGEN );
	CONSTANT->Add( XSKTEXT( 79 ), XGAME::xADJ_MOVE_SPEED );
	CONSTANT->Add( XSKTEXT( 80 ), XGAME::xADJ_ATTACK_SPEED );


	CONSTANT->Add( XSKTEXT( 237 ), XGAME::xADJ_DAMAGE );	// 모든피해보정
	CONSTANT->Add( XSKTEXT( 81 ), XGAME::xADJ_ATTACK_RANGE );
	CONSTANT->Add( XSKTEXT( 82 ), XGAME::xADJ_PHY_DAMAGE );
	CONSTANT->Add( XSKTEXT( 83 ), XGAME::xADJ_MAG_DAMAGE );
	CONSTANT->Add( XSKTEXT( 84 ), XGAME::xADJ_PHY_DAMAGE_RECV );
	CONSTANT->Add( XSKTEXT( 85 ), XGAME::xADJ_MAG_DAMAGE_RECV );
	CONSTANT->Add( XSKTEXT( 216 ), XGAME::xADJ_MELEE_DAMAGE_RECV );
	CONSTANT->Add( XSKTEXT( 217 ), XGAME::xADJ_RANGE_DAMAGE_RECV );
	CONSTANT->Add( XSKTEXT( 118 ), XGAME::xADJ_DAMAGE_RECV );
	CONSTANT->Add( XSKTEXT( 86 ), XGAME::xADJ_HEAL );
	CONSTANT->Add( XSKTEXT( 87 ), XGAME::xADJ_HEAL_RECV );
	CONSTANT->Add( XSKTEXT( 88 ), XGAME::xADJ_SKILL_POWER );
	CONSTANT->Add( XSKTEXT( 89 ), XGAME::xADJ_SIGHT );
	CONSTANT->Add( XSKTEXT( 106 ), XGAME::xADJ_PENETRATION_RATE );
	CONSTANT->Add( XSKTEXT( 107 ), XGAME::xADJ_PENETRATION_PROB );
	CONSTANT->Add( XSKTEXT( 119 ), XGAME::xADJ_EVADE_RATE );
	CONSTANT->Add( XSKTEXT( 242 ), XGAME::xADJ_EVADE_RATE_MELEE );
	CONSTANT->Add( XSKTEXT( 243 ), XGAME::xADJ_EVADE_RATE_RANGE );
	CONSTANT->Add( XTEXT( 1071 ), XGAME::xADJ_EVADE_RATE_SMALL );
	CONSTANT->Add( XTEXT( 1072 ), XGAME::xADJ_EVADE_RATE_MIDDLE );
	CONSTANT->Add( XTEXT( 1073 ), XGAME::xADJ_EVADE_RATE_BIG );
	CONSTANT->Add( XSKTEXT( 120 ), XGAME::xADJ_BLOCKING_RATE );
	CONSTANT->Add( XSKTEXT( 211 ), XGAME::xADJ_CRITICAL_RATE );
	CONSTANT->Add( XSKTEXT( 227 ), XGAME::xADJ_CRITICAL_POWER );
	CONSTANT->Add( XSKTEXT( 248 ), XGAME::xADJ_CRITICAL_RATE_RECV );
	CONSTANT->Add( XSKTEXT( 218 ), XGAME::xADJ_MELEE_CRITICAL_RECV );
	CONSTANT->Add( XSKTEXT( 219 ), XGAME::xADJ_RANGE_CRITICAL_RECV );
	CONSTANT->Add( XSKTEXT( 220 ), XGAME::xADJ_CRITICAL_RECV );
	CONSTANT->Add( XSKTEXT( 205 ), XGAME::xADJ_ADD_DAMAGE_TRIBE );
	CONSTANT->Add( XSKTEXT( 206 ), XGAME::xADJ_ADD_DAMAGE_CLASS );
	CONSTANT->Add( XTEXT( 1039 ), XGAME::xADJ_ATTACK_SPEED_SMALL );
	CONSTANT->Add( XTEXT( 1040 ), XGAME::xADJ_ATTACK_SPEED_MIDDLE );
	CONSTANT->Add( XTEXT( 1041 ), XGAME::xADJ_ATTACK_SPEED_BIG );
	CONSTANT->Add( XTEXT( 1056 ), XGAME::xADJ_DAMAGE_TANKER );
	CONSTANT->Add( XTEXT( 1057 ), XGAME::xADJ_DAMAGE_RANGE );
	CONSTANT->Add( XTEXT( 1058 ), XGAME::xADJ_DAMAGE_SPEED );
	CONSTANT->Add( XTEXT( 1053 ), XGAME::xADJ_DAMAGE_SMALL );
	CONSTANT->Add( XTEXT( 1054 ), XGAME::xADJ_DAMAGE_MIDDLE );
	CONSTANT->Add( XTEXT( 1055 ), XGAME::xADJ_DAMAGE_BIG );
	CONSTANT->Add( XSKTEXT( 12 ), XGAME::xADJ_SKILL_COOL );		// 쿨타임보정
	CONSTANT->Add( XSKTEXT( 13 ), XGAME::xADJ_SKILL_COOL );		// 재사용대기시간보정
	CONSTANT->Add( XTEXT( 1060 ), XGAME::xADJ_DAMAGE_SPEARMAN );
	CONSTANT->Add( XTEXT( 1061 ), XGAME::xADJ_DAMAGE_ARCHER );
	CONSTANT->Add( XTEXT( 1062 ), XGAME::xADJ_DAMAGE_PALADIN );
	CONSTANT->Add( XTEXT( 1063 ), XGAME::xADJ_DAMAGE_MINOTAUR );
	CONSTANT->Add( XTEXT( 1064 ), XGAME::xADJ_DAMAGE_CYCLOPS );
	CONSTANT->Add( XTEXT( 1065 ), XGAME::xADJ_DAMAGE_LYCAN );
	CONSTANT->Add( XTEXT( 1066 ), XGAME::xADJ_DAMAGE_GOLEM );
	CONSTANT->Add( XTEXT( 1067 ), XGAME::xADJ_DAMAGE_TREANT );
	CONSTANT->Add( XTEXT( 1068 ), XGAME::xADJ_DAMAGE_FALLEN_ANGEL );
	CONSTANT->Add( XSKTEXT( 254 ), XGAME::xADJ_DAMAGE_FIRE );
	CONSTANT->Add( XSKTEXT( 92 ), XGAME::xADJ_VAMPIRIC );
	CONSTANT->Add( XSKTEXT( 90 ), XGAME::xHP );
	CONSTANT->Add( XSKTEXT( 246 ), XGAME::xADJ_HIT_RATE );

	// 	CONSTANT->Add( XSKTEXT( 92 ), XGAME::xADJ_VAMPIRIC );

	// 발동시점
	CONSTANT->Add( XTEXT( 1037 ), XSKILL::xJC_START_BATTLE );
	// 발동조건
	CONSTANT->Add( XTEXT( 1043 ), XSKILL::xATTACK_TARGET_TRIBE );
	CONSTANT->Add( XTEXT( 1044 ), XSKILL::xATTACK_TARGET_JOB );
	CONSTANT->Add( XTEXT( 1045 ), XSKILL::xTARGET_ARM );
	CONSTANT->Add( XTEXT( 1046 ), XSKILL::xATTACK_TARGET_SIZE );
	CONSTANT->Add( XTEXT( 1050 ), XSKILL::xTARGET_RANK );
	// 유닛상수등은 XGameCommon::AddConstant()로 옮김.
}

BOOL XSkillMng::CustomParsingEffect( TiXmlAttribute *pAttr,
									const char *cAttrName,
									const char *cParam,
									XSKILL::XSkillDat* pSkillDat,
									XSKILL::EFFECT *pEffect ) 
{ 
	TCHAR szAttrName[1024];
	Convert_utf8_To_TCHAR( szAttrName, 1024, cAttrName );
	if( XE::IsSame( szAttrName, XTEXT(1010) ) )	// 추가능력치직업
	{
		pEffect->idAddAbilityToClass = (XGAME::xtUnit) ParsingParam( cParam );
		return TRUE;
	} else
	if( XE::IsSame( szAttrName, XTEXT(1024) ) )	// 추가능력치종족
	{
		pEffect->idAddAbilityToTribe = (XGAME::xtUnit) ParsingParam( cParam );
		return TRUE;
	} else
	if( XE::IsSame( szAttrName, XTEXT(1009) ) ) // 추가능력치
	{
		float addAbility = (float)pAttr->DoubleValue();
		if( pEffect->valtypeInvokeAbility == XSKILL::xPERCENT )
			pEffect->invokeAddAbility = addAbility / 100.f;
		else
			pEffect->invokeAddAbility = addAbility;
		return TRUE;
	}
	return FALSE; 
}

bool XSkillMng::IsInverseParam( int invokeParam ) const
{
	switch( invokeParam ) {
	case XGAME::xADJ_ATTACK_SPEED:
	case XGAME::xADJ_ATTACK_SPEED_SMALL:
	case XGAME::xADJ_ATTACK_SPEED_MIDDLE:
	case XGAME::xADJ_ATTACK_SPEED_BIG:
		return true;
	}
	return false;
}
bool XSkillMng::IsRateParam( int idxParam ) const
{
	switch( idxParam ) {
	case XGAME::xADJ_ADD_DAMAGE_CLASS:
	case XGAME::xADJ_ADD_DAMAGE_TRIBE:
	case XGAME::xADJ_EVADE_RATE:
	case XGAME::xADJ_EVADE_RATE_MELEE:
	case XGAME::xADJ_EVADE_RATE_RANGE:
	case XGAME::xADJ_EVADE_RATE_SMALL:
	case XGAME::xADJ_EVADE_RATE_MIDDLE:
	case XGAME::xADJ_EVADE_RATE_BIG:
	case XGAME::xADJ_CRITICAL_RATE:
	case XGAME::xADJ_CRITICAL_RATE_RECV:
	case XGAME::xADJ_BLOCKING_RATE:
	case XGAME::xADJ_PENETRATION_RATE:
	case XGAME::xADJ_HIT_RATE:
		return true;
	}
	return false;
}

/**
 @brief 로딩후 pEffect에 잘못써진 파라메터가 있으면 자동고쳐준다.
*/
void XSkillMng::AdjustEffectParam( XSKILL::XSkillDat* pDat, XSKILL::EFFECT *pEffect )
{
	if( IsInverseParam( pEffect->invokeParameter ) ) {
		// 공속값의 경우 빨라지는것(좋아지는것)이 +값, 느려지는것이 -값으로 스크립팅을 하기위해 부호를 바꾼다.
		int size = pEffect->invokeAbilityMin.size();
		XBREAK( pEffect->valtypeInvokeAbility == XSKILL::xVAL );	// 공속을 xVAL로는 가급적 사용하지 말것.
		if( pEffect->valtypeInvokeAbility == XSKILL::xPERCENT ) {
			for( int i = 0; i < size; ++i ) {
				// 공속의 계산방식은 100%를 기준으로 90% 혹은 110와 같은 식으로 표현하지만
				// 편의상 +10% -10%와같은 식으로 쓰기위해 그냥 기존공속에 곱하기만 하면 되게 변환한다.
				float val = pEffect->invokeAbilityMin[i];
				if( val > 0 )
					val = -( 1 / ( val + 1 )*val );		// b = -(1/(val+1)*val)  => a = -(b/(b+1))
				else
					if( val < 0 )
						val = -val;
				pEffect->invokeAbilityMin[i] = val;
			}
		}
	} else 
	if( IsRateParam( pEffect->invokeParameter ) ) {
		// xxx율보정
		// xxx율보정 류는 VAL로 써야함.
		XBREAKF( pEffect->valtypeInvokeAbility == XSKILL::xPERCENT
			, "%s:파라메터가 [xxx율]계열일 경우 #으로 써야함.", pDat->GetstrIdentifier().c_str() );
// 		if( pEffect->valtypeInvokeAbility == XSKILL::xPERCENT )
// 			pEffect->valtypeInvokeAbility = XSKILL::xVAL;
		XARRAYLINEARN_LOOP_AUTO( pEffect->invokeAbilityMin, &abil ) {
			abil = abil / 100.f;
		} END_LOOP;
	} else {
		switch( pEffect->invokeParameter ) {
		case XGAME::xHP:
			// 발동파라메터가 체력을 올리거나 내리는거고 발동지속시간이 있는데 DOT가 없으면 안된다.
			if( pEffect->secInvokeDOT == 0 ) {
				if( pEffect->secDurationInvoke > 0 )
					pEffect->secInvokeDOT = 1.f;
				XARRAYLINEARN_LOOP_AUTO( pEffect->arySecDuration, sec )
				{
					// 지속시간이 있는데 DOT시간이 없으면 기본값으로 입력
					if( sec > 0 ) {
						pEffect->secInvokeDOT = 1.f;
						break;
					}
				} END_LOOP;
			}
			break;
		default:
			break;
		}
	}
}

// XSKILL::xCOND_PARAM XSkillMng::ParsingCond( TiXmlAttribute *pAttr,
// 											const char *cAttrName,
// 											const char *cVal,
// 											XSKILL::XSkillDat* pSkillDat,
// 											XSKILL::EFFECT *pEffect ) 
// {
// 	xCOND_PARAM param;
// 	if( XE::IsSame( cAttrName, ))
// 	return param;
// }
