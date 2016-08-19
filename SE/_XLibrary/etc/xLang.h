#ifndef __LANG_H__
#define __LANG_H__

namespace XE 
{
	enum xtLang {
		xLANG_NONE = 0,
		xLANG_KOREAN,
		xLANG_ENGLISH,
		xLANG_JAPANESE,
		xLANG_CHINA
	};

	extern xtLang g_Lang;					// 언어 옵션
	extern LPCTSTR	g_dirLang[];
	inline LPCTSTR GetCurrentLangDir( void ) {
		return g_dirLang[ g_Lang ];
	}
	inline LPCTSTR GetLangDir( xtLang lang ) {
		return g_dirLang[ lang ];
	}

}

#endif