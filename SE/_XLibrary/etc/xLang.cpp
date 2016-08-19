#include "stdafx.h"
#include "xLang.h"

namespace XE 
{
	xtLang		g_Lang;					// 언어 옵션
#ifdef WIN32
	LPCTSTR	g_dirLang[] = { 0,
									_T("ko\\"),
									_T("en\\"),
									_T("jp\\"),
									_T("ch\\"), };
#else
	LPCTSTR	g_dirLang[] = { 0,
		_T("ko/"),
		_T("en/"),
		_T("jp/"),
		_T("ch/"), };
#endif
}

