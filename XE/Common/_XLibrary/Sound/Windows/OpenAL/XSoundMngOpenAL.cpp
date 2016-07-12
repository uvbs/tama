#include "stdafx.h"
#ifndef _XOGG_EFFECT
#include <stdio.h>
#include <assert.h>
#include <list>
#include "XSoundMngOpenAL.h"
#include "etc/path.h"
#include "etc/ConvertString.h"
#include "XResMng.h"
#include "XFramework/XSoundTable.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;
//------------------------------------------------------------------------------------------------------------
XSoundMngOpenAL::XSoundMngOpenAL()
{
	m_pOpenAL = new XOpenAL;
	m_pBGMusic = NULL;
	m_pOgg = NULL;
	m_pOgg = new XOgg;

}

//------------------------------------------------------------------------------------------------------------

XSoundMngOpenAL::~XSoundMngOpenAL()
{
	RemoveAll();
	SAFE_DELETE( m_pOpenAL );
	SAFE_DELETE( m_pBGMusic );
	SAFE_DELETE( m_pOgg );
}

//------------------------------------------------------------------------------------------------------------
// 패스는 넣지말고 파일타이틀만 넘길것.
bool XSoundMngOpenAL::OpenBGMusic( const char* _filename, bool repeat)
{	
// 	if( !m_pOpenAL )	
// 		return false;
// 	char filename[ 1024 ];
// 	sprintf_s( filename, "%s.ogg", _filename );

	// 이미 같은 파일명이 플레이 되고 있다면 그냥 리턴
// 	if( XE::IsSame( m_szBGMFilename, _filename ) )
// 		return false;
// 
// 	strcpy_s( m_szBGMFilename, _filename );
	const std::string strFile = XE::Format( "%s.ogg", _filename );
	auto bOk = XSoundMng::OpenBGMusic( strFile.c_str(), repeat );
	if( !bOk )
		return false;
	if (m_pBGMusic != nullptr) 
		SAFE_DELETE( m_pBGMusic );

#ifdef _XTOOL
	m_pBGMusic = new XOggObj( strFile.c_str() );
#else
	TCHAR szFullpath[ 1024 ];
// 	TCHAR szRes[ 1024 ];
// 	_tcscpy_s( szRes, C2SZ( XE::MakePath( DIR_BGM, filename ) ) );
	const _tstring strRes = C2SZ( XE::MakePath2( DIR_BGM, strFile ) );
	// szRes의 풀패스를 얻어낸다.
	XE::SetReadyRes( szFullpath, strRes.c_str() );
	if( XE::IsEmpty( szFullpath ) )
		return false;
	const std::string strcFullpath = SZ2C( szFullpath );
	m_pBGMusic = new XOggObj( strcFullpath.c_str() );
#endif
	if( m_pBGMusic->GetError() ) {
		SAFE_DELETE( m_pBGMusic );
		return false;
	}

	m_pBGMusic->SetRepeat(repeat);
//	SetBGMVolume( GetBGMVolume() );
//  m_BGMVolumeTemp = m_BGMVolume;
	CONSOLE( "BGM:%s .......loaded", C2SZ( strFile ) );

	return true;
}

//------------------------------------------------------------------------------------------------------------
/**
 @brief 볼륨조절
 주의해야할것은 float volume은 로컬 볼륨이다. 볼륨 페이드 인/아웃등에 일시적으로 사용한다.
 게임옵션등에서 전체 볼륨을 조절하려면 m_BGMMasterVolume을 조절해야 한다.
*/
void XSoundMngOpenAL::SetBGMVolumeLocal(float volLocal)
{	
	if(!m_pOpenAL) return;
	XSoundMng::SetBGMVolumeLocal( volLocal );
	if( m_pBGMusic )
		m_pBGMusic->SetVolume( volLocal * GetBGMMasterVolume() );
}

void XSoundMngOpenAL::SetBGMVolumeFinal( float volFinal )
{
	if( !m_pOpenAL ) return;
	XSoundMng::SetBGMVolumeFinal( volFinal );
	if( m_pBGMusic )
		m_pBGMusic->SetVolume( volFinal );
}
//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::PlayBGMusic()
{
	if( !m_pOpenAL )
		return;
	if (GetbMuteBGM() == false && m_pBGMusic ) {
		m_pBGMusic->Play();
		m_pBGMusic->SetVolume( GetBGMVolumeLocal() * GetBGMMasterVolume() );
	}
}
//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::StopBGMusic()
{
	XSoundMng::StopBGMusic();
	if(!m_pOpenAL) 
		return;
//	m_szBGMFilename[0] = 0;
	if( m_pBGMusic == nullptr) 
		return;
	m_pBGMusic->Stop();
	SAFE_DELETE( m_pBGMusic );
}
//------------------------------------------------------------------------------------------------------------

// void XSoundMngOpenAL::FadeOutBGM( float fLevel)
// {
// //	if(!m_pOpenAL) return;
// 	if( m_pBGMusic == NULL )	return;          
// 	XSoundMng::FadeOutBGM( fLevel );
// 	m_pBGMusic->SetVolume( m_BGMVolumeTemp );
// }
//------------------------------------------------------------------------------------------------------------
// bool XSoundMngOpenAL::IsBGMusic()
// {	
// 	if (m_pBGMusic != NULL ) return true;
// 	return false;
// }
//------------------------------------------------------------------------------------------------------------
bool XSoundMngOpenAL::OpenSound( ID idSound )
{	
	if( m_pOpenAL == NULL )
		return false;
	if( idSound == 0 )		// 0번 사운드는 없는걸로 침. 툴에ㅐ서 사용.
		return true;
	// 중복 사운드를 찾음.
	Sound_Itor it = m_SoundList.begin();
	while( it != m_SoundList.end() ) {
		if( ( *it )->idSound == idSound ) {
			( *it )->Use++;
			return true;
		}
		it++;
	}
	ST_SOUND *pSound;
	pSound = new ST_SOUND;

	pSound->Sound = NULL;
	pSound->Sound = new XOALObj;
	const std::string strFullpath = GetFullPath( idSound );
// 	const std::string strcExt = XE::GetFileExt( strFullpath );
// 	if( strcExt.empty() )
// #ifdef WIN32
// 		strFullpath += ".wav";
// #else
// 		strFullpath += ".ogg";
// #endif // WIN32
	if( pSound->Sound->Load( strFullpath.c_str() ) == false ) {
		SAFE_DELETE( pSound->Sound );
	}
	if( pSound->Sound != NULL ) {
		pSound->idSound = idSound;
		pSound->Use = 1;
		pSound->alBuffer = pSound->Sound->GetalBuffer();
		m_SoundList.push_back( pSound );
		CONSOLE_TAG( "sound", "sound:%s .......loaded", C2SZ( strFullpath.c_str() ) );
		return true;
	}
	SAFE_DELETE( pSound );
	return false;
}

//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::CloseSound( ID idSound )
{
	if( m_pOpenAL == NULL )
		return;
	ST_SOUND *pSound;
	pSound = NULL;
	Sound_Itor it = m_SoundList.begin();
	while( it != m_SoundList.end() ) {
		if( ( *it )->idSound == idSound ) {
			pSound = ( *it );
			break;
		}
		it++;
	}
	if( pSound != NULL ) {
		pSound->Use--;
		if( pSound->Use == 0 ) {
			m_SoundList.erase( it );
			SAFE_DELETE( pSound->Sound );
			SAFE_DELETE( pSound )
				pSound = NULL;
		}
	}
}

//------------------------------------------------------------------------------------------------------------
ID XSoundMngOpenAL::PlaySound(ID idSound, bool bRepeat )
{	
	if( m_pOpenAL == NULL ) 
		return 0;
	if( IsbMuteSound() ) 
		return 0;
	if( GetSoundVolume() == 0 || GetSoundMasterVolume() == 0 )	
		return 0;
	Sound_Itor it = m_SoundList.begin();
	while( it != m_SoundList.end() ) {
		if( ( *it )->idSound == idSound ) {
			( *it )->Sound->SetVolume( /*m_SoundVolume **/ GetSoundMasterVolume() );
			if( bRepeat )
				( *it )->Sound->SetRepeat( true );
			( *it )->Sound->Play();
			break;
		}
		it++;
	}
	auto idStream = idSound;		// 임시
	if( idStream == 0 ) {
		AddReplayToQ( idSound, bRepeat );
	}
	return idSound;		// 일단 땜빵
}

void XSoundMngOpenAL::StopSound( ID idSound )
{
	if( m_pOpenAL == NULL ) return;
	if (IsbMuteSound()) 
		return;
	
	Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end()) 
	{
	    if ((*it)->idSound == idSound)
		{		    
			(*it)->Sound->Stop();
			break;
		}
		it++;
	}	
}



//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::RemoveAll()
{	
	if( m_pOpenAL == NULL ) return;
   ST_SOUND *pSound;
   pSound = NULL;
   for(Sound_Itor itor = m_SoundList.begin(); itor != m_SoundList.end(); ) {
	   pSound = (*itor);
	   m_SoundList.erase(itor++);
	   if (pSound != NULL) {
		   pSound->Sound->Stop();
		   SAFE_DELETE( pSound->Sound );
		   SAFE_DELETE( pSound );
	   }
   }   
}



#endif // not _XOGG_EFFECT
