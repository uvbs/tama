/*
 *  ResMng.h
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#ifndef __RESMNG_H__
#define __RESMNG_H__

class XBaseRes
{
public:
	enum xtERROR { ERR_SUCCESS=0, ERR_FAILED, ERR_READONLY, ERR_PERMISSION_DENIED, ERR_FILE_NOT_FOUND };
	enum xRWTYPE { xNONE=0, xREAD, xWRITE };
//	static void RegisterLua( XLua *pLua );
	static xtERROR GetLastError() { return s_err; }
	static LPCTSTR GetLastErrorString() {
		switch( s_err ) {
			case ERR_FILE_NOT_FOUND:	return _T("File not found");
			case ERR_PERMISSION_DENIED:	return _T("Permission denied. 체크아웃을 해제하세요");
			case ERR_READONLY: return _T("file is readonly");
			case ERR_FAILED:	return _T("file open failed");
			case ERR_SUCCESS:	return _T("");
			default: return _T("unknown error.");
		}
		return _T("");
	}
	LPCTSTR GetszResName() { return m_szResName; }
#ifdef WIN32
	const char* GetcResName() { return m_cResName; }
#endif
private:
	static xtERROR s_err;
	TCHAR m_szResName[1024];
#ifdef WIN32
	char m_cResName[1024];
#endif
	void Init() {
		m_szResName[0] = 0;
#ifdef WIN32
		m_cResName[0] = 0;
#endif
		s_err = ERR_SUCCESS;
		m_Mode = xNONE;
	}
	void Destroy(){}
protected:
	xRWTYPE m_Mode;
	void SetError( xtERROR err ) { s_err = err; }
	void SetResName( LPCTSTR szResName ) { _tcscpy_s( m_szResName, szResName ); }
#ifdef WIN32
	void SetcResName( const char *cResName ) { strcpy_s( m_cResName, cResName ); }
#endif
public:
	XBaseRes() { Init(); }
	~XBaseRes() { Destroy(); }

//	GET_ACCESSOR( LPCTSTR, szResName );
	virtual int Open( LPCTSTR szFilename, xRWTYPE mode ) = 0;
#ifdef WIN32
	virtual int Open( const char *szFilename, xRWTYPE mode ) = 0;
#endif
	virtual void Close() {}
	virtual long Size() = 0;
	virtual int Seek( long int offset, int whence = SEEK_CUR ) = 0;
	virtual int Read( void *ptr, size_t size, size_t n = 1) = 0;
	virtual int Write( void *ptr, size_t size, size_t n = 1) = 0;
	virtual int WriteString( TCHAR *szStr ) { 	XBREAK(1); return 0; }
	virtual int ReadString( TCHAR *pOutStr ) { 	XBREAK(1); return 0; }
	virtual int Printf( LPCTSTR szFormat, ... ) { return 0; }
	virtual int Getc() { return 0; }
	virtual BOOL IsEndOfRes() { return FALSE; }
	// lua
	virtual void LuaWrite( int num ) {XBREAK(1);}
	virtual int LuaRead() {XBREAK(1);return 0;}
};

/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _VER_IPHONE
#define _tfopen fopen
#endif
#include <stdio.h>
#include "XCrypto.h"
class XResFile : public XBaseRes, public XCrypto
{
public:
//	static void RegisterLua( XLua *pLua );
private:
	FILE *m_fp;
	DWORD m_dwCheckSum;
	void Init() {
		m_fp = NULL;
		m_dwCheckSum = 0;
	}
	void Destroy() { Close(); }
	virtual void Close();
public:
	XResFile() { Init(); }
	~XResFile() { Destroy(); }
	virtual int Open( LPCTSTR szFilename, xRWTYPE mode=xREAD );
#ifdef WIN32
	virtual int Open( const char *szFilename, xRWTYPE mode=xREAD );
#endif
	virtual long Size();
	virtual int Seek( long int offset, int whence = SEEK_CUR ) { return fseek( m_fp, offset, whence ); }
	template<typename T>
	XResFile& operator >> ( T& rhs ) {
		Read( &rhs, sizeof( T ), 1 );
		return *this;
	}
	XResFile& operator << ( DWORD rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( long rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( int rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( unsigned int rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( WORD rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( short rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( BYTE rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( char rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( bool rhs ) {
		BYTE byte = ( rhs )? 1 : 0;
		Write( &byte, sizeof( byte ), 1 );
		return *this;
	}
	XResFile& operator << ( XINT64 rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( float rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( double rhs ) {
		XASSERT( sizeof(rhs) == 8 );
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XResFile& operator << ( const XE::VEC2& rhs ) {
		XE::VEC2 v = rhs;
		Write( &v.x, sizeof( v.x ), 1 );
		Write( &v.y, sizeof( v.y ), 1 );
		return *this;
	}
	XResFile& operator << ( std::string& rhs ) {
		WriteStringA( const_cast<char*>( rhs.c_str() ) );
		return *this;
	}
	XResFile& operator >> ( std::string& rhs ) {
		char cBuff[ 4096 ] = {0,};
		ReadStringA( cBuff );
		rhs = cBuff;
		return *this;
	}
	template<typename T>
	XResFile& operator << ( std::vector<T>& ary ) {
		long size = (long)ary.size();
		Write( &size, sizeof(long), 1 );
		for( auto& elem : ary ) {
			(*this) << elem;
		}
		return *this;
	}
	template<typename T>
	XResFile& operator >> ( std::vector<T>& aryOut ) {
		aryOut.clear();
		long size = 0;
		Read( &size, sizeof(long), 1 );
		for( int i = 0; i < size; ++i ) {
			T elem;
			( *this ) >> elem;
			aryOut.push_back( elem );
		}
		return *this;
	}


	virtual int Read( void *ptr, size_t size, size_t n = 1);
	virtual int Write( void *ptr, size_t size, size_t n = 1);
	inline int Write( DWORD val ) {
		return Write( &val, sizeof( val ) );
	}
	inline int Write( long val ) {
		return Write( &val, sizeof( val ) );
	}
	inline int Write( int val ) {
		return Write( &val, sizeof( val ) );
	}
	inline int Write( char val ) {
		return Write( &val, sizeof( val ) );
	}
	inline int Write( BYTE val ) {
		return Write( &val, sizeof( val ) );
	}
	inline int Write( short val ) {
		return Write( &val, sizeof( val ) );
	}
	inline int Write( WORD val ) {
		return Write( &val, sizeof( val ) );
	}
	inline int Write( float val ) {
		return Write( &val, sizeof( val ) );
	}
	virtual int WriteString( TCHAR *szStr );
	virtual int ReadString( TCHAR *pOutStr );
	virtual int WriteStringA( char *cStr );
	virtual int ReadStringA( char* pOutStr );
	virtual int Printf( LPCTSTR szFormat, ... );
	virtual int Getc() { return fgetc(m_fp); }	
	virtual BOOL IsEndOfRes() { return ( feof( m_fp ) == 0 )? TRUE : FALSE; }
	// lua
	virtual void LuaWrite( int num ) {
		Write( &num, 4 );
	}
	virtual int LuaRead() {
		int num;
		Read( &num, 4 );
		return num;
	}
};

#endif