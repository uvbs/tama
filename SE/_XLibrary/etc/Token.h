#ifndef		__TOKEN_H__
#define		__TOKEN_H__
#include <string.h>
#include "XE.h"
#include "Global.h"
#include "xString.h"
#define		MAX_TOKEN		512

//#define TXT_EUCKR	0
//#define TXT_UTF8	1
//#define TXT_UTF16	2

#define TOKEN_ERROR		0x7fffffff
#define TOKEN_EOF		0x7ffffffe		// end of file
class CToken
{
	TCHAR m_TempBuff[0xffff];
	void Init() {
		m_codeType = TXT_EUCKR;
		m_pCurr = NULL;
		m_pBuff = NULL;
		m_pBackupPos = NULL;
		m_nCurrLine = 0;
		memset( m_Token, 0, sizeof(m_Token) );
		memset( m_strFileName, 0, sizeof(m_strFileName) );
		memset( m_BackupToken, 0, sizeof(m_BackupToken) );
		memset( m_szRemark, 0, sizeof(m_szRemark) );
		memset( m_TempBuff, 0, sizeof(m_TempBuff) );
		m_NextToken[0] = 0;
	}
	void Destroy()
	{
		SAFE_DELETE_ARRAY( m_pBuff );
		m_pCurr = NULL;
	}
public:
	 enum xTYPE { xNONE=0, xINT, xFLOAT, xHEX };
	TCHAR m_strFileName[256];	// 나중에 CHAR(UTF8)버전으로 바꿔라 파일에서 읽을땐 TCHAR-WCHAR-unichar로 읽어서 변환해라 
	xtENCODING m_codeType;			// 파일이 어떤코드로 저장되어 있었는가
	LPCTSTR m_pCurr;			// 현재 포인터
	LPTSTR m_pBuff;			// 시작 포인터
	LPTSTR m_pBackupPos;	// 현재위치 백업
	int		m_nSize;			// 파일 사이즈
	int		m_nCurrLine;		// 현재 읽고있는 라인수
	LPCTSTR m_pLoop;		// 루프시작지점

	TCHAR m_PrevToken[ MAX_TOKEN ];
	TCHAR m_Token[ MAX_TOKEN ];		// 읽은 토큰 스트링
	TCHAR m_NextToken[ MAX_TOKEN ];		// 다음 토큰 스트링
	TCHAR m_BackupToken[ MAX_TOKEN ];	// PushPos에서 씀
	TCHAR m_szRemark[ MAX_TOKEN ];		// 주석

	CToken() { Init(); }
/*	CToken( TCHAR *szString ) {
		m_codeType = TXT_UTF16;
		m_pBuff = szString;
		mpCurr = m_pBuff;
		m_pBackupPos = m_pBuff;
		m_nSize = _tcslen( szString );
	} */
	virtual ~CToken() { Destroy(); }

	BOOL	IsNumeric( TCHAR cData );
	xTYPE	IsNumeric( LPCTSTR pSrc );
	BOOL	IsHangul( TCHAR cData );
	BOOL	IsAlpha( TCHAR cData );
	BOOL	IsWhite( TCHAR cData );
	BOOL IsName( LPCTSTR str );

	XE::xRESULT	LoadFile( LPCTSTR strFileName, xtENCODING codeType );			// 파일로 부터 스크립트를 읽음.
	XE::xRESULT LoadString( LPCTSTR szString );		// 
	void SkipSpaceAndRemark( LPCTSTR &pCurr, int &nCurrLine );

	LPCTSTR GetToken();						// 읽은 스크립트로부터 토큰 하나를 읽음
	LPCTSTR GetNextToken();				// 이번에 읽을토큰을 포인터이동없이 읽어낸다.
	char* CreateGetBlock( const char *szHeader=NULL );		// } 가 나올때까지 모든내용을 스트링으로 읽은후 char*형으로 변환해서 돌려준다, szHeader가 있으면 앞부분에 붙여준다
//	void GetBlock();							
	virtual int		GetNumber();						// 다음 토큰을 숫자 정수로 읽기
	float	GetNumberF();						// 실수로 읽기
	float	GetFloat() { return GetNumberF(); }
	int ToNumber( LPCTSTR szToken );			// 문자열토큰을 정수형으로 변환
	LPCTSTR GetRemark() { return m_szRemark; }
	LPCTSTR NextLine();						// 다음 라인으로 건너뛰기
	TCHAR FastRead();
	void SetStartLoop();						// 루프시작점 지정.
	void JumpLoop();							// 루프시작점으로 점프.
	void PushPos();			// 현재 위치와 토큰을 백업받는다
	void PopPos();				// PushPos으로 백업받은 위치와 토큰을 되살린다


	BOOL operator == (LPCTSTR s1)
	{ return _tcsicmp( m_Token, s1 ) == 0; }
	BOOL operator != (LPCTSTR s1)
	{ return _tcsicmp( m_Token, s1 ) != 0; }
};

//num = token.GET_NUMBER()
//#define GET_NUMBER()	( IsNumeric( m_Token ) == xFLOAT )? GetNumberF(): GetNumber()

#endif
