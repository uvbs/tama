/*
 *  XResMng.h
 *
 *  Created by xuzhu on 13.2.21
 *
 */
#pragma once
#include "XAutoPtr.h"

#define XGET_RESOURCE( PTR, RES ) \
	PTR = XE::CreateResHandle( RES );	\
	XAutoPtr<XBaseRes*> _res( PTR );

class XBaseRes;
namespace XE {
    //
    void SetLoadType( xtLoadType type );
    xtLoadType GetLoadType( void );
    BYTE* CreateFileDataFromFile( LPCTSTR szFile, DWORD *pOutSize );
    BYTE* CreateMemFromFile( LPCTSTR szFile, DWORD *pOutSize );
    BYTE* CreateMemFromFile( LPCTSTR szLocalPath, LPCTSTR szFile, DWORD *pOutSize );
    BYTE* CreateMemFromRes( LPCTSTR szRes, DWORD *pOutSize );
    BYTE* CreateMemFromPackage( LPCTSTR szRes, DWORD *pOutSize );
	/**
	 리소스에서 szRes파일을 찾아 리소스객체 형태로 생성해 리턴한다.
	 szRes는 "spr/test.spr"과 같이 로컬형태여야 한다.
	 플랫폼과 무관하게 추상화 되어있다.
	 szPack은 묶음압축파일인데 현재는 지원하지 않는다.
	 ex)
	 XBaseRes *pRes = XE::CreateResHandle( "spr/test.spr" );
	 pRes.Read( &dw1, 4 );
	 SAFE_DELETE( pRes );
	*/
	XBaseRes* CreateResHandle( LPCTSTR szRes, LPCTSTR szPack=NULL );
	/**
	 패키지패스에서 szRes파일을 읽어서 리소스객체로 만든다.
	 szRes는 "spr/test.spr"형태의 로컬패스여야 한다.
	 ios나 win32의 경우는 리소스 원본폴더의 파일을 직접 억세스 하고
	 안드로이드는 apk에서 압축을 풀어서 메모리리소스로 리턴한다.
	 */
	XBaseRes* CreateResHandleFromPackage( LPCTSTR szRes, LPCTSTR szPack=NULL );
	/**
	워킹패스에서 szRes파일을 읽어서 리소스객체로 만든다.
	szRes는 "spr/test.spr"형태의 로컬패스여야 한다.
	ios의 경우 도큐먼트폴더나 캐쉬폴더에 생성되고 
	안드로이드는 /data/data/패키지이름/Resource/ 에서 읽는다.
	*/
	XBaseRes* CreateResHandleFromWork( LPCTSTR szRes, LPCTSTR szPack=NULL );
	/**
		패키지폴더에서 워킹폴더로 szRes파일을 복사한다.
	*/
	bool CopyPackageToWorkWithDstName( LPCTSTR szRes, LPCTSTR szDstName, LPCTSTR szPack = nullptr );
	inline BOOL CopyPackageToWork( LPCTSTR szRes, LPCTSTR szPack = nullptr ) {
		if( CopyPackageToWorkWithDstName( szRes, szPack, nullptr ) )
			return TRUE;
		return FALSE;
	}
	BOOL IsExistFileInWork( LPCTSTR szRes ) ;
	BOOL SetReadyRes( TCHAR *pOut, int sizeOut, LPCTSTR szRes ) ;
	template<size_t size>
	inline BOOL SetReadyRes( TCHAR (&pBuff)[size], LPCTSTR szRes ) {
		return SetReadyRes( pBuff, size, szRes );
	}
	// 안드로이드의 경우 압축되어 있으므로 해당 파일을 work에 풀어놓은뒤 그 파일의 패스를 리턴한다.
	inline _tstring GetPackageFilePath( LPCTSTR szRes ) {
		TCHAR szBuff[ 1024 ];
		SetReadyRes( szBuff, szRes );
		return _tstring( szBuff );
	}
	unsigned long long CalcCheckSum( LPCTSTR szRes );
	DWORD GetFileSize( LPCTSTR szRes );
#ifdef WIN32
	BOOL CopyPackageToWork( const char* cRes, const char* cPack=NULL );
	BOOL IsExistFileInWork( const char* cRes ) ;
#endif
	BOOL IsExistFileInPackage( LPCTSTR szRes );


};



