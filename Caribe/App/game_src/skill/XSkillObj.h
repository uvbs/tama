#pragma once
//#include "XSkillDat.h"
#include "SkillDef.h"
#include "etc/Timer.h"
#include <list>

XE_NAMESPACE_START( XSKILL )

class XSkillDat;
struct EFFECT;
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
	std::list<EFFECT*>& GetEffectList();
	BOOL IsActive();
	BOOL IsPassive();
	BOOL IsAbility();
	ID GetidSkill();
	const _tstring& GetStrIdentifier();
	LPCTSTR GetIdsSkill();
	BOOL IsSameCastMethod( xCastMethod castMethod );
	// virtual
	virtual int IsCastCondition();		// 시전조건인가?
};

XE_NAMESPACE_END


