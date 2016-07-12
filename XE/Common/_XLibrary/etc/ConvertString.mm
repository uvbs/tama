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
	return strlen(utf8String);
}
const char*	_ConvertEUCKRToUTF8( char *utf8_str, LPCSTR euckr_str, int sizeBuff )
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
	if( len >= sizeBuff )
		XERROR( "string length(%d)is too long.", strlen(utf8String) );
	strcpy_s( utf8_str, sizeBuff, utf8String );
	return utf8_str;
}

int UnicharLength( const unichar *unistr )
{
	unichar *pstr = (unichar *)unistr;
	for( ; pstr[0] != 0; pstr++ );
	return pstr - unistr;
}
// UTF16(유니코드)스트링을 UTF-8스트링으로 변환한다.
/*LPCSTR	_ConvertUTF16ToUTF8( unichar *utf16_str )
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
}*/
size_t UnicodeToUTF8(unichar uc, char* UTF8)
{
    size_t tRequiredSize = 0;
    
    if (uc <= 0x7f)
    {
        if( NULL != UTF8 )
        {
            UTF8[0] = (char) uc;
            UTF8[1] = (char) '\0';
        }
        tRequiredSize = 1;
    }
    else if (uc <= 0x7ff)
    {
        if( NULL != UTF8 )
        {
            UTF8[0] = (char)(0xc0 + uc / (0x01 << 6));
            UTF8[1] = (char)(0x80 + uc % (0x01 << 6));
            UTF8[2] = (char) '\0';
        }
        tRequiredSize = 2;
    }
    else if (uc <= 0xffff)
    {
        if( NULL != UTF8 )
        {
            UTF8[0] = (char)(0xe0 + uc / (0x01 <<12));
            UTF8[1] = (char)(0x80 + uc / (0x01 << 6) % (0x01 << 6));
            UTF8[2] = (char)(0x80 + uc % (0x01 << 6));
            UTF8[3] = (char) '\0';
        }
        tRequiredSize = 3;
    }
    
    return tRequiredSize;
}

size_t UnicodeStrToUTF8Str(unichar *szUni,char *szUTF8)
{
    size_t tRequiredSize = 0;
    
    int i=0;
    for(i=0;szUni[i];i++)
    {
        size_t tSize = 0;
        if( NULL != szUTF8 )
            tSize = UnicodeToUTF8(szUni[i], szUTF8 + tRequiredSize);
        else
            tSize = UnicodeToUTF8(szUni[i], NULL);
        tRequiredSize += tSize;
    }
    return tRequiredSize;
}

/*
size_t UTF8StrToUnicodeStr(char* szUTF8, size_t tUTF8Len, wchar_t* szUni)
{
    size_t tReadPos = 0;
    
    size_t i=0;
    for(i=0; tReadPos<tUTF8Len; i++)
    {
        wchar_t tTempUnicodeChar = 0;
        size_t tSize = UTF8ToUnicode(szUTF8 + tReadPos, tTempUnicodeChar);
        if( NULL != szUni )
            szUni[i] = tTempUnicodeChar;
        tReadPos += tSize;
    }
    return i;
}
*/
const char* _ConvertUTF16ToUTF8( char *pOutUtf8, unichar *utf16_str, int sizeBuff )
{
    
    UnicodeStrToUTF8Str(utf16_str, pOutUtf8);
    return pOutUtf8;
/*	NSString *nsstring = [NSString stringWithCharacters:utf16_str length:(NSInteger)UnicharLength(utf16_str)];
	if( nsstring == NULL )
	{
		XLOG( "UTF16ToUTF8 encoding failed:%s", utf16_str );
		*pOutUtf8 = 0;
		return pOutUtf8;
	}
	LPCSTR utf8String = [nsstring cStringUsingEncoding:NSUTF8StringEncoding ];
	if( strlen(utf8String) >= sizeBuff )
		XERROR( "[%s] string length(%d)is too long.", utf8String, strlen(utf8String) );
	strcpy_s( pOutUtf8, sizeBuff, utf8String );
    //	[nsstring release];
	return pOutUtf8;
 */
}

char* CreateConvertUTF16toUTF8( unichar *utf16_str )
{
	int size = UnicodeStrToUTF8Str( utf16_str, NULL );
	if( XBREAK( size == 0 ) )
		return NULL;
	char *utf8 = new char[ size + 1 ];
	_ConvertUTF16ToUTF8( utf8, utf16_str, size+1 );
	return utf8;
}
