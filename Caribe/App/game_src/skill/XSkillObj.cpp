#include "stdafx.h"
#if 0
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
// int XSkillObj::IsCastCondition()
// {
// 	if( IsPassive() || IsAbility() )
// 		return TRUE;
// 	return FALSE;
// }

const XList4<EFFECT*>& XSkillObj::GetEffectList() const
{
	return GetpDat()->GetlistEffects();
}
bool XSkillObj::IsActiveCategory() const 
{
	return GetpDat()->IsActiveCategory();
}
bool XSkillObj::IsPassiveCategory() const
{
	return GetpDat()->IsPassiveCategory();
}
bool XSkillObj::IsAbilityCategory() const
{
	return GetpDat()->IsAbilityCategory();
}
ID XSkillObj::GetidSkill() const
{
	XBREAK( m_pDat == NULL );
	return m_pDat->GetidSkill();
}
const _tstring& XSkillObj::GetStrIdentifier() const
{
	XBREAK( m_pDat == NULL );
	return m_pDat->GetstrIdentifier();
}
LPCTSTR XSkillObj::GetIdsSkill() const
{
	XBREAK( m_pDat == NULL );
	return m_pDat->GetstrIdentifier().c_str();
}
BOOL XSkillObj::IsSameCastMethod( xCastMethod castMethod ) const
{
	return m_pDat->IsSameCastMethod( castMethod );
}
XE_NAMESPACE_END
#endif // 0
