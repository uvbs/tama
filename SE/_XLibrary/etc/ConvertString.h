/*
 *  ConvertString.h
 *  BnBTest
 *
 *  Created by xuzhu on 09. 07. 21.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */

#ifndef __CONVERTSTRING_H__
#define __CONVERTSTRING_H__

#define C2SZ	Convert_char_To_TCHAR
#define SZ2C	Convert_TCHAR_To_char

////////////////////////////////////////////////////////////////////////////
#ifdef _VER_IPHONE
	const char *_ConvertEUCKRToUTF8( char *utf8_str, LPCSTR euckr_str, int size );
	int _GetSizeEUCKRToUTF8( LPCSTR euckr_str );
	const char *_ConvertUTF16ToUTF8( UNICHAR *utf16_str );
#endif

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
	WCHAR* ConvertUTF8ToUTF16( WCHAR *wBuff, int lenBuff, const char *cUTF8 ) ;
#endif
///////////////////////////////////////////////////////////////////////////
// TCHAR -> char
inline const char *Convert_TCHAR_To_char( LPCTSTR tstr ) {
#ifdef WIN32
	return _ConvertUTF16ToEUCKR( tstr );
#else
	return tstr;
#endif
}
inline const char *Convert_TCHAR_To_char( const _tstring& strt ) {
	return Convert_TCHAR_To_char( strt.c_str() );
}
inline const char *Convert_TCHAR_To_char( char *cbuff, int lenBuff, LPCTSTR tstr ) {
#ifdef WIN32
	return _ConvertUTF16ToEUCKR( cbuff, lenBuff, tstr );
#else
    strcpy_s( cbuff, lenBuff, tstr );
	return cbuff;
#endif
}
// char -> TCHAR
inline LPCTSTR Convert_char_To_TCHAR( const char *cstr ) {
#ifdef WIN32
	return _ConvertEUCKRToUTF16( cstr );
#else
	return cstr;
#endif
}
inline LPCTSTR Convert_char_To_TCHAR( const std::string& strc ) {
	return Convert_char_To_TCHAR( strc.c_str() );
}
inline char* CreateConvert_TCHAR_To_char( LPCTSTR tstr ) {
#ifdef WIN32
	return _CreateConvertUTF16ToEUCKR( tstr );
#else
	XBREAK(1);		// 사용할일 없음
	return NULL;
#endif
}
inline TCHAR* CreateConvert_char_To_TCHAR( const char *cstr ) {
#ifdef WIN32
	return _CreateConvertEUCKRToUTF16( cstr );
#else
	XBREAK(1);		// 사용할일 없음
	return NULL;
#endif
}

inline LPCTSTR Convert_char_To_TCHAR( TCHAR *tbuff, int lenBuff, const char *cstr ) {
#ifdef WIN32
	return _ConvertEUCKRToUTF16( tbuff, lenBuff, cstr );
#else
    strcpy_s( tbuff, lenBuff, cstr );
	return tbuff;
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

#endif // 
