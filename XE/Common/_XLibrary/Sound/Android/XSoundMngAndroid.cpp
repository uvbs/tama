#include "stdafx.h"
#ifdef _VER_ANDROID
#ifndef _XSOUND2
// 구버전(sound.txt를 안쓰는 버전)

#include <stdio.h>

#include <assert.h>

#include <list>

#include "XSoundMngAndroid.h"

#include "path.h"
#include "XResMng.h"
#include "SimpleAudioEngine.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif



using namespace XE;
using namespace CocosDenshion;
//#define _XSTOP_SOUND

//------------------------------------------------------------------------------------------------------------



XSoundMngAndroid::XSoundMngAndroid()

{
	SimpleAudioEngine::sharedEngine();
//	m_pOpenAL = new XOpenAL;
//	m_pBGMusic = NULL;
	m_bBGMLoop = FALSE;
}



//------------------------------------------------------------------------------------------------------------

//#include "AVAudio.h"



XSoundMngAndroid::~XSoundMngAndroid()

{

	RemoveAll();

//	SAFE_DELETE( m_pOpenAL );

//	SAFE_DELETE( m_pBGMusic );

//	AVDestroy();

}



//------------------------------------------------------------------------------------------------------------

BOOL XSoundMngAndroid::OpenBGMusic( const char* _filename, BOOL repeat)

{
	XLOAD_PROFILE1;
	// 이미 같은 파일명이 플레이 되고 있다면 그냥 리턴
	if( XE::IsSame( m_szBGMFilename, _filename ) )
		return FALSE;
  char filename[1024];
  sprintf( filename, "%s.mp3", _filename );

	strcpy_s(m_szBGMFilename, _filename);
  TCHAR szPath[1024]; // fullpath
  XE::SetReadyRes( szPath, XE::MakePath( DIR_BGM, filename ) );
//	XLOGXN("%s", szPath);
//	XLOGXN("preload");
	SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic( szPath );
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume( m_SoundVolume );
//	XLOGXN("set volume");
//    SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.5);
	m_strBGM = szPath;
	m_bBGMLoop = repeat;
//	AVLoad( szPath, repeat );
	XLOAD_PROFILE2;
	XLOGP( "%s, %llu",  XE::GetFileName( C2SZ(filename) ), __llPass );
	return 1;
}

//------------------------------------------------------------------------------------------------------------



void XSoundMngAndroid::SetBGMVolume(float volume)

{	
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(volume);
//	AVSetVolume( volume );
}



//------------------------------------------------------------------------------------------------------------



void XSoundMngAndroid::PlayBGMusic()

{
	XLOGXN("playbgm: %s", m_strBGM.c_str() );
	bool bRepeat = true;
	if( m_bBGMLoop == FALSE )
		bRepeat = false;
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic(m_strBGM.c_str(), bRepeat );

//	AVPlay();
}



//------------------------------------------------------------------------------------------------------------



void XSoundMngAndroid::StopBGMusic()

{
	m_szBGMFilename[0] = 0;
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();

//	AVStop();

	return;
}



//------------------------------------------------------------------------------------------------------------



void XSoundMngAndroid::FadeOutBGM( float fLevel)

{

//	if( m_pBGMusic == NULL )	return;

	XSoundMng::FadeOutBGM( fLevel );

	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume( m_BGMVolumeTemp );

}



//------------------------------------------------------------------------------------------------------------



BOOL XSoundMngAndroid::IsBGMusic( void )

{	
	if( m_strBGM.empty() == false )
		return TRUE;
//	if (m_pBGMusic != NULL ) return TRUE;

	return FALSE;

}



//------------------------------------------------------------------------------------------------------------



BOOL XSoundMngAndroid::OpenSound( int FileID )

{

//	if( m_pOpenAL == NULL )
//		return FALSE;

	// 중복 사운드를 찾음.

	Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end())
	{
	    if ( (*it).FileID == FileID)
		{
			(*it).Use++;
			return TRUE;
		}
		it++;
	}

    ST_SOUND sound;
//	pSound = new ST_SOUND;

	char filename[128];
	sprintf(filename, "%04d.wav", FileID);

    TCHAR szFullpath[1024];
    XE::SetReadyRes( szFullpath, MakePath( DIR_SND, filename) );
//	XLOGXN("open sound:%s", szFullpath);
    SimpleAudioEngine::sharedEngine()->preloadEffect( szFullpath );

	sound.strFile = szFullpath;
	sound.FileID = FileID;
	sound.Use  =  1;
	sound.idSound = 0;
	m_SoundList.push_back(sound);
	return TRUE;

}



//------------------------------------------------------------------------------------------------------------



void XSoundMngAndroid::CloseSound(int FileID)

{
//	if( m_pOpenAL == NULL )
//		return;

    ST_SOUND sound;
    Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end())
	{
	    if ((*it).FileID == FileID)
		{
		    sound = (*it);
		    break;
		}
		it++;
	}

    //
	if (sound.strFile.empty() == false )
	{
        sound.Use--;
		if(sound.Use == 0)
		{
			SimpleAudioEngine::sharedEngine()->stopEffect( sound.idSound );
			SimpleAudioEngine::sharedEngine()->unloadEffect( sound.strFile.c_str() );
			m_SoundList.erase(it);
		}

	}
}



//------------------------------------------------------------------------------------------------------------



void XSoundMngAndroid::PlaySound(int FileID, BOOL bRepeat )

{
//	if( m_pOpenAL == NULL ) return;
	if (m_bMuteSound) return;

	Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end())
	{
	    if ((*it).FileID == FileID)
		{
			ST_SOUND sound = (*it);
//			XLOGXN("play sound:(%.1f) %s", m_SoundVolume, (*it).strFile.c_str() );
			(*it).idSound = SimpleAudioEngine::sharedEngine()->playEffect( (*it).strFile.c_str(), bRepeat);
			SimpleAudioEngine::sharedEngine()->setEffectsVolume( m_SoundVolume );
			return;
		}
		it++;
	}
	XLOGXN("play sound failed: not found id");
}



void XSoundMngAndroid::StopSound( int FileID )

{
//	if( m_pOpenAL == NULL ) return;
	if (m_bMuteSound) return;
	
	Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end())
	{
	    if ((*it).FileID == FileID)
		{
			SimpleAudioEngine::sharedEngine()->stopEffect( (*it).idSound );
			break;
		}
		it++;
	}	
}



//------------------------------------------------------------------------------------------------------------



void XSoundMngAndroid::RemoveAll()
{
//	if( m_pOpenAL == NULL ) return;


	SimpleAudioEngine::sharedEngine()->stopAllEffects();
    SimpleAudioEngine::sharedEngine()->end();

   for(Sound_Itor itor = m_SoundList.begin(); itor != m_SoundList.end(); )
   {
	   m_SoundList.erase(itor++);
   }
}



#endif // not xsound2
#endif // _VER_ANDROID
