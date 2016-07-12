#include "stdafx.h"
#include <stdio.h>
//#include <assert.h>
#include "XSoundMng.h"
#include "etc/path.h"
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
	m_distListener = 0;
	memset(m_szBGMFilename, 0, sizeof(m_szBGMFilename) );
	m_szBGMBackup[0] = 0;

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
}

void XSoundMng::FadeOutBGM(float fLevel)
{	
	m_BGMVolumeTemp -= fLevel;
	if(m_BGMVolumeTemp < 0)m_BGMVolumeTemp = 0;
}
//------------------------------------------------------------------------------------------------------------

void XSoundMng::SetBGMMute(BOOL mute)
{
	m_bMuteBGM = mute;
	if (m_bMuteBGM == TRUE) StopBGMusic();
	else PlayBGMusic();
}

void XSoundMng::PushBGMusic( void )
{
	strcpy_s( m_szBGMBackup, m_szBGMFilename );
}
void XSoundMng::PopBGMusic( void )
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
//------------------------------------------------------------------------------------------------------------
BOOL XSoundMng::SaveConfig( void )
{
	FILE* fp = NULL;
	fp = fopen( MakeDocFullPath("","Sound.cfg"), "wt" );
	if(fp == NULL) return FALSE;
	// BGM OFF ?
	fprintf( fp, "BGM OFF = %d\n", m_bMuteBGM );
	// BGM VOL
	fprintf( fp, "BGM VOL = %f\n", m_BGMVolume );
	// SOUND OFF ?
	fprintf( fp, "SOUND OFF = %d\n", m_bMuteSound );
	// SOUND VOL
	fprintf( fp, "SOUND VOL = %f\n", m_SoundVolume );
    fclose(fp);
	return TRUE;
}

//------------------------------------------------------------------------------------------------------------

BOOL XSoundMng::LoadConfig( void )
{
	FILE* fp;
	
	fp = fopen( MakeDocFullPath("", "Sound.cfg"), "rt" );
	if(fp == NULL) return FALSE;
	// BGM OFF ?
	fscanf_s( fp, "BGM OFF = %d\n", &m_bMuteBGM );
	// BGM VOL
	fscanf_s( fp, "BGM VOL = %f\n", &m_BGMVolume );
	// SOUND OFF ?
	fscanf_s( fp, "SOUND OFF = %d\n", &m_bMuteSound );
	// SOUND VOL
	fscanf_s( fp, "SOUND VOL = %f\n", &m_SoundVolume );
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