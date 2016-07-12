#import <UIKit/UIKit.h>
#include "Alert.h"
#include "debug.h"

#ifndef _NO_ALERT
//#import "GameViewController.h"
#endif

namespace XIOS
{
	void SetAlertDelegate( id delegate );
	id GetAlertDelegate( void );
};

#ifdef _CLIENT
#include "XClientMain.h"
static XClientMain *s_pHandler = NULL;
void SetAlertHandler( XClientMain *pHandler )
{
    s_pHandler = pHandler;
}
XClientMain* GetAlertHandler( void )
{
    return s_pHandler;
}

#endif

// str로 들어오는 스트링은 모두 UTF-8로 가정한다.
int XAlert( int type, LPCSTR str, ...)
{
#ifndef _NO_ALERT
    char            szBuff[8192];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    vsprintf(szBuff, str, vl);
	
	@try {
		NSString *strObj = [[NSString alloc] initWithUTF8String:szBuff];
		if( strObj )
		{
            UIAlertView *baseAlert = [[UIAlertView alloc] 
                                      initWithTitle:@"알림" message:strObj 
                                      delegate:XIOS::GetAlertDelegate() cancelButtonTitle:nil
                                      otherButtonTitles:@"확인", nil];
            [baseAlert show];
            NSLog( @"%@", strObj );
		} else
			NSLog( @"xAlert NSString 생성실패. 문자열이 utf-8이 아닐수 있습니다 %s", szBuff );
	}
	@catch (NSException * e) {
//		NSDictionary *d = [NSDictionary dictionaryWithObject:@"Error xLog"
//													  forKey:NSLocalizedFailureReasonErrorKey];
//		error = [NSError errorWithDomain:NSOSStatusErrorDomain 
//									code:unimpErr 
//								userInfo:d];
	}
    va_end(vl);
#endif 
    //
	return 1;
}

// str로 들어오는 스트링은 모두 UTF-8로 가정한다.
int XAlert( XE::xtAlert type, LPCSTR str, ...)
{
#ifndef _NO_ALERT
    char            szBuff[8192];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    vsprintf(szBuff, str, vl);
	
	@try {
		NSString *strObj = [[NSString alloc] initWithUTF8String:szBuff];
		if( strObj )
		{
            UIAlertView *baseAlert = [[UIAlertView alloc]
                                      initWithTitle:@"알림" message:strObj
                                      delegate:XIOS::GetAlertDelegate() cancelButtonTitle:nil
                                      otherButtonTitles:@"확인", nil];
            [baseAlert show];
            NSLog( @"%@", strObj );
		} else
			NSLog( @"xAlert NSString 생성실패. 문자열이 utf-8이 아닐수 있습니다 %s", szBuff );
	}
	@catch (NSException * e) {
        //		NSDictionary *d = [NSDictionary dictionaryWithObject:@"Error xLog"
        //													  forKey:NSLocalizedFailureReasonErrorKey];
        //		error = [NSError errorWithDomain:NSOSStatusErrorDomain
        //									code:unimpErr
        //								userInfo:d];
	}
    va_end(vl);
#endif 
    //
	return 1;
}

