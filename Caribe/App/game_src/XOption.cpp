#include "stdafx.h"
#include "XOption.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


////////////////////////////////////////////////////////////////
XOption::XOption()
	: m_strcKeyCurrLang( "english" )
{
	Init();
}

bool XOption::Parse( const std::string& strcKey, CToken& token )
{
	if( strcKey == "lang" ) {
//		token.GetToken();		// =
		m_strcKeyCurrLang = SZ2C( token.GetToken2() );
	} else
	if( strcKey == "sound" ) {
//		token.GetToken();
		int flag = token.GetNumber();
		m_bSound = flag != 0;
	} else
	if( strcKey == "music" ) {
//		token.GetToken();
		int flag = token.GetNumber();
		m_bMusic = flag != 0;
	} else
	if( strcKey == "sound_vol" ) {
		m_volSound = token.GetNumberF();
	} else 
	if( strcKey == "music_vol" ) {
		m_volMusic = token.GetNumberF();
#ifndef WIN32
		if( m_volMusic == 1.f )
			m_volMusic = 0.5f;			// 
#endif // not WIN32
	} else
	if( strcKey == "low" ) {
		int flag = token.GetNumber();
		m_bLow = flag != 0;
	}	else {
		XBREAKF( 1, "option.ini: 알수없는 옵션: %s", C2SZ(strcKey) );
	}
	return true;
}
bool XOption::Write( std::string* pOut )
{
	*pOut += XE::Format( "lang = %s\r\n", m_strcKeyCurrLang.c_str() );
	*pOut += XE::Format( "sound = %d\r\n", xboolToByte( m_bSound ) );
	*pOut += XE::Format( "music = %d\r\n", xboolToByte( m_bMusic ) );
	*pOut += XE::Format( "low = %d\r\n", xboolToByte( m_bLow ) );
	*pOut += XE::Format( "sound_vol = %.1f\r\n", m_volSound );
	*pOut += XE::Format( "music_vol = %.1f\r\n", m_volMusic );
	return true;
}

// bool XOption::Load()
// {
// 	CToken token;
// 	if( token.LoadFromDoc( _T("option.ini"), XE::TXT_EUCKR) == xFAIL ) {
// 		return false;
// 	}
// 	while( token.GetToken() ) {
// 		const std::string strcKey = SZ2C( token.m_Token );
// 		if( strcKey == "lang" ) {
// 			token.GetToken();		// =
// 			m_strcKeyCurrLang = SZ2C( token.GetToken2() );
// 		} else
// 		if( strcKey == "sound" ) {
// 			token.GetToken();
// 			int flag = token.GetNumber();
// 			m_bSound = flag != 0;
// 		} else
// 		if( strcKey == "music" ) {
// 			token.GetToken();
// 			int flag = token.GetNumber();
// 			m_bMusic = flag != 0;
// 		}
// 	}
// 	return true;
// }

// bool XOption::Save()
// {
// 	FILE *fp = nullptr;
// 	LPCTSTR _szPath = XE::MakeDocFullPath( _T( "" ), _T( "option.ini" ) );
// 	std::string strPath = SZ2C( _szPath );
// 	fopen_s( &fp, strPath.c_str(), "wt" );
// 	if( fp ) {
// 		fprintf_s( fp, "lang = %s\r\n", m_strcKeyCurrLang.c_str() );
// 		fprintf_s( fp, "sound = %d\r\n", xboolToByte(m_bSound) );
// 		fprintf_s( fp, "music = %d\r\n", xboolToByte(m_bMusic) );
// // 		fprintf_s( fp, "%d\n%d", bSound, nLang );
// 		fclose( fp );
// 		return true;
// 	}
// 	return false;
// }

// void XOption::Load()
// {
// 	FILE *fp = nullptr;
// 	LPCTSTR _szPath = XE::MakeDocFullPath( _T( "" ), _T( "option.ini" ) );
// 	std::string strPath = SZ2C( _szPath );
// 	fopen_s( &fp, strPath.c_str(), "rt" );
// 	if( fp ) 
// // 		fscanf_s( fp, "%d %d", &bSound, &nLang );
// 		fclose( fp );
// 	}
// }
