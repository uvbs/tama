#ifndef __LANG_H__
#define __LANG_H__

class XLang
{
public:
private:
	_tstring m_strKey;		// 언어이름 키
	_tstring m_strFolder;	// 언어폴더 이름
	_tstring m_strPath;		// ko/ 형식으로 미리 만들어둠
	void Init() {
		m_strKey = _T("korean");
		m_strFolder = _T("ko");
		TCHAR szBuff[ 32 ];
		_tcscpy_s( szBuff, m_strFolder.c_str() );
		_tcscat_s( szBuff, _T("/") );
		XPLATFORM_PATH( szBuff );
		m_strPath = szBuff;
	}
	void Destroy() {}
public:
	XLang() { Init(); }
	virtual ~XLang() { Destroy(); }
	//
	GET_ACCESSOR( const _tstring&, strKey );
	GET_ACCESSOR( const _tstring&, strFolder );
	LPCTSTR GetszKey( void ) {
		return m_strKey.c_str();
	}
	void SetszKey( LPCTSTR szKey ) {
		m_strKey = szKey;
	}
	LPCTSTR GetszFolder( void ) {
		return m_strFolder.c_str();
	}
	void SetszFolder( LPCTSTR szFolder ) {
		m_strFolder = szFolder;
		TCHAR szBuff[ 32 ];
		_tcscpy_s( szBuff, m_strFolder.c_str() );
		_tcscat_s( szBuff, _T("/") );
		XPLATFORM_PATH( szBuff );
		m_strPath = szBuff;
	}
	BOOL LoadINI( LPCTSTR szFile );
	LPCTSTR GetLangDir( void ) {
		return m_strPath.c_str();
	}
	LPCTSTR ChangeToLangDir( LPCTSTR szRes, TCHAR *pOut, int sizeOut );
	template<size_t Size>
	inline LPCTSTR ChangeToLangDir( LPCTSTR szRes, TCHAR (&pBuff)[Size] ) {
		return ChangeToLangDir( szRes, pBuff, Size );
	}
#ifdef WIN32
	const char* ChangeToLangDir( const char *cPath, char *pOut, int sizeOut );
	template<size_t Size>
	inline const char* ChangeToLangDir( const char *cPath, char (&pBuff)[Size] ) {
		return ChangeToLangDir( cPath, pBuff, Size );
	}
#endif

};


namespace XE 
{
	extern XLang LANG;
/*
	extern xtLang g_Lang;					// 언어 옵션
	extern LPCTSTR	g_dirLang[];
	inline LPCTSTR GetCurrentLangDir( void ) {
		return g_dirLang[ g_Lang ];
	}
	inline LPCTSTR GetLangDir( xtLang lang ) {
		return g_dirLang[ lang ];
	}
*/

}

#endif
