#include "stdafx.h"
#include <stdio.h>
//#include <assert.h>
#include "XSoundMng.h"
#include "XSoundTable.h"
using namespace XE;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSoundMng *XSoundMng::s_pSoundMng = NULL;

//------------------------------------------------------------------------------------------------------------

XSoundMng::XSoundMng()
{
//	XLOG("create sound manager");
	Init();

	if( LoadConfig() == FALSE)
	{
		m_bMuteBGM   = FALSE;
		m_bMuteSound = FALSE;
		m_BGMVolume = 1.0f;
		m_BGMVolumeTemp = 1.0f;
		m_SoundVolume = 1.0f;
	}

    m_BGMVolumeTemp = m_BGMVolume;

}

//------------------------------------------------------------------------------------------------------------

XSoundMng::~XSoundMng()
{
//	XLOG("destroy sound manager");
//	SetVolumeConfig(m_bMuteBGM, m_BGMVolume, m_bMuteSound, m_SoundVolume);
	SaveConfig();
}

void XSoundMng::FadeOutBGM(float fLevel)
{	
	m_BGMVolumeTemp -= fLevel;
	if(m_BGMVolumeTemp < 0)m_BGMVolumeTemp = 0;
}
//------------------------------------------------------------------------------------------------------------

void XSoundMng::SetBGMMute(bool mute)
{
	m_bMuteBGM = mute;
	if (m_bMuteBGM == TRUE) StopBGMusic();
	else PlayBGMusic();
}

void XSoundMng::PushBGMusic()
{
	strcpy_s( m_szBGMBackup, m_szBGMFilename );
}
void XSoundMng::PopBGMusic()
{
	SOUNDMNG->StopBGMusic();
	if( XE::IsHave( m_szBGMBackup ) )
	{
		SOUNDMNG->OpenBGMusic( m_szBGMBackup );
		SOUNDMNG->PlayBGMusic();
		m_szBGMBackup[0] = 0;
	} else
	{
		m_szBGMFilename[0] = 0;
	}
}

const char* XSoundMng::GetFullPath( ID idSound )
{
	static char s_cFullpath[ 1024 ];
	TCHAR szFullpath[1024];

	const char *cSnd = XSOUND_FILE(idSound);
	if( XE::IsEmpty(cSnd) )
		return "";
	const char *cRes = XE::MakePath( DIR_SND, cSnd );
	// szRes의 풀패스를 얻어낸다.
	XE::SetReadyRes( szFullpath, C2SZ( cRes ) );
	strcpy_s( s_cFullpath, SZ2C( szFullpath ) );
	return s_cFullpath;
}
//------------------------------------------------------------------------------------------------------------
BOOL XSoundMng::SaveConfig()
{
	FILE* fp = NULL;
	_tfopen_s( &fp, MakeDocFullPath(_T(""),_T("Sound.cfg")), _T("wt") );
	if(fp == NULL) return FALSE;
	// BGM OFF ?
	fprintf( fp, "BGM OFF = %d\n", m_bMuteBGM );
	// BGM VOL
	fprintf( fp, "BGM VOL = %f\n", m_BGMMasterVolume );
	// SOUND OFF ?
	fprintf( fp, "SOUND OFF = %d\n", m_bMuteSound );
	// SOUND VOL
	fprintf( fp, "SOUND VOL = %f\n", m_SoundMasterVolume );
    fclose(fp);
	return TRUE;
}

//------------------------------------------------------------------------------------------------------------

BOOL XSoundMng::LoadConfig()
{
	FILE* fp;
	
	_tfopen_s( &fp, MakeDocFullPath(_T(""), _T("Sound.cfg")), _T("rt") );
	if(fp == NULL) return FALSE;
	// BGM OFF ?
	int bMuteBGM;
	fscanf_s( fp, "BGM OFF = %d\n", &bMuteBGM );
	m_bMuteBGM = bMuteBGM != 0;
	// BGM VOL
	fscanf_s( fp, "BGM VOL = %f\n", &m_BGMMasterVolume );
	// SOUND OFF ?
	int bMuteSound;
	fscanf_s( fp, "SOUND OFF = %d\n", &bMuteSound );
	m_bMuteSound = bMuteSound != 0;
	// SOUND VOL
	fscanf_s( fp, "SOUND VOL = %f\n", &m_SoundMasterVolume );
    fclose(fp);
	return TRUE;

}

//------------------------------------------------------------------------------------------------------------
/*
//------------------------------------------------------------------------------------------------------------
BOOL SetVolumeConfig(BOOL bMuteBGM,  float BGMVolume,  BOOL bMuteSound  ,float SndVolume)
{
	FILE* fp;
#ifdef _VER_IOS
	fp = fopen( MakeDocumentPath("Sound.cfg"), "wt" );
#else
	char cBuff[ MAX_PATH ];
	strcpy_s( cBuff, XE::GetCwd( cBuff, MAX_PATH ) );
	strcat_s( cBuff, "Sound.cfg" );		// 실행파일 폴더명 + sound.cfg의 경로를 만듬
	fopen_s( &fp, cBuff, "wt" );
#endif
	if(fp == NULL) return FALSE; 
	// BGM OFF ?
	fprintf( fp, "BGM OFF = %d\n", bMuteBGM );
	// BGM VOL
	fprintf( fp, "BGM VOL = %f\n", BGMVolume );
	// SOUND OFF ?
	fprintf( fp, "SOUND OFF = %d\n", bMuteSound );
	// SOUND VOL
	fprintf( fp, "SOUND VOL = %f\n", SndVolume );
    fclose(fp);
	return TRUE;
}

//------------------------------------------------------------------------------------------------------------

BOOL GetVolumeConfig(BOOL &bMuteBGM, float &BGMVolume, BOOL &bMuteSound ,float &SndVolume)
{
	FILE* fp;
	
#ifdef _VER_IOS
	fp = fopen( MakeDocumentPath("Sound.cfg"), "rt" );
#else
	char cBuff[ MAX_PATH ];
	strcpy_s( cBuff, XE::GetCwd( cBuff, MAX_PATH ) );
	strcat_s( cBuff, "Sound.cfg" );		// 실행파일 폴더명 + sound.cfg의 경로를 만듬
	fopen_s( &fp, cBuff, "rt" );
#endif
	if(fp == NULL) return FALSE; 
	// BGM OFF ?
	fscanf_s( fp, "BGM OFF = %d\n", &bMuteBGM );
	// BGM VOL
	fscanf_s( fp, "BGM VOL = %f\n", &BGMVolume );
	// SOUND OFF ?
	fscanf_s( fp, "SOUND OFF = %d\n", &bMuteSound );
	// SOUND VOL
	fscanf_s( fp, "SOUND VOL = %f\n", &SndVolume );
    fclose(fp);
	return TRUE;

}
*/
//------------------------------------------------------------------------------------------------------------

void XSoundMng::xSoundTimer::AddSound(ID idSound)
{
	xInfo info = xInfo();
	info.idSound = idSound;
	info.timer.On();
	aryTime.push_back(info);
}

bool XSoundMng::xSoundTimer::IsOver(ID idSound, float sec )
{
	bool bFirst = true;	//첫 재생인가
	for (auto itor = aryTime.begin(); itor != aryTime.end(); itor++) {
		if (itor->idSound == idSound) {
			bFirst = false;
			if (itor->timer.GetPassSec() >= sec) {
				itor->timer.On();
				return true;
			}
		}
	}
	if (bFirst) {
		AddSound(idSound);
		return true;
	}
	return false;
}

//bool XSoundMng::xSoundTimer::OpenPlaySound(ID idSound)
//{
//	bool bExist = false;
//	for (auto itor = aryTime.begin(); itor != aryTime.end(); ++itor)
//	{
//		if (itor->idSound == idSound)
//		{
//			bExist = true;
//			if (timeGetTime() - itor->dwTime >= 1000)
//			{
//				this->OpenPlaySound(idSound);
//				itor->dwTime = timeGetTime();
//				return true;
//			}
//		}
//	}
//
//	if (bExist == false)
//		AddSound(idSound);
//
//	return false;
//}
//