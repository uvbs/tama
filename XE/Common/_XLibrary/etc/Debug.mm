#import <UIKit/UIKit.h>
#include "Debug.h"
//#include "path.h"
#include "XLockPThreadMutex.h"
#ifdef _LOG_ALERT
//#import "GameViewController.h"
#endif
#include "XLibrary.h"
#include "XLock.h"

static XLock s_Lock;
#define XMLOCK_OBJ()  XLockObj lockObj(&s_Lock)

int AfxDebugBreak() { 
	XMLOCK_OBJ();
    NSLog( @"DebugBreak" );
    return 0; 
}

namespace XIOS
{
	id x_idAlertDelegate = nil;
	void SetAlertDelegate( id delegate )
	{
		x_idAlertDelegate = delegate;
	}
	id GetAlertDelegate( void ) { return x_idAlertDelegate; }

};

void xPutsTimeString( FILE *fp )
{
	XMLOCK_OBJ();
	char            szStr[8192];
	SYSTEMTIME		time;
	GetLocalTime( &time );
	sprintf_s( szStr, "%d/%2d/%2d   %02d:%02d:%02d   \r\n", 
			  time.wYear, time.wMonth, time.wDay,
			  time.wHour, time.wMinute, time.wSecond );
	fputs( szStr, fp );
 
}

const char* XE::GetLogFilename( char *pOut, int len )
{
	strcpy_s( pOut, len, "error.txt" );
	return pOut;
}

int __xLibConsole( LPCTSTR szFormat, ... )
{
	char szBuff[0xffff];	// utf8이 길어서 넉넉하게 잡았다.
	va_list vl;
	
	va_start(vl, szFormat);
	vsprintf_s(szBuff, szFormat, vl);
	va_end(vl);
	if( XLIB )
		XLIB->Console( "%s", szBuff );
	return 1;
}

void _NSLog( const char *str, ... )
{
	XMLOCK_OBJ();
	char            szBuff[0xffff];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    vsprintf_s(szBuff, str, vl);
	@try {
		NSString *strObj = [[NSString alloc] initWithUTF8String:szBuff];
		if( strObj )
		{
			NSLog( @"%@", strObj );
		} else
			NSLog( @"__xLog NSString 생성실패. 문자열이 utf-8이 아닐수 있습니다 %s", szBuff );
	}
	@catch (NSException * e) {
//		NSDictionary *d = [NSDictionary dictionaryWithObject:@"Error xLog"
//													  forKey:NSLocalizedFailureReasonErrorKey];
//		error = [NSError errorWithDomain:NSOSStatusErrorDomain 
//									code:unimpErr 
//								userInfo:d];
	}
    va_end(vl);
}


// str로 들어오는 스트링은 모두 UTF-8로 가정한다.
int __xLog( int type, LPCSTR str, ...)
{
	XMLOCK_OBJ();
    char            szBuff[0xffff/4];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    vsprintf(szBuff, str, vl);
#ifndef DEBUG 
	// 릴리즈 모드에선 로그나 에러모두 파일에 써야 한다
	{
		FILE *fp;
        // 이안에서 MakePath류 쓰지 말것.
        TCHAR szError[ 1024 ];
		char cFile[ 256 ];
        _tcscpy_s( szError, XE::GetPathDoc() );
		XE::GetLogFilename( cFile );
        strcat_s(szError, cFile );
		fopen_s( &fp, szError, "a" );
        if( fp )
		{
			xPutsTimeString( fp );
            fputs( szBuff, fp );
            fclose(fp);
		}
	}
#endif // not debug
	
#ifdef _VER_IOS
//	NSError *error;
	@try {
		NSString *strObj = [[NSString alloc] initWithUTF8String:szBuff];
		if( strObj )
		{
			NSLog( @"%@", strObj );
#ifdef _LOG_ALERT
			if( type == XLOGTYPE_ERROR || type == XLOGTYPE_ALERT )
			{
				UIAlertView *baseAlert = [[UIAlertView alloc] 
										  initWithTitle:@"알림" message:strObj 
										  delegate:XIOS::x_idAlertDelegate cancelButtonTitle:nil
//										  delegate:g_pViewController cancelButtonTitle:nil
										  otherButtonTitles:@"확인", nil];
				[baseAlert show];
			}
#endif
		} else
        {
			NSLog( @"__xLog NSString create fail. string is not utf-8." );
        }
	}
	@catch (NSException * e) {
//		NSDictionary *d = [NSDictionary dictionaryWithObject:@"Error xLog"
//													  forKey:NSLocalizedFailureReasonErrorKey];
//		error = [NSError errorWithDomain:NSOSStatusErrorDomain 
//									code:unimpErr 
//								userInfo:d];
	}
#else
	DebugOutput( "%s", szBuff );;
#endif
    va_end(vl);

#ifndef _LOG_ALERT			// Alert창이 뜨는상태면 굳이 exit안해도 된다.
//	if( type == XLOGTYPE_ERROR )	// 폰트같은건 설사 spr읽기를 실패해도 게임은 계속 될수 있도록 하기위해 exit를 뺐다
//		exit(1);
#endif // not _LOG_ALERT
	return 1;
}

