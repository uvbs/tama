#include "stdafx.h"
#include "XLoginInfo.h"
#include "etc/InputMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
#define XPORT_LOGIN		8300
#ifdef _XUNIVERSAL
//#define XIP_LOGIN		"211.52.84.20"
  #ifdef _XPATCH
//	#define XIP_LOGIN		"211.52.84.50"
	#define XIP_LOGIN		"1.209.171.76"
  #else
// 	#define XIP_LOGIN		"1.209.171.76"
#pragma message("============================= IP변경 ===========================================")
	#define XIP_LOGIN		"192.168.0.191"
  #endif
//  #define XIP_LOGIN	"211.52.84.50"		// 한국
  #ifdef _XDEV_PATCH
    #define XIP_PATCH	"1.209.171.76"
  #else
    #define XIP_PATCH	"1.209.171.76"
  #endif
#else
  #define XIP_LOGIN	"192.168.100.44"		
  #define XIP_PATCH	"192.168.100.44"
#endif

XLoginInfo LOGIN_INFO;
XConnectINI CONNECT_INI;

void XLoginInfo::MakeUUIDLogin( void )
{
	ClearLoginInfo();
	m_typeLogin = xLT_UUID;
	if( m_strUUID.empty() == true )
	{
		char cUUID[ 48 ];
		XE::GetUUID( cUUID );
		m_strUUID = cUUID;
		XLOGXN("UUID:%s", C2SZ( cUUID ) );
		Save();
	}
}

BOOL XLoginInfo::Load( LPCTSTR szFile )
{
	CToken token;
	m_strLoginFile = szFile;
    // 플랫폼에 관계없이 이 함수로 대동단결
	if( token.LoadFromDoc( szFile, XE::TXT_EUCKR ) == xFAIL )
	{
		XLOGXN("login.txt not found");
// #ifdef _XLOGIN_UUID
// 		MakeUUIDLogin();
// #else
// 		Save();
// #endif
		return FALSE;
	}
	XLOGXN("load login.txt success!");
	if( token.GetToken() == nullptr )
		return FALSE;
#ifdef _XLOGIN_UUID
	if( token == _T("__none") )
	{
		return FALSE;
	} else
	if( token == _T("__uuid") )
	{
		if( token.GetToken() )
			m_strUUID = SZ2C(token.m_Token);
		else
			return FALSE;	// 파일이 깨진것 앱을 다시 설치해야함.
		m_typeLogin = xLT_UUID;
	} else
#endif
#ifdef _XLOGIN_FACEBOOK
// 	if( token == _T("__facebook") )
// 	{
// 		if( token.GetToken() )
// 			m_strFbUserId = SZ2C(token.m_Token);
// 		else
// 			return FALSE;
// 		//
// 		if( token.GetToken() )
// 			m_strFbUsername = token.m_Token;
// 		else
// 		{
// 			// 페북에 유저네임이 없을수도 있게 되어서 에러로 처리하지 않음.
// //			m_strFbUserId.clear();
// //			return FALSE;
// 		}
// 		m_typeLogin = xLT_FACEBOOK;
// 	} else
#endif
#ifdef _XUNIVERSAL
	if( token == _T("__idpw") )
	{
		//
		if( token.GetToken() )
			m_strID = SZ2C( token.m_Token );
		XLOGXN("login id:%s", token.m_Token);
		//
		if( token.GetToken() )
			m_strPassword = SZ2C( token.m_Token );
		else
		{
			m_strID.clear();
			return FALSE;
		}
		m_typeLogin = xLT_IDPW;
  #ifdef _DEV
		XLOGXN("login pw:%s", token.m_Token);
  #endif
	} else
#endif 
	{
		m_typeLogin = xLT_NONE;
		XBREAKF( 1, "unknown login identifier:%s", token.m_Token );
		return FALSE;
	}
// add facebook login mode
//////////////////////////////////////////////////////////////////////////
	m_bLoaded = TRUE;
	return TRUE;
}
BOOL XLoginInfo::Save( void )
{
	FILE *fp = nullptr;
	XBREAK( m_strLoginFile.empty() );
	// fprintf_s때문에 euckr형으로 만들어야 함
	LPCTSTR _szPath = XE::MakeDocFullPath( _T(""), m_strLoginFile.c_str() );
	std::string strPath = SZ2C( _szPath );
	fopen_s( &fp, strPath.c_str(), "wt" );

	if( fp == nullptr )
	{
		XLOGXN("save failed %s", _szPath );
		return FALSE;
	}
#if defined(_XUNIVERSAL) || defined(_XINDONESIA)
	if( m_typeLogin == xLT_UUID )
	{
		XBREAK( m_strUUID.empty() == true );
		fprintf_s( fp, "__uuid\r\n" );
		fprintf_s( fp, "\"%s\"\r\n", m_strUUID.c_str() );
		XLOGXN("save uuid=%s", C2SZ(m_strUUID.c_str()) );
	} else
#endif
#ifdef _XUNIVERSAL
	if( m_typeLogin == xLT_IDPW )
	{
		// 아니면 아이디/비번으로 로그인한것.
		fprintf_s( fp, "__idpw\r\n" );
		fprintf_s( fp, "\"%s\"\r\n", m_strID.c_str() );
		XLOGXN("id=%s", C2SZ(m_strID.c_str()) );
		fprintf_s( fp, "\"%s\"\r\n", m_strPassword.c_str() );
		XLOGXN("pw=%s", C2SZ(m_strPassword.c_str()) );
	} else
#endif
#ifdef _XLOGIN_FACEBOOK
// 	if( m_typeLogin == xLT_FACEBOOK )
// 	{
// 		// 페북으로 로그인한것
// 		fprintf_s( fp, "__facebook\r\n" );
// 		fprintf_s( fp, "\"%s\"\r\n", m_strFbUserId.c_str() );
// 		XLOGXN("save fb id=%s", C2SZ(m_strFbUserId.c_str()) );
// 		fprintf_s( fp, "\"%s\"\r\n", SZ2C(m_strFbUsername.c_str()) );
// 		XLOGXN("save fb username=%s", m_strFbUsername.c_str() );
// 		fprintf_s( fp, "\"%s\"\r\n", SZ2C(m_strFbName.c_str()) );
// 		XLOGXN("save fb name=%s", m_strFbName.c_str() );
// 	} else
#endif
	{
		fprintf_s( fp, "__none\r\n" );
//		XBREAKF(1, "unknown m_typeLogin save" );
	}
	fclose( fp );
	XLOGXN("loginInfo save success %s", _szPath );
	return TRUE;
}



//====================================================
//#include "etc/InputMng.h"
BOOL XConnectINI::Load( LPCTSTR szFile )
{
	CToken token;
	if( token.LoadFromDoc( szFile, XE::TXT_EUCKR ) == xFAIL )
	{
#ifdef WIN32
  #ifdef _DEBUG
		strcpy_s( m_cIP, XIP_LOGIN );
  #else
		_tstring strDocPath = XE::MakeDocFullPath( _T(""), szFile );
		XALERT( "%s file not found", strDocPath.c_str() );
		exit(1);
//		strcpy_s( m_cIP, "49.239.180.50" );
  #endif 
#else
        if( XInputMng::s_Device == XE::DEVICE_IPAD ||
           XInputMng::s_Device == XE::DEVICE_IPOD )
            strcpy_s( m_cIP, XIP_LOGIN );
        else
        if( XInputMng::s_Device == XE::DEVICE_ANDROID )
            strcpy_s( m_cIP, XIP_LOGIN );
        else
        {
            strcpy_s( m_cIP, XIP_LOGIN );
    //		strcpy_s( m_cIP, "59.5.5.245" );
        }
#endif
        m_Port = XPORT_LOGIN;
		strcpy_s( m_cIPPatch, XIP_PATCH );
		strcpy_s( m_cID, "test3" );
		return FALSE;
	}
	AXLOGXN("try read ip");
	strcpy_s( m_cIP, Convert_TCHAR_To_char( token.GetToken() ) );		// ip
	AXLOGXN("login ip:%s", token.m_Token);
	if( XE::IsEmpty(m_cIP) )
		strcpy_s( m_cIP, "211.52.84.50" );
		
	AXLOGXN("try read port");
	m_Port = token.GetNumber();
	AXLOGXN("login port:%d", m_Port);
#ifdef _XBOT2
  #ifdef _XPATCH
  #error "봇 클라이언트에서는 _XPATCH를 빼시오"
  #endif
	token.GetNumber();		// dummy
	m_numBot = token.GetNumber();
#endif
#ifdef _XPATCH
	AXLOGXN("try read patch ip");
	strcpy_s( m_cIPPatch, SZ2C( token.GetToken() ) );	// patchsvr ip
	AXLOGXN("patch ip:%s", token.m_Token);
	if( XE::IsEmpty(m_cIPPatch) )
		strcpy_s( m_cIPPatch, "49.239.180.50" );
#endif

	return TRUE;
}
BOOL XConnectINI::Save( LPCTSTR szFile )
{
	XLOGXN("save %s", szFile );
    FILE *fp = nullptr;
    _tfopen_s( &fp, XE::MakeDocFullPath( _T(""), szFile ), _T("wt") );
    if( fp == nullptr )
	{
		XLOGXN("open failed %s", szFile );
        return FALSE;
	}
    fprintf(fp, "\"%s\"\r\n", m_cIP );
	XLOGXN("ip: %s", m_cIP );
    fprintf(fp, "%d\r\n", (int)m_Port );
	XLOGXN("port: %d", m_Port );
//    fprintf(fp, "\"%s\"\r\n", m_cID );
	fprintf(fp, "\"%s\"\r\n", m_cIPPatch );
	XLOGXN("patch ip: %s", m_cIPPatch );
    fclose(fp);
    
    return TRUE;
}

DWORD XPH::GetDeviceID( void )
{
#ifdef WIN32
	return XE::GetHDDSerial();
#else
    return 0;
#endif
}

// void XPH::GetUUID( char *cOut, int len )
// {
// 	char cBuff[48];
// 	char cBuff2[ 48 ];
// 	UINT rnd;
// 	rnd = xRand();
// 	sprintf_s( cBuff2, "%08x", rnd );
// 	strcpy_s( cBuff, cBuff2 );
// 	rnd = xRand();
// 	sprintf_s( cBuff2, "%08x", rnd );
// 	strcat_s( cBuff, cBuff2 );
// 	rnd = xRand();
// 	sprintf_s( cBuff2, "%08x", rnd );
// 	strcat_s( cBuff, cBuff2 );
// 	rnd = xRand();
// 	sprintf_s( cBuff2, "%08x", rnd );
// 	strcat_s( cBuff, cBuff2 );
// 	rnd = xRand();
// 	sprintf_s( cBuff2, "%08x", rnd );
// 	strcat_s( cBuff, cBuff2 );
// 	strcpy_s( cOut, len, cBuff );
// }
// 
// 
// 
