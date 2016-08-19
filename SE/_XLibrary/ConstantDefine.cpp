#include "stdafx.h"
#include "ConstantDefine.h"
#include "Debug.h"

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///																				////
///  class CDefine																////
///																				////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

//
// Constant.h를 읽어 #define name value들을 모두 메모리에 적재
// Load()를 연속적으로 읽는다면 m_nMaxDef부터 추가로 읽게 된다.
// ex)  Load( "constant1.h" );	0 ~ 300 인덱스
//      Load( "constant2.h" );	301 ~ 600 인덱스
XE::xRESULT CDefine::Load( LPCTSTR strFileName )
{
	CToken	Token;
	TCHAR szName[256];

	if( Token.LoadFile( strFileName, TXT_EUCKR ) == XE::xFAIL )
		return XE::xFAIL;
	while(1)
	{
		if( Token.GetToken() == NULL )	break;
		if( Token == _T("define") )
		{
			Token.GetToken();
			// ex) #define OBJECT_DAO	1
			if( Token.m_Token[0] != '_' )		// #define _HUHU같이 첫자가밑줄 붙은건 읽지 않음
			{
				_tcscpy_s( szName, Token.m_Token );
				int		nVal = Token.GetNumber();
				// 읽어 메모리로 올릴때는 주석을 읽지 않는다.  
				// Object.inc에 있는 NAME부분을 가지고 저장할때만 기록한다.
				if( XE::xFAIL == AddDefine( szName, nVal, Token.GetRemark() ) )			// define 추가
				{
					XERROR( "%s 읽던중 CDefine의 최대 버퍼%d를 넘었다", strFileName, MAX_DEFINE );
					break;
				}
			} else
				XLOG( "define %s skip", Token.m_Token );
		}
	}
	return XE::xSUCCESS;
}

// 바이너리 포맷으로 읽기
XE::xRESULT	CDefine :: LoadBIN( LPCTSTR szFileName )
{
	int		nNum = 0;
	int		nLen = 0;
	TCHAR buff[256];
	int		nMaxDef = 0;

	XResFile file, *pRes = &file;
	if( pRes->Open( szFileName, XBaseRes::xREAD ) == 0 ) {
		XERROR( "%s가 없다", szFileName );
		return XE::xFAIL;
	}
		
//	FILE	*fp = _tfopen( szFileName, _T("rb") );
//	if( fp == NULL )	
//	{
//		XERROR( "%s가 없다", szFileName );
//		return XE::xFAIL;
//	}

	int		i;
//	DEFINE	*pList = m_pList;

	memset( buff, 0, 256 );
	pRes->Read( &nMaxDef, 4 );
	for( i = 0; i < nMaxDef; i ++ )
	{
		pRes->Read( &nLen, 4 );
		pRes->Read( buff, nLen );
		pRes->Read( &nNum, 4 );
		if( XE::xFAIL == AddDefine( buff, nNum ) )			// define 추가
		{
			XERROR( "%s 읽던중 CDefine의 최대 버퍼%d를 넘었다", szFileName, MAX_DEFINE );
			break;
		}
	}

#ifdef	_LE
	g_pWatchView->Message( "%s 로딩완료", szFileName );
#endif

	return XE::xSUCCESS;
}

//
// Find - Name으로 찾기
//
CDefine::DEFINE	*CDefine::Find( LPCTSTR szName )
{
	int		i;
	DEFINE	*pList = m_pList;

	i = m_nMaxDef;
	while( i-- )
	{
		if( _tcscmp( pList->m_strName, szName ) == 0 )		// 풀네임을 비교
			return pList;
		pList++;
	}
	return NULL;			// not found
}

//
// Find - Val로 찾기
//
CDefine::DEFINE	*CDefine::Find( int nVal )
{
	int		i;
	DEFINE	*pList = m_pList;

	i = m_nMaxDef;
	while( i-- )
	{
		if( pList->m_nValue == nVal )		// 값을 비교
			return pList;
		pList++;
	}
	return NULL;			// not found
}

//
// Find - Name의 일부분과 값으로 찾기
//
CDefine::DEFINE	*CDefine :: Find( LPCTSTR szName, int nVal )
{
	int		i;
	int		len = _tcslen(szName);
	DEFINE	*pList = m_pList;

	i = m_nMaxDef;
	while( i-- )
	{
		if( _tcsncmp( pList->m_strName, szName, len ) == 0 && pList->m_nValue == nVal )		// 이름의 일부분과 값으로 검색
			return pList;
		pList++;
	}
	return NULL;			// not found
}



void	AutoTab( XResFile *pRes, int len )
{
	len = 24 - len;
	int rest = len % 4;
	for( int j = 0; j < len/4; j ++ )	pRes->Printf( _T("\t") );
	if( rest )	pRes->Printf( _T("\t") );
}

// Define내용중 OBJECT_ 로만 시작하는 넘들을 저장한다.
#if 0
int		CDefine :: SaveOBJECT( LPCTSTR szFileName )
{
	// 백업
	char	buff[256];
	strcpy( buff, MakePath(SCRIPT_DIR, "DefineObject.h02") );	CopyFile( buff, MakePath(SCRIPT_DIR, "DefineObject.h03"), FALSE );		// 백업
	strcpy( buff, MakePath(SCRIPT_DIR, "DefineObject.h01") );	CopyFile( buff, MakePath(SCRIPT_DIR, "DefineObject.h02"), FALSE );		// 백업
	strcpy( buff, MakePath(SCRIPT_DIR, "DefineObject.h") );		CopyFile( buff,   MakePath(SCRIPT_DIR, "DefineObject.h01"), FALSE );		// 백업


	FILE	*fp = fopen( MakePath(SCRIPT_DIR, szFileName), "wt" );
	if( fp == NULL )	
	{
		XERROR( "%s open failed", GetLastMakePath() );
		return FAIL;
	}

	fprintf( fp, "#ifndef		__DEFINEOBJECT_H__\n" );
	fprintf( fp, "#define		__DEFINEOBJECT_H__\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "// #define name은 64자를 넘지 말것\n" );
	fprintf( fp, "// 이 파일은 LE에서 자동으로 저장된다.\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "// 아무것도 없는 오브젝트\n" );
	fprintf( fp, "#define		OBJECT_EMPTY		0\n" );
	fprintf( fp, "\n" );

	int		i;
	DEFINE	**pList;

	pList = m_pFriend;
	fprintf( fp, "// 주인공\n" );
	for( i = 0; i < m_nMaxFriend; i ++ )
	{
		fprintf( fp, "#define \t" );						// #define
		fprintf( fp, "%s",	pList[i]->m_strName );			// OBJECT_DAO
		AutoTab( fp , strlen(pList[i]->m_strName) );
		fprintf( fp, "%d",		pList[i]->m_nValue );		// 1
		fprintf( fp, "\t\t// %s", pList[i]->m_szRemark );	// 주석
		fprintf( fp, "\n" );
	}
	fprintf( fp, "\n" );

	pList = m_pEnemy;
	fprintf( fp, "// 몬스터\n" );
	for( i = 0; i < m_nMaxEnemy; i ++ )
	{
		fprintf( fp, "#define \t" );					// #define
		fprintf( fp, "%s",	pList[i]->m_strName );		// OBJECT_DAO
		AutoTab( fp , strlen(pList[i]->m_strName) );
		fprintf( fp, "%d",		pList[i]->m_nValue );		// 1
		fprintf( fp, "\t\t// %s", pList[i]->m_szRemark );	// 주석
		fprintf( fp, "\n" );
	}
	fprintf( fp, "\n" );

	pList = m_pWeapon;
	fprintf( fp, "// 공격무기류 \n" );
	for( i = 0; i < m_nMaxWeapon; i ++ )
	{
		fprintf( fp, "#define \t" );					// #define
		fprintf( fp, "%s",	pList[i]->m_strName );		// OBJECT_DAO
		AutoTab( fp , strlen(pList[i]->m_strName) );
		fprintf( fp, "%d",		pList[i]->m_nValue );		// 1
		fprintf( fp, "\t\t// %s", pList[i]->m_szRemark );	// 주석
		fprintf( fp, "\n" );
	}
	fprintf( fp, "\n" );

	pList = m_pItem;
	fprintf( fp, "// 아이템 \n" );
	for( i = 0; i < m_nMaxItem; i ++ )
	{
		fprintf( fp, "#define \t" );					// #define
		fprintf( fp, "%s",	pList[i]->m_strName );		// OBJECT_DAO
		AutoTab( fp , strlen(pList[i]->m_strName) );
		fprintf( fp, "%d",		pList[i]->m_nValue );		// 1
		fprintf( fp, "\t\t// %s", pList[i]->m_szRemark );	// 주석
		fprintf( fp, "\n" );
	}
	fprintf( fp, "\n" );

	pList = m_pEtc;
	fprintf( fp, "// ETC object \n" );
	for( i = 0; i < m_nMaxEtc; i ++ )
	{
		fprintf( fp, "#define \t" );					// #define
		fprintf( fp, "%s",	pList[i]->m_strName );		// OBJECT_DAO
		AutoTab( fp , strlen(pList[i]->m_strName) );
		fprintf( fp, "%d",		pList[i]->m_nValue );		// 1
		fprintf( fp, "\t\t// %s", pList[i]->m_szRemark );	// 주석
		fprintf( fp, "\n" );
	}
	fprintf( fp, "\n" );

	pList = m_pIA;
	fprintf( fp, "// Interact object\n" );
	for( i = 0; i < m_nMaxIA; i ++ )
	{
		fprintf( fp, "#define \t" );					// #define
		fprintf( fp, "%s",	pList[i]->m_strName );		// OBJECT_DAO
		AutoTab( fp , strlen(pList[i]->m_strName) );
		fprintf( fp, "%d",		pList[i]->m_nValue );		// 1
		fprintf( fp, "\t\t// %s", pList[i]->m_szRemark );	// 주석
		fprintf( fp, "\n" );
	}
	fprintf( fp, "\n" );


	fprintf( fp, "#endif\n" );


	pRes->Close();
#ifdef	_LE
	g_pWatchView->Message( "%s 저장완료", szFileName );
#endif

	return SUCCESS;
}
#endif

// Define내용을 모두 bin으로 저장
XE::xRESULT	CDefine :: SaveBIN( LPCTSTR szFileName )
{
	int		nLen = 0;

	XResFile file, *pRes = &file;
	if( pRes->Open( szFileName, XBaseRes::xWRITE ) == 0 )
	{
		XERROR( "%s open failed", szFileName );
		return XE::xFAIL;
	}
/*
	FILE	*fp = _tfopen( szFileName, _T("wb") );
	if( fp == NULL )	
	{
//		XERROR( "%s open failed", GetLastMakePath() );
		XERROR( "%s open failed", szFileName );
		return XE::xFAIL;
	}
*/
	int		i;
	DEFINE	*pList = m_pList;

	pRes->Write( &m_nMaxDef, 4 );
	for( i = 0; i < m_nMaxDef; i ++ )
	{
		nLen = _tcslen(pList[i].m_strName) + 1;
		pRes->Write( &nLen, 4 );						// strlen
		pRes->Write( pList[i].m_strName, nLen );		// Name
		pRes->Write( &pList[i].m_nValue, 4 );		// Value
	}

#ifdef	_LE
	g_pWatchView->Message( "%s 저장완료", szFileName );
#endif

	return XE::xSUCCESS;
}


XE::xRESULT	CDefine :: AddDefine( LPCTSTR szDefine, int nVal, LPCTSTR szRemark )
{
	int		nIdx = m_nMaxDef;

	if( Find( szDefine ) )
	{
		XERROR( "%s 가 이미 정의 되어 있다.  다른이름으로 할 것!", szDefine );
		return XE::xFAIL;
	}

	if( _tcslen( szDefine ) >= MAX_NAME )		// TODO: 매크로로 만들것
		XERROR( "버퍼 오버플로우 %s=%d buffSize=%d", szDefine, _tcslen( szDefine ), MAX_NAME );
	_tcscpy_s( m_pList[nIdx].m_strName, szDefine );	// OBJECT_DAO
	m_pList[nIdx].m_nValue = nVal;					// 1
	if( szRemark && XE::IsHave(szRemark) ) {
		if( _tcslen( szRemark ) >= MAX_REMARK )		// TODO: 매크로로 만들것
			XERROR( "버퍼 오버플로우 %s=%d buffSize=%d", szRemark, _tcslen( szRemark ), MAX_REMARK );
		_tcscpy_s( m_pList[nIdx].m_szRemark, szRemark );
	}
	else {
		if( _tcslen( szDefine ) >= MAX_REMARK )		// TODO: 매크로로 만들것
			XERROR( "버퍼 오버플로우 %s=%d buffSize=%d", szDefine, _tcslen( szDefine ), MAX_REMARK );
		_tcscpy_s( m_pList[nIdx].m_szRemark, szDefine );
	}

	m_nMaxDef ++;
	if( m_nMaxDef >= MAX_DEFINE )
	{
		return XE::xFAIL;
	}

	return XE::xSUCCESS;
}
