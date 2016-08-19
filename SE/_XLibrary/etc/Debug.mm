#include "Debug.h"
#include "path.h"

#ifdef _LOG_ALERT
#import "GameViewController.h"
#endif

int AfxDebugBreak() { 
    NSLog( @"DebugBreak" );
    return 0; 
}

void xPutsTimeString( FILE *fp )
{
/*	char            szStr[8192];
	SYSTEMTIME		time;
	GetLocalTime( &time );
	sprintf_s( szStr, "%d/%2d/%2d   %02d:%02d:%02d   \n", 
			  time.wYear, time.wMonth, time.wDay,
			  time.wHour, time.wMinute, time.wSecond );
	fputs( szStr, fp );
 */
}

void _NSLog( const char *str, ... )
{
	char            szBuff[8192];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    vsprintf(szBuff, str, vl);
	@try {
		NSString *strObj = [[NSString alloc] initWithUTF8String:szBuff];
		if( strObj )
		{
			NSLog( @"%@", strObj );
			[strObj release];
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
    char            szBuff[0xffff];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    vsprintf(szBuff, str, vl);
#ifndef DEBUG 
	// 릴리즈 모드에선 로그나 에러모두 파일에 써야 한다
	{
		FILE *fp;
		fopen_s( &fp, XE::MakeDocumentPath("error.txt"), "a" );
		{
			xPutsTimeString( fp );
		}
		fputs( szBuff, fp );
		
		fclose(fp);
	}
#endif // not debug
	
#ifdef _VER_MAC
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
										  delegate:g_pViewController cancelButtonTitle:nil 
										  otherButtonTitles:@"확인", nil];
				[baseAlert show];
			}
#endif
			[strObj release];
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

