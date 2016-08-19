
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
#ifdef _VER_IPHONE
#undef min
#undef max
#endif
#include "debug.h"
#include "ConvertString.h"
#include "ResMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

class CTest
{
public:
	CTest() {}
	~CTest() {}
	
	void Set( int a ) {}
};

void show_error( const char *str )
{
#ifdef _XTOOL
	CONSOLE( "lua error: %s", Convert_char_To_TCHAR( str ) );
#else
	XBREAKF(1, "lua error: %s", Convert_char_To_TCHAR( str ) );
#endif
}
// ?꾩옱 ?ㅽ깮???곹깭瑜?異쒕젰?댁쨳?덈떎.  
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

BOOL XLua::LoadScript( const char* cFilename )
{
	XResFile file;
	if( file.Open( cFilename, XBaseRes::xREAD ) == FALSE )
		return FALSE;

	_tcscpy_s( m_szFilename, XE::GetFileName( Convert_char_To_TCHAR( cFilename ) ) );
	int size = file.Size();
//	char *buffer = new char[ size + 1 ];
	XBREAK( size >= 0xffff );
	char buffer[ 0xffff ];
	file.Read( buffer, size );
	buffer[ size ] = 0;
	DoString( buffer );
/*	if( luaL_dofile( L, szFilename ) ) {
		ErrorMsg( lua_tostring(L, -1) );
		return FALSE;
	} */
	return TRUE;
}
