#pragma once

class XLang
{
	struct xLang {
		std::string m_strcKey;
		std::string m_strcDir;
		_tstring m_strPath;
	};
public:
private:
 	_tstring m_strKey;		// 언어이름 키			"korea"
 	_tstring m_strFolder;	// 언어폴더 이름		"ko"
 	_tstring m_strPath;		// ko/ 형식으로 미리 만들어둠	"ko/"
	void Init();
	void Destroy() {}
public:
	XLang() : m_strSelectedKey("korea") { Init(); }
	~XLang() { Destroy(); }
	//
	GET_ACCESSOR_CONST( const _tstring&, strKey );
	GET_ACCESSOR_CONST( const _tstring&, strFolder );
	LPCTSTR GetszKey() const {
		return m_strKey.c_str();
	}
// 	void SetszKey( LPCTSTR szKey ) {
// 		m_strKey = szKey;
// 	}
	LPCTSTR GetszFolder() const {
		return m_strFolder.c_str();
	}
	// 현재 언어를 선택한다.
	bool SetSelectedKey( const std::string& strKeySelected ) {
		XBREAK( strKeySelected.empty() );
		XBREAK( !IsHaveKey( strKeySelected ) );
		m_strSelectedKey = strKeySelected;
		auto pLang = FindByKey( strKeySelected );
		if( pLang ) {
			m_strKey = C2SZ( pLang->m_strcKey );
			m_strFolder = C2SZ( pLang->m_strcDir );
			m_strPath = pLang->m_strPath;
			return true;
		}
		return false;
	}
	// 국가 키로 폴더명을 얻는다.
	std::string FindFolderByKey( const std::string& strcKey ) const {
		for( auto& lang : m_aryLangs ) {
			if( lang.m_strcKey == strcKey ) {
				return lang.m_strcDir;
			}
		}
		return std::string();
	}
	inline bool IsHaveKey( const std::string& strcKey ) const {
		const auto strcDir = FindFolderByKey( strcKey );
		return !strcDir.empty();
	}
	LPCTSTR GetLangDir() const {
		return m_strPath.c_str();
	}
	bool LoadINI( LPCTSTR szFile );
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
private:
	const xLang* FindByKey( const std::string& strcKey ) const {
		for( auto& lang : m_aryLangs ) {
			if( lang.m_strcKey == strcKey ) {
				return &lang;
			}
		}
		return nullptr;
	}
	_tstring MakeFolderPath( LPCTSTR szFolder );
private:
	XVector<xLang> m_aryLangs;
	std::string m_strSelectedKey;		// 현재 선택된 국가

};


namespace XE 
{
	extern XLang LANG;
/*
	extern xtLang g_Lang;					// 언어 옵션
	extern LPCTSTR	g_dirLang[];
	inline LPCTSTR GetCurrentLangDir() {
		return g_dirLang[ g_Lang ];
	}
	inline LPCTSTR GetLangDir( xtLang lang ) {
		return g_dirLang[ lang ];
	}
*/

}

