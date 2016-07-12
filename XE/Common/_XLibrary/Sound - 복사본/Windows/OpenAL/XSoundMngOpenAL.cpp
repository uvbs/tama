#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include <list>
#include "XSoundMngOpenAL.h"
#include "etc/path.h"
#include "etc/ConvertString.h"
#include "XResMng.h"

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
BOOL XSoundMngOpenAL::OpenBGMusic( const char* _filename, BOOL repeat)
{	
	if( !m_pOpenAL )	return FALSE;

	char filename[ 1024 ];
	sprintf( filename, "%s.ogg", _filename );

	// 이미 같은 파일명이 플레이 되고 있다면 그냥 리턴
	if( XE::IsSame( m_szBGMFilename, _filename ) )
		return FALSE;

	strcpy_s( m_szBGMFilename, _filename );

	if (m_pBGMusic != NULL) 
		SAFE_DELETE( m_pBGMusic );

#ifdef _XTOOL
	m_pBGMusic = new XOggObj( filename );
#else
#ifdef _XPACKAGE
	TCHAR szFullpath[ 1024 ];
	TCHAR szRes[ 1024 ];
	_tcscpy_s( szRes, C2SZ( XE::MakePath( DIR_BGM, filename ) ) );
	// szRes의 풀패스를 얻어낸다.
	XE::SetReadyRes( szFullpath, szRes );
	XBREAK( XE::IsEmpty( szFullpath ) == TRUE );
	m_pBGMusic = new XOggObj( SZ2C( szFullpath ) );
#else
	m_pBGMusic = new XOggObj( XE::MakePath( DIR_BGM, filename ) );
#endif
#endif
	if( m_pBGMusic->GetError() )
	{
		SAFE_DELETE( m_pBGMusic );
		return FALSE;
	}

	m_pBGMusic->SetRepeat(repeat);
    m_BGMVolumeTemp = m_BGMVolume;
	CONSOLE( "BGM:%s .......loaded", Convert_char_To_TCHAR( filename ) );

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::SetBGMVolume(float volume)
{	
	if(!m_pOpenAL) return;
	XSoundMng::SetBGMVolume( volume );
	if( m_pBGMusic )
		m_pBGMusic->SetVolume( volume );
}
//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::PlayBGMusic()
{
	if( !m_pOpenAL )
		return;
	if (m_bMuteBGM == FALSE && m_pBGMusic != NULL)
	{
		m_pBGMusic->Play();
		m_pBGMusic->SetVolume( m_BGMVolume );
	}
}
//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::StopBGMusic()
{
	if(!m_pOpenAL) return;
	m_szBGMFilename[0] = 0;
	if( m_pBGMusic == NULL ) return;
	m_pBGMusic->Stop();
	SAFE_DELETE( m_pBGMusic );
}
//------------------------------------------------------------------------------------------------------------

void XSoundMngOpenAL::FadeOutBGM( float fLevel)
{
//	if(!m_pOpenAL) return;
	if( m_pBGMusic == NULL )	return;          
	XSoundMng::FadeOutBGM( fLevel );
	m_pBGMusic->SetVolume( m_BGMVolumeTemp );
}
//------------------------------------------------------------------------------------------------------------
BOOL XSoundMngOpenAL::IsBGMusic( void )
{	
	if (m_pBGMusic != NULL ) return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------
BOOL XSoundMngOpenAL::OpenSound( int FileID )
{	
	if( m_pOpenAL == NULL )
		return FALSE;
	if( FileID == 0 )		// 0번 사운드는 없는걸로 침. 툴에ㅐ서 사용.
		return TRUE;
	// 중복 사운드를 찾음.
	Sound_Itor it = m_SoundList.begin();
	while(it != m_SoundList.end())
	{
	    if ( (*it)->FileID == FileID)
		{
			(*it)->Use++;
			return TRUE;
		}
		it++;
	}
    ST_SOUND *pSound;
	pSound = new ST_SOUND;

	pSound->Sound = NULL;
	char filename[100];
	sprintf_s(filename, "%04d.wav", FileID);

	TCHAR szFullpath[ 1024 ];
	const char *cRes = XE::MakePath( DIR_SND, filename );
	// szRes의 풀패스를 얻어낸다.
	XE::SetReadyRes( szFullpath, C2SZ( cRes ) );
	pSound->Sound = new XOALObj;
	if( pSound->Sound->Load(  SZ2C( szFullpath ) ) == FALSE )
	{
		SAFE_DELETE( pSound->Sound );
	}
	if (pSound->Sound != NULL)
	{
		pSound->FileID = FileID;
	    pSound->Use  =  1;
		m_SoundList.push_back(pSound);
		CONSOLE( "sound:%s .......loaded", Convert_char_To_TCHAR( filename ) );
		return TRUE;
	}
	if (pSound != NULL)
		delete pSound;
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::CloseSound(int FileID)
{
	if( m_pOpenAL == NULL )
		return;

    ST_SOUND *pSound;
	pSound = NULL;

    Sound_Itor it = m_SoundList.begin();

	while(it != m_SoundList.end())

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

			m_SoundList.erase(it);

			SAFE_DELETE( pSound->Sound );

			SAFE_DELETE( pSound )

			pSound = NULL;

		}

	}	

}



//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::PlaySound(int FileID, BOOL bRepeat )

{	
	if( m_pOpenAL == NULL ) return;

	if (m_bMuteSound) return;
	if( GetSoundVolume() == 0 )	return;



	Sound_Itor it = m_SoundList.begin();

	while(it != m_SoundList.end()) {

	    if ((*it)->FileID == FileID)

		{		    

			(*it)->Sound->SetVolume( m_SoundVolume );
			if( bRepeat )
				(*it)->Sound->SetRepeat( TRUE );
			(*it)->Sound->Play();

			break;

		}

		it++;

	}

}



void XSoundMngOpenAL::StopSound( int FileID )

{
	if( m_pOpenAL == NULL ) return;

	if (m_bMuteSound) return;

	

	Sound_Itor it = m_SoundList.begin();

	while(it != m_SoundList.end()) {

	    if ((*it)->FileID == FileID)

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

   for(Sound_Itor itor = m_SoundList.begin(); itor != m_SoundList.end(); )
   {
	   pSound = (*itor);
	   m_SoundList.erase(itor++);
	   if (pSound != NULL) 
	   {
		   pSound->Sound->Stop();
		   SAFE_DELETE( pSound->Sound );
		   SAFE_DELETE( pSound );
	   }
   }   
}



