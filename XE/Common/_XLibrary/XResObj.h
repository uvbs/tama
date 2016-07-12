/*

 *  ResObj.h
 *
 *  Created by xuzhu on 13.2.21
 *
 */
#pragma once
#include <string.h>

class XBaseRes
{
public:
	enum xRWTYPE { xNONE=0, xREAD, xWRITE };
//	static void RegisterLua( XLua *pLua );
	static XE::xtERROR GetLastError() { return s_err; }
	static LPCTSTR GetLastErrorString() {
		switch( s_err ) {
			case XE::ERR_FILE_NOT_FOUND:	return _T("File not found");
			case XE::ERR_PERMISSION_DENIED:	return _T("Permission denied. 체크아웃을 해제하세요");
			case XE::ERR_READONLY: return _T("file is readonly");
			case XE::ERR_FAILED:	return _T("file open failed");
			case XE::ERR_SUCCESS:	return _T("");
			default: return _T("unknown error.");
		}
		return _T("");
	}
	LPCTSTR GetszResName() { return m_szResName; }
#ifdef WIN32
	const char* GetcResName() { return m_cResName; }
#endif
private:
	static XE::xtERROR s_err;
	TCHAR m_szResName[1024];
#ifdef WIN32
	char m_cResName[1024];
#endif
	void Init() {
		m_szResName[0] = 0;
#ifdef WIN32
		m_cResName[0] = 0;
#endif
		s_err = XE::ERR_SUCCESS;
		m_Mode = xNONE;
	}
	void Destroy(){}
protected:
	xRWTYPE m_Mode;
	void SetError( XE::xtERROR err ) { s_err = err; }
	void SetResName( LPCTSTR szResName ) { _tcscpy_s( m_szResName, szResName ); }
#ifdef WIN32
	void SetcResName( const char *cResName ) { strcpy_s( m_cResName, cResName ); }
#endif
public:
	XBaseRes() { Init(); }
	virtual ~XBaseRes() { Destroy(); }
	//
//	GET_ACCESSOR( LPCTSTR, szResName );
	// operator
	template<typename T>
	XBaseRes& operator >> ( T& rhs ) {
		Read( &rhs, sizeof( T ), 1 );
		return *this;
	}
	XBaseRes& operator << ( DWORD rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( long rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( int rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( unsigned int rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( WORD rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( short rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( BYTE rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( char rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( bool rhs ) {
		BYTE byte = ( rhs ) ? 1 : 0;
		Write( &byte, sizeof( byte ), 1 );
		return *this;
	}
	XBaseRes& operator << ( XINT64 rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( float rhs ) {
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( double rhs ) {
		XASSERT( sizeof( rhs ) == 8 );
		Write( &rhs, sizeof( rhs ), 1 );
		return *this;
	}
	XBaseRes& operator << ( const XE::VEC2& rhs ) {
		XE::VEC2 v = rhs;
		Write( &v.x, sizeof( v.x ), 1 );
		Write( &v.y, sizeof( v.y ), 1 );
		return *this;
	}
	XBaseRes& operator << ( std::string& rhs ) {
		WriteStringA( const_cast<char*>( rhs.c_str() ) );
		return *this;
	}
	XBaseRes& operator >> ( std::string& rhs ) {
		char cBuff[4096] = {0, };
		ReadStringA( cBuff );
		rhs = cBuff;
		return *this;
	}
	template<typename T>
	XBaseRes& operator << ( std::vector<T>& ary ) {
		long size = (long)ary.size();
		Write( &size, sizeof( long ), 1 );
		for( auto& elem : ary ) {
			( *this ) << elem;
		}
		return *this;
	}
	template<typename T>
	XBaseRes& operator >> ( std::vector<T>& aryOut ) {
		aryOut.clear();
		long size = 0;
		Read( &size, sizeof( long ), 1 );
		for( int i = 0; i < size; ++i ) {
			T elem;
			( *this ) >> elem;
			aryOut.push_back( elem );
		}
		return *this;
	}
	//
	virtual int Open( LPCTSTR szFilename, xRWTYPE mode ) { return 0; }
	inline int Open( const _tstring& strFilename, xRWTYPE mode ) { 
		return Open( strFilename.c_str(), mode );
	}
#ifdef WIN32
	virtual int Open( const char *szFilename, xRWTYPE mode ) {return 0; }
#endif
	virtual void Close() {}
	virtual long Size() = 0;
	virtual int Seek( long int offset, int whence = SEEK_CUR ) = 0;
	virtual int Read( void *ptr, size_t size, size_t n = 1) = 0;
	virtual int Write( void *ptr, size_t size, size_t n = 1) { XBREAKF(1,"not implement"); return 0; }
	virtual int WriteString( LPCTSTR szStr ) { 	XBREAKF(1,"not implement"); return 0; }
	virtual int ReadString( TCHAR *pOutStr ) { 	XBREAKF(1,"not implement"); return 0; }
	virtual int WriteStringA( char *cStr ) { 	XBREAKF(1,"not implement"); return 0; }
	virtual int ReadStringA( char* pOutStr ) { 	XBREAKF(1,"not implement"); return 0; }
	virtual int Printf( LPCTSTR szFormat, ... ) { return 0; }
	virtual int Getc() { return 0; }
	virtual BOOL IsEndOfRes() { return FALSE; }
	// lua
	virtual void LuaWrite( int num ) {XBREAK(1);}
	virtual int LuaRead() {XBREAK(1);return 0;}
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef _VER_IOS
#ifndef WIN32
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
	void Close();
public:
	XResFile() { Init(); }
	~XResFile() { Destroy(); }
	//
	//
	int Open( LPCTSTR szFilename, xRWTYPE mode=xREAD );
	inline int Open( const _tstring& strFilename, xRWTYPE mode = xREAD ) {
		return Open( strFilename.c_str(), mode );
	}
	int OpenForWrite( LPCTSTR szFilename ) {
		return Open( szFilename, xWRITE );
	}
	int OpenForRead( LPCTSTR szFilename ) {
		return Open( szFilename, xREAD );
	}
#ifdef WIN32
	int Open( const char *szFilename, xRWTYPE mode=xREAD );
	int OpenForWrite( const char *szFilename ) {
		return Open( szFilename, xWRITE );
	}
	int OpenForRead( const char *cFilename ) {
		return Open( cFilename, xREAD );
	}
#endif
	void ForceClose() {
		Close();
	}
	long Size() override;
	int Seek( long int offset, int whence = SEEK_CUR ) override {
        return fseek( m_fp, offset, whence );
    }
	int Read( void *ptr, size_t size, size_t n = 1) override;
	int Write( void *ptr, size_t size, size_t n = 1) override;
	int WriteString( LPCTSTR szStr ) override;
	int ReadString( TCHAR *pOutStr ) override;
	int WriteStringA( char *cStr ) override;
	int ReadStringA( char* pOutStr ) override;
	int Printf( LPCTSTR szFormat, ... ) override;
	int Getc() { return fgetc(m_fp); }	
	BOOL IsEndOfRes() { return ( feof( m_fp ) == 0 )? TRUE : FALSE; }
	// lua
	void LuaWrite( int num ) {
		Write( &num, 4 );
	}
	int LuaRead() {
		int num;
		Read( &num, 4 );
		return num;
	}

};

class XResMem : public XBaseRes
{
    BYTE *m_pMem;
    BYTE *m_pCurr;
    int m_Size;
    void Init() {
        m_pMem = NULL;
        m_pCurr = NULL;
        m_Size = 0;
    }
    void Destroy() {
        SAFE_DELETE_ARRAY( m_pMem );
    }
public:
    XResMem( BYTE *pMem, int size ) {
        Init();
        m_pMem = pMem;
        m_pCurr = pMem;
        m_Size = size;
    }
    virtual ~XResMem() { Destroy(); }
    //
	virtual long Size() {
        return m_Size;
    }
    virtual int Seek( long int offset, int whence = SEEK_CUR ) {
        if( whence == SEEK_CUR )
            m_pCurr += offset;
        else if( whence == SEEK_SET )
            m_pCurr = m_pMem;
        else if( whence == SEEK_END )
            m_pCurr = m_pMem + (m_Size - 1);
        BYTE *pEnd = m_pMem + m_Size;
        XBREAK( m_pCurr < m_pMem || m_pCurr > pEnd );
        return 1;
    }
	virtual int Read( void *ptr, size_t size, size_t n = 1) {
        memcpy( ptr, m_pCurr, size * n );
        m_pCurr += (size * n);
        BYTE *pEnd = m_pMem + m_Size;
        XBREAK( m_pCurr < m_pMem || m_pCurr > pEnd );
        return 1;
    }
	virtual int ReadString( TCHAR *pOutStr ) {
        int size;
        Read( &size, 4 );
        XBREAK( size <= 0 );
        XBREAK( size > 0xffff );
        Read( pOutStr, size );
        return size;
    }
};

// namespace XSYSTEM {
// 	BOOL CopyFileX( LPCTSTR szSrcPath, LPCTSTR szDstPath );
// 	BOOL DeleteFile( const char* cFilePath );
// #ifdef WIN32
// 	BOOL CopyFileX( const char* cSrcPath, const char* cDstPath );
// #endif
// }; // xsystem
// 
