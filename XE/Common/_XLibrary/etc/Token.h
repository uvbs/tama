#ifndef		__TOKEN_H__
#define		__TOKEN_H__
#include <string.h>
#include "XE.h"
#include "etc/global.h"
#include "xString.h"
#define		MAX_TOKEN		4096

//#define TXT_EUCKR	0
//#define TXT_UTF8	1
//#define TXT_UTF16	2

#define TOKEN_ERROR		(DWORD)0x7fffffff
#define TOKEN_EOF		(DWORD)0x7ffffffe		// end of file
class XBaseRes;
class CToken
{
	enum { MAX_TEMP=0x10000, xEOF=1, xINVALID_NUMBER=2,  };
//	TCHAR m_TempBuff[0xffff];
	TCHAR *m_pTempBuff;
	int		m_nCurrLine;		// 현재 읽고있는 라인수
	int m_Error;			///< 0:success 1:eof 2:invalid_number
	void Init() {
		m_codeType = XE::TXT_EUCKR;
		m_pCurr = NULL;
		m_pBuff = NULL;
		m_pBackupPos = NULL;
		m_nCurrLine = 0;
		m_nCurrLineBack = 0;
		XCLEAR_ARRAY( m_PrevToken );
		XCLEAR_ARRAY( m_NextToken );
		memset( m_Token, 0, sizeof(m_Token) );
		memset( m_strFileName, 0, sizeof(m_strFileName) );
		memset( m_BackupToken, 0, sizeof(m_BackupToken) );
//		memset( m_szRemark, 0, sizeof(m_szRemark) );
//		memset( m_TempBuff, 0, sizeof(m_TempBuff) );
		m_pTempBuff = NULL;
		m_NextToken[0] = 0;
		m_Error = 0;
	}
	void Destroy( void )
	{
		SAFE_DELETE_ARRAY( m_pTempBuff );
		SAFE_DELETE_ARRAY( m_pBuff );
		m_pCurr = NULL;
	}
	xRESULT	_Load( XBaseRes *pRes, XE::xtENCODING codeType );
public:
	 enum xTYPE { xNONE=0, xINT, xFLOAT, xHEX };
	TCHAR m_strFileName[256];	// 나중에 CHAR(UTF8)버전으로 바꿔라 파일에서 읽을땐 TCHAR-WCHAR-unichar로 읽어서 변환해라 
	XE::xtENCODING m_codeType;			// 파일이 어떤코드로 저장되어 있었는가
	LPCTSTR m_pCurr;			// 현재 포인터
	LPTSTR m_pBuff;			// 시작 포인터
	LPTSTR m_pBackupPos;	// 현재위치 백업
	int m_nCurrLineBack;	// 라인수 백업
	int		m_nSize;			// 파일 사이즈
	LPCTSTR m_pLoop;		// 루프시작지점

	TCHAR m_PrevToken[ MAX_TOKEN ];
	TCHAR m_Token[ MAX_TOKEN ];		// 읽은 토큰 스트링
	TCHAR m_NextToken[ MAX_TOKEN ];		// 다음 토큰 스트링
	TCHAR m_BackupToken[ MAX_TOKEN ];	// PushPos에서 씀
#if defined(_CLIENT) && defined(WIN32)
//	TCHAR m_szRemark[ MAX_TOKEN ];		// 주석
	///< GetToken()을 하면 토큰이 발견되기 전까지의 주석(//)들을 라인단위로 리스트에 붙인다.
	XList4<_tstring> m_listRemarkPrev;	///< 토큰 이전의 주석리스트
	XList4<_tstring> m_listRemarkCurrLine;	///< 토큰 라인의 주석
//	XList<_tstring> m_listRemarkNext;	///< 토큰 이후의 주석리스트.
#endif
public:
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
	BOOL IsEof( void ) {
//		return m_Error == 1;
		return (*m_pCurr) == 0 || m_Error == xEOF;
	}
	BOOL IsError( void ) {
		return m_Error == 2;
	}
	BOOL IsFail( void ) {
		return m_Error != 0;
	}
	int		GetCurrLine( void ) {
		return m_nCurrLine + 1;		// \n문자는 이미 읽은후에 CurrLine++을 하므로 현재라인은 +1이 된다.
	}

	xRESULT	LoadAbsolutePath( LPCTSTR szFile, XE::xtENCODING codeType );
	xRESULT	LoadFromWork( LPCTSTR szRes, XE::xtENCODING codeType );
	xRESULT	LoadFromDoc( LPCTSTR szRes, XE::xtENCODING codeType );
	bool LoadFromPackage( LPCTSTR _szRes, XE::xtENCODING codeType );
	xRESULT	LoadFile( LPCTSTR strFileName, XE::xtENCODING codeType );			// 파일로 부터 스크립트를 읽음.
	inline xRESULT LoadFile( const _tstring& strFileName, XE::xtENCODING codeType ) {			// 파일로 부터 스크립트를 읽음.
		return LoadFile( strFileName.c_str(), codeType );
	}
	xRESULT LoadString( LPCTSTR szString );		// 
	void SkipSpaceAndRemark( LPCTSTR &pCurr, int &nCurrLine, XList4<_tstring> *pOutListRemark, BOOL bReadRemark1Line = FALSE );

	LPCTSTR GetToken( void );						// 읽은 스크립트로부터 토큰 하나를 읽음
	LPCTSTR GetToken2();
	_tstring GetTokenByComma();
	LPCTSTR GetNextToken( void );				// 이번에 읽을토큰을 포인터이동없이 읽어낸다.
	/// true를 리턴하면 eof다.
	bool GetToken( int *pOut ) {
		if( pOut )
			*pOut = GetNumber();
		return IsEof() != FALSE;
	}
	/// true를 리턴하면 eof다.
	bool GetToken( float *pOut ) {
		if( pOut )
			*pOut = GetNumberF();
		return IsEof() != FALSE;
	}
	/// true를 리턴하면 eof다.
	bool GetToken( XINT64 *pOut ) {
		if( pOut )
			*pOut = GetNumber64();
		return IsEof() != FALSE;
	}
	char* CreateGetBlock( const char *szHeader=NULL );		// } 가 나올때까지 모든내용을 스트링으로 읽은후 char*형으로 변환해서 돌려준다, szHeader가 있으면 앞부분에 붙여준다
//	void GetBlock( void );							
	virtual int		GetNumber( bool bCheckError = true );						// 다음 토큰을 숫자 정수로 읽기
	LONGLONG GetNumber64( void );						// 다음 토큰을 숫자 정수로 읽기
	float	GetNumberF( void );						// 실수로 읽기
	float	GetFloat( void ) { return GetNumberF(); }
	bool GetBool() ;
	int ToNumber( LPCTSTR szToken );			// 문자열토큰을 정수형으로 변환
// 	LPCTSTR GetRemark( void ) { return m_szRemark; }
// 	BOOL IsHaveRemark( void ) {
// 		return XE::IsEmpty(m_szRemark) == FALSE;
// 	}
	LPCTSTR NextLine( void );						// 다음 라인으로 건너뛰기
	TCHAR FastRead( void );
	void SetStartLoop( void );						// 루프시작점 지정.
	void JumpLoop( void );							// 루프시작점으로 점프.
	void PushPos( void );			// 현재 위치와 토큰을 백업받는다
	void PopPos( void );				// PushPos으로 백업받은 위치와 토큰을 되살린다
	_tstring GetTokenPath();


	BOOL operator == (LPCTSTR s1)
	{ return _tcsicmp( m_Token, s1 ) == 0; }
	BOOL operator != (LPCTSTR s1)
	{ return _tcsicmp( m_Token, s1 ) != 0; }
};

//num = token.GET_NUMBER()
//#define GET_NUMBER()	( IsNumeric( m_Token ) == xFLOAT )? GetNumberF(): GetNumber()

#endif
