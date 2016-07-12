#include "Debug.h"

#import <UIKit/UIKit.h>

int AfxDebugBreak() { 
    NSLog( @"DebugBreak" );
    return 0; 
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
	}
    va_end(vl);
}
