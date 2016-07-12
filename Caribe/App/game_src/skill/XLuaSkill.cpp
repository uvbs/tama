#include "stdafx.h"
#include "XSkillUser.h"
#include "XSkillReceiver.h"
#include "XLuaSkill.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XSKILL;

XLuaSkill* XLuaSkill::Create( void ) { 
	return new XLuaSkill; 
}

void XLuaSkill::RegisterGlobal( void )
{
	Register_Class<XSkillUser>( "XSkillUser" );
	Register_Class<XSkillReceiver>( "XSkillReceiver" );	// 루아에서 상속된 멤버를 쓰기 위해서 해줘야함
	RegisterCPPFunc<XSkillUser>( "ClearAllSkill", &XSkillUser::DestroyAllSkill );
/*	RegisterCPPFunc<XSkillReceiver>( "SetMP", &XSkillReceiver::LuaSetMP );
	RegisterCPPFunc<XSkillReceiver>( "GetMP", &XSkillReceiver::LuaGetMP );
	RegisterCPPFunc<XSkillReceiver>( "GetMaxMP", &XSkillReceiver::LuaGetMaxMP );
	RegisterCPPFunc<XSkillReceiver>( "SetMaxMP", &XSkillReceiver::LuaSetMaxMP );
	RegisterCPPFunc<XSkillReceiver>( "AddMP", &XSkillReceiver::LuaAddMP ); */
}

void XLuaSkill::RegisterScript( XSkillUser *pCaster, XSkillReceiver *pCastingTarget )
{
	// 클래스 및 API등록
//	pCaster->RegisterScriptUse( this );
	// 전역변수 등록
	if( pCastingTarget )
		pCastingTarget->RegisterInhValRecv( this, "castObj" );	// 상속받은 최하위 클래스포인터 타입으로 루아에 등록시킴
	XBREAK( pCaster == NULL );	
	pCaster->RegisterInhValUse( this, "casterObj" );
	// 전역상수 등록
	RegisterGlobal();			// virtual
 
}
void XLuaSkill::InvokeDoScript( XSkillReceiver *pInvokeTarget, const char *szScript )
{
	XBREAK( pInvokeTarget == NULL );
	XASSERT( szScript && XE::IsHave( szScript ) );

//	pInvokeTarget->RegisterScript( this );		// 발동대상의 클래스와 API를 등록함
	pInvokeTarget->RegisterInhValRecv( this, "invokeObj" );	// 전역변수 등록
	DoString( szScript );		// 스크립트 실행
 
}

void XLuaSkill::TargetDoScript( XSkillReceiver *pTarget, const char *szScript )
{
//	XBREAK( pTarget == NULL );
	XASSERT( szScript && XE::IsHave( szScript ) );

//	pTarget->RegisterScript( this );		// 대상의 클래스와 API를 등록함
	DoString( szScript );		// 스크립트 실행
 
}

