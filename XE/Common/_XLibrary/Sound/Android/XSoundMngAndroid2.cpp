#include "stdafx.h"
#ifdef _VER_ANDROID
#ifdef _XSOUND2
// 신버전. sound.txt를 쓰는 버전
#include <stdio.h>
#include <assert.h>
#include <list>
#include "XSoundMngAndroid2.h"
#include "etc/path.h"
#include "XResMng.h"
#include "XFramework/android/CocosDenshion/include/SimpleAudioEngine.h"

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
	XLOGXN("%s", __FUNCTION__ );
	SimpleAudioEngine::sharedEngine();
//	m_bBGMLoop = false;
}
XSoundMngAndroid::~XSoundMngAndroid()
{
	XLOGXN( "%s", __FUNCTION__ );
	RemoveAll();
	SimpleAudioEngine::sharedEngine()->end();
}
bool XSoundMngAndroid::OpenBGMusic( const char* cFileTitle, bool bRepeat)
{
	XLOGXN( "%s, %s", __FUNCTION__, cFileTitle );
	const std::string strFile = XE::Format( "%s.mp3", cFileTitle );
	XSoundMng::OpenBGMusic( cFileTitle, bRepeat );
//	strcpy_s(m_szBGMFilename, _filename);
	TCHAR szFullpath[1024]; // fullpath
	const std::string strRes = XE::MakePath( DIR_BGM, strFile );
	XE::SetReadyRes( szFullpath, strRes.c_str() );
	m_strBGMFullpath = szFullpath;
	//	XLOGXN("%s", szPath);
	XLOGXN("%s:%s", __FUNCTION__, szFullpath );
	SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic( szFullpath );
// 	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume( GetBGMMasterVolume() );
	//	XLOGXN("set volume");
	//    SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.5);
// 	m_strBGM = szFullpath;
// 	m_bBGMLoop = repeat;
	SetbBGMLoop( bRepeat );
	//	AVLoad( szPath, repeat );
	return true;
}

/**
 @brief 동적으로 볼륨을 변화시킬때(ex:fade in/out)
*/
void XSoundMngAndroid::SetBGMVolumeLocal(float volLocal )
{	
	XSoundMng::SetBGMVolumeLocal( volLocal );
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(volLocal * GetBGMMasterVolume());
}
void XSoundMngAndroid::SetBGMVolumeFinal( float volFinal )
{
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume( volFinal );
}

void XSoundMngAndroid::PlayBGMusic()
{
	if( GetstrBGM().empty() )
		return;
	XSoundMng::PlayBGMusic();
// 	bool bRepeat = true;
// 	if( m_bBGMLoop == false )
// 		bRepeat = false;
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic( m_strBGMFullpath.c_str(), GetbBGMLoop() );
	const float volFinal = GetBGMVolumeLocal() * GetBGMMasterVolume();
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume( volFinal );
}

void XSoundMngAndroid::StopBGMusic()
{
	XSoundMng::StopBGMusic();
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}

// void XSoundMngAndroid::FadeOutBGM( float fLevel)
// {
// 	XSoundMng::FadeOutBGM( fLevel );
// 	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume( m_BGMVolumeTemp );
// }

// bool XSoundMngAndroid::IsBGMusic( void )
// {	
// 	if( m_strBGM.empty() == false )
// 		return true;
// 	return false;
// }

/*
. 안드로이드에서 사운드가 즉시 플레이 되지않는 문제 해결방안
  * 문제: 로드하고 약간의 딜레이가 있어야 idStream이 나온다.
	1. 모든 사운드를 미리 로딩해두는 방안
	  -메모리문제
		-미리 사용할 사운드를 자동으로 선별할 문제
	2. 사운드 플레이 큐 방식
	  - 사운드 플레이 명령을 내렸을때 만약 idStrea=0로 실패하면 재시도 큐에 넣는다.
		- 약간의 딜레이후에 다시 해당 사운드를 플레이시도 하고 이것을 성공할때까지 반복한다.
		- 플레이가 성공하면 큐를 삭제한다.
		- 부작용. 예를들어 A라는 사운드를 n밀리초 간격으로 플레이했는데 이게 다 실패했다.
		  큐에 쌓여서 큐에서 플레이될때 한꺼번에 풀려서 플레이가 될 가능성이 있음.
*/

/**
 @brief 
 @param idSound sound.txt의 아이디
*/
bool XSoundMngAndroid::OpenSound( ID idSound )
{
	// 중복 사운드를 찾음.(java쪽에서 중복관리를 해주므로 필요없지만 현재 게임전반에 걸쳐서 파일명이 아닌 아이디로 제어되어있어서 당분간은 사용해야한다.
	Sound_Itor it = m_SoundList.begin();
	while( it != m_SoundList.end() ) {
		if( ( *it ).idSound == idSound ) {
			//			(*it).Use++;
			return true;
		}
		it++;
	}
	ST_SOUND sound;
// 	char cFullpath[1024];
// 	strcpy_s( cFullpath, GetFullPath( idSound ) );
	const std::string strFullpath = GetFullPath( idSound );
	if( strFullpath.empty() )
		return false;
	SimpleAudioEngine::sharedEngine()->preloadEffect( strFullpath.c_str() );
	XLOGXN("open sound:%s", strFullpath.c_str() );
	sound.strFile = strFullpath;
	sound.idSound = idSound;
	//	sound.Use  =  1;
	//	sound.idStream = 0;
	m_SoundList.push_back( sound );
	return true;
}

/**
 @brief idSound파일을 닫고 이 파일로 플레이중인 모든 스트림을 닫는다.
*/
void XSoundMngAndroid::CloseSound(ID idSound)
{
	for( auto itor = m_SoundList.begin(); itor != m_SoundList.end(); ) {
		//
		ST_SOUND& sound = (*itor);
		if( sound.idSound == idSound ) {
			if( XASSERT(sound.strFile.empty() == false) ) {
	//			sound.Use--;
	//			if( sound.Use == 0 ) {
				for( auto idStream : sound.m_listStreams ) {
					SimpleAudioEngine::sharedEngine()->stopEffect( idStream );
				}
				SimpleAudioEngine::sharedEngine()->unloadEffect( sound.strFile.c_str() );
				m_SoundList.erase( itor++ );
	//			}
			}
			break;
		}
		++itor;
	}
}

/**
 @brief idSound의 사운드파일을 플레이시키고 그 스트림의 아이디를 얻는다.
*/
ID XSoundMngAndroid::PlaySound(ID idSound, bool bRepeat )
{
// 	if (IsbMuteSound()) 
// 		return 0;
	for( auto& sound : m_SoundList ) {
		if (sound.idSound == idSound) {		// 현재는 OpenSound된것만 플레이 가능
			// 플레이를 시키고 idStream을 받는다.
			ID idStream = SimpleAudioEngine::sharedEngine()->playEffect( sound.strFile.c_str(), bRepeat);
			if( idStream != xINVALID_STREAM ) {
				// 일단은 안드로이드땜에 invalid id를 -1로 하지만 아이폰의 경우 -1이 아닐수도 있음.
				sound.m_listStreams.Add( idStream );
				//
				xnSound::xStream stream;
				stream.m_idSound = idSound;
				stream.m_idStream = idStream;
				stream.m_VolLocal = 1.f;
				m_mapStreams[ idStream ] = stream;
				//
				const float volMaster = GetSoundMasterVolume();
				const float volFinal = stream.m_VolLocal * volMaster;
				SimpleAudioEngine::sharedEngine()
					->setEffectsVolumeByStream( idStream, volFinal );
				XTRACE( "play sound:idStream=%d, volfinal=%.1f, (id=%d)%s", idStream
																											, volFinal
																											, idSound
																											, sound.strFile.c_str() );
			} else {
				// 플레이에 실패하면 큐에 넣는다.
				//m_qReplay.push( xReplay( idSound, sound.strFile, bRepeat ) );
				AddReplayToQ( idSound, bRepeat );
				// idStream이 0이 나올때가 있다. 이유는 확실히 모르지만 파일을 로딩하자마자 플레이를 시도했을때 나는느낌이다.
				XTRACE( "play sound:idStream=%d, %s", idStream, sound.strFile.c_str() );
			}
			return idStream;
		} // if (sound.idSound == idSound)
	} // for
	XTRACE("play sound failed: not found id");
	return 0;
}


// void XSoundMngAndroid::Process( float dt )
// {
// 	if( m_qReplay.size() > 0 ) {
// 		auto pReplay  = m_qReplay.front();
// 		m_qReplay.pop();
// // 		ID idStream = SimpleAudioEngine::sharedEngine()->playEffect( replay.m_strFile.c_str()
// // 																															 , replay.m_bRepeat );
// 		// 재시도
// 		ID idStream = PlaySound( pReplay->m_idSound, pReplay->m_bRepeat );
// 		if( idStream == xINVALID_STREAM ) {
// 			// 플레이 실패하면 다시 뒤에 넣음.
// 			m_qReplay.push( pReplay );
// 		} else {
// 			if( pReplay->m_funcCallback )
// 				pReplay->m_funcCallback( idStream );
// 			SAFE_DELETE( pReplay );
// 		}
// 	}
// }

xnSound::xStream* XSoundMngAndroid::GetpStream( ID idStream )
{
	auto itor = m_mapStreams.find( idStream );
	if( itor != m_mapStreams.end() ) {
		return &(itor->second);
	}
	return nullptr;
}

ID XSoundMngAndroid::DelStream( ID idStream )
{
	auto itor = m_mapStreams.find( idStream );
	if( itor != m_mapStreams.end() ) {
		auto& stream = (*itor).second;
		const ID idSound = stream.m_idSound;
		auto pSound = GetpSound( idSound );
		if( pSound ) {
			pSound->DelStream( idStream );
		}
		m_mapStreams.erase( itor );
		return idSound;
	}
	return 0;
}

ST_SOUND* XSoundMngAndroid::GetpSound( ID idSound )
{
	for( auto& sound : m_SoundList ) {
		if( sound.idSound == idSound ) {
			return &sound;
		}
	}
	return nullptr;
}

void XSoundMngAndroid::SetSoundVolume( ID idStream, float volLocal )
{
	auto pStream = GetpStream( idStream );
	if( pStream ) {
		XSoundMng::SetSoundVolume( idStream, volLocal );
		pStream->m_VolLocal = volLocal;
		const float volFinal = volLocal * GetSoundMasterVolume();
		SimpleAudioEngine::sharedEngine()->setEffectsVolumeByStream( idStream, volFinal );
	}
}

void XSoundMngAndroid::SetSoundVolumeAllStream( ID idSound, float volLocal )
{
	XSoundMng::SetSoundVolumeAllStream( idSound, volLocal );
	for( auto& sound : m_SoundList ) {
		if( sound.idSound == idSound ) {
			for( auto idStream : sound.m_listStreams ) {
				auto pStream = GetpStream( idStream );
				if( pStream ) {
					pStream->m_VolLocal = volLocal;
					const float volFinal = volLocal * GetSoundMasterVolume();
					SimpleAudioEngine::sharedEngine()->setEffectsVolumeByStream( idStream, volFinal );
//					SimpleAudioEngine::sharedEngine()->setEffectsVolume( vol * GetSoundMasterVolume() );
				}
			}
			break;
		}
	}
}

/**
 @brief 바뀐 마스터볼륨을 각 스트림의 볼륨과 다시 곱해서 볼륨을 재조정
*/
void XSoundMngAndroid::SetSoundVolumeAllSound( float volMaster )
{
	XSoundMng::SetSoundVolumeAllSound( volMaster );
	for( auto& sound : m_SoundList ) {
		for( auto idStream : sound.m_listStreams ) {
			auto pStream = GetpStream( idStream );
			if( pStream ) {
				const float volFinal = pStream->m_VolLocal * volMaster;
				// 바뀐 마스터볼륨을 다시 곱해서 볼륨을 재조정
				SimpleAudioEngine::sharedEngine()->setEffectsVolumeByStream( idStream, volFinal );
			}
		}
	}
//	SimpleAudioEngine::sharedEngine()->setEffectsVolume( vol * GetSoundMasterVolume() );
}

/**
 @brief idStrea에 플레이중인 사운드를 중지시킨다.
*/
void XSoundMngAndroid::StopSound( ID idStream )
{
// 	if (IsbMuteSound()) 
// 		return;
	SimpleAudioEngine::sharedEngine()->stopEffect( idStream );
	DelStream( idStream );
}
/**
 @brief 플레이중이던 모든 스트림을 중지시키고 사운드를 메모리에서 날림.
*/
void XSoundMngAndroid::RemoveAll()
{
	for( auto& sound : m_SoundList ) {
		SimpleAudioEngine::sharedEngine()->unloadEffect( sound.strFile.c_str() );
	}
	m_SoundList.clear();
	m_mapStreams.clear();
// 	for(Sound_Itor itor = m_SoundList.begin(); itor != m_SoundList.end(); ) {
// 		m_SoundList.erase(itor++);
// 	}
}

/**
 @brief 사운드를 중지시키기만 한다. 메모리에서 날리진 않는다.
*/
void XSoundMngAndroid::StopAllSound() 
{
	SimpleAudioEngine::sharedEngine()->stopAllEffects();
	m_mapStreams.clear();
	for( auto& sound : m_SoundList ) {
		sound.m_listStreams.clear();
	}
}

#endif // xsound2
#endif // _VER_ANDROID
