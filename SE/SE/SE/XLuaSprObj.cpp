#include "StdAfx.h"
#include "XLuaSprObj.h"
#include "SprObj.h"

namespace LUA
{
	//-----------------------------------------------------------------------
	int Console( lua_State *L )
	{
		const char *typeName = lua_typename( L, lua_type(L, 1) );
		if( lua_isnumber( L, 1 ) )
		{
			float arg1 = (float)lua_tonumber( L, 1 );
			CONSOLE( "Console:%f", arg1 );
		} else
		if( lua_isstring( L, 1 ) )
		{
			const char *arg1 = (const char *)lua_tostring( L, 1 );
			CONSOLE( "Console:%s", Convert_char_To_TCHAR( arg1 ) );
		} else
		if( lua_isuserdata( L, 1 ) )
		{
			void *p = lua_touserdata( L, 1 );
			CONSOLE( "Console:0x%08x", (int)p );
			int a = 0; a = 1;
		} else
		{
			void *p = lua_touserdata( L, 1 );
			CONSOLE("Console: unkwoun type");
		}
		return 1;
	}
}

// 전역 루아글루함수 및 변수를 루아에 등록한다
void XLuaSprObj::RegisterLua()
{
	// global func
	RegisterCFunc( "Console", LUA::Console );
	// 각클래스의 등록(코드 재사용을 위해 각 클래스 안으로 들어가야 할듯
	Register_Class<XSprObj>("XSprObj");
	Register_ClassCon<XSprObj,const char*>();		// 
	RegisterCPPFunc<XSprObj>("SetAction", &XSprObj::SetAction );
//	RegisterCPPFunc<XSprObj>("Update", &XSprObj::LuaUpdate );
	RegisterCPPFunc<XSprObj>("SetScale", &XSprObj::LuaSetScale );
	RegisterCPPFunc<XSprObj>("SetScaleXY", &XSprObj::LuaSetScaleXY );
	RegisterCPPFunc<XSprObj>("SetRotate", &XSprObj::LuaSetRotate );
	RegisterCPPFunc<XSprObj>("SetAlpha", &XSprObj::SetfAlpha );
	RegisterCPPFunc<XSprObj>("AddSprObj", &XSprObj::LuaAddSprObj );
	RegisterVar( "PLAY_ONE", xRPT_1PLAY );
//	RegisterVar( "PLAY_ONE_CONT", xRPT_1PLAY_CONT );
	RegisterVar( "PLAY_LOOP", xRPT_LOOP );


}
