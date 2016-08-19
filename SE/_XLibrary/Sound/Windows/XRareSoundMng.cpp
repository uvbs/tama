#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <assert.h>
#include <list>
#include "XRareSoundMng.h"
#include "XSoundMng.h"
#include "path.h"
#include "Property.h"
#include "ConvertString.h"
//------------------------------------------------------------------------------------------------------------
//BOOL SetVolumeConfig(BOOL bMuteBGM,  int BGMVolume,  BOOL bMuteSound  ,int SndVolume);
//BOOL GetVolumeConfig(BOOL &bMuteBGM, int &BGMVolume, BOOL &bMuteSound ,int &SndVolume);

//------------------------------------------------------------------------------------------------------------

XRareSoundMng::XRareSoundMng()
{
    m_Context = NULL;	
	m_Context = Rare_CreateContext();	
	XBREAKF(m_Context == NULL, "사운드 모듈 생성 실패" );

	m_BGMusic = NULL;
}

//------------------------------------------------------------------------------------------------------------

XRareSoundMng::~XRareSoundMng()
{
	SetVolumeConfig(m_bMuteBGM, m_BGMVolume, m_bMuteSound, m_SoundVolume);

	RemoveAll();
    Rare_DestroyContext(m_Context);
	m_Context = NULL;
}

//------------------------------------------------------------------------------------------------------------
void XRareSoundMng::RemoveAll()
{	
   if (m_Context == NULL) return;
   
   ST_SOUND *pSound;
   pSound = NULL;
   for(XSound_Itor itor = m_listSound.begin(); itor != m_listSound.end();  )
   {
	   pSound = (*itor);
	   if (pSound != NULL) 
	   {
		   Rare_CloseSound(pSound->Sound);
		   SAFE_DELETE( pSound );
	   }
	   m_listSound.erase(itor++);
   }   
}

//------------------------------------------------------------------------------------------------------------
BOOL XRareSoundMng::OpenBGMusic( const char* szFilename, BOOL repeat )
{	
	if(!m_Context)	return FALSE;
#if defined(_DEBUG) && defined(WIN32)
//	return TRUE;
#endif
	XSoundMng::OpenBGMusic( szFilename, repeat );

	if (m_BGMusic != NULL)
		Rare_CloseSound(m_BGMusic);
//	if( PROP->GetbSound() == 0 )		// 사운드 오프
//		return TRUE;
	TCHAR szwBuff[1024];
	ConvertEUCKRToUTF16( szwBuff, 1024, szFilename );
	XLOG( "BGM:%s .......", szwBuff );
    m_BGMusic = Rare_OpenSound( m_Context, (char *)szFilename );	
	XLOG( "%s loaded", szwBuff );

	if(m_BGMusic == NULL)
		return FALSE;

    Rare_SetRepeat( m_BGMusic, repeat );
    Rare_SetVolume( m_BGMusic, (LONG)(m_BGMVolume * 255) );
    m_BGMVolumeTemp = m_BGMVolume;
       
	return TRUE;

}
//------------------------------------------------------------------------------------------------------------

void XRareSoundMng::SetBGMVolume( float volume )
{	
	if(!m_Context)	return;

	XSoundMng::SetBGMVolume( volume );
    Rare_SetVolume( m_BGMusic, (LONG)m_BGMVolume );
}

//------------------------------------------------------------------------------------------------------------

void XRareSoundMng::PlayBGMusic()
{
	if (!m_Context)
		return;

	if (m_bMuteBGM == FALSE && m_BGMusic != NULL)
		Rare_PlaySound( m_BGMusic );
}

//------------------------------------------------------------------------------------------------------------

void XRareSoundMng::StopBGMusic()
{
	if(!m_Context)	return;
    Rare_StopSound( m_BGMusic );

}

//------------------------------------------------------------------------------------------------------------

void XRareSoundMng::FadeOutBGM( float fLevel )
{
	if(!m_Context)	return;

	XSoundMng::FadeOutBGM( fLevel );
//	m_BGMVolumeTemp -= Level;
//	if(m_BGMVolumeTemp < 0)m_BGMVolumeTemp = 0;
    Rare_SetVolume( m_BGMusic, (LONG)m_BGMVolumeTemp );
}

//------------------------------------------------------------------------------------------------------------

//BOOL XRareSoundMng::IsBGMusic(RARESOUND sound)
BOOL XRareSoundMng::IsBGMusic( void )
{	
//	if (m_BGMusic != NULL && sound == m_BGMusic) return TRUE;
	if (m_BGMusic != NULL ) return TRUE;
	return FALSE;
}

//------------------------------------------------------------------------------------------------------------
BOOL XRareSoundMng::OpenSound( int FileID )
{	
#if defined(_DEBUG) && defined(WIN32)
//	return TRUE;		// 사운드로딩이 넘 느려서 디버그모드에선 안나게 바꿈
#endif
//	if( PROP->GetbSound() == 0 )		// 사운드 오프
//		return TRUE;
	if (m_Context == NULL)
		return FALSE;

	//중복 로드인지를 검사
	XSound_Itor it = m_listSound.begin();
	while(it != m_listSound.end())
	{
	    if ( (*it)->FileID == FileID)
		{
			(*it)->Use++;
			return TRUE;
		}
		it++;
	}

	//로드해서 리스트에 추가 
    ST_SOUND *pSound;
	pSound = new ST_SOUND;

	pSound->Sound = NULL;
	char filename[128];
//	sprintf_s(filename, "Snd\\%04d.snd", FileID);
	sprintf_s(filename, "%04d.snd", FileID);

	XLOG( "SOUND ID:%d .......", FileID );
	pSound->Sound = Rare_OpenSound(m_Context, (char *)XE::MakePath( DIR_SND, filename ), RARE_OPENEFFECT);
	XLOG( "%d loaded", FileID );
	TCHAR szwLog[1024];
	ConvertEUCKRToUTF16( szwLog, 1024, XE::MakePath( DIR_SND, filename ) );
	XLOG( "%s", szwLog );
	if (pSound->Sound != NULL)
	{
		pSound->FileID = FileID;
	    pSound->Use  =  1;
		m_listSound.push_back(pSound);
		return TRUE;
	}
	if (pSound != NULL)
		delete pSound;

	return FALSE;
}

//------------------------------------------------------------------------------------------------------------

void XRareSoundMng::CloseSound(int FileID)
{
	if (m_Context == NULL)
		return;

    ST_SOUND *pSound;
	pSound = NULL;
    XSound_Itor it = m_listSound.begin();
	while(it != m_listSound.end())
	{
	    if ((*it)->FileID == FileID) 
		{
		    pSound = (*it);
		    break;
		}
		it++;
	}	

    //
	if (pSound != NULL)
	{
        pSound->Use--;
		if(pSound->Use == 0)
		{
			m_listSound.erase(it);
			Rare_CloseSound(pSound->Sound);
			SAFE_DELETE( pSound );
//			delete pSound;
//			pSound = NULL;
		}
	}	
}

//------------------------------------------------------------------------------------------------------------

void XRareSoundMng::PlaySound(int FileID)
{	
	if (m_Context == NULL) return;
	if (m_bMuteSound) return;

	XSound_Itor it = m_listSound.begin();
	while(it != m_listSound.end()) {
	    if ((*it)->FileID == FileID)
		{	
#pragma message( "XRareSound 이거 하드코딩임." )
			if( GetdistListener() < 480.0f )		// 480픽셀이상 떨어진곳에서 나는 소리는 무시함
			{
				if( GetdistListener() > 240.0f )		// 거리가 240이상떨어져있어야 소리를 조정한다.
					m_SoundVolume = 255 - (int)(((GetdistListener() - 240.0f) / 255.0f) * 255.0f);
				else
					m_SoundVolume = 255;
				Rare_SetVolume((*it)->Sound, m_SoundVolume );
				Rare_PlaySound((*it)->Sound);			
			}
			break;
		}
		it++;
	}

}


//------------------------------------------------------------------------------------------------------------
/*
BOOL SetVolumeConfig(BOOL bMuteBGM,  int BGMVolume,  BOOL bMuteSound  ,int SndVolume)
{
	FILE* fp;
	
	fp = fopen( "Sound.cfg", "wt" );
	if(fp == NULL) return FALSE; 

	// BGM OFF ?
	fprintf( fp, "BGM OFF = %d\n", bMuteBGM );

	// BGM VOL
	fprintf( fp, "BGM VOL = %d\n", BGMVolume );

	// SOUND OFF ?
	fprintf( fp, "SOUND OFF = %d\n", bMuteSound );

	// SOUND VOL
	fprintf( fp, "SOUND VOL = %d\n", SndVolume );

    fclose(fp);

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------

BOOL GetVolumeConfig(BOOL &bMuteBGM, int &BGMVolume, BOOL &bMuteSound ,int &SndVolume)
{

	FILE* fp;
	
	fp = fopen( "Sound.cfg", "rt" );
	if(fp == NULL) return FALSE; 

	// BGM OFF ?
	fscanf( fp, "BGM OFF = %d\n", &bMuteBGM );

	// BGM VOL
	fscanf( fp, "BGM VOL = %d\n", &BGMVolume );

	// SOUND OFF ?
	fscanf( fp, "SOUND OFF = %d\n", &bMuteSound );

	// SOUND VOL
	fscanf( fp, "SOUND VOL = %d\n", &SndVolume );

    fclose(fp);

	return TRUE;

}
*/
//------------------------------------------------------------------------------------------------------------