#include "stdafx.h"
#include "XExpTableUser.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XExpTableUser *EXP_TABLE_USER = nullptr;

BOOL XExpTableUser::Load( LPCTSTR szFile )
{
	CToken token;
	if( token.LoadFile( XE::MakePath( DIR_PROP, szFile ), XE::TXT_UTF16 ) == xFAIL )
		return FALSE;
	XArrayLinearN<DWORD, 256> ary;
	ary.Add( 0xFFFFFFFF );	// index0에 0xFFFFFFFF을 하나 넣어둔다.
	int levelLast = 0;
	while(1)
	{
		int level = token.GetNumber();
		if( level == TOKEN_EOF )
			break;
		if( levelLast + 1 != level ) {
			XBREAKF( 1, "%s:레벨이 일정하게 증가하지 않고있다.:lv=%d", szFile, level );
		}
		int exp = token.GetNumber();
		XBREAKF( exp == TOKEN_EOF || exp == TOKEN_ERROR, "%s:exp가 나와야 하는데 파일이 끝나거나 에러가 났다.:lv=%d", szFile, level );
		ary.Add( (DWORD)exp );
		levelLast = level;
	}
	m_Table = ary;

	return TRUE;
}

