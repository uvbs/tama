#include "stdafx.h"
#include "XSkillObj.h"
//#include "xSkill.h"
#include "SkillDef.h"
#include "XSkillDat.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XSKILL )
int XSkillObj::IsCastCondition()
{
	if( IsPassive() || IsAbility() )
		return TRUE;
	return FALSE;
}

std::list<EFFECT*>& XSkillObj::GetEffectList() 
{
	return GetpDat()->GetlistEffects();
}
BOOL XSkillObj::IsActive() 
{
	return GetpDat()->IsActive();
}
BOOL XSkillObj::IsPassive() 
{
	return GetpDat()->IsPassive();
}
BOOL XSkillObj::IsAbility() 
{
	return GetpDat()->IsAbility();
}
ID XSkillObj::GetidSkill() 
{
	XBREAK( m_pDat == NULL );
	return m_pDat->GetidSkill();
}
const _tstring& XSkillObj::GetStrIdentifier() 
{
	XBREAK( m_pDat == NULL );
	return m_pDat->GetstrIdentifier();
}
LPCTSTR XSkillObj::GetIdsSkill() 
{
	XBREAK( m_pDat == NULL );
	return m_pDat->GetstrIdentifier().c_str();
}
BOOL XSkillObj::IsSameCastMethod( xCastMethod castMethod ) 
{
	return m_pDat->IsSameCastMethod( castMethod );
}
XE_NAMESPACE_END