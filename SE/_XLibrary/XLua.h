#ifndef __LUASCRIPT_H__
#define __LUASCRIPT_H__
#ifdef _VER_IPHONE 
#undef min
#undef max
#endif
#include <string>
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
#include "lua_tinker.h"
#include "global.h"
#include "debug.h"
#include "ConvertString.h"
#include "xUtil.h"
//#define	MAX_STRING_KEY		32

char* GetStackStatus(lua_State *L) ;
void show_error( const char *str );

class XLua;
class XLua
{
public:
	enum xtResume { xNONE=0, xFINISH, xYIELD, xRESUME, xERROR };
	static XLua* Create() {
		return new XLua;
	}
protected:
	lua_State *L;
	int	m_nNumArg;
	char m_szErr[ 1024 ];
	TCHAR m_szFilename[256];

	void Init()	{
		memset( m_szErr, 0, sizeof(m_szErr) );
		m_nNumArg = 0;	
		XCLEAR_ARRAY( m_szFilename );
	}
	void ErrorMsg( const char *szErr ) {
		XBREAK( strlen(szErr)+1 > sizeof(m_szErr));
		strcpy_s( m_szErr, szErr );
		XBREAKF( 1, "%s", Convert_char_To_TCHAR( szErr ) );
	}

public:
	XLua() 
	{ 
		Init(); 
		L = lua_open();							// 루아 state를 생성한다. 
//		luaopen_base(L);						// print 루아 함수때문에 실행합니다. 나머지는 실행하면 스택만 길어지므로 빼주시길... 
		luaopen_string(L);						// 필요할때만 로딩할것
	}
	XLua( const char* strFilename )
	{
		Init();
		L = lua_open();							// 루아 state를 생성한다. 
//		luaopen_base(L);						// print 루아 함수때문에 실행합니다. 나머지는 실행하면 스택만 길어지므로 빼주시길... 
		luaopen_string(L);
		LoadScript( strFilename );
 	}
	virtual ~XLua() 
	{
		lua_close(L);								// 루아 state를 해제한다. 
	}
	// get/set
	lua_State* GetL() { return L; }
	GET_ACCESSOR( const char*, szErr );
	
	// Is...
	BOOL IsHaveFunc( const char* szFunc )			// strFunc이란이름의 함수가 있는지 확인
	{
		if( szFunc && XE::IsHave( szFunc ) )
		{
			lua_getfield( L, LUA_GLOBALSINDEX, szFunc );
			BOOL flag = (lua_isfunction( L,-1 ) != 0);
			lua_pop( L, 1 );
			return flag;
		}
		return FALSE;
	}
	
	BOOL LoadScript( const char* szFilename );

/*	BOOL Load( char *szFilename ) {		// 로드만 함
		_tcscpy_s( m_szFilename, XE::GetFileName( Convert_char_To_TCHAR( szFilename ) ) );
		if( luaL_loadfile( L, szFilename ) ) {
			ErrorMsg( lua_tostring( L, -1 ) );
			return FALSE;
		}
		return TRUE;
	} */

/*	int LoadStr( const char *str ) {					// str버퍼에 있는 루아를 로드한다. 이것만 로드해선 실행 안되더라. 이거쓰지말고 DoString을 써야한다.
		if( str == nullptr )		return 0;
		if( str[0] == 0)		return 0;
		return luaL_loadstring( L, str );
	} */
	// 
	int SetModulePath( const char* _path )  {     
//#ifdef WIN32
		char path[1024];
		strcpy_s( path, _path );
		int len = strlen(_path);
#ifdef WIN32
		if( path[ len-1 ] == '\\' )
			path[ len-1 ] = 0;
#else
		if( path[ len-1 ] == '/' )
			path[ len-1 ] = 0;
#endif
			
		luaL_openlibs(L);
		lua_getglobal( L, "package" );     
		lua_getfield( L, -1, "path" ); // get field "path" from table at top of stack (-1)     
		std::string cur_path = lua_tostring( L, -1 ); // grab path string from top of stack     
		//cur_path.append( ";" ); // do your path magic here     
		//cur_path.append( path );     
		cur_path = path;
#ifdef WIN32
		cur_path.append( "\\?.lua" );
#else
		cur_path.append( "/?.lua" );
#endif
		lua_pop( L, 1 ); // get rid of the string on the stack we just pushed on line 5     
		lua_pushstring( L, cur_path.c_str() ); // push the new one     
		lua_setfield( L, -2, "path" ); // set the field "path" in table at -2 with value at top of stack     
		lua_pop( L, 1 ); // get rid of package table from top of stack     
//#endif
		return 0; // all done! 
	} 
	
	template <typename F>
	void RegisterCFunc( const char* szFunc, F func )
	{
		lua_tinker::def(L, szFunc, func);
	}
	template <typename T, typename F>
	void RegisterCPPFunc( const char* strFuncName, F func )
	{
		lua_tinker::class_def<T>( L, strFuncName, func );
	}
	template <typename F>
	void RegisterVar( const char* strVarName, F var )
	{
		lua_tinker::set( L, strVarName, var );							// "this"를 쓸수있게 등록.
	}
	// 클래스와 멤버함수를 한번등록한뒤에 다시한번 클래스를 등록시키면 멤버등록시킨거 싹 날아가는듯 하다. 중복등록시키지 말자
	template <typename T>
	void Register_Class( const char* strClassName )	// const로 바꿈 xuzhu
	{
		lua_tinker::class_add<T>( L, strClassName );										// 클래스 T를 등록
	}
	template <typename T, typename B>
	void Register_ClassInh()
	{
		lua_tinker::class_inh<T, B>(L);
	}
	template <typename T, typename B1, typename B2>
	void Register_ClassInh()
	{
		lua_tinker::class_inh<T, B1, B2>(L);
	}
	template <typename T>
	void Register_ClassCon()	{
		lua_tinker::class_con<T>(L, lua_tinker::constructor<T>);
	}
	template <typename T, typename P1>
	void Register_ClassCon()	{
		lua_tinker::class_con<T>(L, lua_tinker::constructor<T,P1>);
	}
	template <typename T, typename P1, typename P2>
	void Register_ClassCon()	{
		lua_tinker::class_con<T>(L, lua_tinker::constructor<T,P1,P2>);
	}
	template <typename T, typename P1, typename P2, typename P3>
	void Register_ClassCon()	{
		lua_tinker::class_con<T>(L, lua_tinker::constructor<T,P1,P2,P3>);
	}
	template <typename T, typename P1, typename P2, typename P3, typename P4>
	void Register_ClassCon()	{
		lua_tinker::class_con<T>(L, lua_tinker::constructor<T,P1,P2,P3,P4>);
	}
	template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5>
	void Register_ClassCon()	{
		lua_tinker::class_con<T>(L, lua_tinker::constructor<T,P1,P2,P3,P4,P5>);
	}
	template <typename T, typename F>
	void RegisterClassMem( const char* szMember, F var )
	{
		lua_tinker::class_mem<T>(L, szMember, var);
	}
	// table member call
	BOOL IsHaveMemberFunc( const char *tn, const char *fn ) {
		if( lua_tinker::member_isfunction( L, tn, fn ) == true )
			return TRUE;
		else
			return FALSE;
	}
	template <typename RVal>
	RVal MemberCall( const char *tn, const char *fn ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::member_call<RVal>( L, tn, fn );
	}
	template <typename RVal, typename T1>
	RVal MemberCall( const char *tn, const char *fn, T1 arg ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::member_call<RVal, T1>( L, tn, fn, arg );
	}
	template <typename RVal, typename T1, typename T2, typename T3>
	RVal MemberCall( const char *tn, const char *fn, T1 arg1, T2 arg2, T3 arg3 ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::member_call<RVal, T1>( L, tn, fn, arg1, arg2, arg3 );
	}
	template <typename RVal, typename T1, typename T2, typename T3, typename T4>
	RVal MemberCall( const char *tn, const char *fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4 ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::member_call<RVal, T1>( L, tn, fn, arg1, arg2, arg3, arg4 );
	}
	template <typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	RVal MemberCall( const char *tn, const char *fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5 ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::member_call<RVal, T1>( L, tn, fn, arg1, arg2, arg3, arg4, arg5 );
	}
	template <typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	RVal MemberCall( const char *tn, const char *fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6 ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::member_call<RVal, T1>( L, tn, fn, arg1, arg2, arg3, arg4, arg5, arg6 );
	}
	template <typename RVal>
	RVal Call( const char *fn ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::call<RVal>( L, fn );
	}
	template <typename RVal, typename T1>
	RVal Call( const char *fn, T1 arg ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::call<RVal, T1>( L, fn, arg );
	}
	template <typename RVal, typename T1, typename T2>
	RVal Call( const char *fn, T1 arg1, T2 arg2 ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::call<RVal, T1, T2>( L, fn, arg1, arg2 );
	}
	template <typename RVal, typename T1, typename T2, typename T3>
	RVal Call( const char *fn, T1 arg1, T2 arg2, T3 arg3 ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::call<RVal, T1, T2, T3>( L, fn, arg1, arg2, arg3 );
	}
	template <typename RVal, typename T1, typename T2, typename T3, typename T4>
	RVal Call( const char *fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4 ) {
		lua_tinker::def(L, "_ALERT", show_error);
		return lua_tinker::call<RVal, T1, T2, T3, T4>( L, fn, arg1, arg2, arg3, arg4 );
	}
	//
	template <typename RVal>
	RVal GetVar( const char *cVar ) {		// 루아내부변수 값을 읽어온다
		return lua_tinker::get<RVal>( L, cVar );
	}
	template <typename T1>
	void SetVar( const char *cVar, T1 var ) {	// 루아내부변수에 값을 넣는다
		lua_tinker::set<T1>( L, cVar, var );
	}
	//
	int PCall() {									// 
		int ret=0;
		if( (ret = lua_pcall( L, 0, LUA_MULTRET, 0 )) != 0 )
		{
			ErrorMsg( lua_tostring(L, -1) );
			return 0;
		}
		return 1;
	}
	int DoString( const char *str ) {
		lua_tinker::def(L, "_ALERT", show_error);
		lua_tinker::dostring( L, str );
		return 1;
	}
	// 
	void ClearReturn() { m_nNumArg = 0; }
	template<typename T>
	void PushReturn( T arg1 ) {
		lua_tinker::push( L, arg1 );
		m_nNumArg ++;
	}

	xtResume Resume( const char *szFunc ) {	// szFunc함수를 resume으로 실행한다. nullptr이면그냥 resume이다
		if( szFunc && XE::IsHave(szFunc) ) {
			lua_pushstring( L, szFunc );
			lua_gettable( L, LUA_GLOBALSINDEX );
		}
		int ret = lua_resume( L, m_nNumArg );
		m_nNumArg = 0;
		if( ret == LUA_YIELD )
			return xYIELD;
		if( ret == 0 )
			return xFINISH;
		const char *szErr = lua_tostring(L, -1);		
		ErrorMsg( szErr );
		return xERROR;
	}
	template<typename T1>
	xtResume Resume( const char *szFunc, T1 arg1 ) {	// szFunc함수를 resume으로 실행한다. nullptr이면그냥 resume이다
		if( szFunc && XE::IsHave(szFunc) ) {
			lua_pushstring( L, szFunc );
			lua_gettable( L, LUA_GLOBALSINDEX );
			lua_tinker::push( L, arg1 );
			m_nNumArg = 1;
		}
		int ret = lua_resume( L, m_nNumArg );
		m_nNumArg = 0;
		if( ret == LUA_YIELD )
			return xYIELD;
		if( ret == 0 )
			return xFINISH;
		const char *szErr = lua_tostring(L, -1);		
		ErrorMsg( szErr );
		return xERROR;
	}
	template<typename T1, typename T2>
	xtResume Resume( const char *szFunc, T1 arg1, T2 arg2 ) {	// szFunc함수를 resume으로 실행한다. nullptr이면그냥 resume이다
		if( szFunc && XE::IsHave(szFunc) ) {
			lua_pushstring( L, szFunc );
			lua_gettable( L, LUA_GLOBALSINDEX );
			lua_tinker::push( L, arg1 );
			lua_tinker::push( L, arg2 );
			m_nNumArg = 2;
		}
		int ret = lua_resume( L, m_nNumArg );
		m_nNumArg = 0;
		if( ret == LUA_YIELD )
			return xYIELD;
		if( ret == 0 )
			return xFINISH;
		const char *szErr = lua_tostring(L, -1);		
		ErrorMsg( szErr );
		return xERROR;
	}
};

#endif // __LUASCRIPT_H__
