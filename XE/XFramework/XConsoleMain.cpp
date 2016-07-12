#include "stdafx.h"
#include "XFramework/XConsoleMain.h"
#include "XFramework/XDetectDeadLock.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XConsoleMain *CONSOLE_MAIN = NULL;

//////////////////////////////////////////////////////////////////////////
XConsoleMain::XConsoleMain()
{
	CONSOLE_MAIN = this;
	Init();
#ifdef _SERVER
	LoadTags();		// 일단은 서버에서만 사용하는걸로.
	LoadidAcc();
#endif // _SERVER
#if defined(_SERVER) || defined(_XBOT)
	ID idThread = ::GetCurrentThreadId();
	m_idThreadMain = idThread;
	XDetectDeadLock::sGet()->OnCreate();
	XDetectDeadLock::sGet()->AddThread( idThread, nullptr, _T( "main" ) );
#endif // _SERVER
}

XLibrary* XConsoleMain::CreateXLibrary( void )
{
	return new XLibConsole;
}

void XConsoleMain::Create( void )
{
	XLIB = CreateXLibrary();
	XE::xtLoadType typeLoad = XE::xLT_PACKAGE_ONLY;
	for( int i = 0; i < XE::xLT_MAX; ++i ) {
		if( OnSelectLoadType( ( XE::xtLoadType ) i ) ) {
			typeLoad = ( XE::xtLoadType ) i;
			break;
		}
	}
	XE::InitResourceManager( this, typeLoad );

	//	XArrayLinear<_tstring> ary;
	//	LPCTSTR pathPackage = XE::GetPathPackageRoot();
	//	int num = XSYSTEM::CreateFileList( pathPackage, _T("*.*"), &ary );
	//
	const auto seedNew = xGenerateSeed();
	xSRand( seedNew );
	srand( seedNew );
	// 	xSRand( timeGetTime() );
	// 	srand( timeGetTime() );

	CTimer::Initialize();		// pause가능한 가상타이머를 초기화한다

	// text_ko같은것도 앞으로 이쪽으로 옮겨올 예정이므로 그것도 고려해야함.
	DidFinishCreate();		// this의 Create가 끝난후 다른 Create가 있다면 하위클래스에 맡긴다.

}

void XConsoleMain::Destroy()
{
	XTRACE( "destroy XLIB\n" );
	SAFE_DELETE( XLIB );
}


#ifdef _SERVER
void XConsoleMain::LoadTags()
{
	m_bLoadingLog = true;
// 	std::string strPath = XE::GetstrCwd( );
// 	strPath += "//tags.txt";
	// working폴더에서 읽는다.
	FILE *fp = nullptr;
	const std::string strFile( "list_tags.txt" );
	fopen_s( &fp, strFile.c_str(), "rt" );
	if( fp ) {
		char buff[1024];
		m_listTags.clear();
		while(1) {
			if( fgets( buff, 1024, fp ) == nullptr )
				break;
			const int len = strlen(buff);
			const char c = buff[len-1];
			if( len > 0 ) {
				if( c == '\r' || c == '\n' || c == '\t' )
					buff[len-1] = 0;
			}
			m_listTags.Add( std::string(buff) );
		}
		fclose(fp);
//		CONSOLE( "[%s]: load complete. numTag=%d", C2SZ(strFile), m_listTags.size() );
	} else {
//		CONSOLE("not found or error [%s]", C2SZ(strFile) );
	}
	m_bLoadingLog = false;
}

void XConsoleMain::LoadidAcc()
{
	m_bLoadingLog = true;
	// 	std::string strPath = XE::GetstrCwd( );
	// 	strPath += "//tags.txt";
	// working폴더에서 읽는다.
	const std::string strFile("list_idacc.txt");
	FILE *fp = nullptr;
	fopen_s( &fp, strFile.c_str(), "rt" );
	if( fp ) {
		char buff[1024];
		m_listLogidAcc.clear();
		while( 1 ) {
			if( fgets( buff, 1024, fp ) == nullptr )
				break;
			int idAcc = std::atoi( buff );
			m_listLogidAcc.Add( (ID)idAcc );
		}
		fclose( fp );
//		CONSOLE( "[%s]: load complete. numTag=%d", C2SZ(strFile), m_listTags.size() );
	} else {
//		CONSOLE( "not found or error [%s]", C2SZ(strFile) );
	}
	m_bLoadingLog = false;
}

/**
 @brief 로그로 남겨야할 계정아이디중에 idAcc가 있는가.
*/
bool XConsoleMain::IsLogidAcc( ID idAcc )
{
	if( m_bLoadingLog )
		return false;
	return m_listLogidAcc.Findp( idAcc ) != nullptr;
}

void XConsoleMain::AddLogidAcc( ID idAcc ) {
	m_listLogidAcc.Add( idAcc );
}

void XConsoleMain::DelLogidAcc( ID idAcc ) {
	m_listLogidAcc.Del( idAcc );
}
#endif // _SERVER

/**
 @brief strTag내에 리스트에 있는 tag가 포함되어있는가.
*/
bool XConsoleMain::IsHaveTag( const char* strTag )
{
	if( m_bLoadingLog )
		return false;
	for( auto& tag : m_listTags ) {
		if( strstr(strTag, tag.c_str()) ) {
			return true;
		}
	}
	return false;
}

void PrintLockFree( LPCTSTR type, int lockFree )
{
	if( lockFree != 2 )
	{
		if( lockFree == 0 )
			XLOG( "%s: lock-free 아님.", type );
		else
			XLOG( "%s: lock-free일수도 있음.", type );
	}
}

/**
 @brief atomic지원 검사.
*/
void XConsoleMain::CheckLockFree()
{
	PrintLockFree( _T( "bool" ), ATOMIC_BOOL_LOCK_FREE );
	PrintLockFree( _T( "char" ), ATOMIC_CHAR_LOCK_FREE );
	PrintLockFree( _T( "char16" ), ATOMIC_CHAR16_T_LOCK_FREE );
	PrintLockFree( _T( "char32" ), ATOMIC_CHAR32_T_LOCK_FREE );
	PrintLockFree( _T( "wchar_t" ), ATOMIC_WCHAR_T_LOCK_FREE );
	PrintLockFree( _T( "short" ), ATOMIC_SHORT_LOCK_FREE );
	PrintLockFree( _T( "int" ), ATOMIC_INT_LOCK_FREE );
	PrintLockFree( _T( "long" ), ATOMIC_LONG_LOCK_FREE );
	PrintLockFree( _T( "long long" ), ATOMIC_LLONG_LOCK_FREE );
	PrintLockFree( _T( "T*" ), ATOMIC_POINTER_LOCK_FREE );
}
