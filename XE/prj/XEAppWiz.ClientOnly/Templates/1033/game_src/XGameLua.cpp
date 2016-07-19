#include "stdafx.h"
#include "XGameLua.h"
#include "XGameCommon.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

namespace LUA
{
	int Random( short num ) 
	{
		return random( num );
	}
	int RandomRange( short min, short max )
	{
		return random( min, max );
	}
	//-----------------------------------------------------------------------
	int Console( lua_State *L )
	{
		const char *typeName = lua_typename( L, lua_type(L, 1) );
		if( lua_isnumber( L, 1 ) )
		{
			float arg1 = (float)lua_tonumber( L, 1 );
//	#ifdef WIN32
//			_NSLog( "%f", arg1 );
//	#else
			CONSOLE( "Console:%f", arg1 );
//	#endif
		} else
		if( lua_isstring( L, 1 ) )
		{
			const char *arg1 = (const char *)lua_tostring( L, 1 );
			
			CONSOLE( "Console:%s", Convert_char_To_TCHAR( arg1 ) );
		} else
		if( lua_isuserdata( L, 1 ) )
		{
			void *p = lua_touserdata( L, 1 );
			CONSOLE( "Console:0x%08x", (unsigned long)p );
			int a = 0; a = 1;
		} else
		if( lua_isnil( L, 1 ) )
		{
			CONSOLE("Console: nil");
		} else
		{
			void *p = lua_touserdata( L, 1 );
			CONSOLE("Console: unkwoun type");
		}
		return 1;
	}
	void* XText( ID idText )
	{
        TCHAR *p = (TCHAR*)XTEXT( idText );
		return (void*)p;
    }

}

//////////////////////////////////////////////////////////////////////////
XGameLua::XGameLua() 
{ 
	Init(); 
	TCHAR szFullPath[ 1024 ];
#ifdef _SERVER
	_tcscpy_s( szFullPath, XE::_GetPathPackageRoot() );
	_tcscat_s( szFullPath, XE::MakePath( DIR_SCRIPTW, _T("") ) );
	SetModulePath( SZ2C( szFullPath ) );
#else
	_tcscpy_s( szFullPath, XE::GetPathWork() );
	_tcscat_s( szFullPath, XE::MakePath( DIR_SCRIPTW, _T("") ) );
	SetModulePath( SZ2C( szFullPath ) );
#endif
	//		RegisterGlobal();
}

XGameLua::XGameLua( const char *cLua ) 
{ 
	Init(); 
	/*
	루아 내부에서 다른루아를 include한다고 해도 어차피 마스터본에는 lua파일은 암호화되어 배포되어 include하지 못한다.
	그러므로 사전 컴파일된 루아 바이너리를 쓰는 방법을 찾아봐야 할듯.
	*/
	TCHAR szFullPath[ 1024 ];
#ifdef _SERVER
	_tcscpy_s( szFullPath, XE::_GetPathPackageRoot() );
	_tcscat_s( szFullPath, XE::MakePath( DIR_SCRIPTW, _T("") ) );
#else
	switch( XE::GetLoadType() )
	{
	case XE::xLT_PACKAGE_ONLY:
#if defined(WIN32) || defined(_VER_IOS)
		_tcscpy_s( szFullPath, XE::_GetPathPackageRoot() );
		_tcscat_s( szFullPath, XE::MakePath( DIR_SCRIPTW, _T("") ) );
#else
			XBREAKF(1, "do not use! package only type");
#endif
		break;
	case XE::xLT_WORK_FOLDER_ONLY:
	case XE::xLT_WORK_TO_PACKAGE:
	case XE::xLT_WORK_TO_PACKAGE_COPY:
		_tcscpy_s( szFullPath, XE::MakeWorkFullPath( DIR_SCRIPTW, _T("") ) );
		break;
	default:
		XBREAKF( 1, "unknown loadtype" );
	}
#endif
	XLOGXN("lua module path.....%s", szFullPath );
	SetModulePath( SZ2C( szFullPath ) );
	LoadScript( XE::MakePath( DIR_SCRIPTA, cLua ) );
//	RegisterGlobal();
}

void XGameLua::RegisterGlobal( void )
{
	XLua::RegisterGlobal();
	// C함수 등록
	RegisterCFunc( "Random", LUA::Random );
	RegisterCFunc( "RandomRange", LUA::RandomRange );
	RegisterCFunc( "Console", LUA::Console );
	RegisterCFunc( "XTEXT", LUA::XText );

//	Register_Class<XAccount>("XAccount");
	//RegisterCPPFunc<XAccount>("LuaFunc", &XAccount::LuaFunc );

	//
	RegisterVar( "TRUE", TRUE );		// 상수 등록
	RegisterVar( "FALSE", FALSE );
	//
}

