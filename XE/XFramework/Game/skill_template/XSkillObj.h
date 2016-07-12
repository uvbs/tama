#pragma once
#include "XSkillDat.h"
#include "etc/Timer.h"
#include <list>
using namespace std;
NAMESPACE_XSKILL_START
// 스킬사용용 스킬객체
class XSkillObj
{
	XSkillDat	*m_pDat;			// 스킬데이타 포인터. 참조용(파괴금지)
	CTimer m_timerCool;		// 쿨타이머
	BOOL m_bAlreadyCast;		// 이미 시전됨-패시브용
	void Init() {
		m_pDat = NULL;
		m_bAlreadyCast = FALSE;
	}
	void Destroy() {}
public:
//	XSkillObj() { Init(); }
	XSkillObj( XSkillDat *pSkillDat ) { 
		Init();
		m_pDat = pSkillDat;
	}
	virtual ~XSkillObj() { Destroy(); }
	// get/set/is
	GET_ACCESSOR( ::CTimer&, timerCool );
	GET_ACCESSOR( XSkillDat*, pDat );
	GET_SET_ACCESSOR( BOOL, bAlreadyCast );
	XSKILL::EFFECT_LIST& GetEffectList( void ) { 
		return GetpDat()->GetlistEffects(); 
	}
	BOOL IsPassive( void ) { return (GetpDat()->GetCastMethod() == XSKILL::xPASSIVE); }
	ID GetidSkill( void ) {
		XBREAK( m_pDat == NULL );
		return m_pDat->GetidSkill();
	}
	// virtual
	virtual int IsCastCondition( void );		// 시전조건인가?
};

NAMESPACE_XSKILL_END

