#include "stdafx.h"
#include <stdio.h> 
#include <string.h>
extern "C" {
#ifdef WIN32
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#else
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#endif
}
#include "XLua.h"
#ifdef _VER_IOS
#undef min
#undef max
#endif
#include "etc/debug.h"
#include "etc/ConvertString.h"
#include "XResMng.h"
#include "XResObj.h"
#include "XFramework/XConstant.h"
#include "XAutoPtr.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/*class CTest
{
public:
	CTest() {}
	~CTest() {}
	
	void Set( int a ) {}
};*/
namespace LUA
{
	DWORD Color( int r, int g, int b, int a ) 
	{
		return XCOLOR_RGBA( r, g, b, a ); 
	}
}; // namespace LUA

void show_error( const char *str )
{
#ifdef _XTOOL
	CONSOLE( "lua error: %s", Convert_char_To_TCHAR( str ) );
#else
	XBREAKF(1, "lua error: %s", Convert_char_To_TCHAR( str ) );
#endif
}
// 현재 스택의 상태를 출력해줍니다.  
char* GetStackStatus(lua_State *L) 
{ 
	static char strTemp[1024] ="";
	char buff[64] = "";
	    sprintf_s(buff, "(top:%d)", lua_gettop(L)); 
		strcat_s( strTemp, buff );
        for (int i = 1; i <= lua_gettop(L); i++)  
        { 
                switch(lua_type(L, i)) 
                { 
                case LUA_TTABLE : 
                case LUA_TNIL :  
                case LUA_TFUNCTION : 
                case LUA_TUSERDATA : 
                case LUA_TTHREAD : 
                case LUA_TBOOLEAN : 
                        sprintf_s(buff, "(%d:%s)%d ", i, lua_typename(L, lua_type(L, i)), lua_toboolean(L, i)); break; 
                case LUA_TLIGHTUSERDATA : 
                        sprintf_s(buff, "(%d:%s) ", i, lua_typename(L, lua_type(L, i))); break; 
                case LUA_TNUMBER : 
                        sprintf_s(buff, "(%d:%s)%d ", i, lua_typename(L, lua_type(L, i)), (int)lua_tonumber(L, i)); break; 
                case LUA_TSTRING : 
                        sprintf_s(buff, "(%d:%s)%s ", i, lua_typename(L, lua_type(L, i)), lua_tostring(L, i)); break; 
                } 
				strcat_s( strTemp, buff );
        } 
        sprintf_s(buff, "\n"); 
		strcat_s( strTemp, buff );
		return strTemp;
} 

XLua* XLua::Create( void ) 
{
	return new XLua;
}


BOOL XLua::LoadScript( const char* cRes )
{
	LPCTSTR szRes = Convert_char_To_TCHAR( cRes );
    XLOGXN("lua loading.....%s", szRes );
	XBaseRes *pRes;
	XGET_RESOURCE( pRes, szRes );
//	XBaseRes *pRes = XE::CreateResHandle( szRes );
//	XAutoPtr<XBaseRes*> _res( pRes );
	if( pRes == NULL )
	{
		XALERT( "%s file not found", C2SZ( cRes ) );
		return FALSE;
	}
	_tcscpy_s( m_szFilename, XE::GetFileName( szRes ) );

	int size = pRes->Size();
//	char *buffer = new char[ size + 1 ];
	char buffer[ 0xffff ];
	XBREAK( size >= sizeof(buffer) );
	pRes->Read( buffer, size );
	buffer[ size ] = 0;
    
    XLOGXN("DoString..." );
	DoString( buffer );
/*	if( luaL_dofile( L, szFilename ) ) {
		ErrorMsg( lua_tostring(L, -1) );
		return FALSE;
	} */
//    lua_tinker::def(L, "_ALERT", show_error);
    XLOGXN("lua loading success.....%s", szRes );
	return TRUE;
}

void XLua::RegisterGlobal( void )
{
	XBREAK( CONSTANT == NULL );
	CONSTANT->RegisterScript( this );
	//
	RegisterCFunc( "Color", LUA::Color );
		
}