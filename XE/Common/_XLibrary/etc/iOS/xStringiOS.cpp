/*
 *  String.cpp
 *  Skinning
 *
 *  Created by xuzhu on 10. 7. 20..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#ifndef WIN32
#include "string.h"

void sprintf_s( char *cOut, int len, const char *cFormat, ... ) {
	XBREAK( cOut == nullptr );
	char cBuff[1024];
	va_list vl;
	va_start( vl, cFormat );
	vsprintf_s( cBuff, cFormat, vl );
	va_end( vl );
	XBREAK( strlen(cBuff) >= len );
	strcpy( cOut, cBuff );
}
void sprintf_s( char *cOut, const char *cFormat, ... ) {
	XBREAK( cOut == nullptr );
	char cBuff[1024];
	va_list vl;
	va_start( vl, cFormat );
	vsprintf_s( cBuff, cFormat, vl );
	va_end( vl );
	strcpy( cOut, cBuff );
}

CHAR* strlwr( CHAR* pstring )
{
	CHAR *cp;
	
	for(cp=pstring;*cp;++cp)
	{
		if('A'<= *cp && *cp<= 'Z')
		{
			*cp += 'a' - 'A';
		}
	}
	return pstring;
}
CHAR* strupr( CHAR* pstring )
{
	CHAR *cp;
	
	for(cp=pstring;*cp;++cp)
	{
		if('A'<= *cp && *cp<= 'Z')
		{
			*cp += 'A' - 'a';
		}
	}
	return pstring;
}

int stricmp(LPCSTR str1, LPCSTR str2)
{
    int i, cmp;
    for(i=0; *(str1+i)!=0 || *(str2+i)!=0; i++){
        cmp = *(str1+i) - *(str2+i);
        if( cmp == -32 || cmp == 32 ){  // 대소문자 구별 없이
			
            if(((*(str1+i)>=65 && *(str1+i)<=90) || (*(str1+i)>=97 && *(str1+i)<=122)) &&
               ((*(str2+i)>=65 && *(str2+i)<=90) || (*(str2+i)>=97 && *(str2+i)<=122))) {
                cmp = 0;
            }
            else {
                cmp = ( cmp>0 ) ? 1 : -1;
                break;
            }
        }
        else if( cmp!=0 ){
            cmp = ( cmp>0 ) ? 1 : -1;
            break;
        }
    }
	
    return cmp;
	
}

void strrev( char *str )
{
    int len = strlen( str );
    for( int i = 0; i < len/2; ++i )
    {
        char c = str[i];
        str[i] = str[ (len-1) - i ];
        str[ (len-1) - i ] = c;
    }
}

int wcslen( const unichar* szStr )
{
    int len = 0;
    while( *szStr++ )
        ++len;
    return len;
}


#endif // not WIN32
