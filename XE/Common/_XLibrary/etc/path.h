#pragma once

#ifdef WIN32
#include "windows.h"
#endif
#include "XE.h"	// 만약 헤더가 path.h랑 꼬이면 xtLoadType만 xe.h로 모두 옮기고 이건 뺄것.
#include "XList.h"
#include "ConvertString.h"

#ifndef MAX_PATH
#define MAX_PATH 512
#endif

#define DIR_SPR		XE::GetDirSpr()

class XConsoleMain;
namespace XE 
{
	void InitResourceManager( XConsoleMain *pEngineMain, xtLoadType typeLoad );
	const _tstring& GetstrPackage();
	LPCTSTR GetCwd( TCHAR *szBuff, int nSize );	
	const char* GetCwd( char *cBuff, int nSize );
	LPCTSTR GetCwd( void  );			// 
	std::string GetstrCwd();
	LPCTSTR GetExecDir();		// 
	LPCTSTR GetExecFilename();
	LPCTSTR GetExecFileTitle();
//#ifdef _CLIENT
	LPCTSTR	MakePathLang( LPCTSTR szLocalPath, LPCTSTR szFile );
	_tstring MakePathLang2( LPCTSTR szLocalPath, LPCTSTR szFile );
	inline _tstring MakePathLang2( LPCTSTR szLocalPath, const _tstring& strFile ) {
		return MakePathLang2( szLocalPath, strFile.c_str() );
	}
	//#endif
	LPCTSTR	MakePath( LPCTSTR szLocalPath, LPCTSTR szFile );
	inline LPCTSTR	MakePath( LPCTSTR szLocalPath, const _tstring& strFile ) {
		return MakePath( szLocalPath, strFile.c_str() );
	}
	LPCTSTR MakePath( _tstring *pstrOut, LPCTSTR szLocalPath, LPCTSTR szFile );
	void GetDocumentPath( TCHAR *szBuff );
    template<size_t size>
    inline void GetDocumentPath( TCHAR (&pBuff)[size] ) {
        GetDocumentPath( pBuff, size );
    }
	void GetDocumentPath( TCHAR *szBuff );
	LPCTSTR MakeDocumentPath( LPCTSTR szFilename );
  LPCTSTR MakeWorkFullPath( LPCTSTR szLocalPath, LPCTSTR szFile );
  LPCTSTR MakeDocFullPath( LPCTSTR szLocalPath, LPCTSTR szFile );
	inline LPCTSTR MakeDocumentPath( const _tstring& strFilename ) {
		return MakeDocumentPath( strFilename.c_str() );
	}
	inline LPCTSTR MakeWorkFullPath( LPCTSTR szLocalPath, const _tstring& strFile ) {
		return MakeWorkFullPath( szLocalPath, strFile.c_str() );
	}
	inline LPCTSTR MakeDocFullPath( LPCTSTR szLocalPath, const _tstring& strFile ) {
		return MakeDocFullPath( szLocalPath, strFile.c_str() );
	}
	void StringSetAt( TCHAR *pBuff, int lenBuff, int idxAt, TCHAR c );
	template<size_t size>
	inline void StringSetAt( TCHAR (&pBuff)[size], int idxAt, TCHAR c ) {
		StringSetAt( pBuff, size, idxAt, c );
	}
	BOOL MakeHighPath( TCHAR *pOut, int lenOut, LPCTSTR _szSrc );
	template<size_t size>
	inline BOOL MakeHighPath( TCHAR (&pBuff)[size], LPCTSTR szSrc ) {
		return MakeHighPath( pBuff, size, szSrc );
	}
	BOOL MakeDirToWork( LPCTSTR szPath );
	BOOL MakeRecursiveDirToWork( LPCTSTR szLocalPath );
#if defined(WIN32) || defined(_VER_IOS)
	LPCTSTR _GetPathPackageRoot();
#endif 
	LPCTSTR GetPathWork();
	LPCTSTR GetPathDoc();
	std::string MakePath2( const char* cLocalPath, const char* cFile );
	inline std::string MakePath2( const char* cLocalPath, const std::string& strFile ) {
		return MakePath2( cLocalPath, strFile.c_str() );
	}
	LPCTSTR MakePackageFullPath( LPCTSTR szLocalPath, LPCTSTR szFile );
	inline LPCTSTR MakePackageFullPath( LPCTSTR szLocalPath, const _tstring& strFile ) {
		return MakePackageFullPath( szLocalPath, strFile.c_str() );
	}
#ifdef WIN32
	void SetPackagePath( LPCTSTR szPath );
	void SetWorkPath( LPCTSTR szPath );
	void SetDocPath( LPCTSTR szPath );
	const char* MakePath( const char* cLocalPath, const char* cFile );
	_tstring MakePath2( LPCTSTR szLocalPath, LPCTSTR szFile );
	inline _tstring MakePath2( LPCTSTR szLocalPath, const _tstring& strFile ) {
		return MakePath2( szLocalPath, strFile.c_str() );
	}
	const char* MakeDocumentPath( const char *szFilename );
	const char* MakeDocFullPath( const char* cLocalPath, const char* cFile );
	const char* MakeWorkFullPath( const char* cLocalPath, const char* cFile );
	const char* GetPathDocA();
	const char* GetPathWorkA();
	int IsDirExists( LPCTSTR s );
	BOOL CreateDir( LPCTSTR szPath );
	BOOL MakeHighPath( char *pOut, int lenOut, const char *_szSrc );
#else
	inline const char* GetPathWorkA() {
		return GetPathWork();
	}
#endif
	void SetDirSpr( LPCTSTR szDir );
	TCHAR* GetDirSpr();
} // namespace XE
#ifdef _VER_IOS
namespace XIOS
{
    // get bundle path
    LPCSTR _SetPackageDir( char *pOut, int sizeOut );
    LPCSTR GetBundleDir( char *pOut, int sizeOut );
    // get documenty path
    void _GetDocumentPath( CHAR *szBuff, int sizeBuff );
    template<size_t size>
    inline void _GetDocumentPath( TCHAR (&pBuff)[size] ) {
        _GetDocumentPath( pBuff, size );
    }
    // get document/resource path
    LPCSTR SetWorkDir( char *pOut, int sizeOut );
    //
    BOOL GetDirectoryFiles( LPCSTR szPath, CHAR buffer[][MAX_PATH], int nMaxFiles );
} // namespace XIOS
#endif
namespace XSYSTEM
{
	struct XFILE_INFO
	{
		_tstring strFile;
		DWORD size = 0;
	};
	/**
	szPath와 그하위에 있는 모든 폴더의 파일들의 목록을 뽑는다.
	szPath: c:\\test\\ 와 같은 형식으로 들어와야 한다.
	szWildcard: *.*
	pOutAry: 리스트를 받을 XArrayLinearN<1000>형 어레이(나중에 템플릿으로 바꿀것)
	szSubDir: 시작 서브디렉토리 명. 디폴트로 ""이며 목록은 img\\a.png,  spr\\ko\\a.spr 과 같은식으로 뽑힌다.
	*/
	int GetFileList( LPCTSTR szPath, LPCTSTR szWildcard, XVector<XFILE_INFO> *pOutAry, LPCTSTR szSubDir );
	int GetFileList( LPCTSTR szPath, LPCTSTR szWildcard, XArrayLinearN<_tstring, 2048> *pOutAry, LPCTSTR szSubDir = _T("") );
	int GetFileList( LPCTSTR szPath, LPCTSTR szWildcard, XArrayLinearN<XFILE_INFO, 2048> *pOutAry, LPCTSTR szSubDir = _T("") );
	/**
	szPath와 그하위에 있는 모든 폴더의 파일들의 목록을 뽑아 그 수만큼 pOutAry에 메모리를 할당해 넣어준다.
	메모리해제의 책임은 외부에 있다.
	파라메터 설명은 GetFileList와 같다.
	*/
	int CreateFileList( LPCTSTR szPath, LPCTSTR szWildcard, XVector<XFILE_INFO> *pOutAry );
	int CreateFileList( LPCTSTR szPath, LPCTSTR szWildcard, XArrayLinear<_tstring> *pOutAry );
	int CreateFileList( LPCTSTR szPath, LPCTSTR szWildcard, XArrayLinear<XFILE_INFO> *pOutAry );
	void CutStringByChar( const _tstring& strSrc, TCHAR cDiv, XVector<_tstring>* pOutAry );
}; // xsystem

#ifdef WIN32
	inline LPCTSTR xPathIMG( LPCTSTR szFile ) {
		return XE::MakePath( DIR_IMG, szFile );
	}
	inline LPCTSTR xPathUI( LPCTSTR szFile ) {
		return XE::MakePath( DIR_UI, szFile );
	}
	inline LPCTSTR xPathICON( LPCTSTR szFile ) {
		return XE::MakePath( DIR_ICON, szFile );
	}
#endif // WIN32
inline LPCTSTR xPathIMG( const char *cFile ) {
	return XE::MakePath( DIR_IMG, C2SZ( cFile ) );
}
inline LPCTSTR xPathIMG( const _tstring& strFile ) {
	return xPathIMG( strFile.c_str() );
}
//
inline LPCTSTR xPathUI( const _tstring& strFile ) {
	return xPathUI( strFile.c_str() );
}
inline LPCTSTR xPathUI( const char *cFile ) {
	return XE::MakePath( DIR_UI, C2SZ( cFile ) );
}
inline LPCTSTR xPathICON( const _tstring& strFile ) {
	return xPathICON( strFile.c_str() );
}
inline LPCTSTR xPathICON( const char *cFile ) {
	return XE::MakePath( DIR_ICON, C2SZ( cFile ) );
}