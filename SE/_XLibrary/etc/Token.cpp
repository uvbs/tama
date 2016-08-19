#include "stdafx.h"
#include <stdlib.h>
//#include "GameLib.h"
#include "xUtil.h"
#include "Global.h"
#include "Debug.h"
#include "Token.h"
#ifdef _VER_IPHONE
#include "ConvertString.h"
#endif
#include "ResMng.h"
#include "xString.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;

/*
 . 리소스파일에 저장된 스트링은 UTF16-TCHAR-WCHAR버전으로 저장되고 CToken에서도 TCHAR(unichar)로 읽은후 CToken내에 저장할때는 LPSTR(UTF8)로 저장해둔다. 스트링을 비교할때는 m_Token == "set한글"(UTF8) 식으로 비교하면 된다
 . 리소스파일내에 저장된 파일명등의 토큰은 UTF16-TCHAR-WCHAR로 저장되어 있으므로 Load( LPCSTR )함수를 호출하기전에 UTF8로 변환시켜쓴다. 변환은 NSString을 이용한다
 */
#define	__TOKEN_VER2			// this. 일경우 this만 가려내고 12.3일경우 한토큰으로 인식하는 버전

// 아이폰에서는 strFileName은 UTF8상태로 넘어와야 한다
// codeType = TXT_??? 시리즈 가 들어와야 한다. 파일의 저장상태
xRESULT	CToken::LoadFile( LPCTSTR strFileName, xtENCODING codeType )
{
	XResFile file, *pRes = &file;
	if( pRes->Open( strFileName ) == 0 )
//		XERROR( "%s open failed", strFileName );		// 여기다 이거 넣지말것 if( LoadFile() ) 식으로 파일이 없으면 그냥 지나가야 하는 경우도 많다
		return xFAIL;

	_tcscpy_s( m_strFileName, GetFileName( strFileName ) );		// 파일명 받음


	m_codeType = codeType;
#ifdef _VER_IPHONE
	if( codeType == TXT_UTF16 )
	{
		// 일단 utf16으로 전부 읽음
		int nSizeUTF16 = pRes->Size();
//		fseek( fp, 0, SEEK_END );
//		int nSizeUTF16 = ftell( pRes );			// file size
//		fseek( fp, 0, SEEK_SET );
		if( XBREAK( nSizeUTF16 <= 0 ) )
			return xFAIL;
		
		WORD mark;
		pRes->Read( &mark, 2 );
		if( mark != (WORD)0xFEFF )
			XERROR( "%s가 유니코드 문서가 아닙니다", strFileName );
		nSizeUTF16 -= 2;
		UNICHAR *pFile = new UNICHAR[ nSizeUTF16 + sizeof(UNICHAR) ];		// 파일에 저장되어있는것은 UTF16이라 가정한다
//		pRes->Read( pFile, nSizeUTF16, sizeof(UNICHAR) );		// 파일을 통째로 읽음
		pRes->Read( pFile, nSizeUTF16 );		// 파일을 통째로 읽음
		int uniSize = sizeof(unichar);
		pFile[ nSizeUTF16 / uniSize ] = _U('\0');				// 맨끝에 NULL
		
		LPCTSTR strUTF8 = _ConvertUTF16ToUTF8( pFile );		// utf8f로 변환
//		XLOG( "%s", strUTF8 );
		m_nSize = _tcslen( strUTF8 );						// 널 뺀 길이를 얻음
		m_pBuff = new TCHAR[ m_nSize + sizeof(TCHAR) ];		// 널포함 메모리 할당
		memset( m_pBuff, 0, m_nSize * sizeof(TCHAR) + sizeof(TCHAR) );
		_tcscpy_s( m_pBuff,	m_nSize+1, strUTF8 );
		m_pCurr = m_pBuff;
		m_pBuff[ m_nSize ] = _T('\0');	
		SAFE_DELETE_ARRAY( pFile );
	} else
#else
	// not iPhone
	if( codeType == TXT_UTF16 )	
	{
		m_nSize = pRes->Size() - 2;	// FEFF는 크기에서 뺌
		if( XBREAK( m_nSize <= 0 ) )
			return xFAIL;
		char *p = new char[ m_nSize + sizeof(TCHAR) ];
		m_pBuff = (TCHAR *)p;
		m_pCurr = m_pBuff;
		memset( p, 0, m_nSize + sizeof(TCHAR) );

		WORD mark;
		pRes->Read( &mark, 2 );		// 유니코드 식별자
		if( mark != (WORD)0xFEFF )
			XERROR( "%s가 유니코드 문서가 아닙니다", strFileName );
		pRes->Read( m_pBuff, m_nSize );		// 파일을 통째로 읽음
//		m_pBuff[ m_nSize ] = _T('\0');				// 맨끝에 NULL
	} else
#endif
	if( codeType == TXT_EUCKR || codeType == TXT_UTF8 )	// bnb에서는 TXT_EUCKR 옵션을 줘야 한다. _UNICODE옵션을 빼고...
	{
		m_nSize = pRes->Size();
		if( XBREAK( m_nSize <= 0 ) )
			return xFAIL;
//		fseek( fp, 0, SEEK_END );
//		m_nSize = ftell( pRes );			// file size
//		fseek( fp, 0, SEEK_SET );

		CHAR *p = new CHAR[ m_nSize + sizeof(CHAR) ];
		memset( p, 0, m_nSize + sizeof(CHAR) );
		pRes->Read( p, m_nSize );		// 파일을 통째로 읽음
#ifdef _VER_IPHONE 
		if( codeType == TXT_EUCKR )		// EUCKR을 UTF8로 변환
		{
			int nUTF8size = _GetSizeEUCKRToUTF8( p ) + sizeof(CHAR);	// UTF8로 변환되었을때의 길이를 구함
			m_pBuff = new CHAR[ nUTF8size ];
			memset( m_pBuff, 0, nUTF8size );
			_ConvertEUCKRToUTF8( m_pBuff, p, nUTF8size );			// EUCKR을 UTF8로 변환
		} else
			m_pBuff = p;		// UTF8문서라면 그냥 그대로 대입
#endif // IPHONE
#ifdef WIN32
	#ifdef _UNICODE
		if( codeType == TXT_EUCKR )		// EUCKR을 WCHAR로 변환
		{
			// EUCKR을 WCHAR(UTF16)로 변환
			int UnicodeSize = MultiByteToWideChar( CP_ACP, 0, p, (int)strlen(p), NULL, 0 );
			m_pBuff = new WCHAR[ UnicodeSize + sizeof(WCHAR) ];
			memset( m_pBuff, 0, sizeof(WCHAR) * UnicodeSize + sizeof(WCHAR) );
			MultiByteToWideChar( CP_ACP, 0, p, (int)strlen(p), m_pBuff, UnicodeSize * sizeof(WCHAR) + sizeof(WCHAR) );
		} else 
		if( codeType == TXT_UTF8 )
		{
			// UTF8을 WCHAR로 변환
		}
	#else
		// not UNICODE
		if( codeType == TXT_EUCKR )		// EUCKR을 CHAR로 변환
		{
			m_pBuff = (TCHAR *)p
		} else 
		if( codeType == TXT_UTF8 )
		{
			// UTF8을 EUCKR로 변환
		}
	#endif
#endif // WIN32
		m_pCurr = m_pBuff;
	} // EUCKR || UTF8

//	pRes->Close();

	return xSUCCESS;
}

xRESULT CToken::LoadString( LPCTSTR szString )
{
	int len = _tcslen( szString );
	m_pBuff = new TCHAR[ len + 1];
	_tcscpy_s( m_pBuff, len+1, szString );
	m_pBuff[len] = 0;
	m_pCurr = m_pBuff;
	return xSUCCESS;
}

void SendConsole( int nNum, LPCTSTR str, ... );

void CToken::SkipSpaceAndRemark( LPCTSTR &pCurr, int &nCurrLine )
{
	LPTSTR pRemark = m_szRemark;
	memset( m_szRemark, 0, sizeof(m_szRemark) );
	TCHAR data = 1;
	while( data )
	{
		// 공백 건너뜀
		while(1)
		{
			data = *pCurr;
			if( data == '\n' )	nCurrLine ++;
			if( data == '\0' )		
				break;		// 끝이면 중지
			if( !IsWhite( data ) )		break;		// 공백아닌것이 나오면 멈춤
			++pCurr;
		}

		// 위에서 읽은 문자가 /라면 주석일 가능성있음.   //  /* */지원 중복 /**/는 지원안함.
		// 주석이라면 주석부분 모두 건너뜀
		if( data == '/' )
		{
			++pCurr;
			data = *pCurr;		// 다음 문자를 읽어봄
			if( data == '\n' )	nCurrLine ++;
			if( data == 0 )	break;
			++pCurr;
			if( data == '/' )		// 한번더 /가 나왔으므로 주석 시작.
			{
				BOOL bEmpty = XE::IsEmpty( m_szRemark );
				while(1)			// \r이나 \0를 만날때까지 계속 포인터 증가
				{
					data = *pCurr;	
					if( data == '\n' )			// pc에선 \r\n으로 저장되고 맥에선 \n으로만 저장된다
					{ /*pCurr++;*/ nCurrLine ++; break; }		// '\r'을 만나면 그뒤의 \n까지 건너뜀.  여기서 처리가 끝난것이기땜에 data를 갱신시켜줘야 한다.
					if( bEmpty )	// //주석이 연속으로 나오면 앞에 주석만 취하도록
						if( pRemark < m_szRemark + (sizeof(m_szRemark) / sizeof(m_szRemark[0])) )	// 버퍼오버 검사
							*pRemark++ = data;		// 주석을 버퍼로 옮김. // 주석만 인식하게 일단...
					if( data == '\0' )	break;					// NULL 만나면 중지
					++pCurr;
				}
			} else
			if( data == '*' )		// /* 주석 시작
			{
				// *나 NULL을 검사
				// */이나 \0 나올때까지 계속 포인터 증가.  \r\n을 검사하지 않음
				while(1)
				{
					data = *pCurr;
					if( data == '\n' )	nCurrLine ++;
					if( data == '*' )			// *를 만나면 */일 가능성 있음
					{
						data = *++pCurr;		// 곧바로하나 더 읽어서
						if( data == '\n' )	nCurrLine ++;
						if( data == '/' || data == '\0' )	
						{
							++pCurr;
							break;		// */를 만났다.  주석끝
						}
						// 아니었다면 계속 *를 찾음
					}
					++pCurr;
				}
			}
			else
			// 스페이스도 건너뛰고 주석도아니었다면 루프를 빠져나와서 이제부터 토큰을 읽는다. NULL 포함
			{
				break;
			}
			// 이곳까지 왔다면 주석문처리를 하고 온것이다.  
			// 계속해서 스페이스나 주석이 있을지 모르니 반복
			// continue;
		} else // if '/'
		// 주석이 아니라면 빠져나감  NULL포함
		{
			break;
		}
	} // while
}

// 현재 포인터에서 token하나를 읽어서 m_Token[]에 담는다.
LPCTSTR CToken::GetToken()
{
	_tcscpy_s( m_PrevToken, m_Token );
	LPCTSTR pCurr = m_pCurr;
	LPTSTR pToken = m_Token;
    TCHAR *pEnd = &pToken[ MAX_TOKEN-2 ]; // 버퍼의 끝. -1이 맞지만 안전한 디버깅을 위해 -2로 했음
	TCHAR data = 1;
	int		nCurrLine = m_nCurrLine;
	memset( m_Token, 0, sizeof(m_Token) );	// 버퍼 클리어
	memset( m_szRemark, 0, sizeof(m_szRemark) );

	// 공백과 주석을 건너뜀. "//" 주석은 버퍼에 담음
	SkipSpaceAndRemark( pCurr, nCurrLine );

	data = *pCurr++;
	// 루프 빠져나옴
	if( data == '\0' )	
	{ 
		/*pCurr--;  */m_nCurrLine = nCurrLine;  
		return NULL; /*pCurr--;	goto EXIT1; */
	}	// 널이면 빠져나감. 포인터는 널을 지나왔으므로 되돌림

	// 기호들 가려냄
	// 아래 기호들을 나왔다면 요놈만 읽고 리턴한다
//	data = *pCurr++;
//	if( strchr( "~!@#$%^&*()-=|<>?,./{}[]", data ) )		// 기호냐?
	if( _tcschr( _T("~!@#$%^&*()-=|<>?,./{}[];:"), data ) )		// 기호냐?
	{
        XBREAKF( pToken >= pEnd, "%s: %s", m_strFileName, m_Token );
		*pToken++ = data;							// 기호를 버퍼에 넣고
        XBREAKF( pToken >= pEnd, "%s: %s", m_strFileName, m_Token );
		*pToken = '\0';								// 널까지 맞춰서
		goto EXIT1;									// 빠져나감
	}

	// 따옴표를 만나면 스트링 가려냄  "우하하", "ABC DEF" "c:\xuzhu\data" 일때 따옴표는 빼고 내용만 추출
//	data = *pCurr++;
	if( data == '"' )
	{
		while(1)
		{
			data = *pCurr++;
			if( data == '\n' )	nCurrLine ++;
			if( data == '"' )	break;		// 스트링 끝
			if( data == '\0' )	break;
            XBREAKF( pToken >= pEnd, "%s: %s", m_strFileName, m_Token );
			*pToken++ = data;				// 따옴표 나올때까지 모두 버퍼에 옮김
		}
		if( data == '\0' )	pCurr--;
        XBREAKF( pToken >= pEnd, "%s: %s", m_strFileName, m_Token );
		*pToken = '\0';						// 스트링끝에 널
		goto EXIT1;							// 할일 다 끝났음
	}

	// 단어인가? 단어를 가려냄, 알파벳, 한글, 숫자, 밑줄(_)로 시작해서
	// 알파벳, 한글, 숫자, 밑줄, 점(.)이 포함된 연속적인 단어는 한 토큰으로 인식.
//	data = *pCurr++;
	// 알파벳, 한글, 숫자, _밑줄, 점(.) 중의 하나일때.
	// ABc, 3DS, AB_CD, AB12, AB12한글, 한글12A, _AB, 12.3, 12.3ABC
	if( IsAlpha(data) || IsHangul(data) || IsNumeric(data) || data == '_' )
	{
        XBREAKF( pToken >= pEnd, "%s: %s", m_strFileName, m_Token );
		*pToken++ = data;		// 일단 첫자읽은건 옮김
		while(1)
		{
			data = *pCurr;		// 다음 문자 읽고 이동
			if( data == '\n' )	nCurrLine ++;
#ifdef __TOKEN_VER2		// this. 일경우 this만 가려내고 12.3일경우 한토큰으로 인식하는 버전
			// 알파벳도 아니고 _도 아니고 .도 아니고 숫자도 아니고 한글도 아니면 중지
			if( !IsAlpha(data) && data != '_' && data != '.' && !IsNumeric(data) && !IsHangul(data) )	break;
			if( data == '.' && IsNumeric( m_Token )==xNONE )	// .이 나왔고 앞의토큰이 순수숫자가 아니었으면
				break;			// this. 혹은 obj1. 으로 인식하고 중지.
#else
			// 알파벳도 아니고 _도 아니고 .도 아니고 숫자도 아니고 한글도 아니면 중지
			if( !IsAlpha(data) && data != '_' && data != '.' && !IsNumeric(data) && !IsHangul(data) )	break;
#endif
            XBREAKF( pToken >= pEnd, "%s: %s", m_strFileName, m_Token );
			*pToken++ = data;		// 조건에 통과한 글자는 토큰버퍼에 넣음
			++pCurr;
		}
//		if( data == '\n' )	pCurr += 2; 	//  \r을 만나면 바로뒤의 \n도 건너뛰고 그다음 포인터에 위치시킨다.
        XBREAKF( pToken >= pEnd, "%s: %s", m_strFileName, m_Token );
		*pToken = '\0';				// 마무리
		goto EXIT1;
	}
EXIT1:
	// 토큰을 다 읽고 난후 그 뒤에 붙었는는 공백이나 주석을 건너뛴다. 주석은 버퍼에 옮긴다. 
//	if( _tcsicmp( m_Token, _T("endif") ) == 0 )
//		XBREAK(1);
	SkipSpaceAndRemark( pCurr, nCurrLine );	
//	if( data == 0 )
//		XBREAK(1);
//	if( data )			// end of file 에선 뒤로 감으면 안된다
//		pCurr--;		// 주석담에 나온 토큰의 첫자를 읽어버리고 pCurr++하기 때문에 한글자를 잃어버린다 그래서 포인터 감아줌
	// SkipSpaceAndRemark내부에서 pCurr 마무리를 제대로 하게 바꿈.

	m_pCurr = pCurr;
	m_nCurrLine = nCurrLine;
#ifdef _VER_IPHONE
	if( m_codeType == TXT_EUCKR )
	{
		// 아이폰 BNB전용 코드.
		char utf8str[MAX_TOKEN];
		_ConvertEUCKRToUTF8(utf8str, m_Token, MAX_TOKEN);	// utf8로 변환해서 리턴한다.
		strcpy( m_Token, utf8str );
	}
#endif
	// 토큰 읽은게 없으면 end of file로 인식
	if( m_Token[0] == 0 )
		return NULL;
//	data = *pCurr;
//	if( data == 0 )
//		return NULL;
	return m_Token;
}

// cData가 알파벳이냐
BOOL	CToken::IsAlpha( TCHAR cData )
{
	if( (cData >= 'A' && cData <= 'Z') || (cData >= 'a' && cData <= 'z') )
		return TRUE;
	return FALSE;
}

// 한글인가
BOOL	CToken::IsHangul( TCHAR cData )
{
#ifdef UNICODE
	return ( cData >= 0x80 ) ? TRUE : FALSE;
#else
	return (cData & 0x80) ? TRUE : FALSE;
#endif
	return FALSE;
}

// 0 ~ 9
BOOL	CToken::IsNumeric( TCHAR cData )
{
	if( cData >= '0' && cData <= '9' )	
		return TRUE;
	return FALSE;
}

// 1 ~ 32(space)까지, TAB(9)도 있다.
BOOL	CToken::IsWhite( TCHAR cData )
{
	if( cData > 0 && cData <= 32 )
		return TRUE;
	return FALSE;
}



// 숫자로 읽기
int		CToken::GetNumber()
{
	PushPos();		// 현재 위치 백업
	GetToken();		// 일단 다음토큰을 하나 읽어보고
	if( m_Token[0] == '-' || m_Token[0] == '+' )		// +, - 면...
		GetToken();			// 다음토큰을 하나더 읽어본다
	if( m_Token[0] == 0 )
		return TOKEN_EOF;
	if( IsNumeric(m_Token) == xNONE )	// 읽었는데 숫자형이 아니면 경고 내보냄
	{
		TCHAR szBuff[32];
		_tcsncpy_s( szBuff, m_pCurr, 30 );
		XLOG_ALERT( "%s 읽던중 에러\n라인:%d\n토큰:%s\n현재위치: %s", m_strFileName, m_nCurrLine, m_Token, szBuff );
		PopPos();
		return TOKEN_ERROR;
	}
	PopPos();
	GetToken();				// 토큰 하나 읽고
//	return ToNumber( m_Token );
	if( m_Token[0] != '\0' )
	{
		switch( m_Token[0] )	// 첫문자 읽어보고 +나 -면 토큰 하나 더 읽어서 부호로 쓴다.
		{
			case '=':	return -1;		// csv파일등에서 '생략'이라는 의미로 쓰는 =문자.
			case '-': 	GetToken();	return -_tstoi( m_Token );
			case '+':	GetToken();	break;
				break;
		}
		if( m_Token[0] == '0' && m_Token[1] == 'x' )		// 16진수 숫자다
			return _tcstol( m_Token, NULL, 16 );
		return _tstoi( m_Token );		// 숫자로 변환
	}
	return TOKEN_ERROR; 
}
int CToken::ToNumber( LPCTSTR szToken )
{
    TCHAR *szErr = NULL;
	// 문자열토큰을 숫자로 변환
    int num = _tcstol( szToken, &szErr, 10 );
    if( szErr )
    {
        XERROR( "%s 토큰은 숫자가 될 수 없습니다", szToken );
		return TOKEN_ERROR;
    }
/*	int num = _tstoi( szToken );
	if( errno == EINVAL )	// 변환하려는 스트링이 숫자형이 아니면 경고 내보냄
	{
		XERROR( "%s 토큰은 숫자가 될 수 없습니다", szToken );
		return TOKEN_ERROR;
	} */
 
	return num;
}

// 실수 전용 읽기
float	CToken::GetNumberF()
{
	float	fVal;

//	GetNextToken();
//	if( IsNumeric( m_NextToken ) == xNONE )	// 읽었는데 숫자형이 아니면 경고 내보냄
	PushPos();		// 현재 위치 백업
	GetToken();		// 일단 다음토큰을 하나 읽어보고
	if( m_Token[0] == '-' || m_Token[0] == '+' )		// +, - 면...
		GetToken();			// 다음토큰을 하나더 읽어본다
	if( m_Token[0] == 0 )
		return (float)TOKEN_EOF;
	if( IsNumeric( m_Token ) == xNONE )	// 읽었는데 숫자형이 아니면 경고 내보냄
	{
		TCHAR szBuff[32];
		_tcsncpy_s( szBuff, m_pCurr, 30 );
		XLOG_ALERT( "%s(%d) 읽던중 에러 \n%s\n현재위치: %s", m_strFileName, m_nCurrLine, m_Token, szBuff );
		PopPos();
//		float f;
//		*((DWORD *)&f) = (float)TOKEN_ERROR;
		return (float)TOKEN_ERROR;
	}
	else 
	if( IsNumeric( m_Token ) == xINT )	// float로 읽으라고 했는데 토큰이 int형이면 getNumber로 읽어 변환해준다
	{
		PopPos();
		return (float)GetNumber();
	}
	PopPos();
	GetToken();
	if( m_Token[0] != '\0' )
	{
		switch(m_Token[0])
		{
			case '=':	return -1;
			case '-':	GetToken();		fVal = (float)(-_tstof( m_Token ));	return fVal;
			case '+':	GetToken();		break;
		}
		return (float)_tstof( m_Token );
	}
	float f;
	*((DWORD *)&f) = TOKEN_ERROR;
	return f;
}

// 현재 라인은 모두 건너뛰고 다음 라인으로 포인터를 이동시킴
LPCTSTR CToken::NextLine()
{
	LPCTSTR pCurr = m_pCurr;
	TCHAR data;

	while(1)
	{
		data = *pCurr++;		// 모두 건너뛰고
		if( data == '\n' )		// \r을 만나면
		{
			/*pCurr++;*/	// \n도 넘어가고
			break;		// 빠져나가면 다음 라인이다.
		}
	}

	m_pCurr = pCurr;
	return m_pCurr;
}

// 스트링형태의 데이타가 숫자로 변환될만한 것인가
// 숫자, .외에 다른것이 있다면 숫자가 아닌걸로 판단한다.
// 에러 검출에 사용된다
#define __TOKEN_ISNUMERIC2
#ifdef __TOKEN_ISNUMERIC2
CToken::xTYPE CToken::IsNumeric( LPCTSTR pSrc )
{
	int		nLen = _tcslen( pSrc );
	int		i;
	TCHAR cData;
	CToken::xTYPE	bRetv = xNONE;

	if( pSrc[0] == '=' )
		return xINT;
	if( pSrc[0] == '0' && pSrc[1] == 'x' )
		return xHEX;
	for( i = 0; i < nLen; i ++ )
	{
		cData = *pSrc++;
		// 123 123.1 . 12d.1  1d2.1
		if( cData < '0' || cData > '9' )
		{
			if( cData != '.' )	return xNONE;		// 0~9도 아니고 .도 아닌문자 나오면 무조건 FALSE리턴
			else
			{		
				// .이 나왔는데 위에서 안걸러졌다는것은 숫자만 나왔거나 .이 처음나왔다는것.
				if( bRetv == xNONE )	return xNONE;	// 숫자가 나온적없다면 .이 처음 나온것이므로 FALSE
				bRetv = xFLOAT;		// .이 나왔으므로 일단 float형
			}
		} else
			if( bRetv != xFLOAT )	// float로 결정지어진 후엔 int로 바꾸지 않음
				bRetv = xINT;		// 숫자가 먼저 나왔었으면.
	}
	return bRetv;
}
#else
BOOL	CToken::IsNumeric( LPCTSTR pSrc )
{
	int		nLen = _tcslen( pSrc );
	TCHAR cData;

	for( int i = 0; i < nLen; i ++ )
	{
		cData = *pSrc++;
		if( cData < '0' || cData > '9' )
			if( cData != '.' )	return FALSE;
	}
	return TRUE;
}
#endif

TCHAR CToken::FastRead()
{
	return *m_pCurr++;
}

void CToken::SetStartLoop()
{
	m_pLoop = m_pCurr;
}
void CToken::JumpLoop()
{
	m_pCurr = m_pLoop;
}

void CToken::PushPos()
{
	m_pBackupPos = (LPTSTR)m_pCurr;
	_tcscpy_s( m_BackupToken, m_Token );
}
void CToken::PopPos()
{
	m_pCurr = (LPCTSTR)m_pBackupPos;
	_tcscpy_s( m_Token, m_BackupToken );
}
// 이번에 읽을토큰을 포인터이동없이 읽어낸다.
LPCTSTR CToken::GetNextToken()
{
	LPCTSTR pBack = m_pCurr;
	TCHAR pBackToken[ MAX_TOKEN ];
	_tcscpy_s( pBackToken, m_Token );			// 원래토큰스트링 백업.
	GetToken();
	_tcscpy_s( m_NextToken, m_Token );
	_tcscpy_s( m_Token, pBackToken );			// 백업스트링을 다시 복구.
	m_pCurr = pBack;							// 포인터도 다시 되돌림.
	return m_NextToken;
}

// 알파벳으로 시작하고 기호가 포함되어 있지 않은 문자열인가.
BOOL CToken::IsName( LPCTSTR str )
{
	BOOL bFlag = TRUE;
	if( !IsAlpha( str[0] ) )	bFlag = FALSE;		// 첫글자가 문자가 아니면 FALSE
	LPCTSTR pStr = str;
	TCHAR c;
	while( c = *pStr++ )
	{
		if( !IsAlpha(c) && !IsNumeric(c) )		{		// 문자도 아니고 숫자도 아닌게 나오면 FALSE
			bFlag = FALSE;
			break;
		}
	}
	return bFlag;
}
// } 가 나올때까지 모든 내용을 스트링으로 읽는다
char* CToken::CreateGetBlock( const char *szHeader )
{
	LPCTSTR pCurr = m_pCurr;
	TCHAR *pBuff = m_TempBuff;
	TCHAR *pBuffEnd = &m_TempBuff[sizeof(m_TempBuff)/sizeof(TCHAR)];
	TCHAR data = 0;
	memset( m_TempBuff, 0, sizeof(m_TempBuff) );
	// } 가 나올때까지 모든 글자를 버퍼에 담는다
	int numBlock = 1;
	while(1)
	{
		data = *pCurr++;
		if( data == '}' )	
		{
			if( --numBlock <= 0 )
				break;
		}
		else if( data == '{' )
			numBlock++;
		*pBuff++ = data;
		if( XBREAKF( pBuff >= pBuffEnd, "buffer overflow" ) )
			break;
	}
#ifdef WIN32
	// TCHAR로 읽은 블럭내용을 char*로 변환한다
	int sizeMultiByte = WideCharToMultiByte( CP_ACP, 0, m_TempBuff, -1, NULL, 0, NULL, NULL );			// 멀티바이트로 변환될때 필요한 버퍼크기를 얻는다. 널포함해서 얻어진다
	XBREAK( sizeMultiByte <= 0 );
	if( szHeader )
	{
		int len = strlen( szHeader ) + 1;		// space포함
		char *szChar = new char[ sizeMultiByte + len + 1 ];	// numm + szHeader포함해서 할당
		strcpy_s( szChar, sizeMultiByte+len+1, szHeader );
		szChar[len-1] = ' ';		// 헤더뒤에 스페이스 하나 띄워줌
		WideCharToMultiByte( CP_ACP, 0, m_TempBuff, -1, szChar+len, sizeMultiByte, NULL, NULL );
		m_pCurr = pCurr;
		return szChar;
	} else
	{
		char *szChar = new char[ sizeMultiByte ];
		WideCharToMultiByte( CP_ACP, 0, m_TempBuff, -1, szChar, sizeMultiByte, NULL, NULL );
		m_pCurr = pCurr;
		return szChar;
	}
#else
    if( szHeader )
    {
        int lenHeader = strlen( szHeader ) + 1; // space 포함
        int lenLua = strlen( m_TempBuff );
        char *szChar = new char[ lenLua + lenHeader + 1 ]; // null 포함 
        strcpy_s( szChar, lenLua+lenHeader+1, szHeader );
        szChar[lenHeader-1] = ' ';  // 헤더뒤에 스페이스 하나 띄움
        strcpy_s( szChar+lenHeader, lenLua+lenHeader+1, m_TempBuff );   // 본문을 카피
        m_pCurr = pCurr;
        return szChar;
    } else
    {
        int len = strlen(m_TempBuff)+1;
        char *szChar = new char[ len ];
        strcpy_s( szChar, len, m_TempBuff );
        m_pCurr = pCurr;
        return szChar;
    }
#endif
	
}
