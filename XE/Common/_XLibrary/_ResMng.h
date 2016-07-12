/*
 *  ResMng.h
 *
 *  Created by xuzhu on 13.2.21
 *
 */
#pragma once

class XBaseRes;
namespace XE {
    enum xtLoadType { xLT_NONE,
        xLT_PACKAGE_ONLY,    // 패키지에서만 읽음
        xLT_WORK_FOLDER_ONLY,   // 워킹폴더에서만 읽음
        xLT_WORK_TO_PACKAGE,     // 워킹폴더에서 읽어보고 없으면 패키지에서 찾음.
		xLT_WORK_TO_PACKAGE_COPY, // 워킹에서 읽어보고 없으면 패키지에서 찾아서 워크로 카피하여 워크에서 읽음.
		xLT_MAX,
    };
    //
    void SetLoadType( xtLoadType type );
    xtLoadType GetLoadType( void );
    BYTE* CreateFileDataFromFile( LPCTSTR szFile, DWORD *pOutSize );
    BYTE* CreateMemFromFile( LPCTSTR szFile, DWORD *pOutSize );
    BYTE* CreateMemFromFile( LPCTSTR szLocalPath, LPCTSTR szFile, DWORD *pOutSize );
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
	BOOL CopyPackageToWork( LPCTSTR szRes, LPCTSTR szPack=NULL );
	BOOL CopyPackageToWork( const char* cRes, const char* cPack=NULL );
};



