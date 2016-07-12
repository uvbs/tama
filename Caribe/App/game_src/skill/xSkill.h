#pragma once
#include "SkillType.h"
#include "Skilldef.h"
#include "XSkillDat.h"
#include "XSkillObj.h"
#include "XBuffObj.h"
#include "XSkillUser.h"
#include "XSkillReceiver.h"
#include "XLuaSkill.h"
#include "XESkillMng.h"
#include "XBuffObj.h"

#define EFFECT_LOOP( SKILL, I, E )		LIST_LOOP( SKILL->GetEffectList(), EFFECT*, I, E )

NAMESPACE_XSKILL_START
//	int ExecuteInvokeScript( XLua *pLua, const char *szScript );
bool DoDiceInvokeRatio( const EFFECT *pEffect, int level );
NAMESPACE_XSKILL_END
