/*  ConvertString.h*/
/*
 *  BnBTest
 *
 *  Created by xuzhu on 09. 07. 21.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */

#pragma once

#define C2SZ	Convert_char_To_TCHAR
#define SZ2C	Convert_TCHAR_To_char
#ifdef WIN32
//  #define SZ2U8(TSTR)		ConvertUTF16toUTF8( TSTR )
  #define U82SZ(UTF8)		ConvertUTF8ToUTF16( UTF8 )
#else
// iOS나 Android는 TCHAR가 utf8이므로 변환할필요 없다.
//  #define SZ2U8(TSTR)		(TSTR == nullptr)? "" : TSTR
  #define U82SZ(UTF8)		(!UTF8)? "" : UTF8
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef WIN32
//#ifdef _VER_IOS
	const char *_ConvertEUCKRToUTF8( char *utf8_str, LPCSTR euckr_str, int size );
	int _GetSizeEUCKRToUTF8( LPCSTR euckr_str );
//	const char* _ConvertUTF16ToUTF8( UNICHAR *utf16_str );
    const char* _ConvertUTF16ToUTF8( char *pOutUtf8, UNICHAR *utf16_str, int sizeBuff );
    #ifdef __cplusplus
    template <int _Size>
    inline const char* _ConvertUTF16ToUTF8( char (&tBuff)[_Size], UNICHAR *utf16_str ) {
        return _ConvertUTF16ToUTF8( tBuff, utf16_str, _Size );
    }
    #endif
	char* CreateConvertUTF16toUTF8( unichar *utf16_str );
#endif // 
/*#ifdef _VER_ANDROID
    const char* _ConvertUTF16ToUTF8( char *pOutUtf8, UNICHAR *utf16_str, int sizeBuff );
    #ifdef __cplusplus
    template <int _Size>
    inline const char* _ConvertUTF16ToUTF8( char (&tBuff)[_Size], UNICHAR *utf16_str ) {
        return _ConvertUTF16ToUTF8( tBuff, utf16_str, _Size );
    }
#endif */

#ifdef WIN32
	char* _CreateConvertUTF16ToEUCKR( LPCWSTR utf16 );					// UTF-16臾몄옄?댁쓣 EUCKR濡?蹂?섑빐???덈줈 硫붾え由щ? ?좊떦諛쏆븘 ?뚮젮以??
	char* _ConvertUTF16ToEUCKR( char *buff, int lenBuff, LPCWSTR utf16 );		// UTF-16臾몄옄?댁쓣 EUCKR濡?蹂?섑빐??buff???ｌ뼱 ?뚮젮以?? lenBuff??諛곗뿴???ш린??硫붾え由ы겕湲??꾨떂)
	WCHAR* _CreateConvertEUCKRToUTF16( const char *euckr );		// EUCKR臾몄옄?댁쓣 UTF16?쇰줈 蹂?섑빐???덈줈 硫붾え由щ? ?좊떦諛쏆븘 ?뚮젮以??
	WCHAR* _ConvertEUCKRToUTF16( WCHAR *wBuff, int lenBuff, const char *euckr );		// EUCKR臾몄옄?댁쓣 UTF16?쇰줈 蹂?섑빐??wBuff???ｌ뼱 ?뚮젮以?? lenBuff??諛곗뿴???ш린??硫붾え由ы겕湲??꾨떂)
	char* _ConvertUTF16ToEUCKR( LPCWSTR utf16 );	// 理쒕?湲몄씠 1024
	WCHAR* _ConvertEUCKRToUTF16( const char *euckr );	// 理쒕?湲몄씠 1024
	#ifdef __cplusplus
		template <int _Size>
		inline WCHAR* _ConvertEUCKRToUTF16( WCHAR (&wBuff)[_Size], const char *euckr ) {
			return _ConvertEUCKRToUTF16( wBuff, _Size, euckr );
		}
		template <int _Size>
		inline char* _ConvertUTF16ToEUCKR( char (&cBuff)[_Size], LPCWSTR utf16 ) {
			return _ConvertUTF16ToEUCKR( cBuff, _Size, utf16 );
		}
	#endif // c++
	WCHAR* ConvertUTF8ToUTF16( const char *cUTF8 );
	inline WCHAR* ConvertUTF8ToUTF16( const std::string& strUTF8 ) {
		return ConvertUTF8ToUTF16( strUTF8.c_str() );
	}
	WCHAR* ConvertUTF8ToUTF16( WCHAR *wBuff, int lenBuff, const char *cUTF8 ) ;
//  #if defined(_AFX) || defined(_AFXDLL)
	const char* CreateConvertUTF16toUTF8(LPCWSTR szUtf16);
	const char* ConvertUTF16toUTF8( LPCWSTR szUtf16, char *pOutBuff, int sizeOutBuff );
	template<size_t _Size>
	inline const char* ConvertUTF16toUTF8( LPCWSTR szUtf16, char (&pOutBuff)[_Size] ) {
		return ConvertUTF16toUTF8( szUtf16, pOutBuff, _Size );
	}
	const char* ConvertUTF8ToUTF16( LPCWSTR utf16 );
	int Utf8ToUnicode( const char *utf8, WCHAR *utf16Buff );
 // #endif
#endif
///////////////////////////////////////////////////////////////////////////
// TCHAR -> char
inline const char *Convert_TCHAR_To_char( LPCTSTR tstr ) {
#ifdef WIN32
	return _ConvertUTF16ToEUCKR( tstr );
#else
	if( tstr == NULL )
		return "";		// empty상태로 리턴할것.
	return tstr;
#endif
}
inline const char *Convert_TCHAR_To_char( const _tstring& str ) {
	return Convert_TCHAR_To_char( str.c_str() );
}
inline const char *Convert_TCHAR_To_char( char *cbuff, int lenBuff, LPCTSTR tstr ) {
#ifdef WIN32
	return _ConvertUTF16ToEUCKR( cbuff, lenBuff, tstr );
#else
	if( tstr == NULL )
		cbuff[0] = 0;
	else
		strcpy_s( cbuff, lenBuff, tstr );
	return cbuff;
#endif
}
// char -> TCHAR
inline LPCTSTR Convert_char_To_TCHAR( const char *cstr ) {
#ifdef WIN32
	return _ConvertEUCKRToUTF16( cstr );
#else
	if( cstr == NULL )
		return _T("");		// empty상태로 리턴할것.
	return cstr;
#endif
}
inline LPCTSTR Convert_char_To_TCHAR( const std::string& str ) {
	return Convert_char_To_TCHAR( str.c_str() );
}

inline char* CreateConvert_TCHAR_To_char( LPCTSTR tstr ) {
#ifdef WIN32
	return _CreateConvertUTF16ToEUCKR( tstr );
#else
	XBREAK( tstr == NULL );
	return NULL;
#endif
}
inline TCHAR* CreateConvert_char_To_TCHAR( const char *cstr ) {
#ifdef WIN32
	return _CreateConvertEUCKRToUTF16( cstr );
#else
	XBREAK( cstr == NULL );
	return NULL;
#endif
}

inline LPCTSTR Convert_char_To_TCHAR( TCHAR *tbuff, int lenBuff, const char *cstr ) {
#ifdef WIN32
	return _ConvertEUCKRToUTF16( tbuff, lenBuff, cstr );
#else
	if( cstr == NULL )
		tbuff[0] = 0;
	else
		strcpy_s( tbuff, lenBuff, cstr );
	return tbuff;
#endif
}

inline const char* Convert_TCHAR_To_utf8( LPCTSTR tChar, char *pOutUTF8, int lenOut ) {
#ifdef WIN32
	return ConvertUTF16toUTF8( tChar, pOutUTF8, lenOut );
#else
	if( tChar == NULL )
		pOutUTF8[0] = 0;
	else
		strcpy_s( pOutUTF8, lenOut, tChar );
	return pOutUTF8;
#endif
}

template<size_t _Size>
inline const char* Convert_TCHAR_To_utf8( LPCTSTR tChar, char (&pOutUTF8)[_Size] ) {
	return Convert_TCHAR_To_utf8( tChar, pOutUTF8, _Size );
}

inline TCHAR* Convert_utf8_To_TCHAR( TCHAR *pOutTchar, int lenOut, const char *cUtf8 ) {
#ifdef WIN32
	return ConvertUTF8ToUTF16( (WCHAR*)pOutTchar, lenOut, cUtf8 );
#else
	if( cUtf8 == NULL )
		pOutTchar[0] = 0;
	else
		strcpy_s( pOutTchar, lenOut, cUtf8 );
	return pOutTchar;
#endif
}


#ifdef __cplusplus
	template <int _Size>
	inline LPCTSTR Convert_char_To_TCHAR( TCHAR (&tBuff)[_Size], const char *cstr ) {
		return Convert_char_To_TCHAR( tBuff, _Size, cstr );
//		return _ConvertEUCKRToUTF16( tbuff, lenBuff, cstr );
	}
	template <int _Size>
	inline const char* Convert_TCHAR_To_char( char (&cBuff)[_Size], LPCTSTR tstr ) {
		return Convert_TCHAR_To_char( cBuff, _Size, tstr );
//		return _ConvertUTF16ToEUCKR( cBuff, _Size, tstr );
	}
#endif // c++

namespace XE {
	/**
	 utf8형식의 한글 한글자를 utf16 코드로 변환시킨다.
	*/
	inline WCHAR Utf8HangulToUtf16( const char *utf8, size_t len ) {
		//	XBREAK( len != 3 );
		int c1 = utf8[ 0 ] & 0x0f;
		int c2 = utf8[ 1 ] & 0x3f;
		int c3 = utf8[ 2 ] & 0x3f;
		short utf16 = ( c1 << 12 ) | ( c2 << 6 ) | c3;
		return (WCHAR)utf16;
	}
	template<size_t _Size>
	inline WCHAR Utf8HangulToUtf16( const char (&utf8)[_Size] ) {
		return Utf8HangulToUtf16( utf8, _Size );
	}

};

