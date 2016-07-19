#pragma once
#include <stdlib.h>
#include "XDBAccount.h"
#include "XArchive.h"
//#include "game/XFLevel.h"

class XGeneral;
class XAccount : public XDBAccount
{
public:
private:
	DWORD m_Ver;				// 계정 시리얼라이즈 버전
	_tstring m_strID;				// 영문아이디
	_tstring m_strPassword;	// 비밀번호
	_tstring m_strUUID;
	_tstring m_StrSessionKey;
protected:
	_tstring m_strName;			// 닉네임
//	_tstring m_FBUserID;		// FBUserID
//	_tstring m_FBUserName;	// FBUserName

	ID	m_CurrSN;
//	XFLevel m_Level;		// 레벨과 경험치 객체. DB저장시 시리얼라이즈를 참조할것. 읽을때 SetLevelAndExpFromDB를 사용
	void Init() {
		m_CurrSN = 0;
		m_Ver = 0;
	}
	void Destroy();
public:
	XAccount();
	XAccount( ID idAccont );
	XAccount( ID idAccount, LPCTSTR szID );
	virtual ~XAccount() { Destroy(); }
	//
	GET_TSTRING_ACCESSOR( strName );	// LPCTSTR GetstrName(),	SetstrName( LPCTSTR )
//	GET_TSTRING_ACCESSOR( FBUserName );
//	SET_TSTRING_ACCESSOR( FBUserName );
//	GET_SET_TSTRING_ACCESSOR( FBUserID );
	GET_TSTRING_ACCESSOR( strID );
	GET_TSTRING_ACCESSOR( strPassword );
	BOOL IsHavePassword() {
		return ( m_strPassword.empty() == FALSE )? TRUE : FALSE;
	}
	BOOL  IsSamePassword( LPCTSTR szPassword ) {
		return ( m_strPassword == szPassword ) ? TRUE : FALSE;
	}
	void SetPassword( LPCTSTR szPassword ) {
		m_strPassword = szPassword;
	}
	BOOL IsSameID( LPCTSTR szID ) {
		return m_strID == szID;
	}
	GET_ACCESSOR( XFLevel&, Level );
	DWORD GetExp( void ) {
		return m_Level.GetExp();
	}
	void AddExp( int add );

	void SetSessionkey ( LPCTSTR Sessionkey ){ m_StrSessionKey = Sessionkey ; }		
	LPCTSTR  GetSessionKey( ){ return m_StrSessionKey.c_str() ;}

	void SetCurrSN(ID snnum){ m_CurrSN = snnum;}
	ID	  GetCurrSN (){ return m_CurrSN;}
	ID	  GenerateSN ()
	{
		m_CurrSN ++; 
		return m_CurrSN;
	}

	virtual int Serialize( XArchive& ar );
	virtual int DeSerialize( XArchive& ar );

	virtual void Create( void );
};
