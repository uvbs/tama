#include "stdafx.h"
#include "XUnitCommon.h"
#include "XLegionObj.h"
#include "XSquadObj.h"
#include "XLegion.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

//////////////////////////////////////////////////////////////////////////
#ifdef _XMEM_POOL
template<> XPool<XUnitCommon>* XMemPool<XUnitCommon>::s_pPool = NULL;
#endif // _XMEM_POOL

XUnitCommon::XUnitCommon( XSquadObj *pSquadObj, ID idProp, BIT bitSide, const XE::VEC3& vPos, float multipleAbility )
	: XBaseUnit( pSquadObj, idProp, bitSide, vPos, multipleAbility ) {
	Init();
}
void XUnitCommon::Destroy()
{

}

_tstring XUnitCommon::GetstrIds() {
	return GetpPropUnit()->strIdentifier.c_str();
}
