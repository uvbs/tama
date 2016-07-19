#pragma once

using namespace std;

#define XPATCH_URL		"update/"
#define XFULL_DIR			"full/"
#define XCORE_DIR			"core/"
#define FILE_LAST_UPDATE		"LastUpdate.txt"
#define XUPDATE_DIR		"update/"
#define XUPDATE_CORE_DIR		"update/core/"
#define FILE_FULL_LIST		"full_list.txt"

// 마지막 로그인했던 아이디와 비번정보
class XLoginInfo
{
	// 로그인한 타입
	enum xtLogin {
		xLT_NONE,
		xLT_UUID,	// UUID로 로그인
		xLT_IDPW,	// ID/비번으로 로그인
		xLT_FACEBOOK,	// 페이스북으로 로그인
	};
	_tstring m_strLoginFile;
	BOOL m_bLoaded;
	xtLogin m_typeLogin;	// 로그인한 타입
	std::string m_strUUID;
	std::string m_strID;
	std::string m_strPassword;
#ifdef _XLOGIN_FACEBOOK
	std::string m_strFbUserId;	// facebook user id(숫자로 된 스트링형)
	_tstring m_strFbUsername;	// facebook username(영문스트링형)
	_tstring m_strFbName;		// facebook 의 자기 이름
	BOOL m_bLoggedInFacebook;	// 현재 페이스북에 로그인된 상태인가
#endif
private:
	void Init() {
		m_bLoaded = FALSE;
		m_typeLogin = xLT_NONE;
#ifdef _XLOGIN_FACEBOOK
		m_bLoggedInFacebook = FALSE;
#endif
	}
	void Destroy() {}
public:
	XLoginInfo() { Init(); }
	virtual ~XLoginInfo() { Destroy(); }
	//
	GET_ACCESSOR( std::string&, strID );
	GET_ACCESSOR( std::string&, strPassword );
	GET_ACCESSOR( std::string&, strUUID );
	void SetUUIDLogin( const char *cUUID ) {
		m_strUUID = cUUID;
		m_typeLogin = xLT_UUID; 
	}
	// 마지막 로그인한방식이 아이디/비번 방식이었는가?
	BOOL IsLastLoginIDPW( void ) {
		return m_typeLogin == xLT_IDPW;
	}
	BOOL IsLastLoginUUID( void ) {
		return m_typeLogin == xLT_UUID;
	}
	void ClearLoginInfo( void ) {
		m_typeLogin = xLT_NONE;
#ifdef _XLOGIN_FACEBOOK
		m_bLoggedInFacebook = FALSE;
#endif
	}
#ifdef _XLOGIN_FACEBOOK
	GET_SET_STRING_ACCESSOR( std::string&, strFbUserId );
	GET_SET_TSTRING_ACCESSOR( _tstring&, strFbUsername );
	GET_SET_TSTRING_ACCESSOR( _tstring&, strFbName );
	BOOL IsLoggedInFacebook( void ) {
		return m_bLoggedInFacebook;
	}
	void SetFacebookLogin( const char *cUserId, LPCTSTR szUsername, LPCTSTR szName ) {
		m_strFbUserId = cUserId;
		m_strFbUsername = szUsername;
		m_strFbName = szName;
		m_typeLogin = xLT_FACEBOOK;
		m_bLoggedInFacebook = TRUE;
	}
#ifdef WIN32
	void SetFacebookLogin( const char *cUserId, const char *cUsername, LPCTSTR szName ) {
		m_strFbUserId = cUserId;
		m_strFbUsername = C2SZ(cUsername);
		m_strFbName = szName;
		m_typeLogin = xLT_FACEBOOK;
		m_bLoggedInFacebook = TRUE;
	}
#endif
	// 마지막으로 로그인한 방식이 페이스북로그인 방식이었는가?
	BOOL IsLastLoginFacebook( void ) {
		return m_typeLogin == xLT_FACEBOOK;
		// strFbName은 검사하지 말것. Ini3에선 두개만 사용함.
	}
#endif // login_facebook
	void SetIDPwLogin( LPCTSTR szID, LPCTSTR szPassword ) {
		m_strID = SZ2C(szID);
		m_strPassword = SZ2C( szPassword );
		m_typeLogin = xLT_IDPW;
	}
	void MakeUUIDLogin( void );
	BOOL Load( LPCTSTR szFile );
	BOOL Save( void );
};

class XConnectINI
{
public:
	char m_cIP[ 64 ];
	char m_cID[ 64 ];
	char m_cIPPatch[ 64 ];
	WORD m_Port;
#ifdef _XBOT2
	int m_numBot;
#endif

	XConnectINI() {
		m_cIP[0] = 0;
		m_cID[0] = 0;
		m_Port = 8000;
		m_cIPPatch[0] = 0;
#ifdef _XBOT2
		m_numBot = 10;
#endif
	}
	virtual ~XConnectINI() {}
	BOOL Load( LPCTSTR szFile );
    BOOL Save( LPCTSTR szFile );
};

namespace XPH {
extern DWORD GetDeviceID( void );
extern void GetUUID( char *cOut, int len );
};

extern XLoginInfo LOGIN_INFO;
extern XConnectINI CONNECT_INI;