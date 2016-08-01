#include "stdafx.h"
#include "XUnitArcher.h"
#include "XObjEtc.h"
#include "XWndBattleField.h"
#include "XBattleField.h"
#include "XLegionObj.h"
#include "XSoundMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _XMEM_POOL
template<> XPool<XUnitArcher>* XMemPool<XUnitArcher>::s_pPool = NULL;
#endif
////////////////////////////////////////////////////////////////
XUnitArcher::XUnitArcher( XSPSquad spSquadObj,
						ID idProp,
						BIT bitSide, 
						const XE::VEC3& vPos,
						float multipleAbility )
: XUnitCommon( spSquadObj, idProp, bitSide, vPos, multipleAbility )
{
	Init();

}

void XUnitArcher::Destroy()
{
}


void XUnitArcher::ShootRangeAttack( UnitPtr& spTarget,
									const XE::VEC3& vwSrc,
									const XE::VEC3& vwDst,
									float damage,
									bool bCritical,
							const std::string& strType,
							const _tstring& strSpr )
{
	//GetpSquadObj()->PlaySound( "arrow_10over" );
	SOUNDMNG->OpenPlaySoundBySec( 31, xRandomF(0.5f, 1.5f) );
	ShootArrow( spTarget, vwSrc, vwDst, damage, bCritical );
}

