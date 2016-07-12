#include "stdafx.h"
#include "XBaseProp.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define XPROP_LOG(F,...) __xLog( XLOGTYPE_LOG, _T("%s->%s{ }:\n")_T(F), token.m_strFileName, (szBlockName)? szBlockName : _T(""), ##__VA_ARGS__ )	
#define XPROP_BREAKF(EXP, F, ...)		(( (EXP) ) ? XPROP_LOG( F,  ##__VA_ARGS__ ), (AfxDebugBreak(), 1)  : 0)
////////////////////////////////////////////////////////////////////////////////////////////////////
// token의 상태는 { 가 이미 지나간상태여야 한다
/*
BOOL XBaseProp::LoadBlock( CToken& token, void *pBlockObj, LPCTSTR szBlockName, ID _idBlock )
{
	TCHAR szLastToken[64];
	XCLEAR_ARRAY( szLastToken );
	ID idBlock = 0;
	while(1)
	{
		if( token.GetToken() == NULL )	break;
		if( token == _T("{") ) {
#pragma message( "이름 없어도 블럭생성되게 해서 하위계층으로 계속 복사되도록. 스킬처럼" )
//			if( szBlockName )	// 블럭네임이 있을때만 블럭을 생성한다. 스크립트에서 최상위 블럭은 보통 이게 널이다
			void *pSendBlock = NULL;
			void *pNewBlock = pSendBlock = CreateNewBlock( szLastToken, idBlock );
			if( pNewBlock == NULL )		// 새로 할당 못받은경우는 상위에서 넘겨준 포인터로 넘겨준다.
				pSendBlock = pBlockObj;
			if( LoadBlock( token, pSendBlock, szLastToken, idBlock ) == FALSE )
			{
				//XPROP_BREAKF( 1, "%s블럭 읽기실패", szLastToken );
				XBREAKF( 1, "%s->%s{ }:\n알수없는 기호 나옴", token.m_strFileName, (szBlockName)? szBlockName : _T("") );
				SAFE_DELETE( pNewBlock );
				return FALSE;
			} else {
				if( pNewBlock )		// 어쨋든 여기서 블럭 할당 못받은경우는 리스트에 추가할수 없다
					AddNewBlock( pNewBlock );
			}
		}
		else if( token == _T("}") ) 
				break;
		else {
			// 바로 다음 토큰이 블럭{ 이라면 블럭이름으로 인식
			TCHAR szNextToken1[ MAX_TOKEN ];
			TCHAR szNextToken2[ MAX_TOKEN ];
			token.PushPos();		// 현재 위치 백업
			_tcscpy_s( szNextToken1, token.GetToken() );
			_tcscpy_s( szNextToken2, token.GetToken() );		// 다음, 다다음 토큰까지 읽어본다
			token.PopPos();	// 토큰위치 복구
			if( szNextToken1[0] == '{' ) {
				if( token.IsNumeric( token.m_Token ) )				// { 앞에 있는 토큰이 숫자형태라면 
					idBlock = token.ToNumber( token.m_Token );	// 일단 idblock으로 가정
				else {
					_tcscpy_s( szLastToken, token.m_Token );
					idBlock = 0;
				}
			} else if( token.IsNumeric( szNextToken1 ) && szNextToken2[0] == '{' )	// 다음 토큰이 숫자+{ 면 블럭 아이디로 인식
			{
				_tcscpy_s( szLastToken, token.m_Token );	// 일단 블럭이름 받아놓고
				idBlock = token.GetNumber();		// block id
			} else {
				// 토큰 파싱은 버추얼 파서에게 맞긴다
				if( OnParse( token, pBlockObj, szBlockName ) == FALSE ) {	 
				XERROR( "%s->%s{ }:\n알수없는 기호 나옴", token.m_strFileName, (szBlockName)? szBlockName : _T("") );
//					XPROP_BREAKF( 1, "알수없는 기호나옴: %s", token.m_Token );
//					return FALSE;	// 알수없는 기호나와도 일단 계속 읽게 하자
				}
			}
		}
	}
	return TRUE;
}
*/
BOOL XBaseProp::LoadBlock( CToken& token, void *pBlockObj, LPCTSTR szBlockName, ID _idBlock )
{
	TCHAR szLastToken[64];
	XCLEAR_ARRAY( szLastToken );
	ID idBlock = 0;
	while(1)
	{
		if( token.GetToken() == NULL )	break;
		if( token == _T("{") ) {
#pragma message( "이름 없어도 블럭생성되게 해서 하위계층으로 계속 복사되도록. 스킬처럼" )
//			if( szBlockName )	// 블럭네임이 있을때만 블럭을 생성한다. 스크립트에서 최상위 블럭은 보통 이게 널이다
			void *pSendBlock = NULL;
			void *pNewBlock = pSendBlock = CreateNewBlock( szLastToken, idBlock );
			if( pNewBlock == NULL )		// 새로 할당 못받은경우는 상위에서 넘겨준 포인터로 넘겨준다.
				pSendBlock = pBlockObj;
			if( LoadBlock( token, pSendBlock, szLastToken, idBlock ) == FALSE )
			{
				//XPROP_BREAKF( 1, "%s블럭 읽기실패", szLastToken );
				XBREAKF( 1, "%s->%s{ }:\n알수없는 기호 나옴", token.m_strFileName, (szBlockName)? szBlockName : _T("") );
				SAFE_DELETE( pNewBlock );
				return FALSE;
			} else {
				if( pNewBlock )		// 어쨋든 여기서 블럭 할당 못받은경우는 리스트에 추가할수 없다
					AddNewBlock( pNewBlock );
			}
		}
		else if( token == _T("}") ) 
				break;
		else {
			// 토큰 파싱은 버추얼 파서에게 맞긴다
			if( OnParse( token, pBlockObj, szBlockName ) == FALSE ) 
			{	 
				// 토큰해석을 못했다면 블럭 이름일 수 있다.
				// 바로 다음 토큰이 블럭{ 이라면 블럭이름으로 인식
				TCHAR szNextToken1[ MAX_TOKEN ];
				TCHAR szNextToken2[ MAX_TOKEN ];
				token.PushPos();		// 현재 위치 백업
				_tcscpy_s( szNextToken1, token.GetToken() );
				if( token.GetToken() == NULL )
					break;
				_tcscpy_s( szNextToken2, token.m_Token );		// 다음, 다다음 토큰까지 읽어본다
				token.PopPos();	// 토큰위치 복구
				if( szNextToken1[0] == '{' ) {
					if( token.IsNumeric( token.m_Token ) )				// { 앞에 있는 토큰이 숫자형태라면 
						idBlock = token.ToNumber( token.m_Token );	// 일단 idblock으로 가정
					else {
						_tcscpy_s( szLastToken, token.m_Token );
						idBlock = 0;
					}
				} else if( token.IsNumeric( szNextToken1 ) && szNextToken2[0] == '{' )	// 다음 토큰이 숫자+{ 면 블럭 아이디로 인식
				{
					_tcscpy_s( szLastToken, token.m_Token );	// 일단 블럭이름 받아놓고
					idBlock = token.GetNumber();		// block id
				} else {
					// 토큰 파싱은 버추얼 파서에게 맞긴다
//					if( OnParse( token, pBlockObj, szBlockName ) == FALSE ) {	 
	//					XERROR( "%s->%s{ }:\n알수없는 기호 나옴", token.m_strFileName, (szBlockName)? szBlockName : _T("") );
//					}
				}
			}
		}
	}
	return TRUE;
}

BOOL XBaseProp::Load( LPCTSTR szFilename, XE::xtENCODING encode )
{
	CToken token;
	if( token.LoadFile( szFilename, XE::TXT_UTF16 ) == xFAIL )
	{
		XERROR( "%s 읽기 실패", szFilename );
		return FALSE;
	}
	return LoadBlock( token, NULL, NULL );
}
