/*
 *  ResMng.cpp
 *
 *  Created by xuzhu on 13.2.21
 *
 */
#include "stdafx.h"
#include "ResMng.h"
#include "ResObj.h"
#include "path.h"
#ifdef _VER_ANDROID
#include "com_example_gltest2_Cocos2dxHelper.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _XPACKAGE

namespace XE
{
	BYTE* CreateMemFromFile( LPCTSTR szFile, DWORD *pOutSize );
	BOOL CopyPackageToWork( LPCTSTR szRes, LPCTSTR szPack );

    xtLoadType x_LoadType = xLT_NONE;
#ifdef _VER_ANDROID
//    _tstring s_strPackage;      // apk 풀패스. path.cpp로 옮겨짐
#endif
    void SetLoadType( xtLoadType type )
    {
        x_LoadType = type;
		XLOG( "SetLoadType: %d", (int)type );
    }
    xtLoadType GetLoadType( void )
    {
        return x_LoadType;
    }
    // szFile을 읽어서 메모리를 할당해 돌려준다.
    BYTE* CreateMemFromFile( LPCTSTR szFile, DWORD *pOutSize )
    {
        BYTE *pData = NULL;
        DWORD sizeFile = 0;
        do
        {
            {
				FILE *fp;
                _tfopen_s( &fp, szFile, _T("rb") );
                if( fp == NULL )
                {
                    XALERT("%s file not found", szFile );
                    return NULL;
                }
                unsigned long size;
                fseek(fp,0,SEEK_END);
                size = ftell(fp);
                fseek(fp,0,SEEK_SET);
                XBREAK( size == 0 );
                pData = new BYTE[ size ];
                size = fread(pData,sizeof(BYTE), size,fp);
                fclose(fp);
                sizeFile = size;
            }
        } while (0);
        XBREAK( pOutSize == NULL );
        if( pOutSize )
            *pOutSize = sizeFile;
        return pData;
    }
    BYTE* CreateMemFromPackage( LPCTSTR szFile, DWORD *pOutSize )
    {
#ifdef _VER_ANDROID
		return CreateMemFromApk( szFile, pOutSize );
        // apk압축파일에서 꺼낸다.
/*        _tstring strZipPath = s_strPackage; // 경로+apk
        //                XLOG("Apk:%s", strZipPath.c_str() );
        _tstring strPath = "assets/";
        strPath += szFile;
        XLOG("apk/local path:%s, %s", strZipPath.c_str(), strPath.c_str());
        BYTE *pData = XE::GetFileDataFromZip( strZipPath.c_str(), strPath.c_str(), pOutSize );
        if( pData )
            XLOG("success: %d, %s", sizeFile, strPath.c_str());
        else
            XLOG("failed: %s", strPath.c_str());
        return pData; */
#endif // android
#if defined(WIN32) || defined(_VER_IOS)
        // 바로 풀패스를 지정해서 파일을 열어 읽어온다.
        // 패키지의 루트폴더 풀패스. ex) c:\t2\Resource\             //
        _tstring strPath = XE::GetPathPackageRoot();    // iOS의 경우는 번들패스
        strPath += szFile;
        return CreateMemFromFile( strPath.c_str(), pOutSize );
#endif // win32
    }
    /**
     szFile의 내용을 통째로 읽어서 메모리에 담아준다. 플랫폼이나 로드타입에 맞춰 자동으로 쓸수 있도록 추상화 되었다.
     szFile은 로컬패스로 넘겨준다.  ex) spr/test.spr
     미리 로드타입이 지정되어 있어야 한다.
     */
    BYTE* CreateMemFromRes( LPCTSTR szRes, DWORD *pOutSize )
    {
        XBREAK( pOutSize == NULL );
        BYTE *pData = NULL;
        DWORD sizeFile = 0;
        do
        {
            // 패키지에서만 읽는다.
            if( x_LoadType == xLT_PACKAGE_ONLY )
            {
                pData = CreateMemFromPackage( szRes, &sizeFile );
            } else
            if( x_LoadType == xLT_WORK_FOLDER_ONLY )
            {
                _tstring strWork = XE::GetPathWork(); // ex) c:/t2/work/
                strWork += szRes;
                pData = CreateMemFromFile( strWork.c_str(), &sizeFile );
            } else
            if( x_LoadType == xLT_WORK_TO_PACKAGE )
            {
                // 워킹에서 먼저 읽어보고
                _tstring strWork = XE::GetPathWork(); // ex) c:/t2/work/
                strWork += szRes;
                pData = CreateMemFromFile( strWork.c_str(), &sizeFile );
                if( pData == NULL )
                {
                    // 없으면 패키지에서 읽는다.
                    pData = CreateMemFromPackage( szRes, &sizeFile );
                }
            } else
			if( x_LoadType == xLT_WORK_TO_PACKAGE_COPY )
			{
				// 워킹에서 먼저 읽어보고
				_tstring strWork = XE::GetPathWork(); // ex) c:/t2/work/
				strWork += szRes;
				pData = CreateMemFromFile( strWork.c_str(), &sizeFile );
				if( pData == NULL )
				{
					// 없으면 패키지에서 워킹으로 카피해온후 다시 워킹에서 읽는다.
					if( CopyPackageToWork( szRes, NULL ) == TRUE )
						pData = CreateMemFromFile( strWork.c_str(), &sizeFile );
				}
			} else
                XBREAKF(1, "x_LoadType == xLT_NONE");
        } while (0);
        if( pOutSize )
            *pOutSize = sizeFile;
        return pData;
    }
    BYTE* CreateMemFromRes( LPCTSTR szLocalPath, LPCTSTR szFilename, DWORD *pOutSize )
    {
        _tstring strLocal = szLocalPath;
        strLocal += szFilename;
        XLOG("LocalPath:%s", strLocal.c_str());
        return CreateMemFromRes( strLocal.c_str(), pOutSize );
    }
	// 
	XBaseRes* _CreateResObj( LPCTSTR szRes, LPCTSTR szPack )
	{
		XBaseRes *pRes = NULL;
		if( szPack && XE::IsHave( szPack ) )
		{
			// 이경우 szRes는 로컬패스. szPack은 풀패스
			XBREAKF( 1, "not implement" );
			// 이건 나중에 구현
//			pMem = ReadPack( 패킹파일명, 로컬패스 );
//			return new XResMem( pMem );
		} else
		{
			// 이경우 szRes는 풀패스
			pRes = new XResFile;
			int result = pRes->Open( szRes, XBaseRes::xREAD );
			if( result == 0 )
				SAFE_DELETE( pRes );
		}
		return pRes;
	}

#ifdef _VER_ANDROID
	// apk압축파일에서 꺼낸다.
	BYTE* CreateMemFromApk( LPCTSTR szFile, DWORD *pOutSize )
	{
		_tstring strZipPath = GetstrPackage(); // 경로+apk file
//		XLOG("Apk:%s", strZipPath.c_str() );
		_tstring strPath = "assets/";
		strPath += szFile;
		XLOG("apk/local path:%s, %s", strZipPath.c_str(), strPath.c_str());
		BYTE *pData = XE::GetFileDataFromZip( strZipPath.c_str(), strPath.c_str(), pOutSize );
		if( pData )
			XLOG("success: %d, %s", sizeFile, strPath.c_str());
		else
			XLOG("failed: %s", strPath.c_str());
		return pData;
	}
#endif // android
	XBaseRes* CreateResHandle( LPCTSTR szRes, LPCTSTR szPack )
	{
		XBaseRes *pRes = NULL;
		switch( x_LoadType )
		{
		// 패키지에서만 읽는다.
		case xLT_PACKAGE_ONLY:
			pRes = CreateResHandleFromPackage( szRes, szPack );
			break;
		// 워킹폴더에서 먼저 읽어보고 없으면 패키지에서 읽는다.
		case xLT_WORK_TO_PACKAGE:
			{
				// 워킹에서 먼저 읽어봄
				pRes = CreateResHandleFromWork( szRes, szPack );
				if( pRes == NULL )
					// 없으면 패키지에서 읽음.
					pRes = CreateResHandleFromPackage( szRes, szPack );
			}
			break;
		// 워킹폴더에서 먼저 읽어보고 없으면 패키지에서 워킹으로 카피해온후에 다시 워킹에서 읽는다.
		case xLT_WORK_TO_PACKAGE_COPY:
			{
				//  워킹에서 먼저 읽어봄
				pRes = CreateResHandleFromWork( szRes, szPack );
				if( pRes == NULL )
				{
					// 없으면 패키지에서 워킹으로 카피해옴
					BOOL bResult = CopyPackageToWork( szRes, szPack );
					if( bResult )
						pRes = CreateResHandleFromWork( szRes, szPack );
				}
			}
			break;
		// 워킹폴더에서만 읽는다. 
		case xLT_WORK_FOLDER_ONLY:
			pRes = CreateResHandleFromWork( szRes, szPack );
			break;
		} 
		return pRes;
	}

	XBaseRes* CreateResHandleFromPackage( LPCTSTR szRes, LPCTSTR szPack )
	{
		XBaseRes *pRes = NULL;
#if defined(_VER_IOS) || defined(WIN32)
		// ios나 win32는 압축되어 있지 않기때문에 그냥 번들패스를 더해서 그대로 파일접근 하면 된다.
		_tstring strPath = GetPathPackageRoot();
		if( szPack && XE::IsHave( szPack ) )
		{
			strPath += szPack;
			pRes = _CreateResObj( szRes, strPath.c_str() );
		}
		else
		{
			strPath += szRes;
			pRes = _CreateResObj( strPath.c_str(), szPack );
		}
		return pRes;
#endif // ios or win32
#ifdef _VER_ANDROID 
		if( szPack && XE::IsHave( szPack ) )
		{
			XBREAKF( 1, "not implement" );
/*			pPackMem = ReadAPK( apk패스, 패킹파일명 ); // apk에서 패킹파일을 읽어 메모리로 만듬
			메모리핸들 = CreateResObjFromPackMem( szRes, pPackMem ); // 패킹포맷의 메모리를 넘겨서 그안에서 szRes를 열어 메모리로 만듬
			return 메모리핸들 */
			// 하지만 안드로이드에선 이렇게 쓰는걸 피해야 함. apk가 이미 압축되어 있으므로

		} else
		{
			DWORD size;
			BYTE *pMem = CreateMemFromApk( szRes, &size );
			pRes = new XResMem( pMem, size );
		}
#endif // android
		return pRes;
	}
	// 워킹패스에서 szRes파일을 읽어 리소스객체로 만들어 리턴한다.
	// szRes는 "spr/test.spr"과 같은 로컬패스 형태여야 한다."
	XBaseRes* CreateResHandleFromWork( LPCTSTR szRes, LPCTSTR szPack )
	{
		XBaseRes *pRes = NULL;
		_tstring strPath = GetPathWork();
		if( szPack && XE::IsHave( szPack ) )
		{
			strPath += szPack;
			pRes = _CreateResObj( szRes, strPath.c_str() );
		}
		else
		{
			strPath += szRes;
			pRes = _CreateResObj( strPath.c_str(), szPack );
		}
		return pRes;
	}

	// 패키지폴더에서 워킹폴더로 szRes파일을 복사한다.
	BOOL CopyPackageToWork( LPCTSTR szRes, LPCTSTR szPack )
	{
		// 로컬패스를 넘기면 리커시브를 돌며 워킹폴더에 모든 패스를 만든다.
		TCHAR szPath[ 1024 ];
		_tcscpy_s( szPath, XE::GetFilePath( szRes ) );
		BOOL bMake = MakeRecursiveDirToWork( szPath );
		{
			//
			DWORD sizeFile = 0;
			// 패키지안에서 해당파일을 찾아 압축을 풀어서 그 메모리데이타를 받는다.
			BYTE *pFileDat = XE::CreateMemFromPackage( szRes, &sizeFile );
			if( XBREAK( pFileDat == NULL ) )
				return FALSE;
			//
			// 워킹폴더의 루트패스를 얻어온다.
			_tstring strWorkPath = GetPathWork();
			XBREAK( strWorkPath.empty() == true );
			_tstring strFile = strWorkPath + szRes;
//			XLOG("wb: %s", strFile.c_str() );
			XResFile res;
			if( res.Open( strFile.c_str(), XBaseRes::xWRITE ) == 0 )
			{
				XBREAKF( 1, "%s open for write failed", strFile.c_str() );
				return FALSE;
			}
			res.Write( pFileDat, sizeFile );
			XLOG("success copy:%s", strFile.c_str());
			SAFE_DELETE( pFileDat );
		}
		return TRUE;
	}
	BOOL CopyPackageToWork( const char* cRes, const char* cPack ) 
	{
		TCHAR szRes[ 1024 ];
		TCHAR szPack[ 1024 ];
		_tcscpy_s( szRes, Convert_char_To_TCHAR( cRes ) );
		_tcscpy_s( szPack, Convert_char_To_TCHAR( cPack ) );
		CopyPackageToWork( szRes, szPack );
	}
}; // namespace xe

#endif // xpackage
