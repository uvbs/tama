/*
 *  ResMng.cpp
 *
 *  Created by xuzhu on 13.2.21
 *
 */
#include "stdafx.h"
#include "XResMng.h"
#include "XResObj.h"
#include "etc/path.h"
#include "xeDef.h"
#include "XSystem.h"
#ifdef _VER_ANDROID
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <errno.h>
#include "XFramework/android/com_mtricks_xe_Cocos2dxHelper.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//#define _XTRACE		XTRACE
#define _XTRACE(...)		(0)

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
		XLOGXN( "SetLoadType: %d", (int)type );
    }
    xtLoadType GetLoadType( void )
    {
        return x_LoadType;
    }
#ifdef _VER_ANDROID
	// 현재 이 기능은 안드로이드에서만 지원된다
	/**
	 @brief  파일의 수정날짜를 읽어온다.
	 @param cFullpath 파일의 풀패스
	 @return time_t형으로 리턴되는데 win32에선 64비트고 안드로이드에선 32비트더라.
	*/
	time_t GetFileModifyTime( const char *cFullpath )
	{
		struct stat buf;
		if( stat(cFullpath, &buf) == 0 )
		{
//			_XTRACE( "%s:%d", cFullpath, buf.st_mtime );
			return buf.st_mtime;
		} else {
//			_XTRACE("failed stat():%s", cFullpath);
		}
		return 0;
	}
	/**
	 @brief 파일의 수정날짜를 수정한다.
	*/
	void SetFileModifyTime( const char *cFullpath, time_t timeModify )
	{
		struct utimbuf ut;
		ut.modtime = timeModify;
		if( utime( cFullpath, &ut) != -1 )
		{
			_XTRACE("utime success:%s", cFullpath );
			return;
		}
		_XTRACE("utime failed:%s", cFullpath );
	}
	/**
	 @brief 패키지(apk)안에 있는 szRes파일의 수정날짜를 얻어온다.
	*/
	time_t GetFileModifyTimeFromPackage( LPCTSTR szRes )
	{
		std::string strFullpath = GetstrPackage();
		std::string strRes = "assets/";
		strRes += szRes;
		time_t mtime = XE::GetFileModifyTimeFromZip( strFullpath.c_str(), strRes.c_str() );
		return mtime;
	}
	/**
	 @brief work폴더에 있는 szRes파일의 수정날짜를 얻어온다.
	*/
	time_t GetFileModifyTimeFromWork( LPCTSTR szRes )
	{
		std::string strFullpath = GetPathWork();
		strFullpath += szRes;
		time_t mtime = GetFileModifyTime( strFullpath.c_str() );
		return mtime;
	}
#endif // _VER_ANDROID

	BOOL IsExistFileInPackage( LPCTSTR szRes )
	{
		XBREAK( XE::IsEmpty(szRes) );
#if defined(WIN32) || defined(_VER_IOS)
		// 아직 구현안됨
		_tstring strPath = XE::_GetPathPackageRoot();    // iOS의 경우는 번들패스
		strPath += szRes;
		FILE *fp;
		_tfopen_s( &fp, strPath.c_str(), _T("rb") );
		if( fp == NULL )
			return FALSE;
		fclose(fp);
		return TRUE;
#endif 
#ifdef _VER_ANDROID
		_tstring strZipPath = GetstrPackage(); // 경로+apk file
		_tstring strPath = "assets/";
		strPath += szRes;
		return XE::IsExistFileFromZip( strZipPath.c_str(), strPath.c_str() );
#endif
	}
#ifdef _VER_ANDROID
	// apk압축파일에서 꺼낸다.
	BYTE* CreateMemFromApk( LPCTSTR szRes, DWORD *pOutSize )
	{
		_tstring strZipPath = GetstrPackage(); // 경로+apk file
//		XLOG("Apk:%s", strZipPath.c_str() );
		_tstring strPath = "assets/";
		strPath += szRes;
//		XLOG("apk/local path:%s, %s", strZipPath.c_str(), strPath.c_str());
		BYTE *pData = XE::GetFileDataFromZip( strZipPath.c_str(), strPath.c_str(), pOutSize );
/*
		if( pData )
			XLOG("success: %d, %s", *pOutSize, strPath.c_str());
		else
			XLOG("failed: %s", strPath.c_str());
*/
		return pData;
	}
#endif // android
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
                    return NULL;
                unsigned long size;
                fseek(fp,0,SEEK_END);
                size = ftell(fp);
                fseek(fp,0,SEEK_SET);
                XBREAK( size == 0 );
                pData = new BYTE[ size ];//pData = new BYTE[ size + 1 ];
                size = fread(pData,sizeof(BYTE), size,fp);
				// pData를 스트링으로 사용할수도 있으므로 1바이트를 더 할당해서 0으로 마무리를 지음.
//				pData[size] = 0;	
				// 13.11.26 - 끝에 널을 붙여서 루아를 읽었더니 루아가 에러냄. 그냥 파일크기 그대로 읽어야 할듯.
                fclose(fp);
                sizeFile = size;
            }
        } while (0);
        XBREAK( pOutSize == NULL );
        if( pOutSize )
            *pOutSize = sizeFile;
        return pData;
    }
		BYTE* CreateMemFromPackage( LPCTSTR _szRes, DWORD *pOutSize )
		{
			TCHAR szRes[1024];
			_tcscpy_s( szRes, _szRes );
			XPLATFORM_PATH( szRes );
#ifdef _VER_ANDROID
			// apk압축파일에서 꺼낸다.
			return CreateMemFromApk( szRes, pOutSize );
#endif // android
#if defined(WIN32) || defined(_VER_IOS)
			// 바로 풀패스를 지정해서 파일을 열어 읽어온다.
			// 패키지의 루트폴더 풀패^스. ex) c:\t2\Resource\             //
			_tstring strPath = XE::_GetPathPackageRoot();    // iOS의 경우는 번들패스
			strPath += szRes;
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
#if defined(WIN32) || defined(_XPATCH)
#else
				time_t timePkg = GetFileModifyTimeFromPackage( szRes );
 				_XTRACE( "패키지에서의 시간:%s:%d", szRes, timePkg );
				time_t timeWork = GetFileModifyTimeFromWork( szRes );
 				_XTRACE( "워크에서의 시간:%s:%d", szRes, timeWork );
				if( timePkg != timeWork && timePkg ) {
					auto bReadOk = CopyPackageToWork( szRes, nullptr );
					XBREAKF( !bReadOk, "패키지->워크 카피실패:%s", szRes );
					// 카피후에 패키지의 수정날짜로 바꾼다.
					std::string strFullpath = GetPathWork();
					strFullpath += szRes;
					SetFileModifyTime( strFullpath.c_str(), timePkg );
					// 잘 바겼는지 확인
					timeWork = GetFileModifyTimeFromWork( szRes );
					XTRACE( "파일 다시 카피함. 패키지->워크 후 바뀐 시간검사:%s:%d", szRes, timeWork );
				}
#endif
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
	XBaseRes* _CreateResObj( LPCTSTR szFullpath, LPCTSTR szPack )
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
#ifdef _VER_ANDROID
			_XTRACE( "%s open try...... ", szFullpath );
#endif
			int result = pRes->Open( szFullpath, XBaseRes::xREAD );
			if( result == 0 )
				SAFE_DELETE( pRes );
/*            DWORD size;
            BYTE *pMem = CreateMemFromFile( szRes, &size );
            if( pMem )
            {
                pRes = new XResMem( pMem, size );
            } */
		}
		return pRes;
	}

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
// WIN32이거나 패치버전이면 기존방식대로 읽는다.			
#if defined(WIN32) || defined(_XPATCH)
#pragma message("win32 or patch version")
			//  워킹에서 먼저 읽어봄
			pRes = CreateResHandleFromWork( szRes, szPack );
			if( pRes == NULL )
			{
				// 없으면 패키지에서 워킹으로 카피해옴
				BOOL bResult = CopyPackageToWork( szRes, szPack );
				if( bResult )
					pRes = CreateResHandleFromWork( szRes, szPack );
			}
// patch
#else
#pragma message("no win32 and no patch version")
			// win32환경이 아니고 노패치 버전이라면 수정날짜 비교방식을 쓴다.
			// 패키지의 파일과 워크의 파일이 수정날짜가 다르면 패키지의 파일을 다시 카피한다.
			// 이것은 노패치 버전에만 유효하다(패치버전에선 패키지안에 파일이 원본이 아니게됨)
			// 주의사항은 만약 이클립스에서 run을 하면 내부적으로 apk를 다시 만들기 때문에 패키지의 수정날짜가 바뀌어 파일을 다시 카피한다.
			// 폰에서 실행만 시키면 수정날짜는 변하지 않는다.
			// package;
			time_t timePkg = GetFileModifyTimeFromPackage( szRes );
			_XTRACE( "in package:%s:%d", szRes, timePkg );
			time_t timeWork = GetFileModifyTimeFromWork( szRes );
			_XTRACE( "in work:%s:%d", szRes, timeWork );
			BOOL bReadOk = TRUE;
			// NOTE: 주의! 만약 안드로이드폰이 sizeof(time_t)==8이라면 위함수들도 64비트 버전으로 써야한다.
			if( timePkg != timeWork && timePkg )
			{
				bReadOk = CopyPackageToWork( szRes, szPack );
				// 카피후에 패키지의 수정날짜로 바꾼다.
				std::string strFullpath = GetPathWork();
				strFullpath += szRes;
				SetFileModifyTime( strFullpath.c_str(), timePkg );
				// 잘 바겼는지 확인
				timeWork = GetFileModifyTimeFromWork( szRes );
				XTRACE( "패키지에서 워크로 파일 카피함. 시간비교로 인함.:%s:%d", szRes, timeWork );
			}
			if( bReadOk )
				pRes = CreateResHandleFromWork( szRes, szPack );
//no patch
#endif
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
		_tstring strPath = _GetPathPackageRoot();
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
			XBREAKF( pMem == NULL, "NULL = CreateMemFromApk:%s", szRes );
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
	bool CopyPackageToWorkWithDstName( LPCTSTR szRes, LPCTSTR szPack, LPCTSTR szDstName )
	{
		// 로컬패스를 넘기면 리커시브를 돌며 워킹폴더에 모든 패스를 만든다.
		TCHAR szPath[ 1024 ];
		_tcscpy_s( szPath, XE::GetFilePath( szRes ) );
//		XTRACE( "%s", __TFUNC__, szPath ); //hey
		BOOL bMake = MakeRecursiveDirToWork( szPath );
		{
			//
			DWORD sizeFile = 0;
			// 패키지안에서 해당파일을 찾아 압축을 풀어서 그 메모리데이타를 받는다.
			BYTE *pFileDat = XE::CreateMemFromPackage( szRes, &sizeFile );
//			if( XBREAK( pFileDat == NULL ) )
			if( pFileDat == NULL )
				return false;
			//
			// 워킹폴더의 루트패스를 얻어온다.
			_tstring strWorkPath = GetPathWork();
			XBREAK( strWorkPath.empty() == true );
			_tstring strFile = strWorkPath + ((XE::IsHave(szDstName))? szDstName : szRes);
//			XLOG("wb: %s", strFile.c_str() );
			_tcscpy_s( szPath, strFile.c_str() );
			XPLATFORM_PATH( szPath );
			_tstring strPath = XE::GetFilePath( szPath );
			XSYSTEM::MakeRecursiveDir( strPath.c_str() );
			XResFile res;
			if( res.Open( strFile.c_str(), XBaseRes::xWRITE ) == 0 ) {
				XBREAKF( 1, "%s open for write failed", strFile.c_str() );
				return false;
			}
			res.Write( pFileDat, sizeFile );
//			XLOG("success copy:%s", strFile.c_str());
			SAFE_DELETE( pFileDat );
		}
		return true;
	}
#ifdef WIN32
	BOOL CopyPackageToWork( const char* cRes, const char* cPack ) 
	{
		TCHAR szRes[ 1024 ];
		TCHAR szPack[ 1024 ];
		szPack[0] = 0;
		_tcscpy_s( szRes, Convert_char_To_TCHAR( cRes ) );
		if( cPack )
			_tcscpy_s( szPack, Convert_char_To_TCHAR( cPack ) );
		return CopyPackageToWork( szRes, szPack );
	}
#endif
	BOOL IsExistFileInWork( LPCTSTR szRes ) 
	{
		_tstring strWorkPath = GetPathWork();
		XBREAK( strWorkPath.empty() == true );
		_tstring strFile = strWorkPath + szRes;
		FILE *fp;
		_tfopen_s( &fp, strFile.c_str(), _T("rb") );
		if( fp )
		{
			fclose( fp );
			return TRUE;
		}
		return FALSE;
	}
#ifdef WIN32
	BOOL IsExistFileInWork( const char* cRes ) 
	{
		TCHAR szRes[ 1024 ];
		_tcscpy_s( szRes, Convert_char_To_TCHAR( cRes ) );
		return IsExistFileInWork( szRes );
	}
#endif
	// szRes파일명을 직접 쓰는 API를 위해서 현재 로드타입에 따라 풀패스를 만들어 주는 함수
	// Work copy타입이면 워크폴드에 파일도 카피시킴
	BOOL SetReadyRes( TCHAR *pOut, int sizeOut, LPCTSTR _szRes ) 
	{
		XBREAK( XE::IsEmpty( _szRes ) );
		const _tstring strRes = _szRes;
		pOut[0] = 0;
		switch( x_LoadType ) {
		//////////////////////////////////////////////////////////////////////////
		case xLT_PACKAGE_ONLY:
#ifdef _VER_ANDROID
            // 이 함수를 써야한다면 xLT_WORK_TO_PACKAGE_COPY옵션을 써야한다.
            XLOG("do not use this function");
            exit(1);
#else
			if( IsExistFileInPackage( strRes.c_str() ) ) {
				_tcscpy_s( pOut, sizeOut, _GetPathPackageRoot() );
				_tcscat_s( pOut, sizeOut, strRes.c_str() );
				return TRUE;
			} else {
				return FALSE;
			}
#endif
			break;
		//////////////////////////////////////////////////////////////////////////
		case xLT_WORK_FOLDER_ONLY:
			_tcscpy_s( pOut, sizeOut, GetPathWork() );
			_tcscat_s( pOut, sizeOut, strRes.c_str() );
			break;
		//////////////////////////////////////////////////////////////////////////
		case xLT_WORK_TO_PACKAGE:
#ifdef _VER_ANDROID
            // 이 함수를 써야한다면 xLT_WORK_TO_PACKAGE_COPY옵션을 써야한다.
            XLOG("do not use this function");
            exit(1);
#else
			// 워크에 파일이 있나 찾아보고
			if( IsExistFileInWork( strRes.c_str() ) ) {
				// 있으면 워크폴더 패스만듬
				_tcscpy_s( pOut, sizeOut, GetPathWork() );
				_tcscat_s( pOut, sizeOut, strRes.c_str() );
			} else {
				// 없으면 패키지폴더 패스 만듬.
				_tcscpy_s( pOut, sizeOut, _GetPathPackageRoot() );
				_tcscat_s( pOut, sizeOut, strRes.c_str() );
			}
#endif
			break;
		//////////////////////////////////////////////////////////////////////////
		case xLT_WORK_TO_PACKAGE_COPY:
			if( IsExistFileInWork( strRes.c_str() ) ) {
#if defined(WIN32) || defined(_XPATCH)
#else
// 				if( 워크->szRes파일의 수정날짜 != 패키지->szRes의 수정날짜 )
// 					CopyPackageToWork( szRes );
				time_t timePkg = GetFileModifyTimeFromPackage( strRes.c_str() );
// 				_XTRACE( "패키지에서의 시간:%s:%d", szRes, timePkg );
				time_t timeWork = GetFileModifyTimeFromWork( strRes.c_str() );
// 				_XTRACE( "워크에서의 시간:%s:%d", szRes, timeWork );
				if( timePkg != timeWork && timePkg ) {
					auto bReadOk = CopyPackageToWork( strRes.c_str(), nullptr );
					XBREAKF( !bReadOk, "패키지->워크 카피실패:%s", strRes.c_str() );
					// 카피후에 패키지의 수정날짜로 바꾼다.
					std::string strFullpath = GetPathWork();
					strFullpath += strRes;
					SetFileModifyTime( strFullpath.c_str(), timePkg );
					// 잘 바겼는지 확인
					timeWork = GetFileModifyTimeFromWork( strRes.c_str() );
					XTRACE( "파일다시카피함.패키지->워크 후 바뀐 시간검사:%s:%d", strRes.c_str(), timeWork );
				}
#endif
				_tcscpy_s( pOut, sizeOut, GetPathWork() );
				_tcscat_s( pOut, sizeOut, strRes.c_str() );
			} else {
				if( CopyPackageToWork( strRes.c_str() ) == FALSE )
					return FALSE;
				_tcscpy_s( pOut, sizeOut, GetPathWork() );
				_tcscat_s( pOut, sizeOut, strRes.c_str() );
			}
			break;
		//////////////////////////////////////////////////////////////////////////
		default:
			XBREAKF(1, "x_LoadType not define" );
			pOut[0] = 0;
			return FALSE;
		}
		return TRUE;
	} // SetReadyRes()
	//////////////////////////////////////////////////////////////////////////
	unsigned long long CalcCheckSum( LPCTSTR szRes )
	{
		XBaseRes *pRes;
		XGET_RESOURCE( pRes, szRes );
		if( pRes == NULL )
			return 0;
		int size = pRes->Size();
		BYTE *pMem = new BYTE[ size ];
		pRes->Read( pMem, size );
		XUINT64 checksum = XE::GetCheckSum( pMem,size );
		// 파일용량이 큰 경우 checksum값이 오버플로우가 되긴 하지만 체크섬기능엔 
		// 문제가 없는듯 하여 그대로 쓰기로 한다.
		SAFE_DELETE_ARRAY( pMem );
		// 리소스명(ex:icon/test.png)도 함께 체크섬에 포함시킨다.
		checksum += XE::GetCheckSum( szRes );
		return checksum;
	}
	DWORD GetFileSize( LPCTSTR szRes )
	{
		XBaseRes *pRes;
		XGET_RESOURCE( pRes, szRes );
		if( pRes )
		{
			DWORD size = (DWORD) pRes->Size();
			return size;
		}
		return 0;
	}

}; // namespace xe

