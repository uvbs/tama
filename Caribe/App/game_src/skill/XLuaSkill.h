#pragma once

#include "XLua.h"
#ifdef _VER_IPHONE
#undef min
#undef max
#endif

XE_NAMESPACE_START( XSKILL )
class XLuaSkill;
class XSkillUser;
class XSkillReceiver;
class XLuaSkill :	public XLua
{
public:
	static XLuaSkill* Create( void );
private:
	void Init() {
	}
	void Destroy() {}
public:
	XLuaSkill() { Init(); }
	XLuaSkill( const char *szFilename ) : XLua( szFilename ) { Init(); }
	virtual ~XLuaSkill() { Destroy(); }

	void InvokeDoScript( XSkillReceiver *pInvokeTarget, const char *szScript );	// 발동대상에게 szScript를 실행한다
	void TargetDoScript( XSkillReceiver *pTarget, const char *szScript );			// 아무대상에게 szScript를 실행한다
	// virtual
	virtual void RegisterScript( XSkillUser *pCaster, XSkillReceiver *pCastingTarget );
	virtual void RegisterGlobal( void );
};

XE_NAMESPACE_END