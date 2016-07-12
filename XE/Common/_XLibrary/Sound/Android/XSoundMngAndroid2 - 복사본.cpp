#include "stdafx.h"
#ifdef _VER_ANDROID
#ifdef _XSOUND2
// 신버전. sound.txt를 쓰는 버전
#include <stdio.h>
#include <assert.h>
#include <list>
#include "XSoundMngAndroid2.h"
#include "path.h"
#include "XResMng.h"
#include "android/CocosDenshion/include/SimpleAudioEngine.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;
using namespace CocosDenshion;
//------------------------------------------------------------------------------------------------------------
XSoundMngAndroid::XSoundMngAndroid()
{
	SimpleAudioEngine::sharedEngine();
	m_bBGMLoop = FALSE;
}
XSoundMngAndroid::~XSoundMngAndroid()
{
	RemoveAll();
}
BOOL XSoundMngAndroid::OpenBGMusic( const char* _filename, BOOL repeat)
{
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
	return 1;
}

void XSoundMngAndroid::SetBGMVolume(float volume)
{	
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(volume);
}

void XSoundMngAndroid::PlayBGMusic()
{
	XLOGXN("playbgm: %s", m_strBGM.c_str() );
	bool bRepeat = true;
	if( m_bBGMLoop == FALSE )
		bRepeat = false;
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic(m_strBGM.c_str(), bRepeat );
}

void XSoundMngAndroid::StopBGMusic()
{
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
	return;
}

void XSoundMngAndroid::FadeOutBGM( float fLevel)
{
	XSoundMng::FadeOutBGM( fLevel );
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume( m_BGMVolumeTemp );
}

BOOL XSoundMngAndroid::IsBGMusic( void )
{	
	if( m_strBGM.empty() == false )
		return TRUE;
	return FALSE;
}

BOOL XSoundMngAndroid::OpenSound( ID FileID )
{
	// 중복 사운드를 찾음.
	Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end()) {
	    if ( (*it).FileID == FileID) {
			(*it).Use++;
			return TRUE;
		}
		it++;
	}
	ST_SOUND sound;
	char cFullpath[1024];
	strcpy_s( cFullpath, GetFullPath( FileID) );
	if( XE::IsEmpty( cFullpath ) )
		return FALSE;
	SimpleAudioEngine::sharedEngine()->preloadEffect( cFullpath );
//	XLOGXN("open sound:%s", cFullpath);
	sound.strFile = cFullpath;
	sound.FileID = FileID;
	sound.Use  =  1;
	sound.idSound = 0;
	m_SoundList.push_back(sound);
	return TRUE;
}

void XSoundMngAndroid::CloseSound(ID FileID)
{
	ST_SOUND sound;
	Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end()) {
		if ((*it).FileID == FileID)	{
			sound = (*it);
			break;
		}
		it++;
	}
	//
	if (sound.strFile.empty() == false ) {
		sound.Use--;
		if(sound.Use == 0) {
			SimpleAudioEngine::sharedEngine()->stopEffect( sound.idSound );
			SimpleAudioEngine::sharedEngine()->unloadEffect( sound.strFile.c_str() );
			m_SoundList.erase(it);
		}

	}
}

void XSoundMngAndroid::PlaySound(ID FileID, BOOL bRepeat )
{
	if (m_bMuteSound) return;
	Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end()) {
		ST_SOUND sound = (*it);
		if (sound.FileID == FileID) {
			// 타이머가 꺼져있거나 플레이된지 0.1초가 지난것만 다시 플레이 할수 있다.
			if( sound.timerPlay.IsOff() || sound.timerPlay.IsOver(0.1f) ) {
				XTRACE("play sound:(%.1f) %s", m_SoundVolume, sound.strFile.c_str() );
				(*it).idSound = SimpleAudioEngine::sharedEngine()->playEffect( sound.strFile.c_str(), bRepeat);
				SimpleAudioEngine::sharedEngine()->setEffectsVolume( m_SoundVolume );
				(*it).timerPlay.Set(0);
			}
			return;
		}
		it++;
	}
	XTRACE("play sound failed: not found id");
}



void XSoundMngAndroid::StopSound( ID FileID )
{
	if (m_bMuteSound) return;
	Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end()) {
		if ((*it).FileID == FileID) {
			SimpleAudioEngine::sharedEngine()->stopEffect( (*it).idSound );
			break;
		}
		it++;
	}	
}

void XSoundMngAndroid::RemoveAll()
{
	SimpleAudioEngine::sharedEngine()->stopAllEffects();
	SimpleAudioEngine::sharedEngine()->end();
	for(Sound_Itor itor = m_SoundList.begin(); itor != m_SoundList.end(); ) {
		m_SoundList.erase(itor++);
	}
}

void XSoundMngAndroid::StopAllSound() 
{
	RemoveAll();
}

#endif // xsound2
#endif // _VER_ANDROID
