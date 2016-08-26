/*
 for android
 */
#include "stdafx.h"
#ifdef _VER_ANDROID
#include "../etc/ConvertString.h"
#include "XFramework/android/JniHelper.h"

//#define BTRACE(F, ...)					XTRACE( _T(F), ##__VA_ARGS__ )
#define BTRACE(F, ...)					(0)

// EUC-KR스트링을 UTF-8스트링으로 변환한다.
int _GetSizeEUCKRToUTF8( LPCSTR euckr_str )
{
    char buff[0x10000];
		BTRACE("_GetSizeEUCKRToUTF8( LPCSTR euckr_str )");
		BTRACE( ";;;" );
    char *utf8 = JniHelper::Create_EUCKR_To_UTF8( buff, euckr_str );
		BTRACE( "JniHelper::Create_EUCKR_To_UTF8( buff, euckr_str );2" );
    if( XBREAK( utf8 == NULL ) )
        return 0;
    int len = strlen( utf8 );
    return len;
}
const char*	_ConvertEUCKRToUTF8( char *utf8_str, LPCSTR euckr_str, int sizeBuff )
{
//    XLOGXN("convert euckr to utf8");
    char buff[0x10000*2];
    char *utf8 = JniHelper::Create_EUCKR_To_UTF8( buff, euckr_str );
    if( XBREAK( utf8 == NULL ) )
        return 0;
    int len = strlen( utf8 );
	if( len >= sizeBuff )
    {
		XERROR( "utf8 string length(%d)is too long.", strlen(utf8) );
        return 0;
    }
    if( len == 0 )
    {
        XERROR( "failed. utf8 length 0");
        return NULL;
    }
	strcpy_s( utf8_str, sizeBuff, utf8 );
    return utf8_str;
}
/*
int UnicharLength( const unichar *unistr )
{
	unichar *pstr = (unichar *)unistr;
	for( ; pstr[0] != 0; pstr++ );
	return pstr - unistr;
}
 */
/*
// UTF16(유니코드)스트링을 UTF-8스트링으로 변환한다.
LPCSTR	_ConvertUTF16ToUTF8( unichar *utf16_str )
{
	static CHAR utf8_str[0x10000*2];	// prop.lua가 64k를 넘어가서 늘임
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
*/
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

int Utf8ToUnicode( const char *utf8, WCHAR *utf16Buff )
{
	int p = 0;
	int q = 0;
	WCHAR d;
	while( utf8[ p ] ) {
		if( ( utf8[ p ] & 0xE0 ) == 0xE0 ) {
			d = ( ( utf8[ p ] & 0x0f ) << 12 ) | ( ( utf8[ p + 1 ] & 0x3F ) << 6 ) | ( utf8[ p + 2 ] & 0x3F );
			p += 3;
		}
		else if( ( utf8[ p ] & 0xC0 ) == 0xC0 ) {
			d = ( ( utf8[ p ] & 0x1F ) << 6 ) | ( utf8[ p + 1 ] & 0x3F );
			p += 2;
		}
		else {
			d = utf8[ p ] & 0x7F;
			p++;
		}
		utf16Buff[ q ] = d;
		q++;
	}
	utf16Buff[ q ] = 0;
	return q;
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

const char* _ConvertUTF16ToUTF8( char *pOutUtf8, unichar *utf16_str, int sizeBuff )
{
//    XLOGXN(":convert utf16 to utf8");
    if( utf16_str[0] == 0 )
    {
        pOutUtf8[0] = 0;
        return pOutUtf8;
    }
    int size = UnicodeStrToUTF8Str( utf16_str, pOutUtf8 );
	if( size >= sizeBuff )
    {
		XERROR( "utf8 string length(%d)is too long.", size );
        return NULL;
    }
    if( size <= 0 )
    {
        XERROR( "failed. utf8 length 0");
        return NULL;
    }
    return pOutUtf8;
}

// utf16을 utf8로 변환한다 필요한 메모리를 내부에서 할당받는다.
char* CreateConvertUTF16toUTF8( unichar *utf16_str )
{
	// utf8로 변환하는데 필요한 버퍼 크기를 구한다.
	int size = UnicodeStrToUTF8Str( utf16_str, NULL );
	if( XBREAK( size == 0 ) )
		return NULL;
	char *utf8 = new char[ size + 1 ];
	_ConvertUTF16ToUTF8( utf8, utf16_str, size+1 );
	return utf8;
}

#endif // ver_android