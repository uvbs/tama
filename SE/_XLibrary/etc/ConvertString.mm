/*
 *  EUCKR2UTF8.mm
 *  BnBTest
 *
 *  Created by xuzhu on 09. 07. 21.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "ConvertString.h"

#define NSEUCKREncoding		-2147481280

// EUC-KR스트링을 UTF-8스트링으로 변환한다.
int _GetSizeEUCKRToUTF8( LPCSTR euckr_str )
{
	NSString *nsstring = [[NSString alloc] initWithCString:euckr_str
												  encoding:NSEUCKREncoding];
	if( nsstring == NULL )
	{
		XLOG( "already UTF-8 string:%s", euckr_str );
		return strlen( euckr_str );		// 이미 utf8이므로 이것의 길이를 리턴한다
	}
	LPCSTR utf8String = [nsstring cStringUsingEncoding:NSUTF8StringEncoding ];
	[nsstring release];
	return strlen(utf8String);
}
const char*	_ConvertEUCKRToUTF8( char *utf8_str, LPCSTR euckr_str, int size )
{
	NSString *nsstring = [[NSString alloc] initWithCString:euckr_str
												  encoding:NSEUCKREncoding];
	if( nsstring == NULL )
	{
		XLOG( "already UTF-8 string:%s", euckr_str );
		strcpy( utf8_str, euckr_str );	// euckr_str이 이미 UTF-8인경우 그냥 카피해서 돌려준다.( LoadSprite("직접입력.spr"); 일경우임 )
		return utf8_str;
	}
	LPCSTR utf8String = [nsstring cStringUsingEncoding:NSUTF8StringEncoding ];
    int len = strlen(utf8String);
	if( len >= size )
		XERROR( "string length(%d)is too long.", strlen(utf8_str) );
	strcpy( utf8_str, utf8String );
	[nsstring release];
	return utf8_str;
}

int UnicharLength( const unichar *unistr )
{
	unichar *pstr = (unichar *)unistr;
	for( ; pstr[0] != 0; pstr++ );
	return pstr - unistr;
}
// UTF16(유니코드)스트링을 UTF-8스트링으로 변환한다.
LPCSTR	_ConvertUTF16ToUTF8( unichar *utf16_str )
{
	static CHAR utf8_str[0xffff*2];	// prop.lua가 64k를 넘어가서 늘임
	NSString *nsstring = [NSString stringWithCharacters:utf16_str length:(NSInteger)UnicharLength(utf16_str)];
	if( nsstring == NULL )
	{
		XLOG( "UTF16ToUTF8 encoding failed:%s", utf16_str );
		*utf8_str = 0;
		return utf8_str;
	}
	LPCSTR utf8String = [nsstring cStringUsingEncoding:NSUTF8StringEncoding ];
	if( strlen(utf8String) >= sizeof(utf8_str) )
		XERROR( "%s 스트링길이(%d)가 너무길다.", utf8_str, strlen(utf8_str) );
	strcpy( utf8_str, utf8String );
//	[nsstring release];
	return utf8_str;
}
