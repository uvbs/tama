/********************************************************************
	@date:	2016/07/27 9:27
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XBaseUnitH.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

#include "skill/SkillDef.h"

namespace xnUnit {
	class XMsgQ;
	class XMsgBase;
	class XMsgDmg;
	class XMsgDmgFeedback;
	struct xDmg {
		XSPWorldObjW m_spAtkObj;
		XSPUnitW m_spUnitAtker;
		XSPUnitW m_spTarget;
		float m_Damage = 0.f;
		float m_ratioPanet = 0.f;
		XSKILL::xtDamage m_typeDmg = XSKILL::xDMG_NONE;
		BIT m_bitAttrHit = 0;
		XGAME::xtDamageAttr m_attrDamage = XGAME::xDA_NONE;
		bool m_bCritical = false;
		xDmg() {}
		xDmg( XSPWorldObj spAtkObj, XSPUnit spTarget, float damage, float ratioPenet, XSKILL::xtDamage typeDamage, const BIT bitAttrHit, XGAME::xtDamageAttr attrDamage, bool bCritical );
		xDmg( XSPUnit spAtker, XSPUnit spTarget, float damage, float ratioPenet, XSKILL::xtDamage typeDamage, const BIT bitAttrHit, XGAME::xtDamageAttr attrDamage, bool bCritical ) 
			: xDmg( std::static_pointer_cast<XEBaseWorldObj>( spAtker ), spTarget, damage, ratioPenet, typeDamage, bitAttrHit, attrDamage, bCritical ) {}
		inline XBaseUnit* GetpUnit() const {
			return ( m_spUnitAtker.lock() ) ? m_spUnitAtker.lock().get(): nullptr;
		}
	};

	// 유닛간에 주고받는 메시지 타입
	enum xtMsg {
		xUM_NONE,
		xUM_DMG,
		xUM_DMG_FEEDBACK,
		xUM_KILL_TARGET,
	};
}
