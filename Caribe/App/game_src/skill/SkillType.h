#pragma once
#include "SkillDef.h"
#include <list>

NAMESPACE_XSKILL_START

class XSkillObj;
class XBuffObj;
class XSkillReceiver;
class XSkillUser;

typedef XList<XSkillObj *>				XSkillObj_List;
//typedef list<XSkillObj *>::iterator		XSkillObj_Itor;

typedef std::list<XBuffObj *>				XBuffObj_List;
typedef std::list<XBuffObj *>::iterator	XBuffObj_Itor;

typedef std::list<XSkillReceiver *>				XSkillReceiver_List;
typedef std::list<XSkillReceiver *>::iterator	XSkillReceiver_Itor;

typedef std::list<XSkillUser *>				XSkillUser_List;
typedef std::list<XSkillUser *>::iterator	XSkillUser_Itor;

// 범용 스킬델리게이트
class XDelegateSkill
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateSkill() { Init(); }
	virtual ~XDelegateSkill() { Destroy(); }
	//
	/**
	 @brief snObj오브젝트의 포인터를 구해준다. 그 오브젝트는 XSkillReceiver를 상속받은 객체여야 한다.
	*/
	virtual XSkillReceiver* GetTarget( ID snObj ) { return NULL; }
	/**
	 @brief snObj오브젝트의 포인터를 구해준다. 그 오브젝트는 XSkillUser를 상속받은 객체여야 한다.
	*/
	virtual XSkillUser* GetCaster( ID snObj ) { return NULL; }
// 	virtual BIT GetCamp( void ) = 0;	// 사용하는곳이 없어보여서 삭제
// //	virtual xtSide GetFriendship( void ) = 0;
	virtual void DelegateResultEventBeforeAttack( XBuffObj *pBuffObj, XSKILL::EFFECT *pEffect ) {}

};

NAMESPACE_XSKILL_END
