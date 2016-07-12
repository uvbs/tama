// ExtractList.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include <conio.h>
#include "XFramework/XConsoleMain.h"
#include "XFramework/XTextTable.h"
#include "XFramework/XConstant.h"
#include "XPatch.h"
#include "xeDef.h"
#include "XSystem.h"
#include "XResMng.h"

class XMain : public XConsoleMain
{
// 	struct XPatch::XRES_INFO : public XSYSTEM::XFILE_INFO
// 	{
// 		XUINT64 llChecksum;
// 	};
	void Init() {}
	void Destroy() {}
public:
	XMain();
	virtual ~XMain() { Destroy(); }
	//
	virtual XLibrary* CreateXLibrary( void ) {
		return new XLibConsole;
	}
	BOOL LoadINI( void );
	BOOL LoadRes( void );
	void DoExtract( void );
	int GetResIdxByCheckSum( const XVector<XPatch::XRES_INFO> &ary, XUINT64 llCheckSum );
	//	int WriteFullList( XArrayLinear<XPatch::XRES_INFO>& aryAll );
	int WriteFullList( const XVector<XPatch::XRES_INFO>& aryAll );
	int WriteCoreList( const XVector<_tstring>& aryCore );
	//	int WriteCoreList( XArrayLinear<_tstring>& aryCore );
};

XMain *MAIN = NULL;

int _tmain( int argc, _TCHAR* argv[] )
{
	// 목표
	// full_list.txt를 만든다.
	// core_list.txt를 만든다.(apk50메가선을 유지하도록)
	MAIN = new XMain;
	// 리소스 로딩
	MAIN->Create();
	MAIN->LoadRes();
	_tprintf(_T("press any key to continue.................\n"));
	_getch();
	// 리스트 추출
	MAIN->DoExtract();
	//
	SAFE_DELETE( MAIN );
	return 0;
}


XMain::XMain()
{
	Init();
}

BOOL XMain::LoadINI( void )
{
	TCHAR szWorkPath[ 1024 ];
	GetCurrentDirectory( MAX_PATH, szWorkPath );		// 현재 작업폴더를 읽는다
	_tprintf(_T("\ncurrDir=%s\n"), szWorkPath );
	CToken token;
	_tstring strPath = szWorkPath;
	strPath += _T("\\extractlist.ini");
	if( !token.LoadAbsolutePath( strPath.c_str(), XE::TXT_EUCKR ) )	{
		_tprintf( _T( "not found %s" ), strPath.c_str() );
		_getch();
		exit(1);
		return FALSE;
	}
	_tstring strResPath = token.GetTokenPath();	// 리소스 패스를 읽음.
// 	TCHAR szPackage[1024];
// 	_tcscpy_s( szPackage, szExePath );
// 	_tcscat_s( szPackage, _T("\\PatchFile\\") );	// ini작성중 사용자의 실수를 방지하기 위해 아예 고정 폴더로 바꿈.
	// 각 폴더를 ini의 폴더로 재지정.
// 	auto charLast = strResPath.back();
// 	if( charLast != '\\' )
// 		strResPath += _T("\\");
	XE::SetPackagePath( strResPath.c_str() );
	XE::SetWorkPath( strResPath.c_str() );
	XE::SetDocPath( strResPath.c_str() );
	return TRUE;
}


BOOL XMain::LoadRes( void )
{
	LoadINI();

	//
	XE::LANG.LoadINI( XE::MakePath( _T(""), _T("lang.txt") ) );
/*	TCHAR szRes[ 256 ];
	TEXT_TBL = new XTextTable;	
	LPCTSTR szFile  = XE::Format( _T("text_%s.txt"), XE::LANG.GetszFolder() );
	_tcscpy_s( szRes, XE::MakePath( DIR_PROP, szFile ) );
	if( TEXT_TBL->Load( szRes ) == FALSE )
	{
		XALERT("%s load failed", szRes );
		return FALSE;
	}
	//
	CONSTANT = new XConstant;
	CONSTANT->Load( _T("defineGame.h") );			// 상수값들 읽어서 메모리에 적재
*/
	return TRUE;
}

/**
 full_list.txt를 만든다.
 core_list.txt를 만든다.(apk50메가선을 유지하도록)
*/
void XMain::DoExtract( void )
{
	// 일단 모든 리소스 파일의 목록을 뽑는다.
//	XArrayLinear<XSYSTEM::XFILE_INFO> aryAll;
	XVector<XSYSTEM::XFILE_INFO> aryAll;
//	XSYSTEM::CreateFileList( XE::_GetPathPackageRoot(), _T("*.*"), &aryAll );
	XSYSTEM::GetFileList( XE::_GetPathPackageRoot(), _T("*.*"), &aryAll, _T("") );
	if( aryAll.Size() > 1500 ) {

	}
	// full_list.txt를 만든다.
	{
// 		XArrayLinear<XPatch::XRES_INFO> ary;
// 		ary.Create( aryAll.size() );
		XVector<XPatch::XRES_INFO> ary;
		int cnt = 1;
		// 파일별로 체크섬을 뽑아 둔다.
//		XARRAYLINEAR_LOOP( aryAll, XSYSTEM::XFILE_INFO, info )
		for( auto& info : aryAll ) {
			XPatch::XRES_INFO resInfo;
			resInfo.strFile = info.strFile;
			resInfo.size = info.size;
			resInfo.llChecksum = XE::CalcCheckSum( info.strFile.c_str() );
			_tstring strFilename = XE::GetFileName( resInfo.strFile.c_str() );
			int idxDuplicate = GetResIdxByCheckSum( ary, resInfo.llChecksum );
			if( idxDuplicate != -1 ) {
				XALERT( "duplicate checksum file: idx=%d,%d", idxDuplicate, cnt );
			}
			ary.Add( resInfo );
			CONSOLE( "%d:%s.... %d byte ....checksum=0x%I64x", 
																												cnt++, 
																												strFilename.c_str(), 
																												resInfo.size, 
																												resInfo.llChecksum );
		}
		// full_list.txt를 생성한다.
		WriteFullList( ary );
	}
	// core_list.txt를 뽑아낸다.
	{
// 		XArrayLinear<_tstring> aryCore;
// 		aryCore.Create( aryAll.size() );
		XVector<_tstring> aryCore;
		int  i = 0;
//		XARRAYLINEAR_LOOP( aryAll, XSYSTEM::XFILE_INFO, info )
		for( auto& info : aryAll ) {
//			TCHAR szFlename[ 256 ];
//			_tcscpy_s( szFlename, XE::GetFileName( info.strFile.c_str() ) );
			const _tstring strFileName = XE::GetFileName( info.strFile );
			// 테마에 속하지 않는 몬스터중에 mob_ 으로 시작하는 몬스터는 
			// 제작은 되었으나 아직 테마에 반영이 안된 몬스터이므로 제외시킨다.
			BOOL bCore = TRUE;
//			TCHAR szExt[ 16 ];
//			_tcscpy_s( szExt, XE::GetFileExt( strFileName ) );
			const _tstring strExt = XE::GetFileExt( strFileName );
			// 다음 단어가 포함되어 있는 파일은 모두 제외
// 			if( _tcsstr( szFlename, _T( "treant" ) ) ||
// 				_tcsstr( szFlename, _T( "rock" ) ) )
// 				bCore = FALSE;
			// 액셀 파일 제외
//			if( XE::IsSame( szExt, _T("xlsx") ) )
			if( strExt == _T("xlsx") )
				bCore = FALSE;
// 			if( XE::IsSame( szFlename, _T("core_list.txt") ) ||
// 				XE::IsSame( szFlename, _T("full_list.txt") ) )
// 				bCore = FALSE;
			if( strFileName == _T("core_list.txt") 
				|| strFileName == _T("full_list.txt") )
				bCore = FALSE;
			if( _tcsstr( strFileName.c_str(), _T("sample")) )
				bCore = FALSE;
			//
			if( bCore )
			{
				aryCore.Add( info.strFile );
				XTRACE( "%d: %s", i ++, info.strFile.c_str() );
			} else
			{
				// 제외된 파일명
				XTRACE( "-----------%s", info.strFile.c_str() );
			}
			
		}
		// core_list.txt로 저장한다.
		WriteCoreList( aryCore );
	}

}

// int XMain::GetResIdxByCheckSum( XArrayLinear<XPatch::XRES_INFO> &ary, XUINT64 llCheckSum )
int XMain::GetResIdxByCheckSum( const XVector<XPatch::XRES_INFO> &ary, XUINT64 llCheckSum )
{
	int max = ary.size();
	for( int i = 0; i < max; ++i )	{
		if( ary[ i ].llChecksum == llCheckSum )
			return i;
	}
	return -1;
}

void strReplace( char *cBuff, char cSrc, char cDst )
{
	int len = strlen( cBuff );
	for( int i = 0; i < len; ++i )	{
		if( cBuff[ i ] == cSrc )
			cBuff[ i ] = cDst;
	}
}

// full_list.txt를 쓴다.
// int XMain::WriteFullList( XArrayLinear<XPatch::XRES_INFO>& aryAll )
int XMain::WriteFullList( const XVector<XPatch::XRES_INFO>& aryAll )
{
	char cPath[ 1024 ];
	strcpy_s( cPath, SZ2C( XE::_GetPathPackageRoot() ) );
	strcat_s( cPath, "full_list.txt" );
	FILE *fp = NULL;
	fopen_s( &fp, cPath, "wt" );
	if( fp == NULL )
		return 0;
	char cBuff[ 1024 ];
//	XARRAYLINEAR_LOOP( aryAll, XPatch::XRES_INFO, info )
	for( auto& info : aryAll ) {
		strcpy_s( cBuff, SZ2C( info.strFile.c_str() ) );
		strReplace( cBuff, '\\', '/' );
		fprintf_s( fp, "\"%s\"\t%d\t%u\n", cBuff, info.size, info.llChecksum );
	}
	fprintf_s( fp, "// num files %d \n", aryAll.size() );
	fclose( fp );
	return 1;
}

// core_list.txt를 쓴다.
//int XMain::WriteCoreList( XArrayLinear<_tstring>& aryCore )
int XMain::WriteCoreList( const XVector<_tstring>& aryCore )
{
	char cPath[ 1024 ];
	strcpy_s( cPath, SZ2C( XE::_GetPathPackageRoot() ) );
	strcat_s( cPath, "core_list.txt" );
	FILE *fp = NULL;
	fopen_s( &fp, cPath, "wt" );
	if( fp == NULL )
		return 0;
	char cBuff[ 1024 ];
//	XARRAYLINEAR_LOOP( aryCore, _tstring, strRes )
	for( auto& strRes : aryCore ) {
		strcpy_s( cBuff, SZ2C( strRes.c_str() ) );
		strReplace( cBuff, '\\', '/' );
		fprintf_s( fp, "\"%s\"\n", cBuff );
		//		fprintf_s( fp, "\"%s\"\n", SZ2C( strRes.c_str() ) );
	}
	fprintf_s( fp, "// num files %d \n", aryCore.size() );
	fclose( fp );
	return 1;
}

