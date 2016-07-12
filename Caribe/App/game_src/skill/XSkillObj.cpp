#include "stdafx.h"
#include "xSkill.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

NAMESPACE_XSKILL_START
int XSkillObj::IsCastCondition( void )
{
	if( IsPassive() || IsAbility() )
		return TRUE;
// 	switch( GetpDat()->GetCastMethod() )
// 	{
// 	case xPASSIVE:	// 패시브
// 	case xABILITY:	// 특성
// 		return TRUE;		// 패시브는 항상 발동
// 	case xSCRIPT:						// 스크립트에 의한 조건발동
// 		//#pragma message( "아직 구현안됨" )
// 		break;
// 	case xACTIVE:			// 액티브랑 토글은 자동발동 되지 않음
// 	case xTOGGLE:
// 		break;
// 	default:
// 		XBREAKF(1, "%s, %d", GetpDat()->GetstrIdentifier().c_str(), GetpDat()->GetCastMethod() );
// 		break;
// 	}
	return FALSE;
}
NAMESPACE_XSKILL_END