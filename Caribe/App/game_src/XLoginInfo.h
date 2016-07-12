#pragma once

// 마지막 로그인했던 아이디와 비번정보
class XLoginInfo
{
	// 로그인한 타입
	enum xtLogin {
		xLT_NONE,
		xLT_UUID,			// UUID로 로그인
	};
	_tstring m_strLoginFile;
	BOOL m_bLoaded;
	std::string m_strUUID;
#ifdef _XLOGIN_FACEBOOK
	std::string m_strFbUserId;		// facebook user id(숫자로 된 스트링형)
#endif
private:
	void Init() {
		m_bLoaded = FALSE;
	}
	void Destroy() {}
public:
	XLoginInfo() { Init(); }
	virtual ~XLoginInfo() { Destroy(); }
	//
 	GET_ACCESSOR_CONST( const std::string&, strUUID );
	void SetUUIDLogin( const char *cUUID ) {
		m_strUUID = cUUID;
	}
	void ClearLoginInfo( void ) {
		m_strUUID.clear();
	}
#ifdef _XLOGIN_FACEBOOK
	GET_SET_ACCESSOR_CONST( const std::string&, strFbUserId );
	void SetFacebookUserid( const char *cUserId ) {
		m_strFbUserId = cUserId;
	}
#endif // login_facebook
	BOOL Load( LPCTSTR szFile );
	BOOL Save();
};

class XConnectINI
{
public:
	char m_cIP[ 64 ];
	char m_cID[ 64 ];
	char m_cIPPatch[ 64 ];
	WORD m_Port;

	XConnectINI() {
		m_cIP[0] = 0;
		m_cID[0] = 0;
		m_Port = 8000;
		m_cIPPatch[0] = 0;
	}
	~XConnectINI() {}
	std::string GetstrcIPLogin() const {
		return std::string(m_cIP);
	}
	std::string GetstrcIPPatch() const {
		return std::string( m_cIPPatch );
	}
	BOOL Load( LPCTSTR szFile );
  BOOL Save( LPCTSTR szFile );
};

namespace XPH {
extern DWORD GetDeviceID( void );
extern void GetUUID( char *cOut, int len );
};

extern XLoginInfo LOGIN_INFO;
extern XConnectINI CONNECT_INI;
