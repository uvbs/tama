#ifndef __PATH_H__
#define __PATH_H__

#ifdef WIN32
#include "windows.h"
#endif
#include "xLang.h"

#ifndef MAX_PATH
#define MAX_PATH 512
#endif

namespace XE 
{
//	void GetPath( TCHAR *szBuff, LPCTSTR strFullPath, int nBuffSize );		GetFilePath로 옮김
	LPCTSTR GetCwd( TCHAR *szBuff, int nSize );	
	const char* GetCwd( char *cBuff, int nSize );
	LPCTSTR GetCwd( void  );			// 실행파일의 폴더를 얻음
	LPCTSTR GetExecDir();		// 실행파일의 폴더를 얻음(사전에 구해놓은거)
	LPCTSTR	MakePath( LPCTSTR szPath, LPCTSTR szFileName, XE::xtLang lang=xLANG_NONE );
	LPCTSTR SetWorkDir( LPCTSTR defaultWorkDir=NULL );
	LPCTSTR GetWorkDir();
	BOOL GetDirectoryFiles( LPCTSTR szPath, TCHAR buffer[][MAX_PATH], int nMaxFiles );
	void GetDocumentPath( TCHAR *szBuff );
	LPCTSTR MakeDocumentPath( LPCTSTR szFilename );
	BOOL MakeHighPath( TCHAR *pOut, int lenOut, LPCTSTR _szSrc );
	template<size_t size>
	inline BOOL MakeHighPath( TCHAR (&pBuff)[size], LPCTSTR szSrc ) {
		return MakeHighPath( pBuff, size, szSrc );
	}

#ifdef WIN32
	const char*	MakePath( const char* szPath, const char* szFileName );
	int IsDirExists( LPCTSTR s );
	BOOL CreateDir( LPCTSTR szPath );
	BOOL MakeHighPath( char *pOut, int lenOut, const char *_szSrc );
#endif
}

#endif // __PATH_H__