#include "stdafx.h"
#include "xSkill.h"

NAMESPACE_XSKILL_START
int XSkillObj::IsCastCondition( void )
{
	switch( GetpDat()->GetCastMethod() )
	{
	case xPASSIVE:	return TRUE;		// 패시브는 항상 발동
	case xSCRIPT:						// 스크립트에 의한 조건발동
		//#pragma message( "아직 구현안됨" )
		break;
	case xACTIVE:			// 액티브랑 토글은 자동발동 되지 않음
	case xTOGGLE:
		break;
	default:
		XBREAKF(1, "%s, %d", GetpDat()->GetstrIdentifier().c_str(), GetpDat()->GetCastMethod() );
		break;
	}
	return FALSE;
}
NAMESPACE_XSKILL_END